#ifndef VULKANSWAPCHAIN_HPP
#define VULKANSWAPCHAIN_HPP
#include"stdafx.hpp"

#undef min
#undef max

class VulkanSwapChain
{
	public:

		static SwapChainSupportDetails	querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);		

		void					createSwapChain(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkDevice& vulkanLogicalDevice, QueueFamilyIndices& queueIndices, int windowWidth, int windowHeight);
		void					destroySwapChain(VkDevice vulkanLogicalDevice);
		void createImageViews(VkDevice& device);
		VkFormat				swapChainImageFormat;
		VkExtent2D				swapChainExtent;
		VkSwapchainKHR			swapChain;
		std::vector<VkImage>	swapChainImages;
		std::vector<VkImageView>	swapChainImageViews;

	private:
		VkSurfaceFormatKHR		chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
		VkPresentModeKHR		chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D				chooseSwapExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities, int windowWidth, int windowHeight);

		
};

#endif