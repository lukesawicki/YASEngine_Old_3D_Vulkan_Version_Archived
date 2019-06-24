#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include"stdafx.hpp"
#include"TimePicker.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class YasEngine
{
    public:

        void run();

    private:
        void prepareWindow();
        void prepareVulkan();
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
        VkPhysicalDevice physicalDevice;

        // It is logical representation of physical device.
        VkDevice vulkanDevice;
};

#endif

//                                                                            80                                     120
//-----------------------------------------------------------------------------|---------------------------------------|
