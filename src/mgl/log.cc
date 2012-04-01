#include "log.h"

#ifndef QT_CORE_LIB
#define EZLOGGER_OUTPUT_FILENAME "ezlogger.txt"
#include "ezlogger/ezlogger_headers.hpp"
#endif

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



ostream &Log::often()
{
    return EZLOGGERVLSTREAM(axter::log_often).get_log_stream();

}


ostream &Log::rarely()
{
    return EZLOGGERVLSTREAM(axter::log_rarely).get_log_stream();
}

#endif





ostream & Log::error()
{
    return cerr;
}

