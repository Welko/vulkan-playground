#include "debugtool_vulkan.h"

namespace dbt {
#ifdef DBT_DEBUG

    auto messengerMap = std::map<const VkInstance, VkDebugUtilsMessengerEXT>();

    namespace {
        inline bool isInstanceValid(const VkInstance &instance) {
            return instance != VK_NULL_HANDLE;
        }

        inline VkResult CreateDebugUtilsMessengerEXT(
                VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                const VkAllocationCallbacks *pAllocator,
                VkDebugUtilsMessengerEXT *pDebugMessenger) {

            if (!isInstanceValid(instance)) {
                DBT_ERROR("Unable to create PFN_vkCreateDebugUtilsMessengerEXT: " \
                      "Given VkInstance is invalid (probably == VK_NULL_HANDLE)");
            }

            auto procAddr = vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) procAddr;
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        inline void DestroyDebugUtilsMessengerEXT(
                VkInstance instance,
                VkDebugUtilsMessengerEXT debugMessenger,
                const VkAllocationCallbacks *pAllocator) {

            if (!isInstanceValid(instance)) {
                DBT_ERROR("Unable to destroy PFN_vkCreateDebugUtilsMessengerEXT: " \
                      "Given VkInstance is invalid (probably == VK_NULL_HANDLE)");
            }

            auto procAddr = vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) procAddr;
            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }

        inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {

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

            // TODO replace by own system
            std::string msg = "Validation layer: ";
            msg += pCallbackData->pMessage;
            DBT_ERROR(msg.c_str());

            return VK_FALSE;
        }
    }

    void setupMessenger(const VkInstance &instance) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional. Will be passed along to the callback function

        // If k does not match the key of any element in the container, the function inserts a new element with that key
        // and returns a reference to its mapped value
        VkDebugUtilsMessengerEXT &debugMessengerReference = messengerMap[instance];

        DBT_CV(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessengerReference));
    }

    void teardownMessenger(const VkInstance &instance) {
        //auto debugMessengerReference = map[instance];
        auto ite = messengerMap.find(instance);
        if (ite == messengerMap.end()) {
            DBT_ERROR("Unable to tear down VkDebugUtilsMessengerEXT associated with given VkInstance: " \
                      "VkInstance not found");
            return;
        }
        VkDebugUtilsMessengerEXT &debugMessengerReference = ite->second;
        DestroyDebugUtilsMessengerEXT(instance, debugMessengerReference, nullptr);
    }

    std::vector<const char *> getRequiredExtensions() {
        return {
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };
    }

    std::vector<const char *> getRequiredLayers() {
        return {
                "VK_LAYER_KHRONOS_validation"
        };
    }

#endif
} // end of namespace dbt
