#ifndef DEBUGLIB_H
#define DEBUGLIB_H

// TODO: Do this only on debug mode!
//#ifdef DEBUG


// Check Vulkan
// a ... Command to be executed
#define DBT_CV(a) VkResult r = a; if (r != VK_SUCCESS) fprintf(stderr, "ERROR (Vulkan): %s SHOULD BE %i BUT IS %i (%s, line %i)\n", #a, VK_SUCCESS, r, __FILE__, __LINE__)

#define DBT_LOG(a) printf("LOG %s", #a)


//#else // DEBUG
//#endif // DEBUG

#endif //DEBUGLIB_H