#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP
#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VulkanInstance.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

class VulkanDevice {

	public:

		VkDevice						logicalDevice;
		VkPhysicalDevice				physicalDevice = VK_NULL_HANDLE;
		VkSampleCountFlagBits			msaaSamples = VK_SAMPLE_COUNT_1_BIT;
										VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		static bool						isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR surface);
		void							selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface);
		void							createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		VkSampleCountFlagBits			getMaxUsableSampleCount();

	private:
};

#endif
