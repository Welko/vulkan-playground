#ifndef HELLOWORLD_COMMON_H
#define HELLOWORLD_COMMON_H


// TODO: Do this only on debug mode!
//#ifdef DEBUG

// Check Vulkan
// a ... Command to be executed
#define M_CV(a) VkResult r = a; if (r != VK_SUCCESS) fprintf(stderr, "ERROR (Vulkan): %s SHOULD BE %i BUT IS %i (%s, line %i)\n", #a, VK_SUCCESS, r, __FILE__, __LINE__)

//#else
//#endif

#endif //HELLOWORLD_COMMON_H
