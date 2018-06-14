#include"stdafx.hpp"
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

	application = hInstance;

	window =  CreateWindowEx
				(
					NULL,
					"YASEngine window class",
					"YASEngine",
					WS_OVERLAPPEDWINDOW,
					windowPositionX, windowPositionY,
					windowWidth, windowHeight,
					NULL,
					NULL,
					application,
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
	createSurface();
	selectPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

bool YasEngine::checkForExtensionsSupport(const std::vector<const char*> &enabledExtensions, uint32_t numberOfEnabledExtensions)
{

	bool allEnabletExtensionsAreAvailable = false;
	uint32_t numberOfAvailableExtensions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, nullptr);
	
	std::vector<VkExtensionProperties> availableExtensions(static_cast<size_t>(numberOfAvailableExtensions));
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

bool YasEngine::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionsCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);
	
	std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());
	
	for(const char* deviceExtensionName: deviceExtensions)
	{
		bool extensionFound = false;

		for(const VkExtensionProperties& availableExt: availableExtensions)
		{
			if(strcmp(deviceExtensionName, availableExt.extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}
		if(!extensionFound)
		{
			return false;
		}
	}

	return true;
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
	allRequiredExtenstions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);//VK_KHR_WIN32_SURFACE_EXTENSION_NAME

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

void YasEngine::selectPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
	if(deviceCount == 0) {
		throw std::runtime_error("Failed to find Graphics Cards with Vulkan support.");
	}
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalDevices.data());

	for(const VkPhysicalDevice& device: physicalDevices)
	{
		if(isPhysicalDeviceSuitable(device))
		{
			physicalDevice = device;
			std::cout << "YasEngine chosen physical device." << std::endl;
			break;
		}
	}
	if(physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find suitable graphic card");
	}
}

bool YasEngine::isPhysicalDeviceSuitable(VkPhysicalDevice device)
{	
	QueueFamilyIndices indices = findQueueFamilies(device);

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &physicalDeviceFeatures);
	if(physicalDeviceProperties.vendorID == 4130)
	{
		std::cout << "Physical device vendor: AMD" << std::endl;
	}
	else
	{
		if(physicalDeviceProperties.vendorID == 4318)
		{
			std::cout << "Physical device vendor: NVIDIA" << std::endl;
		}
		else
		{
			if(physicalDeviceProperties.vendorID == 8086)
			{
				std::cout << "Physical device vendor: INTEL" << std::endl;
			}
			else
			{
				std::cout << "Physical device vendor: Other vendor." << std::endl;
			}
		}
	}

	bool extensionsSupported = checkPhysicalDeviceExtensionSupport(device);
	bool swapChainSuitable = false;

	if(extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = VulkanSwapChain::querySwapChainSupport(device, surface);
		swapChainSuitable = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainSuitable;
}

QueueFamilyIndices YasEngine::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices queueFamilyIndices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const auto& queueFamily : queueFamilies)//(const VkQueueFamilyProperties& queueFamily: queueFamilies)
	{
		if((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			queueFamilyIndices.graphicsFamily = i;
		}

		VkBool32 presentationFamilySupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationFamilySupport);
		
		if(queueFamily.queueCount > 0 && presentationFamilySupport)
		{
			queueFamilyIndices.presentationFamily = i;
		}
		if(queueFamilyIndices.isComplete())
		{
			break;
		}
		i++;
	}

	return queueFamilyIndices;
}

void YasEngine::createLogicalDevice()
{
	//TODO refactor whole procedure

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentationFamily};
	
	float queuePriority = 1.0f;

	for(int queueFamily: uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &physicalDeviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	if(enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &vulkanLogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to crate logical device");
	}

	vkGetDeviceQueue(vulkanLogicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(vulkanLogicalDevice, indices.presentationFamily, 0, &presentationQueue);
}

void YasEngine::createSurface()
{
	VkWin32SurfaceCreateInfoKHR	surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = application;
	surfaceCreateInfo.hwnd = window;

	VkResult result = vkCreateWin32SurfaceKHR(vulkanInstance, &surfaceCreateInfo, NULL, &surface);
	if(!(result == VK_SUCCESS))
	{
		throw std::runtime_error("Failed to create Vulkan surface!");
	}
}

void YasEngine::createSwapChain()
{
	QueueFamilyIndices queueIndices = findQueueFamilies(physicalDevice);
	vulkanSwapChain.createSwapChain(physicalDevice, surface, vulkanLogicalDevice, queueIndices, windowWidth, windowHeight);
}

void YasEngine::destroySwapChain()
{
	vulkanSwapChain.destroySwapChain(vulkanLogicalDevice);
}

void YasEngine::cleanUp()
{
	destroySwapChain();
	//vkDestroySwapchainKHR(vulkanLogicalDevice, swapChain, nullptr);
	vkDestroyDevice(vulkanLogicalDevice, nullptr);
	if(enableValidationLayers)
	{
		destroyDebugReportCallbackEXT(vulkanInstance, callback, nullptr);
	}
	vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
	//Second parameter is optional allocator which for this version will not be used.
	vkDestroyInstance(vulkanInstance, nullptr);
	DestroyWindow(window);
}

//-----------------------------------------------------------------------------|---------------------------------------|