#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanSwapchain.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

//static method
bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR& surface)
{
	//QueueFamilyIndices indices = findQueueFamilies(physDevice, surface);
	//VkQueueFlags &queueFlag
	retrieveQueueFamilies(physDevice);
	retrieveGrahicsQueue();
	retrievePresentationQueue(physDevice, surface);

//graphicsAndPresentationFamilyQueueIndex
	
	retrieveTransferQueue();
	
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physDevice, &physicalDeviceProperties);
	VkPhysicalDeviceFeatures physicalDeviceSupportedFeatures;
	vkGetPhysicalDeviceFeatures(physDevice, &physicalDeviceSupportedFeatures);

	bool extensionsSupported = vulkanInstance.layersAndExtensions->CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(physDevice);
	bool swapchainSuitable = false;
	
	if(extensionsSupported)
	{
		SwapchainSupportDetails swapchainSupport = VulkanSwapchain::querySwapchainSupport(physDevice, surface);
		swapchainSuitable = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	return deviceSupportsAllrequiredQueuesFamily() && extensionsSupported && swapchainSuitable && physicalDeviceSupportedFeatures.samplerAnisotropy;
}

VulkanDevice::VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	selectPhysicalDevice(vulkanInstance, surface);
	createLogicalDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
	inforAboutDeviceAndDrivers();
}

void VulkanDevice::selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);

	if(deviceCount == 0)
	{
		throw std::runtime_error("Failed to find Graphics Cards with Vulkan support.");
	}

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, physicalDevices.data());

	for(const VkPhysicalDevice& device: physicalDevices)
	{
		if(isPhysicalDeviceSuitable(device, vulkanInstance, surface))
		{
			physicalDevice = device;
			msaaSamples = getMaxUsableSampleCount();
			break;
		}
	}

	if(physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find suitable graphic card");
	}
}

void VulkanDevice::createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	//QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	//std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentationFamily};
	std::set<uint32_t> uniqueQueueFamilies = {graphicsFamilyQueueIndex, presentationFamilyQueueIndex};
	float queuePriority = 1.0F;

	for(uint32_t queueFamily: uniqueQueueFamilies)
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
//lukesawicki 2018-09-06 0744
	//physicalDeviceFeatures.sampleRateShading = VK_FALSE;

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
//tutaj to powinnno chyba inczaej wygladac bo gdzie niby trafia to w graphicQueue
	vkGetDeviceQueue(logicalDevice, graphicsFamilyQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, presentationFamilyQueueIndex, 0, &presentationQueue);
}

VkSampleCountFlagBits VulkanDevice::getMaxUsableSampleCount()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);

	if(counts & VK_SAMPLE_COUNT_64_BIT)
	{
		return VK_SAMPLE_COUNT_64_BIT;
	}
	if(counts & VK_SAMPLE_COUNT_32_BIT)
	{
		return VK_SAMPLE_COUNT_32_BIT;
	}
	if(counts & VK_SAMPLE_COUNT_16_BIT)
	{
		return VK_SAMPLE_COUNT_16_BIT;
	}
	if(counts & VK_SAMPLE_COUNT_8_BIT)
	{
		return VK_SAMPLE_COUNT_8_BIT;
	}
	if(counts & VK_SAMPLE_COUNT_4_BIT)
	{
		return VK_SAMPLE_COUNT_4_BIT;
	}
	if(counts & VK_SAMPLE_COUNT_2_BIT)
	{
		return VK_SAMPLE_COUNT_2_BIT;
	}
	return VK_SAMPLE_COUNT_1_BIT;
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

void VulkanDevice::retrieveGrahicsQueue()
{
	bool found = false;
	for (unsigned int i = 0; i < queueFamilyCount; i++){
		if (isGraphicsQueueFamily(queueFamilyProperties[i].queueFlags)){
			found = true;
			graphicsFamilyQueueIndex = i;
			break;
		}
	}
	if(!found)
	{
		throw std::runtime_error("Failed to found graphic family queue.");
	}
}

void VulkanDevice::retrievePresentationQueue(VkPhysicalDevice physDevice, VkSurfaceKHR& surface)
{
	bool found = false;
	for(unsigned int i=0; i<queueFamilyCount; i++)
	{
		if(isPresentationQueueFamily(physDevice, i, surface))
		{
			found = true;
			presentationFamilyQueueIndex = 1;
			break;
		}
	}
	if(!found)
	{
		throw std::runtime_error("Failed to found presentation family queue.");
	}
}

void VulkanDevice::retrieveTransferQueue()
{
	bool found = false;
	for (unsigned int i = 0; i < queueFamilyCount; i++){
		if (isTransferQueueFamily(queueFamilyProperties[i].queueFlags)){
			found = true;
			transferFamilyQueueIndex = i;
			break;
		}
	}
	if(!found)
	{
		throw std::runtime_error("Failed to found transfer queue family.");
	}
}

bool VulkanDevice::deviceSupportsAllrequiredQueuesFamily()
{
	return ( (graphicsFamilyQueueIndex != UINT32_MAX && presentationFamilyQueueIndex != UINT32_MAX && transferFamilyQueueIndex != UINT32_MAX) || graphicsAndPresentationFamilyQueueIndex!=UINT32_MAX );
}

//void VulkanDevice::retrievePresentationQueue(VkSurfaceKHR& surface)
//{
//	for(unsigned int i=0; i<queueFamilyCount; i++)
//	{
//		if(isPresentationQueueFamily(i, surface))
//		{
//			presentationFamilyQueueIndex = 1;
//		}
//	}
//}

void VulkanDevice::retrieveQueueFamilies(VkPhysicalDevice physDevice)
{
//lukesawicki
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, NULL);
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilyProperties.data());
}

bool VulkanDevice::isPresentationQueueFamily(VkPhysicalDevice physDevice, uint32_t queueFamilyIndex, VkSurfaceKHR& surface)
{
		//lukesawicki //vkGetPhysicalDeviceSurfaceSupportKHR
		VkBool32 presentationFamilySupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, queueFamilyIndex, surface, &presentationFamilySupport);
		return presentationFamilySupport;
}

bool VulkanDevice::isTransferQueueFamily(VkQueueFlags & queueFlag)
{
	return queueFlag & VK_QUEUE_TRANSFER_BIT;
}

bool VulkanDevice::isGraphicsAndTransferAndPresentationFamily()
{
	return graphicsFamilyQueueIndex == transferFamilyQueueIndex && graphicsFamilyQueueIndex == presentationFamilyQueueIndex;
}

bool VulkanDevice::isGraphicsQueueFamily(VkQueueFlags &queueFlag)
{
		return queueFlag & VK_QUEUE_GRAPHICS_BIT;
}

bool VulkanDevice::isGraphicsAndPresentationFamily()
{
	return graphicsFamilyQueueIndex == presentationFamilyQueueIndex;
}