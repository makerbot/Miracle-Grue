#ifdef _WIN32
#include <windows.h>
#endif

//#undef QT_CORE_LIB
#ifndef QT_CORE_LIB
#define EZLOGGER_OUTPUT_FILENAME "ezlogger.txt"
#include "ezlogger_headers.hpp"
#endif

#include "log.h"

using namespace mgl;
using namespace std;

ostream & Log::severe()
{
    return cerr;
}

verbosity mgl::g_debugVerbosity = log_finest;


#ifdef QT_CORE_LIB


struct nullstream : std::ostream {
    nullstream() : std::ios(0), std::ostream(0) {}
};

static nullstream nullout;



ostream & Log::info()
{

	if( g_debugVerbosity >= log_info )
		return std::cout;
	return nullout;
}


ostream & Log::fine()
{
	if( g_debugVerbosity >= log_fine )
		return std::cout;
	return nullout;

}

ostream & Log::finer()
{
	if(g_debugVerbosity >= log_finer )
		return std::cout;
	return nullout;
}

ostream & Log::finest()
{
	if(g_debugVerbosity >= log_finer )
		return std::cout;
	return nullout;
}

#else



ostream &Log::info()
{
    return EZLOGGERVLSTREAM(axter::log_info).get_log_stream();
}

ostream &Log::fine()
{
    return EZLOGGERVLSTREAM(axter::log_fine).get_log_stream();
}

ostream &Log::finer()
{
    return EZLOGGERVLSTREAM(axter::log_finer).get_log_stream();
}


ostream &Log::finest()
{
    return EZLOGGERVLSTREAM(axter::log_finest).get_log_stream();
}

/*
 * std::ostream &severe() {
	EZLOGGERVLSTREAM(axter::log_severe).get_log_stream();
}
 */
#endif
