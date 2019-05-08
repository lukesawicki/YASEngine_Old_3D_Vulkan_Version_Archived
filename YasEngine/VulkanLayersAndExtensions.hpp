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

        // Set by programmer. Which validation wanted to use.
		std::vector<const char*>		requestedValidationLayers;

        // Set by programmer. Which instance extensions wanted to use.
		std::vector<const char*>		requestedInstanceExtensions;

        // Set by programmer. Which device extensions wanted to use. Device extensions are deprecated.
		std::vector<const char*>		requestedDeviceExtensions;

	private:		
};

#endif
