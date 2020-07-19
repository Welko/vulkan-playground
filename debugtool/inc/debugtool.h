#ifndef DEBUGTOOL_H
#define DEBUGTOOL_H

#include <vector>

// TODO remove
#define DBT_DEBUG

#ifdef DBT_DEBUG

#define DBT_IFDEBUG_EXECUTE(statement) statement
#define DBT_IFDEBUG_IMPLEMENT(ret) ;

#define DBT_LOG(string) printf("LOG %s\n", string)

//#define DBT_ERROR(string) printf("ERROR %s\n", string)
#define DBT_ERROR(string) fprintf(stderr, "ERROR: %s (%s, line %i)\n", string, __FILE__, __LINE__)

#else // NDEBUG

#define DBT_IFDEBUG_EXECUTE(statement)
#define DBT_IFDEBUG_IMPLEMENT(ret) { return ret; }
#define DBT_LOG(string)
#define DBT_ERROR(string)

#endif // DBT_DEBUG

namespace dbt {

}

#endif //DEBUGTOOL_H