#include "log.h"

using namespace mgl;
using namespace std;

#ifdef QT_CORE_LIB

ostream & Log::often()
{
    // return EZLOGGERVLSTREAM(axter::log_often);
    return std::cout;
}


ostream & Log::rarely()
{
    return std::cout;
}

#else

#define EZLOGGER_OUTPUT_FILENAME "ezlogger.txt"

ostream & Log::often()
{
    return EZLOGGERVLSTREAM(axter::log_often);

}


ostream & Log::rarely()
{
    return EZLOGGERVLSTREAM(axter::log_rarely);
}

#endif





ostream & Log::error()
{
    return cerr;
}

