#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include"stdafx.hpp"
#include"VulkanSwapchain.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanDevice.hpp"
#include"YasLog.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

//#define NDEBUG

VkResult createDebugReportCallbackEXT ( VkInstance& vulkanInstance, const VkDebugReportCallbackCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugReportCallbackEXT* callback);

class YasEngine
{

	public:

		YasEngine();
		YasLog<int> logInt;
		void							run(HINSTANCE hInstance);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugReportFlagsEXT debugReportFlags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);

		static int						windowPositionX;
		static int						windowPositionY;
		static int						windowWidth;
		static int						windowHeight;

		static const std::string				MODEL_PATH;//="Models\\chalet.obj";
		static const std::string				TEXTURE_PATH;//="Textures\\chalet.jpg";

		#ifdef NDEBUG
			const bool					enableValidationLayers = false;
		#else
			const bool					enableValidationLayers = true;
		#endif

		static bool framebufferResized;
	//public end

	private:

		void							createWindow(HINSTANCE hInstance);
		void							mainLoop();
		void							cleanUp();
		void							createVulkanInstance();
		void							initializeVulkan();
		void							setupDebugCallback();
		void							selectPhysicalDevice();
		void							createLogicalDevice();
		void							createSurface();
		void							createSwapchain();
		void							recreateSwapchain();
		void							cleanupSwapchain();
		void							destroySwapchain();
		void							createImageViews();
		void							createRenderPass();
		void							createGraphicsPipeline();
		void							createFramebuffers();
		VkShaderModule					createShaderModule(const std::vector<char>& code);
		void							createCommandPool();
		void							createCommandBuffers();
		void							createVertexBuffer();
		void							createIndexBuffer();
		uint32_t						findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertiesFlags);
		void							drawFrame(float deltaTime);
		void							createSyncObjects();
		void							createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void							copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize deviceSize);
		void							createDescriptorSetLayout();
		void							createUniformBuffers();
		void							updateUniformBuffer(uint32_t currentImage, float deltaTime);
		void							createDescriptorPool();
		void							createDescriptorSets();
		void							createTextureImage();
		void							createImage(uint32_t width, uint32_t height, uint32_t mipLevelsNumber, VkSampleCountFlagBits samplesNumber, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkCommandBuffer					beginSingleTimeCommands();
		void							endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void							transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,  uint32_t mipLevelsNumber);
		void							copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t imageWidth, uint32_t imageHeight);
		void							createTextureImageView();
		void							createTextureSampler();
		void							createDepthResources();
		VkFormat						findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat						findDepthFormat();
		bool							hasStencilComponent(VkFormat format);
		void							loadModel();
		void							generateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth,int32_t textureHeight,uint32_t mipLevelsNumber);
		void							createColorResources();

		HINSTANCE						application;
		HWND							window;
		size_t							currentFrame = 0;
		std::vector<VkSemaphore>		imageAvailableSemaphores;
		std::vector<VkSemaphore>		renderFinishedSemaphores;
		std::vector<VkFence>			inFlightFences;
		VulkanInstance					vulkanInstance;
		VkDebugReportCallbackEXT		callback;
		VkSurfaceKHR					surface;
		VulkanDevice*					vulkanDevice;
		VkQueue							graphicsQueue = nullptr;
		VkQueue							presentationQueue;
		VulkanSwapchain					vulkanSwapchain;
		VkRenderPass					renderPass;
		VkDescriptorSetLayout			descriptorSetLayout;
		VkPipelineLayout				pipelineLayout;
		VkPipeline						graphicsPipeline;
		std::vector<VkFramebuffer>		swapchainFramebuffers;
		VkCommandPool					commandPool;
		std::vector<VkCommandBuffer>	commandBuffers;
		VkBuffer						vertexBuffer;
		VkDeviceMemory					vertexBufferMemory;
		VkBuffer						indexBuffer;
		VkDeviceMemory					indexBufferMemory;
		std::vector<VkBuffer>			uniformBuffers;
		std::vector<VkDeviceMemory>		uniformBuffersMemory;
		VkDescriptorPool				descriptorPool;
		std::vector<VkDescriptorSet>	descriptorSets;
		VkImage							textureImage;
		uint32_t						mipLevels;
		VkDeviceMemory					textureImageMemory;
		VkImageView						textureImageView;
		VkSampler						textureSampler;
		VkImage							depthImage;
		VkDeviceMemory					depthImageMemory;
		VkImageView						depthImageView;
		VkImage							colorImage;
		VkDeviceMemory					colorImageMemory;
		VkImageView						colorImageView;
		float zeroTime = 0;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t mipLevelsNumber;
	//private end
};

#endif

