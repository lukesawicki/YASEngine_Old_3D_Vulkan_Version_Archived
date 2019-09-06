#include"stdafx.hpp"
#include"YasEngine.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

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

const char* YasEngine::engineName = "YasEngine";
const char* YasEngine::applicationName = "YasEngine Demo Application";

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
    // Enumerating layer properties
    uint32_t numberOfValidationLayers = 0;
    vkEnumerateInstanceLayerProperties(&numberOfValidationLayers, nullptr);
    vkEnumerateInstanceLayerProperties(&numberOfValidationLayers, availableValidationLayersProperties.data());
    
    std::map<const char*, bool> availableLayers;
    std::map<const char*, bool> availableExtensions;

    try
    {
        for(const char* layerName: requiredInstanceLayerNames)
        {
            for(VkLayerProperties layerProperty: availableValidationLayersProperties)
            {
                if( strcmp(layerProperty.layerName, layerName) )
                {
                    availableLayers.insert( {layerProperty.layerName, true} );
                    break;
                }
                if(availableLayers.find(layerName) == availableLayers.end())
                {
                    // CORRECT THE SAME CODE FOR LAYERS
                    availableLayers.insert({layerName, false});
                }
            }
        }

        if(availableLayers.size() < requiredInstanceLayerNames.size())
        {
            throw new MissingValidationLayersException();
        }

        std::cout << "Availabla layers:" << std::endl;
        for(VkLayerProperties layerProperty: availableValidationLayersProperties)
        {
            std::cout << layerProperty.layerName << std::endl;
        }

        // Enumerating instance extensions
        uint32_t nuberOfInstanceExtensions = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &nuberOfInstanceExtensions, nullptr);
        vkEnumerateInstanceExtensionProperties(nullptr, &nuberOfInstanceExtensions, availableExtentionProperties.data());

        for(const char* extensionName: requiredExtenstionsNames)
        {
            for(VkExtensionProperties extensionProperty: availableExtentionProperties)
            {
                if( strcmp(extensionProperty.extensionName, extensionName) )
                {
                    availableExtensions.insert( {extensionProperty.extensionName, true} );
                    break;
                }
            }
            if(availableExtensions.find(extensionName) == availableExtensions.end())
            {
                availableExtensions.insert({extensionName, false});
            }
        }

        if(availableExtensions.size() < requiredInstanceLayerNames.size())
        {
            throw new MissingValidationLayersException();
        }

        std::cout << "Availabla layers:" << std::endl;
        for(VkExtensionProperties extensionProperty: availableExtentionProperties)
        {
            std::cout << extensionProperty.extensionName << std::endl;
        }

    }
    catch(MissingInstanceExtensionsException exception)
    {
        std::cout << exception.what() << std::endl;
        std::cout << "Missing extensions: " << std::endl;
        for(std::map<const char*, bool>::iterator layer = availableExtensions.begin(); layer!=availableExtensions.end(); ++layer)
        {
            if(layer->second == false)
            {
                std::cout << layer->first << std::endl;
            }
        }
        exit(EXIT_FAILURE);
    }
    catch (MissingValidationLayersException exception)
    {
        std::cout << exception.what() << std::endl;
        std::cout << "Missing layers: " << std::endl;
        for(std::map<const char*, bool>::iterator layer = availableLayers.begin(); layer!=availableLayers.end(); ++layer)
        {
            if(layer->second == false)
            {
                std::cout << layer->first << std::endl;
            }
        }
        exit(EXIT_FAILURE);
    }

    // Instance creation
    VkApplicationInfo vulkanApplicationInfo = {};

    vulkanApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vulkanApplicationInfo.pNext = NULL;
    vulkanApplicationInfo.pApplicationName = applicationName;
    vulkanApplicationInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    vulkanApplicationInfo.pEngineName = engineName;
    vulkanApplicationInfo.engineVersion = VK_MAKE_VERSION(0,2,0);
    vulkanApplicationInfo.apiVersion = VK_API_VERSION_1_1; // VK_MAKE_VERSION(1,1,101,0);

    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &vulkanApplicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtenstionsNames.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtenstionsNames.data();
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredInstanceLayerNames.size());
    instanceCreateInfo.ppEnabledLayerNames = requiredInstanceLayerNames.data();

    if(vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
    {
        throw new InstanceCreationException();
    }

    // Enumerate physical devices

    vkEnumeratePhysicalDevices(vulkanInstance, &numberOfPhysicalDevices, nullptr);
    vkEnumeratePhysicalDevices(vulkanInstance, &numberOfPhysicalDevices, physicalDevices.data());

    // Getting physical devices properties

    

    for (VkPhysicalDevice physDevice: physicalDevices)
    {
        VkPhysicalDeviceProperties* physProp = new VkPhysicalDeviceProperties();
        physicalDevicesProperties.push_back(physProp);
        vkGetPhysicalDeviceProperties(physDevice, physProp);
        // lukesawicki tu skonczylem 16-07-2019
    }


    // Create logical device
    //vkGetPhysicalDeviceQueueFamilyProperties(
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

