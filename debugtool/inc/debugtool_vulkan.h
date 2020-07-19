#ifndef HELLOWORLD_DEBUGTOOL_VULKAN_H
#define HELLOWORLD_DEBUGTOOL_VULKAN_H

#include "debugtool.h"
#include <vulkan/vulkan.h>
#include <map>

#ifdef DBT_DEBUG

// Check Vulkan
#define DBT_CV(statement) VkResult r = statement; if (r != VK_SUCCESS) fprintf(stderr, "ERROR (Vulkan): %s SHOULD BE %i BUT IS %i (%s, line %i)\n", #statement, VK_SUCCESS, r, __FILE__, __LINE__)

#else

#define DBT_CV(statement)

#endif

namespace dbt {

    void setupMessenger(const VkInstance &) DBT_IFDEBUG_IMPLEMENT_ELSE()
    void teardownMessenger(const VkInstance &) DBT_IFDEBUG_IMPLEMENT_ELSE()

    void getRequiredExtensions(std::vector<const char *> &out) DBT_IFDEBUG_IMPLEMENT_ELSE()
    void getRequiredLayers(std::vector<const char *> &out) DBT_IFDEBUG_IMPLEMENT_ELSE()

    void populateVkDebugUtilsMessengerCreateInfoEXT(VkDebugUtilsMessengerCreateInfoEXT &createInfo) DBT_IFDEBUG_IMPLEMENT_ELSE();

}

#endif //HELLOWORLD_DEBUGTOOL_VULKAN_H
