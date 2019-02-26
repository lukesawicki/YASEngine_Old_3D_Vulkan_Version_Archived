#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP
#include"stdafx.hpp"
#include"VulkanDevice.hpp"
#include"VulkanInstance.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

class VulkanDevice {

	public:

										VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		bool							isPhysicalDeviceSuitable(VkPhysicalDevice& physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR& surface);
		void							selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface);
		void							createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		void							inforAboutDeviceAndDrivers();
		bool							isGraphicsQueueFamily(const VkQueueFlags& queueFlag);
		uint32_t						getGraphicQueue(VkPhysicalDevice&  physDevice);
		uint32_t						getPresentationQueue(VkPhysicalDevice&  physDevice, VkSurfaceKHR& surface);

		VkDevice						logicalDevice;
		VkPhysicalDevice				physicalDevice = VK_NULL_HANDLE;
		VkSampleCountFlagBits			msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		std::map<int, std::string>		vendors {
			{4098, "AMD"},
			{4112, "ImgTec"},
			{4318, "NVIDIA"},
			{5045, "ARM"},
			{20803, "Qualcomm"},
			{32902, "INTEL"}
		};

	VkQueue									queue;
	uint32_t								queueFamilyCount;

	private:

		bool								isGraphicsFamilyTheSameAsPresentation;
		bool								isGraphicsFamilyTheSameAsTransfer;
		bool								isGraphicsFamilyTheSameAsPresentationAndTransfer;
};

#endif
