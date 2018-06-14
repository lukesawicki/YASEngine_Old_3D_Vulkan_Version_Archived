#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include"stdafx.hpp"

VkResult createDebugReportCallbackEXT
(
	VkInstance vulkanInstance,
	const VkDebugReportCallbackCreateInfoEXT* createInfo,
	const VkAllocationCallbacks* allocator,
	VkDebugReportCallbackEXT* callback
);

class YasEngine
{
	public:
		YasEngine();
		//Window variables and functionss
		void run(HINSTANCE hInstance);

		//Vulkan variables and functions
		static int				windowPositionX;
		static int				windowPositionY;
		static int				windowWidth;
		static int				windowHeight;
		
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
			const bool enableValidationLayers = false;
		#else
			const bool enableValidationLayers = true;
		#endif
	//public end

	private:

		//Window variables and functions
		void					createWindow(HINSTANCE hInstance);
		void					mainLoop();

		HINSTANCE				application;
		HWND					window;

		//Both Window and Vulkan variable and functions
		void					cleanUp();

		//Vulkan variables and functions
		void					createVulkanInstance();
		void					initializeVulkan();
		std::vector<const char*> getRequiredExtensions();
		bool					checkForExtensionsSupport(const std::vector<const char*> &enabledExtensions, uint32_t numberOfEnabledExtensions);
		bool					checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice); // moze lepiej przez reerencje
		bool					checkValidationLayerSupport();

		void					setupDebugCallback();
		void					selectPhysicalDevice();
		bool					isPhysicalDeviceSuitable(VkPhysicalDevice device);
		void					createLogicalDevice();
		void					createSurface();
		void					createSwapChain();
		void					destroySwapChain();

		QueueFamilyIndices		findQueueFamilies(VkPhysicalDevice device);

		VkInstance				vulkanInstance;
		VkDebugReportCallbackEXT callback;
		VkDevice				vulkanLogicalDevice;
		VkSurfaceKHR			surface;
		VkPhysicalDevice		physicalDevice = VK_NULL_HANDLE;
		VkQueue					graphicsQueue;
		VkQueue					presentationQueue;
		VulkanSwapChain			vulkanSwapChain;

		const std::vector<const char*> validationLayers =
		{
			"VK_LAYER_LUNARG_standard_validation"	
		};

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		
	//private end
};
#endif

