#include <osapi/ScoreSystem/I2C_reg.hpp>
#include <iostream>

//! Constructor. Opens /dev/i2c-1. Creates the Message queue
I2C_reg::I2C_reg()
{
    fd_ = open("/dev/i2c-1", O_RDWR);
    mq_ = new osapi::MsgQueue(20);
}

//! Constructor. Closes /dev/i2c-1. Deletes the Message queue
I2C_reg::~I2C_reg()
{
    close(fd_);
    delete mq_;
}

void I2C_reg::testStatic()
{
    std::cout << pthread_self() << std::endl;
}

//! Function to be used by the LCD to write to the display
void I2C_reg::displayWrite(uint8_t addr, uint8_t data)
{
    if(currentAddr_ != screenAdress_)
    {
        ioctl(fd_, 0x0703, screenAdress_);
        currentAddr_ = screenAdress_;
    }

    uint8_t sendData[2] = {addr, data};
    write(fd_, sendData, 2);
}

//! Function to be used by the LCD to read from display registers
uint8_t I2C_reg::displayRead(uint8_t addr)
{
    uint8_t val;
    if(currentAddr_ != screenAdress_)
    {
        ioctl(fd_, 0x0703, screenAdress_);
        currentAddr_ = screenAdress_;
    }

    write(fd_, &addr, 1);

    read(fd_, &val, 1);

    return val;
}

//! Function to be used by the LCD to initialize the display
void I2C_reg::displayInit()
{
    mut_.lock();
    char val0 = 0xff;
    char val1 = 0x00;
    // set defaults!
    write(fd_, &val1, 1);
    write(fd_, &val0, 1);  // all inputs
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);
    write(fd_, &val1, 1);

    mut_.unlock();
}

//! ThreadFunctor function. Thread function
void I2C_reg::run()
{
    while(running_)
    {
        unsigned long id;
        osapi::Message* m = mq_->receive(id);
        handleMsg(id, m);
        delete m;
    }
}

//! Function to handle messages received in the message queue
void I2C_reg::handleMsg(unsigned long id, osapi::Message *msg)
{
    switch(id)
    {
        case TIMER_OUT:
            psocUpdate();
            break;

        case RFID_TIMER_OUT:
            rfidUpdate();
            break;

        case ARDUINOMESSGE:
            sendArduinoMessage();
            break;

        case PSOCBROADCAST:
            sendPsocBroadcast();
            break;

        case PSOC1MESSAGE:
            sendPsocMessage(1);
            break;

        case PSOC2MESSAGE:
            sendPsocMessage(2);
            break;
        
    }
}

//! Function which starts the timers to get updates from the psocs, and sets nr. of cups on the psocs
void I2C_reg::setUP(int cups)
{
    if(cups == 6) psocMessage_ = 0x06;
    else psocMessage_ = 0x0A;
    //psocMessage_ = 1;

    sendPsocBroadcast();
    /* TESTING NEW TIMER
    timer1_ = osapi::createNewTimer(mq_, i2c_messages::TIMER_OUT);
    timer1_->arm(100); // Timeout in 0.1sec
    */
    if (timer1_) stopPsocPolling();

    timer1_ = new Timer(100, i2c_messages::TIMER_OUT, mq_); // REMEMBER TO SET TO 100
    tt_ = new osapi::Thread(timer1_);
    tt_->start();
}

//! Function that disables the timer polling the psocs for updates.
void I2C_reg::stopPsocPolling()
{
    if(timer1_)
    {
        timer1_->stopTimer();
        tt_->join();
        delete tt_;
        delete timer1_;
    }
    timer1_ = nullptr;
}

//! Function to send a message to the arduino
void I2C_reg::setArduinoMessage(uint8_t message)
{
    arduinoMessage_ = message;
    mq_->send(i2c_messages::ARDUINOMESSGE, new osapi::Message());
}

//! Function to send a message to either both psocs, by leaving it blank, or a specific psoc by writing 1 or 2
void I2C_reg::setPsocMessage(uint8_t message, uint8_t pSoC)
{
    psocMessage_ = message;
    if(pSoC == 1) 
    {
        receivingPsoC_ = pSoC;
        mq_->send(i2c_messages::PSOC1MESSAGE, new osapi::Message());
    }
    else if(pSoC == 2) 
    {
        receivingPsoC_ = pSoC;
        mq_->send(i2c_messages::PSOC2MESSAGE, new osapi::Message());
    }
    else mq_->send(i2c_messages::PSOCBROADCAST, new osapi::Message());
}

//! Private function which gets updates from the psocs.
void I2C_reg::psocUpdate()
{
    uint8_t val = 255;
    mut_.lock();
    ioctl(fd_, 0x0703, psoc1Adress_);
    currentAddr_ = psoc1Adress_;

    uint8_t tries_ = 0; // Tries for I2C-comm

    while ((read(fd_, &val, 1) < 1) && tries_ < 3) {tries_++;}
    if(!((val & 0x0f) == 0x0f)) ScoreSystemMQ_->send(PSOC1UPDATE, new psocUpdateMessage(val));

    //if(val != 255) 
    std::cout << "PSOC1: " << (int)(val & 0x0f) << " " << (int)(val>>4); // Used shortly for debugging..

    //Resetting value
    val = 255;

    ioctl(fd_, 0x0703, psoc2Adress_);
    currentAddr_ = psoc2Adress_;

    tries_ = 0;

    while ((read(fd_, &val, 1) < 1) && tries_ < 3) {tries_++;}
    if(!((val & 0x0f) == 0x0f)) ScoreSystemMQ_->send(PSOC2UPDATE, new psocUpdateMessage(val));

    //if(val != 255) 
    std::cout << " --- PSOC2: " << (int)(val & 0x0f) << " " << (int)(val>>4) << std::endl; // Used shortly for debugging..

    mut_.unlock();
    
    //timer1_->reArm();
}

//! Function which sends messages to both psocs. private
void I2C_reg::sendPsocBroadcast()
{
    //std::cout << "Hello from sendPsocMessage" << (int)psocMessage_ << std::endl;
    
    mut_.lock();

    ioctl(fd_, 0x0703, psoc1Adress_);
    currentAddr_ = psoc1Adress_;

    uint8_t tries_ = 0; // Tries for I2C-comm

    while((write(fd_, &psocMessage_, 1) < 1) && tries_ < 3) {tries_++;}

    ioctl(fd_, 0x0703, psoc2Adress_);
    currentAddr_ = psoc2Adress_;

    tries_ = 0;

    while((write(fd_, &psocMessage_, 1) < 1) && tries_ < 3) {tries_++;}

    mut_.unlock();
    psocMessage_ = 0;
}

//! Function which sends messages to a specific psoc. private.
void I2C_reg::sendPsocMessage(uint8_t psocNr)
{
    mut_.lock();
    if(psocNr == 1) sendPsoc1Message();
    else sendPsoc2Message();

    psocMessage_ = 0;
    mut_.unlock();
}

//! Function which sends a message to psoc 1. private
void I2C_reg::sendPsoc1Message()
{
    ioctl(fd_, 0x0703, psoc1Adress_);
    currentAddr_ = psoc1Adress_;

    write(fd_, &psocMessage_, 1);
}

//! Function which sends a message to psoc 2. private
void I2C_reg::sendPsoc2Message()
{
    ioctl(fd_, 0x0703, psoc2Adress_);
    currentAddr_ = psoc2Adress_;

    write(fd_, &psocMessage_, 1);
}

//! Function which sends a message to the arduino. private
void I2C_reg::sendArduinoMessage()
{    
    mut_.lock();
    ioctl(fd_, 0x0703, arduinoAdress_);
    currentAddr_ = arduinoAdress_;

    uint8_t checkSum = 0;

    for(int i = 0; i < 5; i++) // FUCK ARDUINO I2C
    {
        uint8_t tries_ = 0;

        while((write(fd_, &arduinoMessage_, 1) < 1) && tries_ < 3) {tries_++;}

        tries_ = 0;

        while((read(fd_, &checkSum, 1) < 1) && tries_ < 3) {tries_++;}

        if (checkSum == arduinoMessage_) break;
    }
    mut_.unlock();
    arduinoMessage_ = 0;
    
}

void I2C_reg::rfidUpdate()
{
    if(!timer2_) return;

    mut_.lock();
    ioctl(fd_, 0x0703, rfidAdress_); // RFID ADRESS
    currentAddr_ = rfidAdress_;
    

    uint8_t val[4] = {0};

    uint8_t tries_ = 0; // Tries for I2C-comm

    while ((read(fd_, &val[0], 1) < 1) && tries_ < 3) {tries_++;}
    tries_ = 0;

    while ((read(fd_, &val[1], 1) < 1) && tries_ < 3) {tries_++;}
    tries_ = 0;

    while ((read(fd_, &val[2], 1) < 1) && tries_ < 3) {tries_++;}
    tries_ = 0;

    while ((read(fd_, &val[3], 1) < 1) && tries_ < 3) {tries_++;}
    tries_ = 0;

    for(int i = 0; i < 4; i++)
    {      
        if (val[i] != 0) 
        {
            ScoreSystemMQ_->send(RFIDUPDATE, new rfidUpdateMessage(val[0], val[1], val[2], val[3]));
            break;
        }
    }
    mut_.unlock();
}

void I2C_reg::startRfidPolling()
{
    if(timer2_) return;

    timer2_ = new Timer(500, i2c_messages::RFID_TIMER_OUT, mq_); // REMEMBER TO SET TO 100
    tt2_ = new osapi::Thread(timer2_);
    tt2_->start();
}

void I2C_reg::stopRfidPolling()
{
    if(timer2_)
    {
        timer2_->stopTimer();
        tt2_->join();
        delete tt2_;
        delete timer2_;
    }
    timer2_ = nullptr;
}

void I2C_reg::forcePsocMessage(uint8_t message)
{
    psocMessage_ = message;
    uint8_t returnVal = 0;
    mut_.lock();
    if (message == 0x02)
    {
        for(int i = 0; i < 1000; i++)
        {
            sendPsoc1Message();
            usleep(5);
            read(fd_, &returnVal, 1);

            if (returnVal == 255) break;
        }

        returnVal = 0;

        for(int i = 0; i < 1000; i++)
        {
            sendPsoc2Message();
            usleep(5);
            read(fd_, &returnVal, 1);

            if (returnVal == 255) break;
        }
    }
    mut_.unlock();
    psocMessage_ = 0;
}

//! Function to lock the mutex. Used by the LCD class.
void I2C_reg::lockI2C()
{
    mut_.lock();
}

//! Function to unlock the mutex. Used by the LCD class.
void I2C_reg::unlockI2C()
{
    mut_.unlock();
}