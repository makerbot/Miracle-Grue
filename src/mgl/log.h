#ifndef LOG_H
#define LOG_H

#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#include "../../submodule/EzCppLog/ezlogger_headers.hpp"

namespace mgl
{

enum verbosity {
	log_default_level = 3,
	log_verbosity_unset = 0,
	log_severe = 1, //was always
	log_info, //was often
	log_fine, //was regularly
	log_finer, ///was rarely
	log_finest // was very_rarely
};

extern verbosity g_debugVerbosity;

class Log
    {
    public:
        static std::ostream &severe();
        static std::ostream &info();
        static std::ostream &fine();
        static std::ostream &finer();
        static std::ostream &finest();
    };
}

#endif // LOG_H
