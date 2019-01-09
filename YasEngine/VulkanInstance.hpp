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
		
		VkInstance						instance;
		VulkanLayersAndExtensions*		layersAndExtensions;

	private:
};

#endif
