#ifndef I2C_REG_H_
#define I2C_REG_H_
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <osapi/ThreadFunctor.hpp>
#include <osapi/MsgQueue.hpp>
#include <osapi/Message.hpp>
#include <osapi/Conditional.hpp>
#include <osapi/Mutex.hpp>
#include <osapi/Utility.hpp>
#include <osapi/Thread.hpp>
#include <osapi/ScoreSystem/Timer.hpp>

enum i2c_messages
{
    TIMER_OUT,
    ARDUINOMESSGE,
    PSOCBROADCAST,
    PSOC1MESSAGE,
    PSOC2MESSAGE,
    RFID_TIMER_OUT,
    PSOC1UPDATE = 50,
    PSOC2UPDATE = 60,
    RFIDUPDATE = 70
};

struct psocUpdateMessage : public osapi::Message
{
    psocUpdateMessage(uint8_t val) : val_(val) {}
    uint8_t val_;
};

struct rfidUpdateMessage : public osapi::Message
{
    rfidUpdateMessage(uint8_t val0, uint8_t val1, uint8_t val2, uint8_t val3) : val0_(val0), val1_(val1), val2_(val2), val3_(val3) {}
    uint8_t val0_;
    uint8_t val1_;
    uint8_t val2_;
    uint8_t val3_;
};

class I2C_reg : public osapi::ThreadFunctor
{
public:
    // So that we only have 1 instance of our i2C device. 
    static I2C_reg& getInstance()
    {
        static I2C_reg i2c_dev;
        return i2c_dev;
    }

    void testStatic();

    void setMsgQueueScoreSystem(osapi::MsgQueue *smq) { ScoreSystemMQ_ = smq; }

    osapi::MsgQueue* getMsgQueue() { return mq_; }

    void setUP(int cups); // This sets up the timers for the psocs, so that we can get updates on the cupzone.

    void setArduinoMessage(uint8_t message);

    void setPsocMessage(uint8_t message, uint8_t psoc = 0);

    void displayWrite(uint8_t addr, uint8_t data);
    uint8_t displayRead(uint8_t addr);
    void displayInit();

    void lockI2C();
    void unlockI2C();

    void stopPsocPolling();
    void forcePsocMessage(uint8_t message);

    void startRfidPolling();
    void stopRfidPolling();

private:
    I2C_reg();
    ~I2C_reg();
    void run();
    
    void handleMsg(unsigned long id, osapi::Message *msg);
    void psocUpdate();
    void rfidUpdate();
    void sendPsocBroadcast();
    void sendPsocMessage(uint8_t psocNr);
    void sendPsoc1Message();
    void sendPsoc2Message();
    void sendArduinoMessage();

    int fd_;

    unsigned int state_;

    bool running_ = true;

    uint8_t currentAddr_ = 0;

    /* UNIT ADRESSES */
    uint8_t arduinoAdress_ = 0x06;
    uint8_t rfidAdress_ = 0x12;
    uint8_t psoc1Adress_ = 0x10;
    uint8_t psoc2Adress_ = 0x11;
    uint8_t screenAdress_ = 0x20;

    uint8_t arduinoMessage_ = 0;
    uint8_t psocMessage_ = 0;
    uint8_t receivingPsoC_ = 0; // 0 for both, 1 for team1, 2 for team2's PSoC

    //osapi::ITimerId*  timer1_;
    Timer *timer1_;
    osapi::Thread *tt_; // Timer thread
    Timer *timer2_;
    osapi::Thread *tt2_; // Timer thread

    osapi::MsgQueue *mq_;

    osapi::MsgQueue *ScoreSystemMQ_;
    osapi::MsgQueue *displayMQ_;

    osapi::Mutex mut_;
    osapi::Conditional cond_;
};

#endif