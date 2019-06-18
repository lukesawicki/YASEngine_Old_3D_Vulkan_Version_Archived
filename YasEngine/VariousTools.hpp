#ifndef VARIOUSTOOLS_HPP
#define VARIOUSTOOLS_HPP
#include"stdafx.hpp"
#include"YasMathLib.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|



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
			if(!isFrequencyOfThePerformanceCounterPickedUp) {
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


// TODO lukesawicki
static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags imageAspectFlags, VkDevice& vulkanLogicDevice, uint32_t mipLevelsNumber)
{
    // VkImageView it is objects which allow to access to image(VkImage)

    // VkImageViewCreateInfor structure contains informations needed to create image views (VkImageView)
    // imageview types, format, data for remaping color/depth/stencil, range of mipmaplevels and arrays layers
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

    // Function which are using to create image views(VkImageView
	if(vkCreateImageView(vulkanLogicDevice, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create image view");
	}
	return imageView;
}

#endif
