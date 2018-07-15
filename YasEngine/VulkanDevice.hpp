#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP
#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VulkanInstance.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

class VulkanDevice
{
	public:
		VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		VkDevice						logicalDevice;
		VkPhysicalDevice				physicalDevice = VK_NULL_HANDLE;

		static bool						isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR surface);

		void							selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface);
		void							createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface,
											VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);


	private:

};

#endif
