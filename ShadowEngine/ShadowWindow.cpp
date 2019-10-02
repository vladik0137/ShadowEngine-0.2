#include "ShadowWindow.h"


#include <sstream>
#include <memory>
#include <assert.h>

ShadowWindow::WindowClass ShadowWindow::WindowClass::wndClass;

ShadowWindow::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	//register windows class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"Shadow Engine Window";
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);


}

ShadowWindow::WindowClass::~WindowClass()
{
	//UnregisterClass(wndClassName, GetInstance());
}

//const char* ShadowWindow::WindowClass::GetName() noexcept
//{
//	return wndClassName;
//}

HINSTANCE ShadowWindow::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

ShadowWindow::ShadowWindow(int width, int height, const wchar_t* name)
	:
	mwidth(width),
	mheight(height)
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw SHWND_LAST_EXCEPT();
	}
	// create window
	hWnd = CreateWindow(
		L"Shadow Engine Window", name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	if (hWnd == nullptr)
	{
		throw SHWND_LAST_EXCEPT();
	}
	// show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	// create graphics object
	pGraphics = std::make_unique<DX12>(hWnd);
	SetWidth();
	SetHeight();
}

ShadowWindow::~ShadowWindow()
{
	DestroyWindow(hWnd);
}

void ShadowWindow::SetTitle(const std::wstring& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw SHWND_LAST_EXCEPT();
	}
}

DX12& ShadowWindow::Gfx()
{
	return *pGraphics;
}

LRESULT WINAPI ShadowWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer 
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		ShadowWindow* const pWnd = static_cast<ShadowWindow*>(pCreate->lpCreateParams);
		// set WimAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&ShadowWindow::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI ShadowWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window class
	ShadowWindow* const pWnd = reinterpret_cast<ShadowWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT ShadowWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		pGraphics->OnResize();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	// clear keystate when window loses focus
	case WM_KILLFOCUS:
		kbd.ClearState();
		return 0;
	/* Handles for keyboard events */
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled())
		{
			kbd.OnKeyPress(static_cast<unsigned char>(wParam));
		}
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyRelease(static_cast<unsigned char>(wParam));
		return 0;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		return 0;
	/* End of keyboard event handles*/
	//////////////////////////////////
	/*Mouse event handles*/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// check to see if we are in client region
		if (pt.x >= 0 && pt.x < mwidth && pt.y >= 0 && pt.y < mheight)
		{
			//log enter and capture mouse movements
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// if we aren't in the client window
		else
		{
			// if the mouse button is still held down keep capturing
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// Release on mouse up
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPress(pt.x, pt.y);
		if (pt.x < 0 || pt.x >= mwidth || pt.y < 0 || pt.y >= mheight)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		return 0;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftRelease(pt.x, pt.y);
		if (pt.x < 0 || pt.x >= mwidth || pt.y < 0 || pt.y >= mheight)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPress(pt.x, pt.y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightRelease(pt.x, pt.y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(pt.x, pt.y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(pt.x, pt.y);
		}
		return 0;
	}
	/*End of mouse handles*/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ShadowWindow::SetWidth()
{
	mwidth = pGraphics->mClientWidth;
}

void ShadowWindow::SetHeight()
{
	mheight = pGraphics->mClientHeight;
}

bool ShadowWindow::isPaused()
{
	return mAppPaused;
}

ShadowWindow::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	ShadowException(line,file),
	hr(hr)
{}

const char* ShadowWindow::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ShadowWindow::Exception::GetType() const noexcept
{
	return "Shadow Exception";
}

std::string ShadowWindow::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

void ShadowWindow::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount;
		float mspf = 1000.f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText =
			L"   fps: " + fpsStr +
			L"  mspf: " + mspfStr;

		SetWindowText(hWnd, windowText.c_str());

		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

HRESULT ShadowWindow::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string ShadowWindow::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
