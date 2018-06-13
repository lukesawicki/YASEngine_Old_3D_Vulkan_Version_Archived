#include"stdafx.hpp"
#include"YasEngine.hpp"
int YasEngine::windowPositionX				= 64;
int YasEngine::windowPositionY				= 64;
int YasEngine::windowWidth					= 640;
int YasEngine::windowHeight					= 480;

struct YasEngine::QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentationFamily = -1;
	
	bool isComplete()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

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

SwapChainSupportDetails	YasEngine::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails swapChainDetails;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	
	if(formatCount != 0)
	{
		swapChainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
	}

	uint32_t presentModesCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);

	if(presentModesCount != 0)
	{
		swapChainDetails.presentModes.resize(presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, swapChainDetails.presentModes.data());
	}

	return swapChainDetails;
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
	YasEngine::QueueFamilyIndices indices = findQueueFamilies(device);

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
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainSuitable = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainSuitable;
}

YasEngine::QueueFamilyIndices YasEngine::findQueueFamilies(VkPhysicalDevice device)
{
	YasEngine::QueueFamilyIndices queueFamilyIndices;

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

VkSurfaceFormatKHR YasEngine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	for(const VkSurfaceFormatKHR& availableFormat: availableFormats)
	{
		//This format results in more accurate perceived colors
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	//Tke first in the row.
	return availableFormats[0];
}

VkPresentModeKHR YasEngine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	//To avoid tearing it is goode idea to choose tripple buffering
	for(const VkPresentModeKHR& availablePresentMode: availablePresentModes)
	{
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else
		{
			if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				chosenPresentMode = availablePresentMode;
			}	
		}
	}
	//VK_PRESENT_MODE_FIFO_KHR is mode that is guaranteed to be available
	return chosenPresentMode;
}

VkExtent2D	YasEngine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities)
{
	if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{		
		VkExtent2D actualExtent = {static_cast<uint32_t>(YasEngine::windowWidth), static_cast<uint32_t>(YasEngine::windowHeight)};
		actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.width = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);		
		return actualExtent;
	}
}

void YasEngine::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices queueIndices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = {(uint32_t)queueIndices.graphicsFamily, (uint32_t)queueIndices.presentationFamily};

	if(queueIndices.graphicsFamily != queueIndices.presentationFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if(vkCreateSwapchainKHR(vulkanLogicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(vulkanLogicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkanLogicalDevice, swapChain, &imageCount, swapChainImages.data());
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void YasEngine::cleanUp()
{
	vkDestroySwapchainKHR(vulkanLogicalDevice, swapChain, nullptr);
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