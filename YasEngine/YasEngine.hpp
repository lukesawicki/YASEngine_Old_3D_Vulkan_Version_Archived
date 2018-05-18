#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include<Windows.h>

class YasEngine
{
	public:
		//Window functions and variables
		void createWindow(HINSTANCE hInstance);
		void renderLoop();
		//Vulkan API functions and variables

		static int				windowPositionX;
		static int				windowPositionY;
		static int				windowWidth;
		static int				windowHeight;
	//public end
	private:
		//Window functions and variables
		
		HINSTANCE				windowInstance;
		HWND					window;
		//Vulkan API functions and variables

	//private end
};

#endif