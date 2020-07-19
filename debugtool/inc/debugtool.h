#ifndef DEBUGTOOL_H
#define DEBUGTOOL_H

#include <vector>

// TODO remove
#define DBT_DEBUG

#ifdef DBT_DEBUG

#define DBT_IFDEBUG_EXECUTE(statement) statement
#define DBT_IFDEBUG_IMPLEMENT_ELSE_RETURN(ret) ;

#define DBT_EXIT throw std::runtime_error("")

#define DBT_LOG(string) printf("%s\n", string)

//#define DBT_ERROR(string) printf("ERROR %s\n", string)
//#define DBT_ERROR(string) fprintf(stderr, "ERROR: %s (%s, line %i)\n", string, __FILE__, __LINE__)
#define DBT_ERROR(string) {fprintf(stderr, "%s (%s, line %i)\n", string, __FILE__, __LINE__); DBT_EXIT;}

#else // NDEBUG

#define DBT_IFDEBUG_EXECUTE(statement)
#define DBT_IFDEBUG_IMPLEMENT_ELSE_RETURN(ret) { return ret; }
#define DBT_LOG(string)
#define DBT_ERROR(string)
#define DBT_EXIT

#endif // DBT_DEBUG

namespace dbt {

}

#endif //DEBUGTOOL_H