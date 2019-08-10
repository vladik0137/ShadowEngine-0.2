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
	void StartFrame();
private:
	ShadowWindow wnd;
	GameTimer timer;
};