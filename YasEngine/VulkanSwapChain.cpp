#include"stdafx.hpp"
#include"VulkanSwapchain.hpp"
#include"VariousTools.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

VkSurfaceCapabilitiesKHR VulkanSwapchain::getSwapchainCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    // This structure contains information about surface capabilities like: min/max number of image surfaces supported
    // image dimention range, maximum number of image arrays possible, types of intransformation features supported
    // by the surface
	VkSurfaceCapabilitiesKHR capabilities;

    // This function retrieve data with information about surface: current size, minimum/maximum size possible(maximum or minimum number
    // of images the specified device supports for swapchain created for surface, possible
    // transformation capabilities and other things
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);
	return capabilities;
}

std::vector<VkSurfaceFormatKHR> VulkanSwapchain::getSwapchainSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    // VkFormat is value specifying the format the swapchain image(s) will be created with
    // describes the format and type of the data elements that will be contained in image

    uint32_t formatCount;
    
    // Default format is 32-bit RGBA
	std::vector<VkSurfaceFormatKHR> formats;

    // Retrieve data with information about surface formats:
    // VkSurfaceFormatKHR - is data structure with informations describing a format-color space pair that is compatible
    // with the specified surface (which depends on platform and device)
    // first call retrieve formats count
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	
	if(formatCount != 0)
	{
		formats.resize(formatCount);
        // second call retrieve data for all formats
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
	}
	return formats;
}

std::vector<VkPresentModeKHR> VulkanSwapchain::getSwapchainPresentModes(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t presentModesCount;

    // VkPresentModeKHR described below
	std::vector<VkPresentModeKHR> presentModes;
    
    // This function retrieve the surface presentation modes
    // they are exposed via the VkPresentModeKHR in this case in vector of VkPresentModeKHR
    // presentation modes determine how the incoming presentation requests will be processed and queued inernally.
    // there are four types of present modes (it is all about time and order of (rendering) and update image.
    // in this call function retrieve number of present modes
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);

	if(presentModesCount != 0)
	{
		presentModes.resize(presentModesCount);
        // in this call function retrieve present modes
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, presentModes.data());
	}
	return presentModes;
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	for(const VkSurfaceFormatKHR& availableFormat: availableFormats)
	{
		//This format results in more accurate perceived colors
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	//Take first in the row.
	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
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

VkExtent2D	VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR surfaceCapabilities, HWND& window)
{
	if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		RECT windowClientRect;
		GetClientRect(window,&windowClientRect);
		VkExtent2D actualExtent = {static_cast<uint32_t>(windowClientRect.right - 1), static_cast<uint32_t>(windowClientRect.bottom - 1)};
		actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);		
		return actualExtent;
	}
}

void VulkanSwapchain::createSwapchain(VkSurfaceKHR& surface, VulkanDevice& vulkanDevice, HWND& window)
{
	VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKhr = VulkanSwapchain::getSwapchainCapabilities(vulkanDevice.physicalDevice, surface);
	std::vector<VkSurfaceFormatKHR> vkSurfaceFormatsKhr = VulkanSwapchain::getSwapchainSurfaceFormats(vulkanDevice.physicalDevice, surface);
	std::vector<VkPresentModeKHR> vkPresentModesKhr = VulkanSwapchain::getSwapchainPresentModes(vulkanDevice.physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(vkSurfaceFormatsKhr);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(vkPresentModesKhr);
    
    // Extents describe the size of rectangular region of pixels within an image or framebuffer in 2D(width, height)
    // in 3D same + depth
    // VkExtent2D - Size of the swapchain color images
	VkExtent2D extent = chooseSwapExtent(vkSurfaceCapabilitiesKhr, window);
	uint32_t imageCount = vkSurfaceCapabilitiesKhr.minImageCount + 1;
	if(vkSurfaceCapabilitiesKhr.maxImageCount > 0 && imageCount > vkSurfaceCapabilitiesKhr.maxImageCount)
	{
		imageCount = vkSurfaceCapabilitiesKhr.maxImageCount;
	}

    // VkSwapchainCreateInfoKHR contains information about creation of the swapchain object. Held in the VkSwapchainKHR
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { vulkanDevice.getGraphicQueue(vulkanDevice.physicalDevice), vulkanDevice.getPresentationQueue(vulkanDevice.physicalDevice, surface) };

	if(queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

    // This is a bit field of VkSurfaceTransformFlag-BitsKHR that
    // describes the transform relative to the presentation engine's natural
    // orientation, which is applied to the image content prior to the
    // presentation.
	createInfo.preTransform = vkSurfaceCapabilitiesKhr.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	
    // vkCreateSwapchainKHR is a function which create swapchainobject
	if(vkCreateSwapchainKHR(vulkanDevice.logicalDevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}
    // vkGetSwapchainImagesKHR is function to retrieve image objects (VkImage)
	vkGetSwapchainImagesKHR(vulkanDevice.logicalDevice, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkanDevice.logicalDevice, swapchain, &imageCount, swapchainImages.data());
	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void VulkanSwapchain::createImageViews(VkDevice& device, int32_t mipLevelsNumber)
{
	swapchainImageViews.resize(swapchainImages.size());
	for(size_t i=0; i<swapchainImages.size(); i++)
	{
		swapchainImageViews[i] = createImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, device, 1);
	}
}

void VulkanSwapchain::destroySwapchain(VkDevice vulkanLogicalDevice)
{
	for(VkImageView imageView: swapchainImageViews)
	{
		vkDestroyImageView(vulkanLogicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(vulkanLogicalDevice, swapchain, nullptr);
}

bool VulkanSwapchain::isSwapchainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    return getSwapchainSurfaceFormats(device, surface).size() > 0 && getSwapchainPresentModes(device, surface).size() > 0;
}
