/*!
Author: Søren Skieller
*/

#ifndef WEBSITESCOREHANDLING_H_
#define WEBSITESCOREHANDLING_H_

#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <time.h>

void setNewIPJS(); //! Global function; Is called from ScoreSystem upon startup

class WebsiteScoreHandling
{
public:
	WebsiteScoreHandling(std::string teamName1, std::string teamName2, uint8_t scoreTeam1, uint8_t scoreTeam2, unsigned long gameTime, uint8_t doubleShots);
	
private:
	int getScoreTeam(int teamNumber);
	int getTotalDoubleCupShots();
	std::string getTeamName(int teamNumber);
	std::string getGameTime();
	std::string getDate();
	std::string getStartTimeDate();
	std::string getStartTime();
	std::string getEndTime();

	void setScoreTeam(int teamNumber, int score);
	void setTotalDoubleCupShots(int amountOfShots);
	void setTeamName(int teamNumber, std::string teamName);
	void setGameTime(int gameTime);
	void setDate();
	void setStartTime();
	void setEndTime();

	void writeToCSV();
	void getCurrentID();

	std::fstream fs_, errorFs_;
	std::ifstream ifs_;
	std::string headers = "Spil ID,Hold 1,Hold 2,Score Hold 1,Score Hold 2,Varighed,Starttidspunkt,Sluttidspunkt,Dobbelt skud";
	std::string lastGameID = "000000",		
		currentGameID = "000000",	
		oldName = "",				
		newName = "",				
		lineToRead = "",			
		lineToCopy = "";			
	unsigned int gameIDInteger = 0,	
		currentLine = 0,
		lineNumber = 0;

	std::string teamName1_ = "",
		teamName2_ = "";
	unsigned int scoreTeam1_ = 0,
		scoreTeam2_ = 0,
		totalDoubleCupShots_ = 0;

	struct DATESTRUCT {
		int day_ = 0;
		int month_ = 0;
		int year_ = 0;
	};

	struct TIMESTRUCT {
		int seconds_ = 0;
		int minutes_ = 0;
		int hours_ = 0;
	};

	DATESTRUCT date_;
	TIMESTRUCT gameTime_;
	TIMESTRUCT timeStart_;
	DATESTRUCT timeStartDate_;
	TIMESTRUCT timeEnd_;

	time_t t = time(NULL);
	struct tm *timeKeeper_;
};

#endif