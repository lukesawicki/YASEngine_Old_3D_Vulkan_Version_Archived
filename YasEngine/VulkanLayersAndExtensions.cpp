#include"stdafx.hpp"
#include"VulkanLayersAndExtensions.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

VulkanLayersAndExtensions::VulkanLayersAndExtensions() {
	
	validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

bool VulkanLayersAndExtensions::CheckIfAllRequestedInstanceExtensionAreSupported() {

	bool allEnabletExtensionsAreAvailable = false;
	uint32_t numberOfAvailableExtensions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(numberOfAvailableExtensions);
	vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, availableExtensions.data());
	int extensionsCounter = 0;

	for(size_t i=0; i<instanceExtensions.size(); i++) {
		for(int j=0; j<static_cast<int>(availableExtensions.size()); j++) {
			if(strcmp(instanceExtensions[i], availableExtensions[j].extensionName) == 0) {
				++extensionsCounter;
			}
			if(extensionsCounter == instanceExtensions.size()) {
				return true;
			}
		}
	}
	
	return false;
}

bool VulkanLayersAndExtensions::CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(VkPhysicalDevice device) {

	uint32_t extensionsCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());
	
	for(const char* deviceExtensionName: deviceExtensions) {
		bool extensionFound = false;

		for(const VkExtensionProperties& availableExt: availableExtensions) {
			if(strcmp(deviceExtensionName, availableExt.extensionName) == 0) {
				extensionFound = true;
				break;
			}
		}
		if(!extensionFound) {
			return false;
		}
	}
	return true;
}

bool VulkanLayersAndExtensions::CheckIfAllRequestedLayersAreSupported() {

	uint32_t instanceLayersCount;
	vkEnumerateInstanceLayerProperties(&instanceLayersCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(instanceLayersCount);
	vkEnumerateInstanceLayerProperties(&instanceLayersCount, availableLayers.data());
	
	for(const char* layerName: validationLayers) {
		bool layerFound = false;
		for(const VkLayerProperties& layerProperties: availableLayers) {
			if(strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if(!layerFound) {
			return false;
		}
	}
	return true;
}
