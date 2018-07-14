#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include"stdafx.hpp"
#include"VulkanSwapchain.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanDevice.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

VkResult createDebugReportCallbackEXT
(
	VkInstance& vulkanInstance,
	const VkDebugReportCallbackCreateInfoEXT* createInfo,
	const VkAllocationCallbacks* allocator,
	VkDebugReportCallbackEXT* callback
);

class YasEngine
{

	public:
		YasEngine();
		//Window variables and functionss
		void							run(HINSTANCE hInstance);

		//Vulkan variables and functions
		static int						windowPositionX;
		static int						windowPositionY;
		static int						windowWidth;
		static int						windowHeight;
		
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
			(
				VkDebugReportFlagsEXT debugReportFlags,
				VkDebugReportObjectTypeEXT objectType,
				uint64_t object,
				size_t location,
				int32_t code,
				const char* layerPrefix,
				const char* msg,
				void* userData
			);

		#ifdef NDEBUG
			const bool					enableValidationLayers = false;
		#else
			const bool					enableValidationLayers = true;
		#endif

	//public end

	private:

		//Window variables and functions
		void							createWindow(HINSTANCE hInstance);
		void							mainLoop();

		HINSTANCE						application;
		HWND							window;

		//Both Window and Vulkan variable and functions
		void							cleanUp();

		//Vulkan variables and functions
		void							createVulkanInstance();
		void							initializeVulkan();

		void							setupDebugCallback();
		void							selectPhysicalDevice();
		void							createLogicalDevice();
		void							createSurface();
		void							createSwapchain();
		void							destroySwapchain();
		void							createImageViews();
		void							createRenderPass();
		void							createGraphicsPipeline();
		void							createFramebuffers();
		VkShaderModule					createShaderModule(const std::vector<char>& code);
		void							createCommandPool();
		void							createCommandBuffers();
		void							drawFrame();
		void							createSyncObjects();

		size_t							currentFrame = 0;
		std::vector<VkSemaphore>		imageAvailableSemaphores;
		std::vector<VkSemaphore>		renderFinishedSemaphores;
		std::vector<VkFence>			inFlightFences;
		VulkanInstance					vulkanInstance;
		VkDebugReportCallbackEXT		callback;
		VkSurfaceKHR					surface;
		VulkanDevice*					vulkanDevice;
		VkQueue							graphicsQueue;
		VkQueue							presentationQueue;
		VulkanSwapchain					vulkanSwapchain;
		VkRenderPass					renderPass;
		VkPipelineLayout				pipelineLayout;
		VkPipeline						graphicsPipeline;
		std::vector<VkFramebuffer>		swapchainFramebuffers;
		VkCommandPool					commandPool;
		std::vector<VkCommandBuffer>	commandBuffers;
		
	//private end
};
#endif

