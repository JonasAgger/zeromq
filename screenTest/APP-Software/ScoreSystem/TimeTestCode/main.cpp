#include <osapi/ClockTime.hpp>
#include <iostream>
#include <unistd.h>
#include <string>

using std::cout;
using std::endl;

using namespace osapi;

int main()
{
    cout << getCurrentTime().secs() << endl << getCurrentMonotonicTime().secs() << endl;

    sleep(5);

    cout << getCurrentTime().secs() << endl << getCurrentMonotonicTime().secs() << endl;
}