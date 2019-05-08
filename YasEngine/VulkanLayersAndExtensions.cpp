#include"stdafx.hpp"
#include"VulkanLayersAndExtensions.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

VulkanLayersAndExtensions::VulkanLayersAndExtensions()
{	// Adding layers and extensions requested by user for later use
	requestedValidationLayers.push_back("VK_LAYER_LUNARG_standard_validation");

	requestedDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	requestedInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	requestedInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	requestedInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

bool VulkanLayersAndExtensions::CheckIfAllRequestedInstanceExtensionAreSupported()
{
	bool allEnabledExtensionsAreAvailable = false;
	uint32_t numberOfAvailableExtensions = 0;
    // Retrieve Instance Extensions Porperties(number of them)
    // Extensions may be new data types, and new behaviour of the Vulkan API
    // First call retrieve number of extensions properties
	vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(numberOfAvailableExtensions);
	// Second call retrieve extensions properties
    vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, availableExtensions.data());
	int extensionsCounter = 0;

	for(size_t i=0; i<requestedInstanceExtensions.size(); i++)
	{
		for(int j=0; j<static_cast<int>(availableExtensions.size()); j++)
		{
			if(strcmp(requestedInstanceExtensions[i], availableExtensions[j].extensionName) == 0)
			{
				++extensionsCounter;
			}
			if(extensionsCounter == requestedInstanceExtensions.size())
			{
				return true;
			}
		}
	}
	
	return false;
}

bool VulkanLayersAndExtensions::CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(VkPhysicalDevice device)
{
	uint32_t extensionsCount;
    // Retrieving extensions for all retrieved instance layer property for each physical device
    // first call retrieve number of extensions
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);
	
    std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
    
    // second call retrieve extensions properties
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());
	
	for(const char* deviceExtensionName: requestedDeviceExtensions)
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

bool VulkanLayersAndExtensions::CheckIfAllRequestedLayersAreSupported()
{
	uint32_t instanceLayersCount;
    // It plugguble components that can be dynamically pulled in by the Vulkan loader at runtime.
    // mostly used for debugging and validation.. Layers have vary number of extensions each.
	vkEnumerateInstanceLayerProperties(&instanceLayersCount, nullptr);
	
    std::vector<VkLayerProperties> availableLayers(instanceLayersCount);
    // First call was retrieving the number of layers and this call retrieve layers.
	vkEnumerateInstanceLayerProperties(&instanceLayersCount, availableLayers.data());
	

	for(const char* layerName: requestedValidationLayers)
	{
		bool layerFound = false;
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
