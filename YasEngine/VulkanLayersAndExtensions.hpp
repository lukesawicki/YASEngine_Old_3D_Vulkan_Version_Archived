#ifndef VULKANLAYERSANDEXTENSIONS_HPP
#define VULKANLAYERSANDEXTENSIONS_HPP
#include"stdafx.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

class VulkanLayersAndExtensions
{
	public:

		VulkanLayersAndExtensions();
		bool							CheckIfAllRequestedLayersAreSupported();
		bool							CheckIfAllRequestedInstanceExtensionAreSupported();
		bool							CheckIfAllRequestedPhysicalDeviceExtensionAreSupported(VkPhysicalDevice device);

		std::vector<const char*>		validationLayers;
		std::vector<const char*>		instanceExtensions;
		std::vector<const char*>		deviceExtensions;

	private:		
};

#endif
