#ifndef VARIOUSTOOLS_HPP
#define VARIOUSTOOLS_HPP
#include"stdafx.hpp"

struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentationFamily = -1;
	
	bool isComplete()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif

