#ifndef VARIOUSTOOLS_HPP
#define VARIOUSTOOLS_HPP
#include"stdafx.hpp"
#include"YasMathLib.hpp"


//-----------------------------------------------------------------------------|---------------------------------------|

struct QueueFamilyIndices {

	int graphicsFamily = -1;
	int presentationFamily = -1;
	
	bool isComplete() {
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapchainSupportDetails {

	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

static std::vector<char> readFile(const std::string& fileName) {

	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if(!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface) {

	QueueFamilyIndices queueFamilyIndices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const VkQueueFamilyProperties& queueFamily : queueFamilies) {

		if((queueFamily.queueCount > 0) && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			queueFamilyIndices.graphicsFamily = i;
		}

		VkBool32 presentationFamilySupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationFamilySupport);

		if(queueFamily.queueCount > 0 && presentationFamilySupport) {
			queueFamilyIndices.presentationFamily = i;
		}

		if(queueFamilyIndices.isComplete()) {
			break;
		}
		i++;
	}

	return queueFamilyIndices;
}

class TimePicker {

	private:

		LARGE_INTEGER frequency;
		bool isFrequencyOfThePerformanceCounterPickedUp;
		TimePicker() {
			isFrequencyOfThePerformanceCounterPickedUp = (QueryPerformanceFrequency(&frequency) != 0);
		}

	public:

		float getSeconds() {
			if(!isFrequencyOfThePerformanceCounterPickedUp) {
				return GetTickCount64() / 1000.0F;
			} else {
				LARGE_INTEGER ticks;
				QueryPerformanceCounter(&ticks);
				return (float)(ticks.QuadPart / (double)frequency.QuadPart);
			}
		}

		static TimePicker* getTimePicker() {
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
struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
	//YasMathLib::vec2 pos;
	//YasMathLib::vec3 color;
		
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription vertInBindingDescription = {};
		vertInBindingDescription.binding = 0;
		vertInBindingDescription.stride = sizeof(Vertex);
		vertInBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			
		return vertInBindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

#endif

