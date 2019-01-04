#include <osapi/ScoreSystem/I2C_reg.hpp>
#include <iostream>

I2C_reg::I2C_reg()
{
    fd_ = open("/dev/i2c-1", O_RDWR);
    mq_ = new osapi::MsgQueue(20);
}

I2C_reg::~I2C_reg()
{
    close(fd_);
    delete mq_;
}

void I2C_reg::testStatic()
{
    std::cout << pthread_self() << std::endl;
}

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

void I2C_reg::handleMsg(unsigned long id, osapi::Message *msg)
{
    switch(id)
    {
        case TIMER_OUT:
            psocUpdate();
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
    timer1_ = new Timer(100, i2c_messages::TIMER_OUT, mq_); // REMEMBER TO SET TO 100
    tt_ = new osapi::Thread(timer1_);
    tt_->start();
}

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

void I2C_reg::setArduinoMessage(uint8_t message)
{
    arduinoMessage_ = message;
    mq_->send(i2c_messages::ARDUINOMESSGE, new osapi::Message());
}

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

void I2C_reg::psocUpdate()
{
    uint8_t val;
    mut_.lock();
    ioctl(fd_, 0x0703, psoc1Adress_);
    currentAddr_ = psoc1Adress_;

    while (read(fd_, &val, 1) < 1);
    if(!((val & 0x0f) == 0x0f)) ScoreSystemMQ_->send(PSOC1UPDATE, new psocUpdateMessage(val));

    //if(val != 255) 
    std::cout << "PSOC1: " << (int)(val & 0x0f) << " " << (int)(val>>4); // Used shortly for debugging..

    ioctl(fd_, 0x0703, psoc2Adress_);
    currentAddr_ = psoc2Adress_;

    while (read(fd_, &val, 1) < 1);
    if(!((val & 0x0f) == 0x0f)) ScoreSystemMQ_->send(PSOC2UPDATE, new psocUpdateMessage(val));

    //if(val != 255) 
    std::cout << " --- PSOC2: " << (int)(val & 0x0f) << " " << (int)(val>>4) << std::endl; // Used shortly for debugging..

    mut_.unlock();
    
    //timer1_->reArm();
}

void I2C_reg::sendPsocBroadcast()
{
    //std::cout << "Hello from sendPsocMessage" << (int)psocMessage_ << std::endl;
    
    mut_.lock();

    ioctl(fd_, 0x0703, psoc1Adress_);
    currentAddr_ = psoc1Adress_;

    while(write(fd_, &psocMessage_, 1) < 1);

    ioctl(fd_, 0x0703, psoc2Adress_);
    currentAddr_ = psoc2Adress_;

    while (write(fd_, &psocMessage_, 1) < 1);

    mut_.unlock();
    psocMessage_ = 0;
}

void I2C_reg::sendPsocMessage(uint8_t psocNr)
{
    mut_.lock();
    if(psocNr == 1) sendPsoc1Message();
    else sendPsoc2Message();

    psocMessage_ = 0;
    mut_.unlock();
}

void I2C_reg::sendPsoc1Message()
{
    ioctl(fd_, 0x0703, psoc1Adress_);
    currentAddr_ = psoc1Adress_;

    write(fd_, &psocMessage_, 1);
}
void I2C_reg::sendPsoc2Message()
{
    ioctl(fd_, 0x0703, psoc2Adress_);
    currentAddr_ = psoc2Adress_;

    write(fd_, &psocMessage_, 1);
}

void I2C_reg::sendArduinoMessage()
{
    //std::cout << "Hello from sendArduinoMessage" << std::endl;
    
    
    mut_.lock();
    ioctl(fd_, 0x0703, arduinoAdress_);
    currentAddr_ = arduinoAdress_;

    uint8_t tries_ = 0;

    while((write(fd_, &arduinoMessage_, 1) < 1) && tries_ < 3) {tries_++;}

    mut_.unlock();
    arduinoMessage_ = 0;
    
}

void I2C_reg::lockI2C()
{
    mut_.lock();
}

void I2C_reg::unlockI2C()
{
    mut_.unlock();
}