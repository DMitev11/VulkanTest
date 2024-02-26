#include "../inc/HelloTriangleApp.h"
#include "../inc/vulkan_utils.hpp"

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
    validate((VkDevice**) &m_VkPhysicalDevice);
    *(VkPhysicalDevice*) m_VkPhysicalDevice = getValidPhysicalDevice(getPhysicalDevices(*(VkInstance *)m_VkInstance), m_DeviceExtensions,  *(VkSurfaceKHR*)m_VkSurface);
    
    QueueFamilies familyIndices = getQueueFamilyIndices(*(VkPhysicalDevice*) m_VkPhysicalDevice, *(VkSurfaceKHR*)m_VkSurface);
    std::vector<VkDeviceQueueCreateInfo> queueInfos = getDeviceQueueCreateInfo(*(VkPhysicalDevice*) m_VkPhysicalDevice, familyIndices);


    validate((VkDevice**) &m_VkDevice);
    VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<const char *> layers =
        checkValidationLayerSupport(m_ValidationLayers) ? m_ValidationLayers : std::vector<const char *>{};
    VkDeviceCreateInfo createInfo = getDeviceCreateInfo(&queueInfos, &deviceFeatures, &layers, &m_DeviceExtensions);
    if (vkCreateDevice(*(VkPhysicalDevice*) m_VkPhysicalDevice, &createInfo, nullptr, (VkDevice *)m_VkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    validate((VkQueue**) &m_VkPresentQueue);
    vkGetDeviceQueue(*(VkDevice *)m_VkDevice, familyIndices.presentation, 0, (VkQueue*)m_VkPresentQueue);
}

void HelloTriangleApp::createSwapChain()
{   
    QueueFamilies familyIndices = getQueueFamilyIndices(*(VkPhysicalDevice*) m_VkPhysicalDevice, *(VkSurfaceKHR*)m_VkSurface);
    SwapChainSupportDetails swapChainSupport = queryPhysicalDeviceSwapChainSupport(*(VkPhysicalDevice*) m_VkPhysicalDevice, *(VkSurfaceKHR*)m_VkSurface);
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    validate((VkSwapchainKHR**) &m_VkSwapChain);
    VkSwapchainCreateInfoKHR createInfo = getSwapChainCreateInfo(swapChainSupport, familyIndices, *(VkPhysicalDevice*) m_VkPhysicalDevice, *(VkSurfaceKHR *)m_VkSurface);
    createInfo.imageExtent = extent;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = *(VkSwapchainKHR*) m_VkSwapChain;
    
    if (vkCreateSwapchainKHR(*(VkDevice *)m_VkDevice, &createInfo, nullptr, (VkSwapchainKHR*) m_VkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    //@todo retrieve all images
}

void HelloTriangleApp::createImageViews() { 
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(*(VkDevice *) m_VkDevice, *(VkSwapchainKHR*) m_VkSwapChain, &imageCount, nullptr);
    std::vector<VkImage> swapChainImages = std::vector<VkImage>();
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(*(VkDevice *) m_VkDevice, *(VkSwapchainKHR*) m_VkSwapChain, &imageCount, swapChainImages.data());

    m_VkSwapChainImageViews = std::vector<void*>();
    m_VkSwapChainImageViews.resize(swapChainImages.size());

    for(int i = 0; i < swapChainImages.size(); i++) { 
        auto createInfo = getSwapChainImageViewCreateInfo(swapChainImages[i]);
        validate((VkImageView**) &m_VkSwapChainImageViews[i]);
        if (vkCreateImageView(*(VkDevice *) m_VkDevice, &createInfo, nullptr, (VkImageView*)m_VkSwapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void HelloTriangleApp::mainLoop()
{
    init();
    initInstance();
    initWindow();
    setupDebugMessanger();
    createSurface();
    createDevice();
    createSwapChain();
    createImageViews();

    while (!glfwWindowShouldClose((GLFWwindow *)m_Window))
    {
        glfwPollEvents();
    }
}

void HelloTriangleApp::cleanup()
{
    for(auto imageView : m_VkSwapChainImageViews) { 
        vkDestroyImageView(*(VkDevice *) m_VkDevice, *(VkImageView*)imageView, nullptr);
    }

    vkDestroySwapchainKHR(*(VkDevice *)m_VkDevice, *(VkSwapchainKHR*) m_VkSwapChain, nullptr);
    vkDestroyDevice(*(VkDevice *)m_VkDevice, nullptr);
    destroyDebugUtilsMessengerEXT(*(VkInstance *)m_VkInstance, *(VkDebugUtilsMessengerEXT *)m_VkDebugMessager, nullptr);
    vkDestroyInstance(*(VkInstance *)m_VkInstance, nullptr);
    glfwDestroyWindow((GLFWwindow *)m_Window);
    glfwTerminate();

    m_VkInstance = nullptr;
    m_Window = nullptr;
}