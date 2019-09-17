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
		StartFrame();
	}
}

void System::StartFrame()
{
	wnd.ProcessMessages();
}
