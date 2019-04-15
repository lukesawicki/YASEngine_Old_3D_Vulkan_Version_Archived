#include"stdafx.hpp"
#include"VulkanInstance.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

VulkanInstance::VulkanInstance()
{
	layersAndExtensions = new VulkanLayersAndExtensions();
}

void VulkanInstance::createVulkanInstance(bool areValidationLayersEnabled)
{
	if(areValidationLayersEnabled && !layersAndExtensions->CheckIfAllRequestedLayersAreSupported())
	{
		throw std::runtime_error("Requested validation layers are not available");
	}	

    // Structure required by VkInstance. It contains information about this application
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "YasEngine Demo";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
	applicationInfo.pEngineName = "Yas Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_1;


    // Structure which contains VkApplicationInfo and other information required for creating VkInstance
    // and used among others to enable instance-specific layers
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	if(!layersAndExtensions->CheckIfAllRequestedInstanceExtensionAreSupported())
	{
		throw std::runtime_error("Not all required extensions available! Can't create Vulkan Instance");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(layersAndExtensions->requestedInstanceExtensions.size());
	createInfo.ppEnabledExtensionNames = layersAndExtensions->requestedInstanceExtensions.data();

	if(areValidationLayersEnabled)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(layersAndExtensions->requestedValidationLayers.size());
		createInfo.ppEnabledLayerNames = layersAndExtensions->requestedValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan instance!");
	}
}