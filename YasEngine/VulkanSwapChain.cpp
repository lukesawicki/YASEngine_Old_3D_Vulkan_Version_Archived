#include"stdafx.hpp"
#include"VulkanSwapchain.hpp"
#include"VariousTools.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|



SwapchainSupportDetails	VulkanSwapchain::querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {

	SwapchainSupportDetails swapchainDetails;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainDetails.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	
	if(formatCount != 0) {
		swapchainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapchainDetails.formats.data());
	}

	uint32_t presentModesCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);

	if(presentModesCount != 0) {
		swapchainDetails.presentModes.resize(presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, swapchainDetails.presentModes.data());
	}

	return swapchainDetails;
}


VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {

	for(const VkSurfaceFormatKHR& availableFormat: availableFormats) {
		//This format results in more accurate perceived colors
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	//Tke first in the row.
	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
	VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	//To avoid tearing it is goode idea to choose tripple buffering
	for(const VkPresentModeKHR& availablePresentMode: availablePresentModes) {
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		} else {
			if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				chosenPresentMode = availablePresentMode;
			}	
		}
	}
	//VK_PRESENT_MODE_FIFO_KHR is mode that is guaranteed to be available
	return chosenPresentMode;
}

VkExtent2D	VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities, HWND& window) {

	if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())	{
		return surfaceCapabilities.currentExtent;
	} else {		

		RECT windowClientRect;
		GetClientRect(window,&windowClientRect);
		VkExtent2D actualExtent = {static_cast<uint32_t>(windowClientRect.right - 1), static_cast<uint32_t>(windowClientRect.bottom - 1)};
		actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);		
		return actualExtent;
	}
}

void VulkanSwapchain::createSwapchain(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkDevice& vulkanLogicalDevice, QueueFamilyIndices& queueIndices, HWND& window) {

	SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities, window);
	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
	if(swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
		imageCount = swapchainSupport.capabilities.maxImageCount;
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

	if(queueIndices.graphicsFamily != queueIndices.presentationFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if(vkCreateSwapchainKHR(vulkanLogicalDevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(vulkanLogicalDevice, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkanLogicalDevice, swapchain, &imageCount, swapchainImages.data());


	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void VulkanSwapchain::createImageViews(VkDevice& device) {

	swapchainImageViews.resize(swapchainImages.size());
	for(size_t i=0; i<swapchainImages.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if(vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create ImageViews");
		}
	}
}

void VulkanSwapchain::destroySwapchain(VkDevice vulkanLogicalDevice) {

	for(VkImageView imageView: swapchainImageViews) {
		vkDestroyImageView(vulkanLogicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(vulkanLogicalDevice, swapchain, nullptr);
}
