#include"Main.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	YasEngine yasEngine = YasEngine();
	yasEngine.createWindow(hInstance);
	yasEngine.renderLoop();
}