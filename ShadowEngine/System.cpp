#include "System.h"
#include "DX12.h"

#include <sstream>
#include <iomanip>

System::System()
	:
	wnd(800,600,L"Fuck Box!")	
{}

System::~System()
{
}

int System::Run()
{
	MSG msg = { 0 };

	timer.Reset();

	//while queue has messages, remove and dispatch them (but do not block exe.)
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();

			if (wnd.isPaused() == false)
			{
				wnd.CalculateFrameStats();
				wnd.pGraphics->EndFrame();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}