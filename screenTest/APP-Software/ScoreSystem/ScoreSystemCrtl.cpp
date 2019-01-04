#include <osapi/ScoreSystem/ScoreSystemCrtl.hpp>
#include <fstream>
#include <iostream>

using namespace std;

/*! ------------ DEFAULT CONSTRUCTOR ------------- 
------- SETS UP VARIABLES, LOADS IN PAGES -------
-- STARTING I2C AND BUTTON THREADS AND STARTS THE DISPLAY -- */
ScoreSystemCrtl::ScoreSystemCrtl()
{
    cout << "STARTING POWERPONG" << endl;

    // SETTING UP OUR MESSAGEQUEUE
    mq_ = new osapi::MsgQueue(10);

    // LOADING IN OUR PAGES FROM OUR "LIBRARIES"/.txt FILES
    pages_.push_back(new Page("page1.txt"));
    pages_.push_back(new Page("page2.txt"));
    pages_.push_back(new Page("page3.txt"));
    pages_.push_back(new Page("page4.txt"));
    pages_.push_back(new Page("page5.txt"));
    pages_.push_back(new Page("page6.txt"));
    pages_.push_back(new Page("page7.txt"));
    pages_.push_back(new Page("page8.txt"));
    pages_.push_back(new Page("page9.txt"));
    pages_.push_back(new Page("page10.txt"));
    pages_.push_back(new Page("page11.txt")); // Not really page 11, but our Calibrating page

    // DEFAULT VALUES
    teamName1_ = "torben"; // moon moon
    teamName2_ = "jeppe"; // red pandas

    /* ----- For debugging purposes ------

    for(uint8_t i = 0; i < screenInfo.size(); i++)
    {
        cout << screenInfo[i] << endl;
    }

    */

    loadRfidDatabase();

    // SETTING UP OUR BUTTON
    btn = new Button(mq_);

    // CREATING OUR I2C AND BUTTON THREAD
    i2cThread = new osapi::Thread(&I2C_reg::getInstance());
    buttonThread = new osapi::Thread(btn);

    I2C_reg::getInstance().setMsgQueueScoreSystem(mq_);

    i2cThread->start();
    buttonThread->start();

    // SETTING SCREEN TO CALIBRATE
    currentScreen = 10;

    // SETTING UP THE DISPLAY
    display = new LCD();
    display->begin();

    usleep(5000);
    I2C_reg::getInstance().setPsocMessage(0x02);
    usleep(5000);
    I2C_reg::getInstance().forcePsocMessage(0x02);
    I2C_reg::getInstance().setArduinoMessage(0);
}

/*! --------- GET MESSAGEQUEUE ---------- 
---- RETURNS POINTER TO MESSAGEQUEUE ----*/
osapi::MsgQueue* ScoreSystemCrtl::getMsgQueue()
{
    return mq_;
}

/*! --------- GET IP ---------- 
---- LOADS IN IP FROM FILE ----*/
string ScoreSystemCrtl::getIP()
{
    system("./wifiScript.sh");
    ifstream file;
    file.open("wifiAutoconnect.txt");
    if(file.is_open())
    {
        string line;
        getline(file, line);
        file.close();
        return line.substr(8);
    }
    return nullptr;
}

void ScoreSystemCrtl::loadRfidDatabase()
{
	std::ifstream file;
	file.open("rfidData.csv");

	vector<string> userData;

	if (file.is_open())
	{
		string line;

		while (getline(file, line))
		{
			userData.push_back(line);
		}
		file.close();
	}

	for(auto i : userData)
	{
		struct rfidDataStruct rfidUserData;

        string tempName;

		sscanf(i.c_str(), "%hhu, %hhu, %hhu, %hhu", &rfidUserData.value[0], &rfidUserData.value[1], &rfidUserData.value[2], &rfidUserData.value[3]);

        uint8_t pos = i.find_first_of(';')+1;

        uint8_t pos2 = i.find(';', pos+1);

        tempName = i.substr(pos, pos2-pos);

        cout << //(int)rfidUserData.value[0] << endl << (int)rfidUserData.value[1] << endl << (int)rfidUserData.value[2] << endl << (int)rfidUserData.value[3] << endl << 
        tempName << endl << (int)pos << endl << (int)pos2 << endl; 

        rfidUserData.name_ = new string(tempName);

        rfidUserData.used = false;

        rfidDatabase.push_back(rfidUserData);
	}

    
}

/*! ------ RUN ------- 
---- THREAD METHOD ----*/
void ScoreSystemCrtl::run()
{
	//! Gets message from the Message Queue, and handles the received message
    pages_[currentScreen]->displayScreen(display);
    while(1)
    {
        unsigned long id;
        osapi::Message *msgPtr = mq_->receive(id);
        handleMsg(id, msgPtr);
        delete msgPtr;
        cout << "Delete" << endl;
    }
}

/*! --------------- HANDLEMSG ---------------- 
---- HANDLES MESSAGES IN OUR MESSAGEQUEUE ----*/
void ScoreSystemCrtl::handleMsg(unsigned int id, osapi::Message *msgPtr)
{
	/*! Handles the received message. 
	If the button is pressed, then page->buttonPressed is called.
	Same happens with rotateLeft & rotateRight.
	It also handles receiving updates from the 2 psocs, where it calls handlePsocUpdate
	*/
    state_ = pageEvent::noUpdate;
    if(currentScreen == 12) return; 

    switch(id)
    {
        case buttonEvent::btnLeft:
            pages_[currentScreen]->buttonLeft(display);
            break;
        
        case buttonEvent::btnPressed:
            pages_[currentScreen]->buttonPressed(display, tempString_, state_);
            if(state_ != pageEvent::noUpdate) handleState();
            flushMsgQueue();
            break;

        case buttonEvent::btnRight:
            pages_[currentScreen]->buttonRight(display);
            break;

        case i2c_messages::PSOC1UPDATE:
            handlePsocUpdate(1, msgPtr);
            break;

        case i2c_messages::PSOC2UPDATE:
            handlePsocUpdate(2, msgPtr);
            break;

        case i2c_messages::RFIDUPDATE:
            handleRfidUpdate(msgPtr);
            break;
        
    }
}


/*! -------------- HANDLE STATE --------------- 
---- HANDLES STATES RETURNED BY OUR PAGES ----*/
void ScoreSystemCrtl::handleState()
{
	/*! Handles the state received from the buttonPressed function.
	It then takes action accouring to the state received
	Most commonly it increases the currentScreen variable and displays the next screen.
	It will also save teamNames and send information to the PsoC's/Arduino
	*/
    switch(state_)
    {
        case pageEvent::nextPage:
            currentScreen++;
            break;

        case pageEvent::quickPlay:
            currentScreen = 5;
            I2C_reg::getInstance().setUP(10);
            break;

        case pageEvent::teamNameEntered:
            if(currentScreen == 1) teamName1_ = tempString_;
            else teamName2_ = tempString_;
            cout << teamName1_ << " --- " << teamName2_ << endl;
            I2C_reg::getInstance().stopRfidPolling();
            currentScreen++;
            break;

        case pageEvent::syncMusic:
            currentScreen++;
            // DO SOMETHING
            break;

        case pageEvent::halfGame:
            currentScreen++;
            I2C_reg::getInstance().setUP(6);
            nr_Cups_In_Game_ = 6;
            // USE i2C_reg to set cups to 6
            break;

        case pageEvent::fullGame:
            currentScreen++;
            I2C_reg::getInstance().setUP(10);
            // USE i2C_reg to set cups to 10
            break;

        case pageEvent::placeCupsExit:
            resetGame();
            break;

        case pageEvent::startGame:
            I2C_reg::getInstance().setPsocMessage(0x01);
            I2C_reg::getInstance().setArduinoMessage(0x01);
            gameTime_ = osapi::getCurrentMonotonicTime().secs();
            currentScreen++;
            break;

        case pageEvent::reArrangeCups:
            cout << "REARRANGE CUPS" << endl;
            I2C_reg::getInstance().setArduinoMessage(0x06); // SHOULD BE 6, REMEMBER TO CHANGE
            currentScreen++;
            break;

        case pageEvent::team1Rearrange:
            cout << "TEAM 1 REARRANGE" << endl;
            I2C_reg::getInstance().setPsocMessage(0x09, 1);
            usleep(500);
            I2C_reg::getInstance().setArduinoMessage(0x07);
            reArranging_ = true;
            reArrangingZone_ = 1;
            currentScreen++;
            break;

        case pageEvent::team2Rearrange:
            cout << "TEAM 2 REARRANGE" << endl;
            I2C_reg::getInstance().setPsocMessage(0x09, 2);
            usleep(500);
            I2C_reg::getInstance().setArduinoMessage(0b01110000);
            reArranging_ = true;
            reArrangingZone_ = 2;
            currentScreen++;
            break;

        case pageEvent::doneRearrange:
            reArranging_ = false;
            cout << "Done Rearranging" << endl;
            I2C_reg::getInstance().setPsocMessage(0x08, reArrangingZone_);
            I2C_reg::getInstance().setArduinoMessage(0x01); // SHOULD BE 1, REMEMBER TO CHANGE------ BLA BLA
            reArrangingZone_ = 0;
            currentScreen = 7;
            break;

        case pageEvent::calibrate:
            I2C_reg::getInstance().setPsocMessage(pSocMessages::CALIBRATE); // NEEDS TO BE CALIBRATE
            currentScreen = 0;
            display->clear();
            display->stringWrite("CALIBRATING");
            sleep(2);
            display->clear();
            display->setCursor(0,0);
            display->stringWrite("CONNECTING TO");
            display->setCursor(0,1);
            display->stringWrite("THE INTERNET");
            ip_ = getIP(); //! CONNECTING TO INTERNET, NEEDS TO WAIT FOR CONNMANCTL TO START, HENCE THE DELAY
            setNewIPJS();
            I2C_reg::getInstance().setPsocMessage(0x02);
            break;

        default:
            currentScreen++;
            break;

    }

    if (currentScreen == 3) currentScreen++;

    pages_[currentScreen]->displayScreen(display);

    if(currentScreen == 7)
    {
        pages_[currentScreen]->displayScreen(display, &ip_);
    }
    else if(currentScreen == 8)
    {
        pages_[currentScreen]->displayScreen(display, &teamName1_, &teamName2_);
    }
    
    else if(currentScreen == 1)
    {
        I2C_reg::getInstance().startRfidPolling();
    }
      else if(currentScreen == 2)
    {
        I2C_reg::getInstance().startRfidPolling();
    }
    
}


/*! -------------- HANDLE PSOC UPDATE --------------- 
---- HANDLES THE VALUES RETURNED FROM THE PSOC ----*/
void ScoreSystemCrtl::handlePsocUpdate(uint8_t psoc, osapi::Message *msgPtr)
{
	/*! Handles the updates received from the psocs, by casting the Message pointer to a 
	psocUpdateMessage, where an uint8_t value can be retrived from.
	It will then discard the update, if its received during rearrange, which means, that no team can finish, while 1 is rearranging.
	It will also ready the game, when initially setting up the cups, making it possible to press play game.
	
	Otherwise it checks if the current state is of less value than the newly received state, which will cause a change of state.
	This happens untill a cupZoneReady state is received. 

	The teams scores are also updated in this function. 
	*/
    psocUpdateMessage *msg = static_cast<psocUpdateMessage*>(msgPtr);

    //cout << "Message from PSoC " << (int)psoc << " -- " << (int)msg->val_ << endl;

    if(reArranging_) 
    {
        return; // Checking if we're reArranging the cups. If we are, then we just return
    }

    if(currentScreen == 5)
    {
        if((msg->val_ & 0x0f) == pSocMessages::ALL_CUPS_PLACED)
        {
            if(psoc == 1) zone1State_ = pSocMessages::ALL_CUPS_PLACED;
            else if(psoc == 2) zone2State_ = pSocMessages::ALL_CUPS_PLACED;

            cout << "CUPZONE " << (int)psoc << " READY" << endl;
        }

        if((zone1State_ == pSocMessages::ALL_CUPS_PLACED) && (zone2State_ == pSocMessages::ALL_CUPS_PLACED))
        {
            cout << "CUP ZONE READY" << endl;
            currentScreen++;
            pages_[currentScreen]->displayScreen(display);
            //I2C_reg::getInstance().setPsocMessage(0x01);
        }
    }
    else if(currentScreen == 6)
    {
        if((msg->val_ & 0x0f) != pSocMessages::ALL_CUPS_PLACED)
        {
            if(psoc == 1) zone1State_ = pSocMessages::NO_CHANGE;
            else if(psoc == 2) zone2State_ = pSocMessages::NO_CHANGE;

            currentScreen = 5;
            pages_[currentScreen]->displayScreen(display);
        }
    }
    else if(currentScreen == 7)
    {
        if(psoc == 1)
        {
            switch(msg->val_ & 0x0f)
            {
                case pSocMessages::ONE_BALL_ONE_CUP:
                    if(zone1State_ >= pSocMessages::ALL_CUPS_PLACED)
                    {
                        cout << teamName2_ << " HIT 1 BALL IN " << teamName1_ << "'s CUPZONE" << endl;
                        zone1State_ = pSocMessages::ONE_BALL_ONE_CUP;
                        I2C_reg::getInstance().setArduinoMessage(0x02); // CHECK
                    }
                    break;

                case pSocMessages::ONE_BALL_TWO_CUPS:
                    if(zone1State_ < pSocMessages::ONE_BALL_TWO_CUPS)
                    {
                        cout << teamName2_ << " HIT 2 BALLS IN 2 CUPS IN " << teamName1_ << "'s CUPZONE" << endl;
                        zone1State_ = pSocMessages::ONE_BALL_TWO_CUPS;
                        I2C_reg::getInstance().setArduinoMessage(0x03); // CHECK LATER SHOULD BE 2 BALLS 2 CUPS
                    }
                    break;

                case pSocMessages::TWO_BALLS_ONE_CUP:
                    if(zone1State_ < pSocMessages::TWO_BALLS_ONE_CUP)
                    {
                        cout << teamName2_ << " HIT 2 BALLS IN 1 CUP IN " << teamName1_ << "'s CUPZONE" << endl;
                        zone1State_ = pSocMessages::TWO_BALLS_ONE_CUP;
                        collectiveDoubleShots_++;
                        I2C_reg::getInstance().setArduinoMessage(0x04); // CHECK LATER, SHOULD BE 2 BALLS 1 CUP
                    }
                    break;

                case pSocMessages::CUP_ZONE_READY:
                    if(zone1State_ < pSocMessages::CUP_ZONE_READY)
                    {
                        cout << "CUPZONE 1 READY AGAIN" << endl;
                        zone1State_ = CUP_ZONE_READY;
                    }
                    break;

                case pSocMessages::EMPTY_CUPZONE:
                    if(zone1State_ < pSocMessages::EMPTY_CUPZONE)
                    {
                        cout << teamName1_ << "'s CUPZONE IS EMPTY" << endl;
                        zone1State_ = pSocMessages::EMPTY_CUPZONE;
                        currentScreen = 12;
                        I2C_reg::getInstance().setArduinoMessage(0x05); // CHECK LATER, SHOULD BE WINNER
                        score_Team_2_ = nr_Cups_In_Game_;
                        endGame(2);
                    }
                    break;

                default:
                    // wat
                    break;
            }
            if((msg->val_>>4) < 11)
                score_Team_2_ = ((msg->val_>>4) & 0x0f);
        }
        else
        {
            switch(msg->val_ & 0x0f)
            {
                case pSocMessages::ONE_BALL_ONE_CUP:
                    if(zone2State_ >= pSocMessages::ALL_CUPS_PLACED)
                    {
                        cout << teamName1_ << " HIT 1 BALL IN " << teamName2_ << "'s CUPZONE" << endl;
                        zone2State_ = pSocMessages::ONE_BALL_ONE_CUP;
                        I2C_reg::getInstance().setArduinoMessage(0b00100000); // CHECK
                    }
                    break;

                case pSocMessages::ONE_BALL_TWO_CUPS:
                    if(zone2State_ < pSocMessages::ONE_BALL_TWO_CUPS)
                    {
                        cout << teamName1_ << " HIT 2 BALLS IN 2 CUPS IN " << teamName2_ << "'s CUPZONE" << endl;
                        zone2State_ = pSocMessages::ONE_BALL_TWO_CUPS;
                        I2C_reg::getInstance().setArduinoMessage(0b00110000); // CHECK LATER SHOULD BE 2 BALLS 2 CUPS
                    }
                    break;

                case pSocMessages::TWO_BALLS_ONE_CUP:
                    if(zone2State_ < pSocMessages::TWO_BALLS_ONE_CUP)
                    {
                        cout << teamName1_ << " HIT 2 BALLS IN 1 CUP IN " << teamName2_ << "'s CUPZONE" << endl;
                        zone2State_ = pSocMessages::TWO_BALLS_ONE_CUP;
                        collectiveDoubleShots_++;
                        I2C_reg::getInstance().setArduinoMessage(0b01000000); // CHECK LATER, SHOULD BE 2 BALLS 1 CUP
                    }
                    break;

                case pSocMessages::CUP_ZONE_READY:
                    if(zone2State_ < pSocMessages::CUP_ZONE_READY)
                    {
                        cout << "CUPZONE 2 READY AGAIN" << endl;
                        zone2State_ = CUP_ZONE_READY;
                    }
                    break;

                case pSocMessages::EMPTY_CUPZONE:
                    if(zone2State_ < pSocMessages::EMPTY_CUPZONE)
                    {
                        cout << teamName2_ << "'s CUPZONE IS EMPTY" << endl;
                        zone2State_ = pSocMessages::EMPTY_CUPZONE;
                        currentScreen = 12;
                        I2C_reg::getInstance().setArduinoMessage(0b01010000); // CHECK LATER, SHOULD BE WINNER
                        score_Team_1_ = nr_Cups_In_Game_;
                        endGame(1);
                    }
                    break;

                default:
                    // wat
                    break;
            }
            if((msg->val_>>4) < 11)
                score_Team_1_ = ((msg->val_>>4) & 0x0f);
        }
    }
}


void ScoreSystemCrtl::handleRfidUpdate(osapi::Message * msgPtr)
{
    rfidUpdateMessage *msg = static_cast<rfidUpdateMessage*>(msgPtr);
    for(int i = 0; i < rfidDatabase.size(); i++)
    {
        bool found = true;
        if (!rfidDatabase[i].used)
        {
            if (msg->val0_ != rfidDatabase[i].value[0]) 
            {
                found = false;
            }
            if (msg->val1_ != rfidDatabase[i].value[1]) 
            {
                found = false;
            }
            if (msg->val2_ != rfidDatabase[i].value[2]) 
            {
                found = false;
            }
            if (msg->val3_ != rfidDatabase[i].value[3]) 
            {
                found = false;
            }

            if (found) 
            {
                pages_[currentScreen]->getNameRFID(display, rfidDatabase[i].name_);
                rfidDatabase[i].used = true;
                I2C_reg::getInstance().stopRfidPolling();
                break;
            }
        }
    }
}

void ScoreSystemCrtl::flushMsgQueue()
{
    mq_->flush();
}

/*! ------------- RESET GAME -------------- 
---- RESETS THE TABLE TO INITIAL STATE ----*/
void ScoreSystemCrtl::resetGame()
{
	//! Resets pages, psocs, Arduino, names, sates etc. to initial values.
    I2C_reg::getInstance().setArduinoMessage(0x00);

    for(auto i : pages_) i->resetPage();

    for(int i = 0; i < rfidDatabase.size(); i++)
    {
        rfidDatabase[i].used = false;
    }

    I2C_reg::getInstance().forcePsocMessage(0x02);
    usleep(2500);
    I2C_reg::getInstance().stopPsocPolling();

    zone1State_ = pSocMessages::NO_CHANGE;
    zone2State_ = pSocMessages::NO_CHANGE;
    teamName1_ = "torben";
    teamName2_ = "jeppe";
    currentScreen = 0;
    collectiveDoubleShots_ = 0;
    nr_Cups_In_Game_ = 10;
}


/*! -------------------------- END GAME -------------------------- 
---- ENDS GAME, UPLOADS TO WEBSITE AND RESETS TO INITIAL STATE ----*/
void ScoreSystemCrtl::endGame(int winner)
{
    I2C_reg::getInstance().stopPsocPolling();

    gameTime_ = osapi::getCurrentMonotonicTime().secs() - gameTime_; // Getting length of the game in seconds
    // VICTORY SCREEN
    display->clear();
    display->setCursor(0,0);
    if(winner == 1) display->stringWrite(teamName1_ + " WINS");
    else if(winner == 2) display->stringWrite(teamName2_ + " WINS");
    display->setCursor(0,1);
    display->stringWrite("----SCORE----");
    display->setCursor(0,2);
    display->stringWrite(teamName1_);
    display->setCursor(18,2);
    display->stringWrite(to_string(score_Team_1_));
    display->setCursor(0,3);
    display->stringWrite(teamName2_);
    display->setCursor(18,3);
    display->stringWrite(to_string(score_Team_2_));

	websitePtr_ = new WebsiteScoreHandling(teamName1_, teamName2_, score_Team_1_, score_Team_2_, gameTime_, collectiveDoubleShots_);

    cout << "TEAM 1 SCORE: " << (int)score_Team_1_ << endl << "TEAM 2 SCORE: " << (int)score_Team_2_ << endl << "GAME TIME: " << gameTime_ << endl;

	//! Waits 15 seconds before returning to the welcome screen
    for(int i = 0; i < 15; i++)
    {
        display->setCursor(18,1);
        display->stringWrite(to_string(15-i));
        if(i == 6)
        {
            display->setCursor(19,1);
            display->stringWrite(" ");
        }
        sleep(1);
    }

    delete websitePtr_;

    resetGame();
    flushMsgQueue();

    pages_[currentScreen]->displayScreen(display);
}