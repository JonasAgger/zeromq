#include <osapi/ScoreSystem/Button.hpp>

//! constructor. Opens the Button node in /dev and sets the message queue
Button::Button(osapi::MsgQueue *mq) : mq_(mq)
{
    fd = open("/dev/Button", O_RDONLY);
}

Button::~Button()
{
    close(fd);
}

/*! Tries to read a value in from the button node every 25ms. The read is blocking in the driver
so it will only return once there's new data avaliable, and once there is, it returns to the function
and it's able to send that data to the main thread via the message queue 
*/
void Button::run()
{
	// Cleaning up input, by resetting it. This is done by just reading

    while(1)
	{
        value[0] = '5';
		read(fd, value, 1);

		if(value[0] == '1') mq_->send(buttonEvent::btnPressed, new buttonMessage(5));
		else if(value[0] == '2') mq_->send(buttonEvent::btnRight, new buttonMessage(5));
		else if(value[0] == '3') mq_->send(buttonEvent::btnLeft, new buttonMessage(5));

		usleep(25000);
	}
}