#ifndef VULKANINSTANCE_HPP
#define VULKANINSTANCE_HPP
#include"stdafx.hpp"
#include"YasLog.hpp"
#include"VulkanLayersAndExtensions.hpp"

class VulkanInstance
{
	public:
		VulkanInstance();
		void createVulkanInstance(bool areValidationLayersEnabled);
		
		// Vulkan types
		VkInstance						instance;

		// YasEngine types
		VulkanLayersAndExtensions*		layersAndExtensions;
	private:
		//...
};

#endif