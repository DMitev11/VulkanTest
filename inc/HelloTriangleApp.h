#pragma once
#include <iostream>
#include <stdexcept>
#include <vector>
#include "./fwd.hpp"

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
        "VK_LAYER_KHRONOS_validation"};

    void *m_Window = nullptr;
    void *m_VkInstance = nullptr;
    void *m_VkDebugMessager = nullptr;
    void *m_VkDevice = nullptr;
    void *m_VkSurface = nullptr;

private:
    void initWindow();
    void initVulkan();
    void setupDebugMessanger();
    void createDevice();
    void createSurface();

    void mainLoop();
    void cleanup();
};