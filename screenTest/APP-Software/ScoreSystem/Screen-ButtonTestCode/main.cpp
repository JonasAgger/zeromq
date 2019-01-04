#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <osapi/ScoreSystem/LCD.hpp>

using namespace std;

int main()
{
	LCD display;

    display.begin();

	int button = open("/dev/Button", O_RDONLY);

	while(1)
	{
		uint8_t value[2];
		read(button, value, 1);
		if(value[0] == '1') 
		{
			display.clear();
			display.stringWrite("Button pressed");
		}
		else if(value[0] == '2')
		{
			display.clear();
			display.stringWrite("Rotate Left");
		} 
		else if(value[0] == '3')
		{
			display.clear();
			display.stringWrite("Rotate Right");
		} 

		value[0] = 0;
		usleep(25000);
	}
	return 0;
}
