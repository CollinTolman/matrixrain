#include "MatrixRain.h"
#include <windows.h>


void RunScreenSaver();

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	int nArgs;
	LPWSTR * args = CommandLineToArgvW(GetCommandLine(), &nArgs);

	if (nArgs == 1)
	{
		RunScreenSaver();
	}
	else if (nArgs == 2)
	{
		if (lstrcmpW(args[1], L"/s") == 0)
		{
			// Run fullscreen
			RunScreenSaver();
		}
	}
}


void RunScreenSaver()
{
	mr::MatrixRain matrixRain;
	matrixRain.Start();
}