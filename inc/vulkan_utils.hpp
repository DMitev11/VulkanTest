#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstring>
#include <cstdint> 
#include <limits>
#include "fwd.hpp"

#pragma region structs
struct QueueFamilies { 
    uint32_t graphics;
    uint32_t presentation;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};  
#pragma endregion

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

bool checkValidationLayerSupport(const std::vector<const char *> validationLayers = {})
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

#pragma region swapChain
SwapChainSupportDetails queryPhysicalDeviceSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    
    return details;
}

bool doesPhysicalDeviceSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details = queryPhysicalDeviceSwapChainSupport(device, surface);
    return !details.formats.empty() && !details.presentModes.empty();
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window = nullptr) {
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}

VkSwapchainCreateInfoKHR getSwapChainCreateInfo(
    SwapChainSupportDetails supportDetails,
    QueueFamilies queueFamilies,
    VkPhysicalDevice device, 
    VkSurfaceKHR surface) { 
    uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
    if (supportDetails.capabilities.maxImageCount > 0 && 
        imageCount > supportDetails.capabilities.maxImageCount) {
        imageCount = supportDetails.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageArrayLayers = 1; //VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = supportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(queueFamilies.graphics != queueFamilies.presentation) { 
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = new uint32_t[]{queueFamilies.graphics, queueFamilies.presentation};
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    return createInfo;
}

VkImageViewCreateInfo getSwapChainImageViewCreateInfo(VkImage image, VkFormat imageFormat = VK_FORMAT_B8G8R8A8_SRGB) { 
    VkImageViewCreateInfo createInfo{};
    createInfo.image = image;
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = imageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    return createInfo;
}

#pragma endregion

#pragma region devices
VkPhysicalDeviceProperties getPhysicalDeviceProperties(VkPhysicalDevice device) { 
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    return deviceProperties;
}

VkPhysicalDeviceFeatures getPhysicalDeviceFeatures(VkPhysicalDevice device) { 
    VkPhysicalDeviceFeatures deviceFeatures;    
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceFeatures;
}

bool doesPhysicalDeviceSupportExtensions(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    } 

    return requiredExtensions.empty();
}

bool isPhysicalDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions, VkSurfaceKHR surface)
{
    auto properties = getPhysicalDeviceProperties(device);
    auto features = getPhysicalDeviceFeatures(device);
    auto supportExtensions = doesPhysicalDeviceSupportExtensions(device, deviceExtensions);
    auto supportSwapChain =  doesPhysicalDeviceSwapChainSupport(device, surface);
    return (properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) &&
        features.geometryShader && 
        supportExtensions && 
        supportSwapChain;
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

VkPhysicalDevice getValidPhysicalDevice(std::vector<VkPhysicalDevice> physicalDevices, const std::vector<const char*> deviceExtensions, VkSurfaceKHR surface)
{
    VkPhysicalDevice physicalDevice;
    while (!physicalDevices.empty())
    {
        auto device = *physicalDevices.begin();
        if (isPhysicalDeviceSuitable(device, deviceExtensions, surface))
        {
            physicalDevice = device;
        }
        physicalDevices.erase(physicalDevices.begin());
    }

    return physicalDevice;
}

std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice physicalDevice) { 
    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);

    std::vector<VkQueueFamilyProperties> properties(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, properties.data());
    return properties;        
}

QueueFamilies getQueueFamilyIndices(
    VkPhysicalDevice physicalDevice, 
    VkSurfaceKHR surface) 
{     
    std::vector<VkQueueFamilyProperties> queueProperties = getQueueFamilyProperties(physicalDevice); 
    QueueFamilies queueFamilies;
    for (auto i = 0; i < queueProperties.size(); i++)
    {
        //check graphics
        if (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilies.graphics = i;
        }

        VkBool32 surfaceSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupport);
        if(surfaceSupport) { 
            queueFamilies.presentation = i;
        }
    }    
    return queueFamilies;
}

std::vector<VkDeviceQueueCreateInfo> getDeviceQueueCreateInfo(VkPhysicalDevice physicalDevice, QueueFamilies queueFamilies)
{
    std::vector<VkDeviceQueueCreateInfo> createInfos; 

    float queuePriority = 1.0f;
    std::vector<uint32_t> families = std::vector<uint32_t>{queueFamilies.graphics, queueFamilies.presentation};
    for(auto family : families) { 
        VkDeviceQueueCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueFamilyIndex = family;
        createInfo.queueCount = 1;
        createInfo.pQueuePriorities = &queuePriority;
        createInfos.push_back(createInfo);
    } 

    return createInfos;
}

VkDeviceCreateInfo getDeviceCreateInfo(
    const std::vector<VkDeviceQueueCreateInfo>* queueCreateInfos,
    const VkPhysicalDeviceFeatures *deviceFeatures,
    const std::vector<const char *>* layers = {},
    const std::vector<const char *>* extensions = {}
    )
{
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos->size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos->data();
    deviceCreateInfo.pEnabledFeatures = deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions->size());
    deviceCreateInfo.ppEnabledExtensionNames = extensions->data();
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers->size());
    deviceCreateInfo.ppEnabledLayerNames = layers->data();

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

template <typename T> 
auto validate(T** lhs) { 
    if (*lhs == nullptr)
    {
        *lhs = new T();
    }
}
#pragma endregion