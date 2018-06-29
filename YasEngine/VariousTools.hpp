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

static std::vector<char> readFile(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);
	if(!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

#endif

