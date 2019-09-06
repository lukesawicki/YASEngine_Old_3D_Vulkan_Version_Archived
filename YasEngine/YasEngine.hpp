#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include"stdafx.hpp"
#include"TimePicker.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

#define YASENGINE_VALIDATION_LAYERS_ENABLED

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class YasEngine
{
    public:
        YasEngine(HINSTANCE hInstance);
        void prepareWindow();
        void prepareVulkan();
        void run();

    private:

        static const char* engineName;
        static const char* applicationName;

        // Windows API
        int windowXposition                         = 10;//TODO take system width
        int windowYposition                         = 10;//TODO teke system height
        int windowWidth                      = 1280;
        int windowHeight                     = 720;
        WNDCLASSEX windowClassEx;
        HWND windowHandle;
        HINSTANCE applicationHandle;

        // Vulkan API

        // Represents connections application to Vulkan runtime
        VkInstance vulkanInstance;

        // It represents device compatible with Vulkan API
        std::vector<VkPhysicalDevice> physicalDevices;
        std::vector<VkPhysicalDeviceProperties*> physicalDevicesProperties;

        uint32_t numberOfPhysicalDevices;
        // It is logical representation of physical device.
        VkDevice vulkanDevice;

        std::vector<VkQueueFamilyProperties> queueFamilyProperties;

        // Api used in this project:
        // https://vulkan.lunarg.com/doc/sdk/1.1.101.0/windows/layer_configuration.html
        std::vector<const char*> requiredInstanceLayerNames = {
            "VK_LAYER_GOOGLE_threading",                // deprecated in 1.1.106.0 and newer versions
            "VK_LAYER_LUNARG_parameter_validation",     // deprecated in 1.1.106.0 and newer versions
            "VK_LAYER_LUNARG_object_tracker",           // deprecated in 1.1.106.0 and newer versions
            "VK_LAYER_LUNARG_core_validation",          // deprecated in 1.1.106.0 and newer versions
            "VK_LAYER_GOOGLE_unique_objects"            // deprecated in 1.1.106.0 and newer versions
        };

        std::vector<const char*> requiredExtenstionsNames = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME
        };
        // Newer api: https://vulkan.lunarg.com/doc/sdk/1.1.106.0/windows/layer_configuration.html
        std::vector<VkLayerProperties> availableValidationLayersProperties;
        std::vector<VkExtensionProperties> availableExtentionProperties;
};

class MissingValidationLayersException: public std::exception
{
    public:
        const char* what() const throw()
        {
            return "Missing validation layers!";
        }
    private:
        int some;
};

class MissingInstanceExtensionsException: public std::exception
{
    public:
        const char* what() const throw()
        {
            return "Missing instance extensions layers!";
        }
    private:
        int some;
};

class InstanceCreationException: public std::exception
{
    public:
        const char* what() const throw()
        {
            return "Error during Instance creation!";
        }
    private:
        int some;
};

#endif

//                                                                            80                                     120
//-----------------------------------------------------------------------------|---------------------------------------|
