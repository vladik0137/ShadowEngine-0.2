#include "ShadowWindow.h"

#include <sstream>
#include <memory>

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
	width(width),
	height(height)
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

std::optional<int> ShadowWindow::ProcessMessages()
{
	MSG msg;
	//while queue has messages, remove and dispatch them (but do not block exe.)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

DX12 & ShadowWindow::Gfx()
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
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
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
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
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
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
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
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
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

HRESULT ShadowWindow::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string ShadowWindow::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}