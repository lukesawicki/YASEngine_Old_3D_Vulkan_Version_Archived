#include"stdafx.hpp"
#include"Main.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	YasEngine yasEngine = YasEngine();
	yasEngine.run(hInstance);
	system("PAUSE");
	return 0;
}

