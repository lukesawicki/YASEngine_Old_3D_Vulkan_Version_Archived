#include"YasEngine.hpp"

// Static variables Window
int YasEngine::windowPositionX				= 64;
int YasEngine::windowPositionY				= 64;
int YasEngine::windowWidth					= 640;
int YasEngine::windowHeight					= 480;

// Windows functions and procedures

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void YasEngine::createWindow(HINSTANCE hInstance)
{
	WNDCLASSEX windowClassEx;
	
	windowClassEx.cbSize				= sizeof(WNDCLASSEX);
	windowClassEx.style					= CS_VREDRAW | CS_HREDRAW;
	windowClassEx.lpfnWndProc			= windowProcedure;
	windowClassEx.cbClsExtra			= 0;
	windowClassEx.cbWndExtra			= 0;
	windowClassEx.hInstance				= hInstance;
	windowClassEx.hIcon					= LoadIcon(0, IDI_APPLICATION);
	windowClassEx.hCursor				= LoadCursor(0, IDC_CROSS);
	windowClassEx.hbrBackground			= (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClassEx.lpszMenuName			= 0;
	windowClassEx.lpszClassName			= "YASEngine window class";
	windowClassEx.hIconSm				= LoadIcon(0, IDI_APPLICATION);

	//Register window class here:
	RegisterClassEx(&windowClassEx);

	//Create window
	window =  CreateWindowEx(
									NULL,
									"YASEngine window class",
									"YASEngine",
									WS_OVERLAPPEDWINDOW,
									windowPositionX, windowPositionY,
									windowWidth, windowHeight,
									NULL,
									NULL,
									hInstance,
									NULL
								);

	//Show and refresh window
	ShowWindow(window, SW_NORMAL);
	SetForegroundWindow(window);
	SetFocus(window);
}

void YasEngine::renderLoop()
{
	MSG message;
	
	while(GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
		if(message.message == WM_QUIT)
		{
			break;
		}
	}
}
