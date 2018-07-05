#ifndef TIMER_H_
#define TIMER_H_
#include <osapi/ThreadFunctor.hpp>
#include <osapi/MsgQueue.hpp>


class Timer : public osapi::ThreadFunctor
{
public:
    Timer(unsigned long timeout, unsigned long id, osapi::MsgQueue *mq);
    virtual ~Timer();

    void stopTimer();

private:
    void run();

    osapi::MsgQueue *mq_ = nullptr;
    unsigned long id_ = 0;
    unsigned long timeout_ = 0;

    bool running_ = true;
};

#endif