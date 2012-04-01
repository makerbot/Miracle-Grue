#ifndef LOG_H
#define LOG_H

#include <iostream>

namespace mgl
{
    class Log
    {
    public:
        static std::ostream &error();
        static std::ostream &often();
        static std::ostream &rarely();
    };
}

#endif // LOG_H
