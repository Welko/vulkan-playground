#ifndef DEBUGTOOL_H
#define DEBUGTOOL_H

// TODO: Do this only on debug mode!
#ifndef NDEBUG

#define DBT_DEBUG
#define DBT_IFDEBUG(statement) statement

// Check Vulkan
#define DBT_CV(statement) VkResult r = statement; if (r != VK_SUCCESS) fprintf(stderr, "ERROR (Vulkan): %s SHOULD BE %i BUT IS %i (%s, line %i)\n", #statement, VK_SUCCESS, r, __FILE__, __LINE__)

#define DBT_LOG(string) printf("LOG %s\n", string)

#define DBT_ERROR(string) printf("ERROR %s\n", string)

#else // NDEBUG

#define DBT_IFDEF(statement)
#define DBT_CV(statement) statement
#define DBT_LOG(string)
#define DBT_ERROR(string)

#endif // NDEBUG

#endif //DEBUGTOOL_H