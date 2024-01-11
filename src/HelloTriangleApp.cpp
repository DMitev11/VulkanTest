#include "../inc/HelloTriangleApp.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "../inc/vulkan_utils.hpp"

void HelloTriangleApp::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanTest", nullptr, nullptr);
}

void HelloTriangleApp::initVulkan()
{
    auto applicationInfo = getApplicationInfo();
    std::vector<const char *> layers = checkValidationLayerSupport(m_ValidationLayers) ? m_ValidationLayers : std::vector<const char *>{};
    std::vector<const char *> extensions = getRequiredExtensions();

    if (m_VkInstance == nullptr)
    {
        m_VkInstance = new VkInstance();
    }

    auto debugInfo = getDebugMessangerInfo();
    auto createInfo = getCreateApplicationInfo(&applicationInfo, &layers, &extensions, &debugInfo);
    if (vkCreateInstance(&createInfo, nullptr, (VkInstance *)m_VkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void HelloTriangleApp::setupDebugMessanger()
{
    auto debugInfo = getDebugMessangerInfo();
    if (m_VkDebugMessager == nullptr)
    {
        m_VkDebugMessager = new VkDebugUtilsMessengerEXT();
    }
    if (CreateDebugUtilsMessengerEXT(*(VkInstance *)m_VkInstance, &debugInfo, nullptr, (VkDebugUtilsMessengerEXT *)m_VkDebugMessager) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void HelloTriangleApp::mainLoop()
{
    initWindow();
    initVulkan();
    setupDebugMessanger();

    while (!glfwWindowShouldClose((GLFWwindow *)m_Window))
    {
        glfwPollEvents();
    }
}

void HelloTriangleApp::cleanup()
{
    DestroyDebugUtilsMessengerEXT(*(VkInstance *)m_VkInstance, *(VkDebugUtilsMessengerEXT *)m_VkDebugMessager, nullptr);
    vkDestroyInstance(*(VkInstance *)m_VkInstance, nullptr);
    glfwDestroyWindow((GLFWwindow *)m_Window);
    glfwTerminate();

    m_VkInstance = nullptr;
    m_Window = nullptr;
}