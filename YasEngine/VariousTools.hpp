#ifndef VARIOUSTOOLS_HPP
#define VARIOUSTOOLS_HPP
#include"stdafx.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentationFamily = -1;
	
	bool isComplete()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapchainSupportDetails
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

static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface)
{
	QueueFamilyIndices queueFamilyIndices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			queueFamilyIndices.graphicsFamily = i;
		}

		VkBool32 presentationFamilySupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationFamilySupport);
		
		if(queueFamily.queueCount > 0 && presentationFamilySupport)
		{
			queueFamilyIndices.presentationFamily = i;
		}
		if(queueFamilyIndices.isComplete())
		{
			break;
		}
		i++;
	}

	return queueFamilyIndices;
}

class TimePicker
{
	private:
		LARGE_INTEGER frequency;
		bool isFrequencyOfThePerformanceCounterPickedUp;
		TimePicker()
		{
			isFrequencyOfThePerformanceCounterPickedUp = (QueryPerformanceFrequency(&frequency) != 0);
		}
	public:

		float getSeconds()
		{
			if(!isFrequencyOfThePerformanceCounterPickedUp)
			{
				return GetTickCount64() / 1000.0F;
			}
			else
			{
				LARGE_INTEGER ticks;
				QueryPerformanceCounter(&ticks);
				return (float)(ticks.QuadPart / (double)frequency.QuadPart);
			}
		}

		static TimePicker* getTimePicker()
		{
			return new TimePicker();
		}
};

#endif

