#include <unistd.h>
#include <osapi/Utility.hpp>

namespace osapi
{
    void sleep(unsigned long msecs)
    {
        usleep(1000*msecs);
    }
}
