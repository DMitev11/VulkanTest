#include "../inc/vulkan_utils.hpp"
#include "../inc/HelloTriangleApp.h"

void HelloTriangleApp::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void HelloTriangleApp::initWindow()
{
    m_Window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanTest", nullptr, nullptr);
}

void HelloTriangleApp::initInstance()
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

void HelloTriangleApp::createSurface()
{
    validate<VkSurfaceKHR>((VkSurfaceKHR **) &m_VkSurface);
    
    VkWin32SurfaceCreateInfoKHR createInfo = getSurfaceCreateInfo(GetModuleHandle(nullptr), (GLFWwindow *)m_Window);
    if (vkCreateWin32SurfaceKHR(*(VkInstance *)m_VkInstance, &createInfo, nullptr, (VkSurfaceKHR *)m_VkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    if (glfwCreateWindowSurface(*(VkInstance *)m_VkInstance, (GLFWwindow *)m_Window, nullptr, (VkSurfaceKHR *)m_VkSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

}

void HelloTriangleApp::createDevice()
{
    VkPhysicalDevice physicalDevice = getValidPhysicalDevice(getPhysicalDevices(*(VkInstance *)m_VkInstance), m_DeviceExtensions);
    QueueFamilies familyIndexes = getQueueFamilyIndexes(physicalDevice, getQueueFamilyProperties(physicalDevice), *(VkSurfaceKHR*)m_VkSurface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = getDeviceQueueCreateInfo(physicalDevice, familyIndexes);
    std::vector<VkDeviceQueueCreateInfo> queueInfos = 
        getDeviceQueueCreateInfo(physicalDevice, familyIndexes);


    validate((VkDevice**) &m_VkDevice);
    VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<const char *> layers =
        checkValidationLayerSupport(m_ValidationLayers) ? m_ValidationLayers : std::vector<const char *>{};
    VkDeviceCreateInfo createInfo = getDeviceCreateInfo(&queueInfos, &deviceFeatures, &layers, &m_DeviceExtensions);
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, (VkDevice *)m_VkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    validate((VkQueue**) &m_VkPresentQueue);
    vkGetDeviceQueue(*(VkDevice *)m_VkDevice, familyIndexes.presentation, 0, (VkQueue*)m_VkPresentQueue);
}

void HelloTriangleApp::mainLoop()
{
    init();
    initInstance();
    initWindow();
    setupDebugMessanger();
    createSurface();
    createDevice();

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