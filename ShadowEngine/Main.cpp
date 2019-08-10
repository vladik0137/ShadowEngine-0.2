#include "System.h"

#include <sstream>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	
	try
	{
		return System{}.Run();
	}

	catch(const ShadowException& e)
	{
		/// Want to convert to unicode in the future
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}

	catch (const std::exception& e)
	{
		/// Want to convert to unicode in the future
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	catch (...)
	{
		MessageBox(nullptr, L"No details availavle", L"Unkown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}