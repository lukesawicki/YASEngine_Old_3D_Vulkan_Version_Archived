#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanSwapchain.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

//static method
bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR surface)
{	
	QueueFamilyIndices indices = findQueueFamilies(physDevice, surface);

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physDevice, &physicalDeviceProperties);

	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(physDevice, &physicalDeviceFeatures);
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

	bool extensionsSupported = vulkanInstance.layersAndExtensions->CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(physDevice);
	std::cout << "extensionSupported= " << extensionsSupported << std::endl;
	bool swapchainSuitable = false;
	
	if(extensionsSupported)
	{
		SwapchainSupportDetails swapchainSupport = VulkanSwapchain::querySwapchainSupport(physDevice, surface);
		swapchainSuitable = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
		std::cout <<"swapchainSuitable= " << swapchainSuitable << std::endl;
	}
	std::cout << "Before return in isPhysicalDeviceSuitable(VkPhysicalDevice device) " << std::endl;
	return indices.isComplete() && extensionsSupported && swapchainSuitable;
}

VulkanDevice::VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
		selectPhysicalDevice(vulkanInstance, surface);
		createLogicalDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
}

void VulkanDevice::selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);
	if(deviceCount == 0) {
		throw std::runtime_error("Failed to find Graphics Cards with Vulkan support.");
	}
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, physicalDevices.data());

	for(const VkPhysicalDevice& device: physicalDevices)
	{
		if(isPhysicalDeviceSuitable(device, vulkanInstance, surface))
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

void VulkanDevice::createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers)
{
	//TODO refactor whole procedure

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	
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

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentationFamily, 0, &presentationQueue);
}