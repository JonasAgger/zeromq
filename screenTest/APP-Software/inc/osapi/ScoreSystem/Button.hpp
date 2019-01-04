#ifndef BUTTON_H_
#define BUTTON_H_

//OSAPI INCLUDES
#include <osapi/ThreadFunctor.hpp>
#include <osapi/MsgQueue.hpp>
#include <osapi/Message.hpp>
// LINUX INCLUDES
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

struct buttonMessage : public osapi::Message
{
    buttonMessage(uint8_t b) : x(b) {}
    uint8_t x;
};

enum buttonEvent
{
    btnPressed,
    btnRight,
    btnLeft
};

class Button : public osapi::ThreadFunctor
{
public:
    Button(osapi::MsgQueue *mq);
    ~Button();
private:
    void run();

    bool running = false;
    int fd; // File descriptor
    char value[2];

    osapi::MsgQueue *mq_;
};

#endif