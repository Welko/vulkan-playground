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

            std::string msg = "Validation layer: ";
            bool crash = false;

            switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    msg += "VERBOSE";
                    break; // Diagnostic message
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    msg += "INFO   ";
                    break; // Informational message like the creation of a resource
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    msg += "WARNING";
                    break; // Message about behavior that is not necessarily an error, but very likely a bug in your application
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    msg += "ERROR  ";
                    crash = true;
                    break; // Message about behavior that is invalid and may cause crashes
                default:
                    msg += "UNKNOWN";
                    break;
            }
            msg += " ";

            switch (messageType) {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    msg += "GENERAL    ";
                    break; // Some event has happened that is unrelated to the specification or performance
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    msg += "VALIDATION ";
                    break; // Something has happened that violates the specification or indicates a possible mistake
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    msg += "PERFORMANCE";
                    break; // Potential non-optimal use of Vulkan
                default:
                    break;
            }
            msg += " ";

            // TODO include following information
            // The pCallbackData parameter refers to a VkDebugUtilsMessengerCallbackDataEXT struct containing the details of
            //   the message itself, with the most important members being:
            //
            //    pMessage: The debug message as a null-terminated string
            //    pObjects: Array of Vulkan object handles related to the message
            //    objectCount: Number of objects in array

            // Finally, the pUserData parameter contains a pointer that was specified during the setup of the callback and
            //   allows you to pass your own data to it.

            // TODO replace by own system
            msg += pCallbackData->pMessage;
            //DBT_ERROR(msg.c_str());

            // TODO better way?
            if (crash) throw std::runtime_error(msg);
            else DBT_ERROR(msg.c_str());

            return VK_FALSE;
        }
    }

    void populateVkDebugUtilsMessengerCreateInfoEXT(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional. Will be passed along to the callback function
    }

    void setupMessenger(const VkInstance &instance) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateVkDebugUtilsMessengerCreateInfoEXT(createInfo);

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

    void getRequiredExtensions(std::vector<const char *> &out) {
        out = {
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };
    }

    void getRequiredLayers(std::vector<const char *> &out) {
        out = {
                "VK_LAYER_KHRONOS_validation"
        };
    }

#endif
} // end of namespace dbt
