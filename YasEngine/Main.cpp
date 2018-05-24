#include"Main.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	YasEngine yasEngine = YasEngine();
	yasEngine.run(hInstance);
	//int stop=0;
	//std::cin >> stop;
	return 0;
}