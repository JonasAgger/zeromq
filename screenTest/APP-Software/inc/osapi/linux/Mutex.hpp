#ifndef OSAPI_LINUX_MUTEX_HPP
#define OSAPI_LINUX_MUTEX_HPP

#include <pthread.h>

namespace osapi
{
    class Conditional;


    class Mutex
    {
    public:
        Mutex();
        ~Mutex();
        void lock();
        void unlock();

    private:
      friend class Conditional;
        pthread_mutex_t *mut;
    };
}

#endif
