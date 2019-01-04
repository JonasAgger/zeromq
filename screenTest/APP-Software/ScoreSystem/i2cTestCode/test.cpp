#include <osapi/ScoreSystem/test.hpp>
#include <unistd.h>

void Test::run()
{
    int x = 0;
    
    while(1)
    {
        sleep(5);
        I2C_reg::getInstance().setArduinoMessage(x);
        x++;
        if (x > 6) x = 0;
    }
}