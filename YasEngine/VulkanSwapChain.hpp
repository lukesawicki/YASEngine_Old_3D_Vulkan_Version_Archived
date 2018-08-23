#ifndef VULKANSWAPCHAIN_HPP
#define VULKANSWAPCHAIN_HPP
#include"stdafx.hpp"
#include"VariousTools.hpp"
#undef min
#undef max

//-----------------------------------------------------------------------------|---------------------------------------|

class VulkanSwapchain {

	public:

		void							createSwapchain(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkDevice& vulkanLogicalDevice, QueueFamilyIndices& queueIndices, HWND& window);
		static SwapchainSupportDetails	querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);		
		void							destroySwapchain(VkDevice vulkanLogicalDevice);
		void							createImageViews(VkDevice& device, int32_t mipLevelsNumber);

		VkFormat						swapchainImageFormat;
		VkExtent2D						swapchainExtent;
		VkSwapchainKHR					swapchain;
		std::vector<VkImage>			swapchainImages;
		std::vector<VkImageView>		swapchainImageViews;

	private:

		VkSurfaceFormatKHR				chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
		VkPresentModeKHR				chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D						chooseSwapExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities, HWND& window);
};

#endif
