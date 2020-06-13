#include "common.h"

// "GLFW will include its own definitions and automatically load the Vulkan header with it"
// Source: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Base_code
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

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

        // Not optional and tells the Vulkan driver which global extensions and validation layers we want to use. Global
        // here means that they apply to the entire program and not a specific device
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // must be VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
        //createInfo.pNext = nullptr;
        //createInfo.flags = 0; // must be 0
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0; // TODO: Determine the global validation layers to enable
        //createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = extensionCount_glfw;
        createInfo.ppEnabledExtensionNames = extensions_glfw;

        // Finally issue the vkCreateInstance call
        M_CV(vkCreateInstance(&createInfo, nullptr, &mInstance)); // pAllocator (callbacks) nullptr TODO: change?

        printf("Instance should have been created successfully now! :D");
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents(); // Checks for events (like pressing the "close" (X) button)
        }
    }

    void cleanup() {
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