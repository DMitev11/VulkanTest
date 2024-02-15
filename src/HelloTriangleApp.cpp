#include "../inc/vulkan_utils.hpp"
#include "../inc/HelloTriangleApp.h"

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
    if (createDebugUtilsMessengerEXT(*(VkInstance *)m_VkInstance, &debugInfo, nullptr, (VkDebugUtilsMessengerEXT *)m_VkDebugMessager) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void HelloTriangleApp::createDevice()
{
    if (m_VkDevice == nullptr)
    {
        m_VkDevice = new VkDevice();
    }

    VkPhysicalDevice physicalDevice = getValidPhysicalDevice(getPhysicalDevices(*(VkInstance *)m_VkInstance));
    VkDeviceQueueCreateInfo queueInfo = getDeviceQueueCreateInfo(physicalDevice);
    VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<const char *> layers =
        checkValidationLayerSupport(m_ValidationLayers) ? m_ValidationLayers : std::vector<const char *>{};
    VkDeviceCreateInfo createInfo = getDeviceCreateInfo(&queueInfo, &deviceFeatures, &layers);

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, (VkDevice *)m_VkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }
}

void HelloTriangleApp::createSurface()
{
    VkWin32SurfaceCreateInfoKHR createInfo = getSurfaceCreateInfo(GetModuleHandle(nullptr), (GLFWwindow *)m_Window);
    if (vkCreateWin32SurfaceKHR(*(VkInstance *)m_VkInstance, &createInfo, nullptr, (VkSurfaceKHR *)m_VkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void HelloTriangleApp::mainLoop()
{
    initWindow();
    initVulkan();
    setupDebugMessanger();
    createDevice();
    // createSurface();

    while (!glfwWindowShouldClose((GLFWwindow *)m_Window))
    {
        glfwPollEvents();
    }
}

void HelloTriangleApp::cleanup()
{
    destroyDebugUtilsMessengerEXT(*(VkInstance *)m_VkInstance, *(VkDebugUtilsMessengerEXT *)m_VkDebugMessager, nullptr);
    vkDestroyDevice(*(VkDevice *)m_VkDevice, nullptr);
    vkDestroyInstance(*(VkInstance *)m_VkInstance, nullptr);
    glfwDestroyWindow((GLFWwindow *)m_Window);
    glfwTerminate();

    m_VkInstance = nullptr;
    m_Window = nullptr;
}