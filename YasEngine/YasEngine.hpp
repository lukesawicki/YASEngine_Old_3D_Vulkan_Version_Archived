#ifndef YASENGINE_HPP
#define YASENGINE_HPP
#include"stdafx.hpp"
#include"VulkanSwapchain.hpp"
#include"VariousTools.hpp"
#include"VulkanInstance.hpp"
#include"VulkanDevice.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|
//                                                                            80                                     120

class YasEngine
{
    public:
        void prepareWindow();
        void prepareVulkan();
        void run();

    private:
        int WindowSurfaceWidth                      = 1280;
        int WindowSurfaceHeight                     = 720;

};

#endif

//                                                                            80                                     120
//-----------------------------------------------------------------------------|---------------------------------------|
