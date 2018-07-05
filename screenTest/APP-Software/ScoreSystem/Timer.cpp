#include <osapi/ScoreSystem/Timer.hpp>
#include <unistd.h>

/*! Normal constructor. Sets the time of which it is to overflow, and what id it's supposed to pass along.
it also takes what message queue to place the message in.
*/
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

//! Stops the timer, so that it can be joined and deleted
void Timer::stopTimer()
{
    running_ = false;
}

//! ThreadFunctor function, Sends a message every X ms. 
void Timer::run()
{
    while(running_)
    {
        if(timeout_ > 999)
            sleep(timeout_/1000);
        else
            usleep(1000*timeout_);
        mq_->send(id_, new osapi::Message());
    }
}