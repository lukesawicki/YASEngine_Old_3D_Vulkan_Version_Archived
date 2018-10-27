#ifndef VULKANQUEUES_HPP
#define VULKANQUEUES_HPP

class VulkanQueues
{
	public:
		/*VkQueue									queue;
		std::vector<VkQueueFamilyProperties>	queueFamilyProperties;
		uint32_t								graphicsFamilyQueueIndex=UINT32_MAX;
		uint32_t								presentationFamilyQueueIndex=UINT32_MAX;
		uint32_t								transferFamilyQueueIndex=UINT32_MAX;
		uint32_t								graphicsAndPresentationFamilyQueueIndex=UINT32_MAX;
		uint32_t								queueFamilyCount;

		bool									isGraphicsFamilyTheSameAsPresentation;
		bool									isGraphicsFamilyTheSameAsTransfer;
		bool									isGraphicsFamilyTheSameAsPresentationAndTransfer;

		
										VulkanDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		bool							isPhysicalDeviceSuitable(VkPhysicalDevice physDevice, VulkanInstance& vulkanInstance, VkSurfaceKHR& surface);
		void							selectPhysicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface);
		void							createLogicalDevice(VulkanInstance& vulkanInstance, VkSurfaceKHR& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue, bool enableValidationLayers);
		VkSampleCountFlagBits			getMaxUsableSampleCount();
		void							inforAboutDeviceAndDrivers();
		void							retrieveQueueFamilies(VkPhysicalDevice physDevice);
		void							retrieveGrahicsQueue();
		void							retrievePresentationQueue(VkPhysicalDevice physDevice, VkSurfaceKHR& surface);
		void							retrieveTransferQueue();
		bool							deviceSupportsAllrequiredQueuesFamily();
		bool							isGraphicsAndTransferAndPresentationFamily();
		bool							isGraphicsAndPresentationFamily();
		bool							isGraphicsQueueFamily(VkQueueFlags &queueFlag);
		bool							isPresentationQueueFamily(VkPhysicalDevice physDevice, uint32_t queueFamilyIndex, VkSurfaceKHR& surface);
		bool							isTransferQueueFamily(VkQueueFlags &queueFlag);

		VkDevice						logicalDevice;
		VkPhysicalDevice				physicalDevice = VK_NULL_HANDLE;
		VkSampleCountFlagBits			msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		std::map<int, std::string> vendors
		{
			{4098, "AMD"},
			{4112, "ImgTec"},
			{4318, "NVIDIA"},
			{5045, "ARM"},
			{20803, "Qualcomm"},
			{32902, "INTEL"}
		};*/
};

#endif

