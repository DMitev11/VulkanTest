#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <cstring>

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
    const std::vector<const char *> layerNames = {},
    const std::vector<const char *> extensionNames = {},
    const void *next = nullptr)
{
    VkInstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = applicationInfo;

    createInfo.enabledLayerCount = static_cast<uint32_t>(layerNames.size());
    if (layerNames.size() > 0)
        createInfo.ppEnabledLayerNames = layerNames.data();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
    if (extensionNames.size() > 0)
        createInfo.ppEnabledExtensionNames = extensionNames.data();

    if (next != nullptr)
        createInfo.pNext = next;
    return createInfo;
}

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
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;
    createInfo.pfnUserCallback = debugMessageCallback;
    createInfo.pUserData = nullptr;
    return createInfo;
}

VkResult CreateDebugUtilsMessengerEXT(
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

void DestroyDebugUtilsMessengerEXT(
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