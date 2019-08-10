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

	while (true)
	{

		if (const auto ecode = ShadowWindow::ProcessMessages())
		{
			return *ecode;
		}

		StartFrame();
	}
}

void System::StartFrame()
{
	wnd.Gfx().EndFrame();
}
