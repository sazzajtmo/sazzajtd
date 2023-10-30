// SazzajTD.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "SazzajTD.h"
#include "AppManager.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	//_CrtSetBreakAlloc(2460);

	if( cAppManager::GetInstance()->Init() )
		cAppManager::GetInstance()->MainLoop();

	cAppManager::GetInstance()->DestroyInstance();

	_CrtDumpMemoryLeaks();

    return 0;
}
