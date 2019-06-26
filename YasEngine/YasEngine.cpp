#include"stdafx.hpp"
#include"YasEngine.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

YasEngine::YasEngine(HINSTANCE hInstance)
{
        // Allocates a new console for the calling process.
	AllocConsole();
    // Attaches the calling process to the console of the specified process.
	AttachConsole(GetCurrentProcessId());
    // Stream object
	FILE* file;
    // fropen_s open existing file with another name
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "w", stderr);
	SetConsoleTitle("YasEngine logging");
    std::cout.clear();

    applicationHandle = hInstance;
}

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DISPLAYCHANGE:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);
            break;
        case WM_KEYDOWN:
        {
            // TODO
        }
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void YasEngine::prepareWindow()
{
    std::cout << "Preaparing window..." << std::endl;
    // Size of windowClassEx object
    windowClassEx.cbSize                        = sizeof(WNDCLASSEX);

    // Window style. Look and behavior
    windowClassEx.style                         = CS_VREDRAW | CS_HREDRAW;

    // Pointer to window procedure
    windowClassEx.lpfnWndProc                   = windowProcedure;

    // Size of extra memory for the application
    windowClassEx.cbClsExtra                    = 0;

    // Size of extra memory for window created using this window class
    windowClassEx.cbWndExtra                    = 0;

    // handle to application
    windowClassEx.hInstance                     = applicationHandle;

    // Appliction icon in the alt + tab dialog
    windowClassEx.hIcon                         = LoadIcon(0, IDI_APPLICATION);

    // Handle to cursor displayed when cursor is over the window
    windowClassEx.hCursor                       = LoadCursor(0, IDC_CROSS);

    // Handle to brush for redrawing window
    windowClassEx.hbrBackground                 = (HBRUSH)GetStockObject(WHITE_BRUSH);

    // Handle to menu which will be attached to this window
    windowClassEx.lpszMenuName                  = 0;

    // Name of window
    windowClassEx.lpszClassName                 = "YASEngine window class";

    // Handle to icon whitch will be show on windows bar.
    windowClassEx.hIconSm                       = LoadIcon(0, IDI_APPLICATION);

    // Function whtch create, set values and register window class in the system.
    RegisterClassEx(&windowClassEx);

    // Function tho create window with specyfied properties.
    windowHandle = CreateWindowEx(NULL, "YASEngine window class", "YASEngine", WS_OVERLAPPEDWINDOW, windowXposition,
                                  windowYposition, windowWidth, windowHeight, NULL, NULL, applicationHandle, NULL);
    
    // Set window's show state
	ShowWindow(windowHandle, SW_NORMAL);

    // Brings thread that created this window into the foreground and activates the window.
	SetForegroundWindow(windowHandle);

    // Set focus to specified window.
	SetFocus(windowHandle);
}

void YasEngine::prepareVulkan()
{
    std::cout << "Preparing Vulkan instance object..." << std::endl;

    // Enumerating validation and extensions layers:
    uint32_t validationLayersNumber = 0;
    vkEnumerateInstanceLayerProperties(&validationLayersNumber, nullptr);
    vkEnumerateInstanceLayerProperties(&validationLayersNumber, validationLayersProperties.data);


        
//VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
//    uint32_t*                                   pPropertyCount,
//    VkLayerProperties*                          pProperties);

    // Create Vulkan instance (VkInstance)

    VkApplicationInfo vulkanApplicationInfo = {};



    std::cout << "Adding requested validations layers and extensions" << std::endl;

}

void YasEngine::run()
{
    prepareWindow();
    prepareVulkan();

    float time;
    float newTime;
    float deltaTime;
    float fps;
    float fpsTime;
    unsigned int frames;
    MSG message;

    TimePicker timePicker = TimePicker();
    time = timePicker.getSeconds();
    fpsTime = 0.0F;
    frames = 0;
    message.message = WM_NULL;

    while(message.message != WM_QUIT)
    {
        if(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            newTime = timePicker.getSeconds();
            deltaTime = newTime - time;
            time = newTime;
            // TODO drawFrame(deltaTime);            
            frames++;
            fpsTime = fpsTime + deltaTime;
            if(fpsTime >= 1.0F)
            {
                fps = frames / fpsTime;
                frames = 0;
                fpsTime = 0.0F;
            }
        }
    }
}

//                                                                            80                                     120
//-----------------------------------------------------------------------------|---------------------------------------|
