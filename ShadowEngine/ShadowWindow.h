#pragma once
//#include "ShadowWin.h"
#include <Windows.h>

#include "ShadowException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "DX12.h"

#include <optional>
#include <memory>

class HWNDKey
{
	friend class DX12;
public:
	HWNDKey(const HWNDKey&) = delete;
	HWNDKey& operator=(HWNDKey&) = delete;
protected:
	HWNDKey() = default;
protected:
	
};

class ShadowWindow : public HWNDKey
{
	friend class DX12;
public:
	class Exception : public ShadowException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
	// manage registration/cleanup of window class
	class WindowClass
	{
	public:
		//static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		//static constexpr const char* wndClassName = "Shadow Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	ShadowWindow(int width, int height, const wchar_t* name);
	~ShadowWindow();
	ShadowWindow(const ShadowWindow&) = delete;
	ShadowWindow& operator=(const ShadowWindow&) = delete;
	void SetTitle(const std::wstring& title);
	void CalculateFrameStats();
	//int ProcessMessages();
	DX12& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	void SetWidth();
	void SetHeight();
public:
	Keyboard kbd;
	Mouse mouse;
	GameTimer mTimer;
	HWND hWnd;
	std::unique_ptr<DX12> pGraphics;
	bool isPaused();
	int mwidth;
	int mheight;
private:


	bool mAppPaused = false;
	bool mMinimized = false;
	bool mMaximized = false;
	bool mResizing = false;
	bool mFullscreenState = false;
	//HINSTANCE mhAppInst = nullptr;
	
};

#define SHWND_EXCEPT(hr) ShadowWindow::Exception(__LINE__,__FILE__, hr)
#define SHWND_LAST_EXCEPT(hr) ShadowWindow::Exception(__LINE__,__FILE__, GetLastError())

