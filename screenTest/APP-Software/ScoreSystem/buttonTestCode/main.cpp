#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

using namespace std;

uint8_t formerValue = 0;

int main()
{
	// pthread_t pressed, turnLeft, turnRight;



	//fstream button("/dev/Button");
	int button = open("/dev/Button", O_RDONLY);
	//cout << pressed.gcount() << endl;
	
	/* ------------------------ 
	TODO:

		Implement the button functionality in the driver instead of the user space file

	---------------------------*/



	while(1)
	{
		char value[2];
		//string value;
		read(button, value, 1);
		//button >> value;

		if(value[0] == '1') 
		{
			cout << "Button pressed" << endl;
			formerValue = 0;
		}
		else if(value[0] == '2') 
		{
			if (formerValue == 0)
			{
				formerValue = 0;
				cout << "Rotate Left" << endl;
			}
			else formerValue = 0;
		}
		else if(value[0] == '3') 
		{
			if (formerValue == 0)
			{
				formerValue = 0;
				cout << "Rotate Right" << endl;
			}
			else formerValue = 0;
		}

		value[0] = '5';
		usleep(10000);
		//cout << value << endl;
	}
	return 0;
}
