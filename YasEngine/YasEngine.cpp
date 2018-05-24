#include"YasEngine.hpp"

int YasEngine::windowPositionX				= 64;
int YasEngine::windowPositionY				= 64;
int YasEngine::windowWidth					= 640;
int YasEngine::windowHeight					= 480;

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

VkResult createDebugReportCallbackEXT
	(
		VkInstance vulkanInstance,
		const VkDebugReportCallbackCreateInfoEXT* createInfo,
		const VkAllocationCallbacks* allocator,
		VkDebugReportCallbackEXT* callback
	)
{
	PFN_vkCreateDebugReportCallbackEXT debugReportCallbackFunction = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugReportCallbackEXT");
	if(debugReportCallbackFunction != nullptr)
	{
		return debugReportCallbackFunction(vulkanInstance, createInfo, allocator, callback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroyDebugReportCallbackEXT(VkInstance vulkanInstance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* allocator)
{
	PFN_vkDestroyDebugReportCallbackEXT destroyFunction = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugReportCallbackEXT");
	if(destroyFunction != nullptr)
	{
		destroyFunction(vulkanInstance, callback, allocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL YasEngine::debugCallback
	(
	VkDebugReportFlagsEXT debugReportFlags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData
	)
{
	std::cerr << "Validation layer: " << msg << std::endl;
	//If return true, then call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT
	//because this is used to test the validation layers themeselves
	//then for now always return false
	return VK_FALSE;
}

YasEngine::YasEngine()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "w", stderr);
	SetConsoleTitle("YasEngine logging");
}

void YasEngine::setupDebugCallback()
{
	if(!enableValidationLayers)
	{
		return;
	}
	
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;
	
	if(createDebugReportCallbackEXT(vulkanInstance, &createInfo, nullptr, &callback) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug callback function");
	}
}

void YasEngine::run(HINSTANCE hInstance)
{
	createWindow(hInstance);
	initializeVulkan();
	mainLoop();
	cleanUp();
}

//Private Methods

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

	RegisterClassEx(&windowClassEx);

	windowInstance = hInstance;

	window =  CreateWindowEx(
									NULL,
									"YASEngine window class",
									"YASEngine",
									WS_OVERLAPPEDWINDOW,
									windowPositionX, windowPositionY,
									windowWidth, windowHeight,
									NULL,
									NULL,
									windowInstance,
									NULL
								);

	ShowWindow(window, SW_NORMAL);
	SetForegroundWindow(window);
	SetFocus(window);
}

void YasEngine::mainLoop()
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

void YasEngine::initializeVulkan()
{
	createVulkanInstance();
	setupDebugCallback();
}

bool YasEngine::checkForExtensionsSupport(const std::vector<const char*> &enabledExtensions, uint32_t numberOfEnabledExtensions)
{

	bool allEnabletExtensionsAreAvailable = false;
	uint32_t numberOfAvailableExtensions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, nullptr);
	
	std::vector<VkExtensionProperties> availableExtensions(static_cast<int>(numberOfAvailableExtensions));
	vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, availableExtensions.data());
	int extensionsCounter = 0;

	for(int i=0; i<static_cast<int>(numberOfEnabledExtensions); i++)
	{
		for(int j=0; j< static_cast<int>(availableExtensions.size()); j++)
		{
			if(strcmp(enabledExtensions[i], availableExtensions[j].extensionName) == 0)
			{
				++extensionsCounter;
			}
			if(extensionsCounter == numberOfEnabledExtensions)
			{
				return true;
			}
		}
	}
	
	return false;
}

bool YasEngine::checkValidationLayerSupport()
{
	uint32_t layersCount;
	vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
	
	std::vector<VkLayerProperties> availableLayers(layersCount);
	vkEnumerateInstanceLayerProperties(&layersCount, availableLayers.data());
	
	for(const char* layerName: validationLayers)
	{
		bool layerFound = false;
		// Attention
		for(const VkLayerProperties& layerProperties: availableLayers)
		{
			if(strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if(!layerFound)
		{
			return false;
		}
	}
	return true;
}

std::vector<const char*> YasEngine::getRequiredExtensions()
{
	std::vector<const char*> allRequiredExtenstions = std::vector<const char*>();

	allRequiredExtenstions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	allRequiredExtenstions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	if(enableValidationLayers)
	{
		allRequiredExtenstions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	return allRequiredExtenstions;
}

void YasEngine::createVulkanInstance()
{
	if(enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Requested validation layers are not available");
	}	

	std::cout << "Creating Vulkan Instance..." << std::endl;
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "YasEngine Demo";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "Yas Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	std::vector<const char*> allEnabledExtenstions = getRequiredExtensions();
	uint32_t extensionsCount = static_cast<uint32_t>(allEnabledExtenstions.size());

	bool allExtensionsAvailable = checkForExtensionsSupport(allEnabledExtenstions, extensionsCount);

	if(!allExtensionsAvailable)
	{
		throw std::runtime_error("Not all required extensions available! Can't create Vulkan Instance");
	}
	createInfo.enabledExtensionCount = extensionsCount;
	createInfo.ppEnabledExtensionNames = allEnabledExtenstions.data();

	if(enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
	
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan instance!");
	}
}

void YasEngine::cleanUp()
{
	if(enableValidationLayers)
	{
		destroyDebugReportCallbackEXT(vulkanInstance, callback, nullptr);
	}
	//Second parameter is optional allocator which for this version will not be used.
	vkDestroyInstance(vulkanInstance, nullptr);
	DestroyWindow(window);
}
//----------------------------------------------------------------------------------------------------------------------