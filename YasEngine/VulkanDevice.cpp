#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanSwapchain.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

//static method
bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR surface) {

	uint32_t graphicQueue = -1; 
	graphicQueue = getGraphicQueue(physDevice);
	uint32_t presentationFamilyQueueIndex = -1;
	presentationFamilyQueueIndex = getPresentationQueue(physDevice, surface);
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physDevice, &physicalDeviceProperties);
	VkPhysicalDeviceFeatures physicalDeviceSupportedFeatures;
	vkGetPhysicalDeviceFeatures(physDevice, &physicalDeviceSupportedFeatures);

	if(physicalDeviceProperties.vendorID == 4130) // to reformatt lukesawicki
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
	bool extensionsSupported = vulkanInstance.layersAndExtensions->CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(physDevice);
	bool swapchainSuitable = false; //lukesawicki uwaga zahardkodowane 20190513
	
	if(extensionsSupported)
	{
        swapchainSuitable = VulkanSwapchain::isSwapchainAdequate(physDevice, surface);
	}

	return (graphicQueue>=0) && (presentationFamilyQueueIndex>=0) && extensionsSupported && swapchainSuitable && physicalDeviceSupportedFeatures.samplerAnisotropy;
}

VulkanDevice::VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	selectPhysicalDevice(vulkanInstance, surface);
	createLogicalDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
    msaaSamples = getMaxUsableSampleCount();
	informationAboutDeviceAndDrivers();
}

void VulkanDevice::selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface)
{
	uint32_t deviceCount = 0;
    // This function in this call retrieve number of available Physical Devices (graphics cards)
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);

	if(deviceCount == 0)
	{
		throw std::runtime_error("Failed to find Graphics Cards with Vulkan support.");
	}

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    // This function in this call retrieve available Physical Devices (graphics cards)
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, physicalDevices.data());

	for(const VkPhysicalDevice& device: physicalDevices)
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

void VulkanDevice::createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	//std::set<uint32_t> queueFamilies;
	//queueFamilies.push_back(getGraphicQueue(physicalDevice));
	//queueFamilies.push_back(getPresentationQueue(physicalDevice, surface));//Here using physicalDevice because it was created in createPhysicalDevice
    std::set<uint32_t> uniqueQueueFamilies = {getGraphicQueue(physicalDevice), getPresentationQueue(physicalDevice, surface)};
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

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &physicalDeviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanInstance.layersAndExtensions->requestedDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = vulkanInstance.layersAndExtensions->requestedDeviceExtensions.data();
	
	if(enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanInstance.layersAndExtensions->requestedValidationLayers.size()); //->validationLayers.size());
		createInfo.ppEnabledLayerNames = vulkanInstance.layersAndExtensions->requestedValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

    // This function create logical representation(VkDevice) of the physical device in the application space.
	if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device.");
	}

    // this fuction Acuire compatible queue in this case graphic queue;
    auto it = uniqueQueueFamilies.begin();
    
	vkGetDeviceQueue(logicalDevice, *it, 0, &graphicsQueue);
    // this fuction Acuire compatible queue in this case presentationQueue
    it++;
	vkGetDeviceQueue(logicalDevice, *it, 0, &presentationQueue);
}

void VulkanDevice::informationAboutDeviceAndDrivers()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
    
    // this function retrieving information about physical device and store them in VkPhysicalDeviceProperties
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

    //This function - vkGetPhysicalDeviceQueueFamilyProperties
    //retrieve data with informations about queue family which are
    // type of operations that are supported and number of queues that can be created based on that family
    //In in this call function retriev number of queue families
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamiliesPropertiesCount);

    //In this call function retriev all data abour each queue family -> VkQueueFamilyProperties
    // queueFlags thies field indicates which family it is
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, queueFamiliesProperties.data());

	int result = -1;
	for (unsigned int i = 0; i < queueFamiliesPropertiesCount; i++) {
		if (isGraphicsQueueFamily(queueFamiliesProperties[i].queueFlags)) {
            foundGraphicQueueFamily = i;
			return i;
		}
	}
	return result;
}

uint32_t VulkanDevice::getPresentationQueue(VkPhysicalDevice  physDevice, VkSurfaceKHR surface)
{
	uint32_t queueFamiliesPropertiesCount = 0;

    // This function - vkGetPhysicalDeviceQueueFamilyProperties
    // retrieve data with informations about queue family which are
    // type of operations that are supported and number of queues that can be created based on that family
    // In in this call function retriev number of queue families
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamiliesPropertiesCount);

    //In this call function retriev all data abour each queue family -> VkQueueFamilyProperties
    //queueFlags thies field indicates which family it is
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamiliesPropertiesCount, queueFamiliesProperties.data());

	VkBool32 presentationFamilySupport = false;
	int result = -1;
	for (unsigned int i = 0; i < queueFamiliesPropertiesCount; i++)
	{
		presentationFamilySupport = false;
        // This function determine whether a queue family of physical device supports presentation to given surface
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentationFamilySupport);
		if (presentationFamilySupport && i!=foundGraphicQueueFamily)
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