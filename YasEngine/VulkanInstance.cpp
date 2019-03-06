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

	std::cout << "Creating Vulkan Instance..." << std::endl;
	VkApplicationInfo applicationInfo = {}; 
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "YasEngine Demo";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "Yas Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_1; //?

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	bool allExtensionsAvailable = layersAndExtensions->CheckIfAllRequestedInstanceExtensionAreSupported();

	if(!allExtensionsAvailable)
	{
		throw std::runtime_error("Not all required extensions available! Can't create Vulkan Instance");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(layersAndExtensions->instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = layersAndExtensions->instanceExtensions.data();

	if(areValidationLayersEnabled)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(layersAndExtensions->validationLayers.size());
		createInfo.ppEnabledLayerNames = layersAndExtensions->validationLayers.data();
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