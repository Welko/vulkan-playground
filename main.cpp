#include "debuglib.h"

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
    }

    void createInstance() {
        uint32_t enabledLayerCount;
        const char *const * enabledLayerNames;
#ifdef MELO_DEBUG
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

        // Vulkan is a platform agnostic API, which means that you need an extension to interface with the window
        // system. GLFW has a handy built-in function that returns the extension(s) it needs to do that which we can
        // pass to the struct
        uint32_t extensionCount_glfw = 0;
        const char** extensions_glfw;
        extensions_glfw = glfwGetRequiredInstanceExtensions(&extensionCount_glfw);
        printf("Extensions required by GLFW:\n");
        for (int i = 0; i < extensionCount_glfw; ++i) {
            const auto extension = extensions_glfw[i];
            printf("  %s\n", extension);
        }

        // To retrieve a list of supported extensions before creating an instance
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        // Each VkExtensionProperties struct contains the name and version of an extension
        printf("Available extensions:\n");
        for (const auto &extension : extensions) {
            printf("  %s (version %i)\n", extension.extensionName, extension.specVersion);
        }

        // Not optional and tells the Vulkan driver which global extensions and validation layers we want to use. Global
        // here means that they apply to the entire program and not a specific device
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // must be VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        //createInfo.pNext = nullptr;
        //createInfo.flags = 0; // must be 0
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = enabledLayerCount;
        createInfo.ppEnabledLayerNames = enabledLayerNames;
        createInfo.enabledExtensionCount = extensionCount_glfw;
        createInfo.ppEnabledExtensionNames = extensions_glfw;

        // Finally issue the vkCreateInstance call
        DBT_CV(vkCreateInstance(&createInfo, nullptr, &mInstance)); // pAllocator (callbacks) nullptr TODO: change?
    }

    bool checkValidationLayerSupport(std::vector<const char*> requiredLayers) {
        // Check if all of the requested layers are available

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        printf("Required layers:\n");
        for (const auto &layerName : requiredLayers) {
            printf("  %s\n", layerName);
        }
        printf("Available layers:\n");
        for (const auto & layerProperties : availableLayers) {
            printf("  %s (spec version %i, impl version %i, description: %s)\n",
                    layerProperties.layerName,
                    layerProperties.specVersion,
                    layerProperties.implementationVersion,
                    layerProperties.description);
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

    void mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents(); // Checks for events (like pressing the "close" (X) button)
        }
    }

    void cleanup() {
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