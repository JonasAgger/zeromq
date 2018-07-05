#include <osapi/Mutex.hpp>

namespace osapi
{
    Mutex::Mutex()
    {
        mut = new pthread_mutex_t;
        if (pthread_mutex_init(mut, nullptr) != 0) throw MutexError();
    }

    Mutex::~Mutex()
    {
        pthread_mutex_unlock(mut);
        pthread_mutex_destroy(mut);
    }

    void Mutex::lock()
    {
        if (pthread_mutex_lock(mut) != 0) throw MutexError();
    }

    void Mutex::unlock()
    {
        if (pthread_mutex_unlock(mut) != 0) throw MutexError();
    }
}