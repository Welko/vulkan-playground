#include "debugtool.h"

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
        DBT_IFDEBUG(setupDebugMessenger());
    }

    void createInstance() {
        uint32_t enabledLayerCount;
        const char *const * enabledLayerNames;
#ifdef DBT_DEBUG
        const std::vector<const char*> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };
        if (!checkValidationLayerSupport(validationLayers)) {
            DBT_ERROR("Validation layers requested, but not available!");
        }
        enabledLayerCount = validationLayers.size();
        enabledLayerNames = validationLayers.data();
#else
        enabledLayerCount = 0;
        enabledLayerNames = nullptr;
#endif

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

        // Not optional and tells the Vulkan driver which global extensions and validation layers we want to use. Global
        // here means that they apply to the entire program and not a specific device
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // must be VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        //createInfo.pNext = nullptr;
        //createInfo.flags = 0; // must be 0
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = enabledLayerCount;
        createInfo.ppEnabledLayerNames = enabledLayerNames;
        createInfo.enabledExtensionCount = (uint32_t) extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Finally issue the vkCreateInstance call
        DBT_CV(vkCreateInstance(&createInfo, nullptr, &mInstance)); // pAllocator (callbacks) nullptr TODO: change?
    }

    bool checkValidationLayerSupport(std::vector<const char*> requiredLayers) {
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

#ifdef DBT_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

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

#ifdef DBT_DEBUG
    void setupDebugMessenger() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional. Will be passed along to the callback function

        DBT_CV(CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger));
    }

    VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger) {

        auto procAddr = vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) procAddr;
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator) {

        auto procAddr = vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) procAddr;
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

        // The first parameter specifies the severity of the message, which is one of the following flags:
        //
        //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
        //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
        //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error,
        //      but very likely a bug in your application
        //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause
        //      crashes
        //
        // The values of this enumeration are set up in such a way that you can use a comparison operation to check if a
        //   message is equal or worse compared to some level of severity

        // The messageType parameter can have the following values:
        //
        //    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the
        //      specification or performance
        //    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened that violates the specification or
        //      indicates a possible mistake
        //    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non-optimal use of Vulkan

        // The pCallbackData parameter refers to a VkDebugUtilsMessengerCallbackDataEXT struct containing the details of
        //   the message itself, with the most important members being:
        //
        //    pMessage: The debug message as a null-terminated string
        //    pObjects: Array of Vulkan object handles related to the message
        //    objectCount: Number of objects in array

        // Finally, the pUserData parameter contains a pointer that was specified during the setup of the callback and
        //   allows you to pass your own data to it.

        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
#endif

    void mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents(); // Checks for events (like pressing the "close" (X) button)
        }
    }

    void cleanup() {
        DBT_IFDEBUG(DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr));
        vkDestroyInstance(mInstance, nullptr);

        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    GLFWwindow *mWindow;
    uint32_t mWindowWidth = 800;
    uint32_t mWindowHeight = 600;

    VkInstance mInstance;

#ifdef DBT_DEBUG
    VkDebugUtilsMessengerEXT mDebugMessenger;
#endif
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