#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanSwapchain.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

//static method
bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR surface)
{
	uint32_t graphicQueue = -1; 
	graphicQueue = getGraphicQueue(physDevice);
	uint32_t presentationFamilyQueueIndex = -1;
	presentationFamilyQueueIndex = getPresentationQueue(physDevice, surface);
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physDevice, &physicalDeviceProperties);
	VkPhysicalDeviceFeatures physicalDeviceSupportedFeatures;
	vkGetPhysicalDeviceFeatures(physDevice, &physicalDeviceSupportedFeatures);

	bool extensionsSupported = vulkanInstance.layersAndExtensions->CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(physDevice);
	bool swapchainSuitable = false;

	if(extensionsSupported)
	{
		//SwapchainSupportDetails swapchainSupport = VulkanSwapchain::querySwapchainSupport(physDevice, surface);
		swapchainSuitable = !VulkanSwapchain::getSwapchainSurfaceFormats(physDevice, surface).empty() && !VulkanSwapchain::getSwapchainPresentModes(physDevice, surface).empty();
		//swapchainSuitable = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	return (graphicQueue>=0) && (presentationFamilyQueueIndex>=0) && extensionsSupported && swapchainSuitable && physicalDeviceSupportedFeatures.samplerAnisotropy;
}

VulkanDevice::VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	selectPhysicalDevice(vulkanInstance, surface);
	createLogicalDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
	inforAboutDeviceAndDrivers();
}

void VulkanDevice::selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR surface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);

	if(deviceCount == 0)
	{
		throw std::runtime_error("Failed to find Graphics Cards with Vulkan support.");
	}

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, physicalDevices.data());

	for(VkPhysicalDevice device: physicalDevices)
	{
		if(isPhysicalDeviceSuitable(device, vulkanInstance, surface))
		{
			physicalDevice = device;
			break;
		}
	}

	if(physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find suitable graphic card");
	}
}

void VulkanDevice::createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<uint32_t> queueFamilies;
	queueFamilies.push_back(getGraphicQueue(physicalDevice));
	queueFamilies.push_back(getPresentationQueue(physicalDevice, surface));//Here using physicalDevice because it was created in createPhysicalDevice
	float queuePriority = 1.0F;

	for(uint32_t queueFamily: queueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &physicalDeviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanInstance.layersAndExtensions->deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = vulkanInstance.layersAndExtensions->deviceExtensions.data();
	
	if(enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanInstance.layersAndExtensions->validationLayers.size());
		createInfo.ppEnabledLayerNames = vulkanInstance.layersAndExtensions->validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device.");
	}

	vkGetDeviceQueue(logicalDevice, queueFamilies[0], 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilies[1], 0, &presentationQueue);
}

void VulkanDevice::inforAboutDeviceAndDrivers()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	std::cout << "Chosen physical device properties: " << std::endl;
	std::cout << "Name:\t" << physicalDeviceProperties.deviceName << std::endl;
	std::cout << "Type:\t" << physicalDeviceProperties.deviceType << std::endl;
	std::cout << "ID\t" << physicalDeviceProperties.deviceID << std::endl;
	std::cout << "Vendor" << vendors.at(physicalDeviceProperties.vendorID) << std::endl;
}

uint32_t VulkanDevice::getGraphicQueue(VkPhysicalDevice physDevice)
{
	uint32_t queueFamiliesPropertiesCount;// = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamiliesPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, queueFamiliesProperties.data());

	int result = -1;
	for (unsigned int i = 0; i < queueFamiliesPropertiesCount; i++) {
		if (isGraphicsQueueFamily(queueFamiliesProperties[i].queueFlags)) {
			return i;
		}
	}
	return result;
}

uint32_t VulkanDevice::getPresentationQueue(VkPhysicalDevice  physDevice, VkSurfaceKHR surface)
{
	uint32_t queueFamiliesPropertiesCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamiliesPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, queueFamiliesProperties.data());

	VkBool32 presentationFamilySupport = false;
	int result = -1;
	for (unsigned int i = 0; i < queueFamiliesPropertiesCount; i++)
	{
		presentationFamilySupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentationFamilySupport);
		if (presentationFamilySupport)
		{
			return i;
		}
	}
	return result = -1;
}

bool VulkanDevice::isGraphicsQueueFamily(const VkQueueFlags& queueFlag)
{
	return queueFlag & VK_QUEUE_GRAPHICS_BIT;
}