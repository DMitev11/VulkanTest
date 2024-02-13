#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include "fwd.hpp"

#pragma region application
VkApplicationInfo getApplicationInfo()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Triangle Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    return appInfo;
}

VkInstanceCreateInfo getCreateApplicationInfo(
    VkApplicationInfo *applicationInfo,
    const std::vector<const char *> *layerNames = {},
    const std::vector<const char *> *extensionNames = {},
    const void *next = nullptr)
{
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = applicationInfo;

    createInfo.enabledLayerCount = static_cast<uint32_t>(layerNames->size());
    if (layerNames->size() > 0)
        createInfo.ppEnabledLayerNames = layerNames->data();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames->size());
    if (extensionNames->size() > 0)
        createInfo.ppEnabledExtensionNames = extensionNames->data();

    if (next != nullptr)
        createInfo.pNext = next;
    return createInfo;
}
#pragma endregion

bool checkValidationLayerSupport(std::vector<const char *> validationLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto layer : validationLayers)
    {
        if (std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties &property)
                         { return strcmp(property.layerName, layer) == 0; }) == availableLayers.end())
        {
            return false;
        }
    }
    return true;
}

#pragma region debug
std::vector<const char *> getRequiredExtensions()
{
    uint32_t glfwExtensionCount;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{

    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        std::cerr << "Debug Message: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT getDebugMessangerInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugMessageCallback;
    createInfo.pUserData = nullptr;
    return createInfo;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
#pragma endregion

#pragma region devices
bool isPhysicalDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
}

std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance)
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);

    if (count <= 0)
        return std::vector<VkPhysicalDevice>{};

    std::vector<VkPhysicalDevice> devices = std::vector<VkPhysicalDevice>(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());

    return devices;
}

VkPhysicalDevice getValidPhysicalDevice(std::vector<VkPhysicalDevice> physicalDevices)
{
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

    return physicalDevice;
}

VkDeviceQueueCreateInfo getDeviceQueueCreateInfo(VkPhysicalDevice physicalDevice)
{
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

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = familyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    return queueCreateInfo;
}

VkDeviceCreateInfo getDeviceCreateInfo(
    VkDeviceQueueCreateInfo *queueCreateInfo,
    VkPhysicalDeviceFeatures *deviceFeatures,
    std::vector<const char *> layers)
{
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    deviceCreateInfo.ppEnabledLayerNames = layers.data();

    return deviceCreateInfo;
}
#pragma endregion

#pragma region surface
VkWin32SurfaceCreateInfoKHR getSurfaceCreateInfo(HINSTANCE instance, GLFWwindow *window)
{
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(window);
    createInfo.hinstance = instance;
    return createInfo;
}
#pragma endregion
#pragma region templates
template <typename T>
bool allValid(T arg, std::vector<bool (*)(T)> predicates)
{
    for (auto predicate : predicates)
    {
        if (!predicate(arg))
        {
            return false;
        }
    }

    return true;
}

template <typename T>
auto equal(T rhs)
{
    return [rhs](T lhs)
    {
        return lhs == rhs;
    };
}
#pragma endregion