#ifndef VARIOUSTOOLS_HPP
#define VARIOUSTOOLS_HPP
#include"stdafx.hpp"
#include"YasMathLib.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

//Various tools which i will move to classes or runctions.

//struct QueueFamilyIndices
//{
//
//	int graphicsFamily = -1;
//	int presentationFamily = -1;
//	bool isComplete()
//	{
//		return graphicsFamily >= 0 && presentationFamily >= 0;
//	}
//};

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

//static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface)
//{
//
//	QueueFamilyIndices queueFamilyIndices;
//	uint32_t queueFamilyCount = 0;
//	
//	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//	int i = 0;
//	for(const VkQueueFamilyProperties& queueFamily : queueFamilies)
//	{
//		if((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
//		{
//			queueFamilyIndices.graphicsFamily = i;
//		}
//
//		VkBool32 presentationFamilySupport = false;
//		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationFamilySupport);
//
//		if(queueFamily.queueCount > 0 && presentationFamilySupport)
//		{
//			queueFamilyIndices.presentationFamily = i;
//		}
//
//		if(queueFamilyIndices.isComplete())
//		{
//			break;
//		}
//		i++;
//	}
//
//	return queueFamilyIndices;
//}

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
//////////// FORMATING TO CORRECT
		float getSeconds()
		{
			if(!isFrequencyOfThePerformanceCounterPickedUp)
			{
				return GetTickCount64() / 1000.0F;
			} else{
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
 /* TODO implement vectors in YasMathLib
	struct vec2
	{
		float x;
		float y;
	};

	struct vec3
	{
		float x;
		float y;
		float z;
	};
*/


struct Vertex
{

	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	//YasMathLib::vec2 pos;
	//YasMathLib::vec3 color;

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription vertInBindingDescription = {};
		vertInBindingDescription.binding = 0;
		vertInBindingDescription.stride = sizeof(Vertex);
		vertInBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return vertInBindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributeDescription = {};
		vertexInputAttributeDescription[0].binding = 0;
		vertexInputAttributeDescription[0].location = 0;
		vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescription[0].offset = offsetof(Vertex, pos);

		vertexInputAttributeDescription[1].binding = 0;
		vertexInputAttributeDescription[1].location = 1;
		vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescription[1].offset = offsetof(Vertex, color);

        vertexInputAttributeDescription[2].binding = 0;
        vertexInputAttributeDescription[2].location = 2;
        vertexInputAttributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
        vertexInputAttributeDescription[2].offset = offsetof(Vertex, texCoord);

		return vertexInputAttributeDescription;
	}
};

template<> struct std::hash<Vertex>
{
	size_t operator()(Vertex const& vertex) const
	{
		return ((std::hash<glm::vec3>()(vertex.pos) ^ (std::hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (std::hash<glm::vec2>()(vertex.texCoord) << 1);
	}
};

static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkDevice& vulkanLogicDevice, uint32_t mipLevelsNumber)
{
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.subresourceRange.aspectMask = imageAspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = mipLevelsNumber;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;

	if(vkCreateImageView(vulkanLogicDevice, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create image view");
	}
	return imageView;
}

#endif
