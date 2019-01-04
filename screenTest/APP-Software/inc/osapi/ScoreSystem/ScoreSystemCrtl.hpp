#ifndef SCORESYSTEM_H_
#define SCORESYSTEM_H_
//! PROJECT INCLUDES
#include <osapi/ScoreSystem/LCD.hpp>
#include <osapi/ScoreSystem/Page.hpp>
#include <osapi/ScoreSystem/Button.hpp>
#include <osapi/ScoreSystem/I2C_reg.hpp>
#include <osapi/ScoreSystem/Button.hpp>
#include <osapi/ScoreSystem/WebsiteScoreHandling.hpp>
//! NORMAL INCLUDES
#include <string>
#include <vector>
//! OSAPI INCLUDES
#include <osapi/ClockTime.hpp>
#include <osapi/Time.hpp>
#include <osapi/MsgQueue.hpp>
#include <osapi/Message.hpp>
#include <osapi/ThreadFunctor.hpp>
#include <osapi/Thread.hpp>
#include <osapi/ClockTime.hpp>

using std::string;
using std::vector;

//! RECEIVABLE PSOC MESSAGES
enum pSocMessages
{
    ONE_BALL_ONE_CUP = 0x01,
    ONE_BALL_TWO_CUPS = 0x02,
    TWO_BALLS_ONE_CUP = 0x03,
    ALL_CUPS_PLACED = 0x04,
    CUP_ZONE_READY = 0x05,
    EMPTY_CUPZONE = 0x06,
    CALIBRATE = 0x07,
    NO_CHANGE = 0xFF
};

class ScoreSystemCrtl : public osapi::ThreadFunctor
{
public:
    ScoreSystemCrtl();
    osapi::MsgQueue* getMsgQueue();

private:
    void run(); // The threadFunctor method.

    // HANDLER FUNCTIONS
    void handleMsg(unsigned int id, osapi::Message *msgPtr);
    void handleState();
    void handlePsocUpdate(uint8_t psoc, osapi::Message *msgPtr);
    void handleRfidUpdate(osapi::Message *msgPtr);

    // HELPER FUNCTIONS
    void resetGame();
    void endGame(int winner = 1);
    string getIP();
    void flushMsgQueue();
    void loadRfidDatabase();

	// UTILITY VARIABLES
    string ip_; 
    string tempString_; 
    unsigned int state_ = pageEvent::noUpdate; 
    uint8_t currentScreen = 0; 
    unsigned long gameTime_ = 0; 
    uint8_t nr_Cups_In_Game_ = 10; 

    // TEAM VARIABLES
    string teamName1_ = "moon moon"; //! DEFAULT VALUES
    string teamName2_ = "red pandas"; //! DEFAULT VALUES
    enum pSocMessages zone1State_ = pSocMessages::NO_CHANGE; //! DEFAULT VALUES
    enum pSocMessages zone2State_ = pSocMessages::NO_CHANGE; //! DEFAULT VALUES
    uint8_t score_Team_1_ = 0;
    uint8_t score_Team_2_ = 0; 
    uint8_t collectiveDoubleShots_ = 0; 

    // REARRANGING
    bool reArranging_ = false;
    uint8_t reArrangingZone_ = 0;
    
    // PAGE INFORMATION
    vector<Page*> pages_;

    // DISPLAY 
    LCD *display;

    // BUTTON
    Button *btn;

    // WEBSITE
    WebsiteScoreHandling *websitePtr_;

    // THREADS
    osapi::Thread *i2cThread;
    osapi::Thread *buttonThread;
    // MESSAGEQUEUE
    osapi::MsgQueue *mq_;

    struct rfidDataStruct{
        uint8_t value[4];
        string *name_;
        bool used;
    };
    vector<rfidDataStruct> rfidDatabase;
};

#endif