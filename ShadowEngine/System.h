#pragma once

#include "ShadowWindow.h"
#include "DX12.h"

class System
{
public:
	System();
	~System();
	int Run();
private:
	ShadowWindow wnd;
};