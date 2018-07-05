#include <osapi/ScoreSystem/Timer.hpp>
#include <unistd.h>

Timer::Timer(unsigned long timeout, unsigned long id, osapi::MsgQueue *mq)
{
    timeout_ = timeout;
    id_ = id;
    mq_ = mq;
}

Timer::~Timer()
{
    running_ = false;
    mq_ = nullptr;
}

void Timer::stopTimer()
{
    running_ = false;
}

void Timer::run()
{
    while(running_)
    {
        usleep(1000*timeout_);
        mq_->send(id_, new osapi::Message());
    }
}