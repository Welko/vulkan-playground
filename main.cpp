#ifndef NDEBUG
#define DBT_DEBUG
#endif
//#include "debugtool.h"
#include "debugtool_vulkan.h"

#define MELO_EXIT(msg) throw std::runtime_error(msg)

// "GLFW will include its own definitions and automatically load the Vulkan header with it"
// Source: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Base_code
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow() {
        glfwInit();

        // Don't create OpenGL context
        // After all, we're using Vulkan...
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // Resizable windows require special care... TODO: we'll deal with it later
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Last param. is only relevant to OpenGL
        mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "HelloWorld (Vulkan)", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        dbt::setupMessenger(mInstance);
        pickPhysicalDevice();
    }

    void pickPhysicalDevice() {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            MELO_EXIT("No GPU was found that supports Vulkan");
        }
        std::string msg = "";
        msg += std::to_string(deviceCount);
        msg += " device(s) found";
        DBT_LOG(msg.c_str());

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());
        for (const auto &device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                //break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            MELO_EXIT("No suitable GPU found");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        // Basic device properties like the name, type and supported Vulkan version
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // Optional features like texture compression, 64 bit floats and multi viewport rendering
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

#ifdef DBT_DEBUG
        std::string msg = "";
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            msg += "(discrete GPU) ";
        } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            msg += "(integrated GPU) ";
        } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
            msg += "(CPU) ";
        }
        msg += deviceProperties.deviceName;
        DBT_LOG(msg.c_str());
#endif

        return true;
    }

    void createInstance() {
        std::vector<const char *> requiredLayers;
        dbt::getRequiredLayers(requiredLayers);
        uint32_t requiredLayerCount = requiredLayers.size();

        if (!checkLayerSupport(requiredLayers)) {
            //DBT_ERROR("Some requested layers were not available.");
            MELO_EXIT("Some requested layers were not available.");
        }

        // This data is technically optional, but it may provide some useful information to the driver in order to
        // optimize our specific application (e.g. because it uses a well-known graphics engine with certain special
        // behavior)
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // must be VK_STRUCTURE_TYPE_APPLICATION_INFO
        //appInfo.pNext = nullptr; // must be NULL
        appInfo.pApplicationName = "HelloWorld";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        //appInfo.pEngineName = nullptr;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        auto extensions = getRequiredExtensions();

        void *pNext = nullptr;
#ifdef DBT_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        dbt::populateVkDebugUtilsMessengerCreateInfoEXT(debugCreateInfo);
        pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#endif

        // Not optional and tells the Vulkan driver which global extensions and validation layers we want to use. Global
        // here means that they apply to the entire program and not a specific device
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // must be VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        createInfo.pNext = pNext;
        //createInfo.flags = 0; // must be 0
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = requiredLayerCount;
        createInfo.ppEnabledLayerNames = requiredLayers.data();
        createInfo.enabledExtensionCount = (uint32_t) extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Finally issue the vkCreateInstance call
        DBT_CV(vkCreateInstance(&createInfo, nullptr, &mInstance)); // pAllocator (callbacks) nullptr TODO: change?
    }

    bool checkLayerSupport(std::vector<const char*> requiredLayers) {
        // Check if all of the requested layers are available

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        printf("Available layers:\n");
        for (const auto & layerProperties : availableLayers) {
            printf("  %s (spec version %i, impl version %i, description: %s)\n",
                    layerProperties.layerName,
                    layerProperties.specVersion,
                    layerProperties.implementationVersion,
                    layerProperties.description);
        }
        printf("Required layers:\n");
        for (const auto &layerName : requiredLayers) {
            printf("  %s\n", layerName);
        }

        for (const char* layerName : requiredLayers) {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                printf("Required layer %s not available\n", layerName);
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> getRequiredExtensions() {
        // Vulkan is a platform agnostic API, which means that you need an extension to interface with the window
        // system. GLFW has a handy built-in function that returns the extension(s) it needs to do that which we can
        // pass to the struct
        uint32_t extensionCount_glfw = 0;
        const char * * extensions_glfw;
        extensions_glfw = glfwGetRequiredInstanceExtensions(&extensionCount_glfw);

        std::vector<const char*> extensions(extensions_glfw, extensions_glfw + extensionCount_glfw);

        std::vector<const char*> requiredExtensions;
        dbt::getRequiredExtensions(requiredExtensions);
        for (const auto &extension : requiredExtensions) {
            extensions.push_back(extension);
        }

        // To retrieve a list of supported extensions before creating an instance
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions_supported(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions_supported.data());
        // Each VkExtensionProperties struct contains the name and version of an extension
        printf("Available extensions:\n");
        for (const auto &extension : extensions_supported) {
            printf("  %s (version %i)\n", extension.extensionName, extension.specVersion);
        }

        printf("Required extensions:\n");
        for (const auto &extension : extensions) {
            printf("  %s\n", extension);
        }

        return extensions;
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents(); // Checks for events (like pressing the "close" (X) button)
        }
    }

    void cleanup() {
        dbt::teardownMessenger(mInstance);
        vkDestroyInstance(mInstance, nullptr);

        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    GLFWwindow *mWindow;
    uint32_t mWindowWidth = 800;
    uint32_t mWindowHeight = 600;

    VkInstance mInstance;

};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}