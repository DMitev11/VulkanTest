#pragma once
#include <iostream>
#include <stdexcept>
#include <vector> 

class HelloTriangleApp
{
public:
    void run()
    {
        mainLoop();
        cleanup();
    }

private:
    const int WIDTH = 800, HEIGHT = 600;
    const std::vector<const char *> m_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char *> m_DeviceExtensions = {
        "VK_KHR_swapchain",
    };

    void *m_Window = nullptr;
    void *m_VkInstance = nullptr;
    void *m_VkDebugMessager = nullptr;
    void *m_VkPhysicalDevice = nullptr;
    void *m_VkDevice = nullptr;
    void *m_VkSurface = nullptr;
    void *m_VkPresentQueue = nullptr;
    void *m_VkSwapChain = nullptr;
    std::vector<void*> m_VkSwapChainImageViews = std::vector<void*>();

private:
    void init();
    void initInstance();
    void initWindow();
    void setupDebugMessanger();
    void createSurface();
    void createDevice();
    void createSwapChain();
    void createImageViews();

    void mainLoop();
    void cleanup();
};