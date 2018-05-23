#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include<iostream>
#include<vector>
#include<Windows.h>
#include <vulkan/vulkan.h>

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

		const std::vector<const char*> validationLayers =
		{
			"VK_LAYER_LUNARG_standard_validation"	
		};

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
		

		HINSTANCE				windowInstance;
		HWND					window;

		//Vulkan variables and functions
		void					createVulkanInstance();
		void					vulkanInitialization();
		bool					checkForExtensionsSupport(const std::vector<const char*> &enabledExtensions, uint32_t numberOfEnabledExtensions);
		bool					checkValidationLayerSupport();

		VkInstance				vulkanInstance;
		
		//Both Window and Vulkan variable and functions
		void					cleanUp();

	//private end
};

#endif