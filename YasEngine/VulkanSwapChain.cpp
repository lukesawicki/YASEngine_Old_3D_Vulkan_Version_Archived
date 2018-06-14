#include"stdafx.hpp"
#include"VulkanSwapChain.hpp"

SwapChainSupportDetails	VulkanSwapChain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails swapChainDetails;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	
	if(formatCount != 0)
	{
		swapChainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
	}

	uint32_t presentModesCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);

	if(presentModesCount != 0)
	{
		swapChainDetails.presentModes.resize(presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, swapChainDetails.presentModes.data());
	}

	return swapChainDetails;
}


VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	for(const VkSurfaceFormatKHR& availableFormat: availableFormats)
	{
		//This format results in more accurate perceived colors
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	//Tke first in the row.
	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	//To avoid tearing it is goode idea to choose tripple buffering
	for(const VkPresentModeKHR& availablePresentMode: availablePresentModes)
	{
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else
		{
			if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				chosenPresentMode = availablePresentMode;
			}	
		}
	}
	//VK_PRESENT_MODE_FIFO_KHR is mode that is guaranteed to be available
	return chosenPresentMode;
}

VkExtent2D	VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities, int windowWidth, int windowHeight)
{
	if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{		
		VkExtent2D actualExtent = {static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight)};
		actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.width = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);		
		return actualExtent;
	}
}

void VulkanSwapChain::createSwapChain(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkDevice& vulkanLogicalDevice, QueueFamilyIndices& queueIndices, int windowWidth, int windowHeight)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, windowWidth, windowHeight);
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = {(uint32_t)queueIndices.graphicsFamily, (uint32_t)queueIndices.presentationFamily};

	if(queueIndices.graphicsFamily != queueIndices.presentationFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if(vkCreateSwapchainKHR(vulkanLogicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(vulkanLogicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkanLogicalDevice, swapChain, &imageCount, swapChainImages.data());
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void VulkanSwapChain::destroySwapChain(VkDevice vulkanLogicalDevice)
{
	vkDestroySwapchainKHR(vulkanLogicalDevice, swapChain, nullptr);
}
