#ifndef TEST_H
#define TEST_H
#include <osapi/ThreadFunctor.hpp>
#include <osapi/ScoreSystem/I2C_reg.hpp>

class Test : public osapi::ThreadFunctor
{
private:
    void run();
};
#endif