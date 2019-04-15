#ifndef VULKANINSTANCE_HPP
#define VULKANINSTANCE_HPP
#include"stdafx.hpp"
#include"VulkanLayersAndExtensions.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

class VulkanInstance
{
	public:
										VulkanInstance();
		void							createVulkanInstance(bool areValidationLayersEnabled);
		// Object which represents connection between application and Vulkan runtime. One instance on whole application.
		VkInstance						instance;
        
        // Class containing layers and extensions
		VulkanLayersAndExtensions*		layersAndExtensions;

	private:
};

#endif
