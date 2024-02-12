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

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices(*(VkInstance *)m_VkInstance);
    VkPhysicalDevice physicalDevice;
    while (!physicalDevices.empty())
    {
        auto device = *physicalDevices.begin();
        if (isPhysicalDeviceSuitable(device))
        {
            physicalDevice = device;
        }
        physicalDevices.erase(physicalDevices.begin());
    }

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int familyIndex;
    for (auto i = 0; i < queueFamilies.size(); i++)
    {
        const auto queueFamily = queueFamilies[i];
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            familyIndex = i;
            break;
        }
    }

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = familyIndex;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    std::vector<const char *> layers = checkValidationLayerSupport(m_ValidationLayers) ? m_ValidationLayers : std::vector<const char *>{};
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, (VkDevice *)m_VkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }
}

void HelloTriangleApp::mainLoop()
{
    initWindow();
    initVulkan();
    setupDebugMessanger();
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