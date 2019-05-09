#include"stdafx.hpp"
#include"Main.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

// Main function off windows application. Return int. 0 - there were not errors. !=0 there were errors.
// hInstance handle to this application. hPrevInstance - not using (deprecated)
// lpCmdLine pointer to char table- string with witch applications are call.
// nShowCmd - variable which tell how to show window.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	YasEngine yasEngine = YasEngine();
	yasEngine.run(hInstance);
	system("PAUSE");
	return 0;
}

