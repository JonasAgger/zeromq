#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

using std::cout;
using std::endl;

int main()
{
    uint8_t x = 0;
    uint8_t y = 0;
    int fd_ = open("/dev/i2c-1", O_RDWR);
    ioctl(fd_, 0x0703, 0x06);
    while(1)
    {
        sleep(1);
        for(int i = 0; i < 5; i++)
        {
            uint8_t tries_ = 0;

            while((write(fd_, &x, 1) < 1) && tries_ < 3) {tries_++;}

            tries_ = 0;

            //while((read(fd_, &y, 1) < 1) && tries_ < 3) {tries_++;}

            //if (y == x) break;
        }
        cout << (int)x << "\t" << (int)y << endl;
        x++;
        if (x > 6) x = 0;
    }
    close(fd_);

    sleep(5);
}