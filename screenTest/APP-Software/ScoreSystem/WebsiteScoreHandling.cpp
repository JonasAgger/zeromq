/*!
Author: Søren Skieller
*/
#include <osapi/ScoreSystem/WebsiteScoreHandling.hpp>

WebsiteScoreHandling::WebsiteScoreHandling(std::string teamName1, std::string teamName2, uint8_t scoreTeam1, uint8_t scoreTeam2, unsigned long gameTime, uint8_t doubleShots)
{
	//! Setting timeinfo for class
	timeKeeper_ = localtime(&t);

	//! Setting game stats
	setTeamName(1, teamName1);
	setScoreTeam(1, scoreTeam1);
	setTeamName(2, teamName2);
	setScoreTeam(2, scoreTeam2);
	setTotalDoubleCupShots(doubleShots);
	setGameTime(gameTime);

	//! Getting time info
	setDate();
	setEndTime();
	setStartTime();

	//! Setting up website
	writeToCSV();
}

int WebsiteScoreHandling::getScoreTeam(int teamNumber)
{
	if (teamNumber == 1)
		return scoreTeam1_;
	else if (teamNumber == 2)
		return scoreTeam2_;
}

int WebsiteScoreHandling::getTotalDoubleCupShots()
{
	return totalDoubleCupShots_;
}


std::string WebsiteScoreHandling::getTeamName(int teamNumber)
{
	if (teamNumber == 1)
		return teamName1_;
	else if (teamNumber == 2)
		return teamName2_;
}

std::string WebsiteScoreHandling::getGameTime()
{
	std::string returnString = "";
	if (gameTime_.hours_ >= 0 && gameTime_.hours_ <= 9)
	{
		returnString += "0" + std::to_string(gameTime_.hours_) + ":";
	}
	else if (gameTime_.hours_ == 0) {}
	//! Do nothing, only removing the zeroes for the hour counter

	else
		returnString += std::to_string(gameTime_.hours_) + ":";

	if (gameTime_.minutes_ >= 0 && gameTime_.minutes_ <= 9)
		returnString += "0" + std::to_string(gameTime_.minutes_) + ":";

	else
		returnString += std::to_string(gameTime_.minutes_) + ":";

	if (gameTime_.seconds_ >= 0 && gameTime_.seconds_ <= 9)
		returnString += "0" + std::to_string(gameTime_.seconds_);

	else
		returnString += std::to_string(gameTime_.seconds_);

	return returnString;
}

std::string WebsiteScoreHandling::getDate()
{
	return std::to_string(date_.day_) + "." + std::to_string(date_.month_) + "." + std::to_string(date_.year_);
}

std::string WebsiteScoreHandling::getStartTimeDate()
{
	return std::to_string(timeStartDate_.day_) + "." + std::to_string(timeStartDate_.month_) + "." + std::to_string(timeStartDate_.year_);
}

//! Printing StartTime while filling out zeroes when number is lower than 10
std::string WebsiteScoreHandling::getStartTime()
{
	std::string returnString = "";
	if (timeStart_.hours_ >= 0 && timeStart_.hours_ <= 9)
		returnString += "0" + std::to_string(timeStart_.hours_) + ":";
	
	else
		returnString += std::to_string(timeStart_.hours_) + ":";

	if (timeStart_.minutes_ >= 0 && timeStart_.minutes_ <= 9)
		returnString += "0" + std::to_string(timeStart_.minutes_) + ":";

	else
		returnString += std::to_string(timeStart_.minutes_) + ":";

	if (timeStart_.seconds_ >= 0 && timeStart_.seconds_ <= 9)
		returnString += "0" + std::to_string(timeStart_.seconds_);

	else
		returnString += std::to_string(timeStart_.seconds_);

	return getStartTimeDate() + " " + returnString;
}

std::string WebsiteScoreHandling::getEndTime()
{
	std::string returnString = "";
	if (timeEnd_.hours_ >= 0 && timeEnd_.hours_ <= 9)
		returnString += "0" + std::to_string(timeEnd_.hours_) + ":";

	else if (timeEnd_.hours_ == 0) {}
	//! Do nothing

	else
		returnString += std::to_string(timeEnd_.hours_) + ":";

	if (timeEnd_.minutes_ >= 0 && timeEnd_.minutes_ <= 9)
		returnString += "0" + std::to_string(timeEnd_.minutes_) + ":";

	else
		returnString += std::to_string(timeEnd_.minutes_) + ":";

	if (timeEnd_.seconds_ >= 0 && timeEnd_.seconds_ <= 9)
		returnString += "0" + std::to_string(timeEnd_.seconds_);

	else
		returnString += std::to_string(timeEnd_.seconds_);

	return getDate() + " " + returnString;
}


void WebsiteScoreHandling::setScoreTeam(int teamNumber, int score)
{
	if (teamNumber == 1)
		scoreTeam1_ = score;
	else if (teamNumber == 2)
		scoreTeam2_ = score;
}
void WebsiteScoreHandling::setTeamName(int teamNumber, std::string teamName)
{
	if (teamNumber == 1)
		teamName1_ = teamName;
	else if (teamNumber == 2)
		teamName2_ = teamName;
}
void WebsiteScoreHandling::setTotalDoubleCupShots(int amountOfShots)
{
	if (amountOfShots <= 0)
		totalDoubleCupShots_ = 0;
	else
		totalDoubleCupShots_ = amountOfShots;
}
void WebsiteScoreHandling::setGameTime(int gameTime)
{
	//! Setting seconds
	gameTime_.seconds_ = gameTime % 60; //! Find seconds
	gameTime -= gameTime_.seconds_;
	gameTime_.minutes_ = (gameTime % 3600) / 60; //! Find minutes
	gameTime -= gameTime_.minutes_;
	gameTime_.hours_ = gameTime / 3600; //! Find hours
}

void WebsiteScoreHandling::setDate()
{
	date_.day_ = timeKeeper_->tm_mday;
	date_.month_ = timeKeeper_->tm_mon + 1;
	date_.year_ = timeKeeper_->tm_year % 100;

	timeStartDate_ = date_; //! Setting default start date
}

//! Turning back time from the EndTime point to the StartTime point
void WebsiteScoreHandling::setStartTime()
{
	int secondCounter = 0, minuteCounter = 0, hourCounter = 0,
		dayCounter = 0, monthCounter = 0, yearCounter = 0;

	//! Removing gametime from current timer
	secondCounter = timeEnd_.seconds_ - gameTime_.seconds_;
	minuteCounter = timeEnd_.minutes_ - gameTime_.minutes_;
	hourCounter = timeEnd_.hours_ - gameTime_.hours_;

	dayCounter = timeStartDate_.day_;
	monthCounter = timeStartDate_.month_;
	yearCounter = timeStartDate_.year_;


	if (secondCounter < 0) //! If the seconds go below 0 
	{
		minuteCounter--;
		timeStart_.seconds_ = secondCounter + 60;
	}
	else
		timeStart_.seconds_ = secondCounter;

	if (minuteCounter < 0) //! Minutes
	{
		hourCounter--;
		timeStart_.minutes_ = minuteCounter + 60;
	}
	else
		timeStart_.minutes_ = minuteCounter;

	if (hourCounter < 0) //! Hours
	{
		dayCounter--;
		timeStart_.hours_ = hourCounter + 24;
	}
	else
		timeStart_.hours_ = hourCounter;

	if (dayCounter <= 0) //! Days
	{
		monthCounter--;
		if (monthCounter == 0 || monthCounter == 2 || monthCounter == 4 ||
			monthCounter == 6 || monthCounter == 7 || monthCounter == 9 ||
			monthCounter == 11 || monthCounter == -1) //! 12 and 0 = december
		{
			timeStartDate_.day_ = dayCounter + 31;
		}
		else if (monthCounter == 3 || monthCounter == 5 ||
			monthCounter == 8 || monthCounter == 10)
		{
			timeStartDate_.day_ = dayCounter + 30;
		}
		else
		{
			timeStartDate_.day_ = dayCounter + 28;
		}
	}
	else
		timeStartDate_.day_ = dayCounter;

	if (monthCounter <= 0) //! Months
	{
		yearCounter--;
		timeStartDate_.month_ = monthCounter + 12;
	}
	else
		timeStartDate_.month_ = monthCounter;

	timeStartDate_.year_ = yearCounter;
}

void WebsiteScoreHandling::setEndTime()
{
	timeEnd_.hours_ = timeKeeper_->tm_hour + 1; //! Since RPI is GMT
	timeEnd_.minutes_ = timeKeeper_->tm_min;
	timeEnd_.seconds_ = timeKeeper_->tm_sec;
}

void WebsiteScoreHandling::writeToCSV()
{
	//! Creating new file temp, using app to add added content to end
	fs_.open("/www/pages/temp.csv", std::fstream::in | std::fstream::out | std::fstream::app);
	fs_ << std::flush; //! Using flush to write nothing to file making sure it is created

	if (fs_.is_open())
	{
		getCurrentID(); //! Getting ID from last game from final.csv
						//!  "Spil ID,Hold 1,Hold 2,Score Hold 1,Score Hold 2,Varighed,Starttidspunkt,Sluttidspunkt,Dobbelt skud";
		fs_ << headers << std::endl <<
			//! Spil ID					//! Hold 1					//! Hold 2
			currentGameID	<< "," <<	getTeamName(1)	<< "," <<	getTeamName(2)	<< "," <<
			//! Score Hold 1			//! Score Hold 2			//! Varighed
			getScoreTeam(1) << "," << 	getScoreTeam(2) << "," <<	getGameTime()	<< "," <<
			//! Starttidspunkt			//! Sluttidspunkt			//! Dobbelt skud
			getStartTime()	<< "," <<	getEndTime()	<< "," <<	getTotalDoubleCupShots() << std::endl;

		//! Opening final.csv for overwriting
		ifs_.open("/www/pages/final.csv", std::ifstream::in);
		if (!ifs_.is_open()) {
			std::cerr << "Could not open final.csv" << std::endl;
		}

		//! Discarding the lines we read but do not need
		currentLine = 0, lineNumber = 1;
		while (currentLine < lineNumber)
		{
			ifs_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			++currentLine;
		}

		//! Positioning the read pointer to second line
		ifs_.seekg(ifs_.tellg());
		lineToCopy = "";

		//! Reading the rest of the file one line at the time and writing that to fs_. Since eof flag does
		//! not work when the final.csv is placed in the same folder as the website solution, we are here 
		//! checking ourselves whether the end of document is reached.
		int hasReachedEnd = 0;
		while (getline(ifs_, lineToCopy) && !hasReachedEnd)
		{
			fs_ << lineToCopy << std::endl;
			if (lineToCopy == "000000,000000,000000,00,00,0000,0000000000,00")
				hasReachedEnd = 1;
		}

		//! Closing both file streams
		fs_.close();
		ifs_.close();

		//! =============== Delete old final.csv, replace it with new temp.csv ======
		if (remove("/www/pages/final.csv") != 0)
			std::cerr << "Error deleting file" << std::endl;
		else
		{
			int result;
			oldName = "/www/pages/temp.csv";
			newName = "/www/pages/final.csv";
			result = rename(oldName.c_str(), newName.c_str());
			if (result != 0)
				std::cerr << "Error renaming file" << std::endl;
		}
	}
	else
		std::cerr << "DEBUG: Could not open output file\n";

	fs_.flush(); //! Finally flush the stream 
	fs_.close(); //! and close it if not closed yet
}

void WebsiteScoreHandling::getCurrentID()
{
	if (!ifs_.is_open()) //! If not opened beforehand, open now
		ifs_.open("/www/pages/final.csv", std::ifstream::in);

	if (ifs_.fail()) //! If file not created, create standard file
	{
		std::cerr << "=== FAILBIT SET === CREATING NEW FILE ===" << std::endl;
		errorFs_.open("/www/pages/final.csv", std::fstream::in | std::fstream::out | std::ios::app);
		errorFs_ << headers << std::endl
			<< "000000,000000,000000,00,00,0000,0000000000,00" << std::endl;

		errorFs_.close(); //! Closing read/write
		ifs_.open("/www/pages/final.csv", std::ifstream::in); //! Opening read
		ifs_.clear(); //! Resetting ifstream flags
	}

	if (ifs_.is_open()) //! If able to open file
	{
		lineToRead = "";
		ifs_.seekg(std::ios::beg); //! Reading from start of file

		currentLine = 0, lineNumber = 1;
		while (currentLine < lineNumber) //! Discarding the lines we read but do not need
		{
			ifs_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			++currentLine;
		}
		//! Positioning the read pointer to second line
		ifs_.seekg(ifs_.tellg());

		getline(ifs_, lineToRead); //! Get 2nd line
		lastGameID = lineToRead.substr(0, 6); //! Getting first element, 6 chars long
		if (lastGameID.length() < 6) //! If file is empty
		{
			ifs_.close(); //! Closing read
			std::cerr << "=== PRINTING NEW START CHARACTERS TO FILE ===" << std::endl;
			errorFs_.open("/www/pages/final.csv", std::fstream::in | std::fstream::out | std::ios::trunc);
			errorFs_ << headers << std::endl 
				<< "000000,000000,000000,00,00,0000,0000000000,00" << std::endl;

			errorFs_.close(); //! Closing read/write
			ifs_.open("/www/pages/final.csv", std::ifstream::in); //! Opening read
			ifs_.clear(); //! Resetting ifstream flags
			getCurrentID();
			return;
		}
		gameIDInteger = stoi(lastGameID, nullptr) + 1; //! New gameID

		//! Deciding amount of extra numbers for ID (MAX ID: 999999)
		if (gameIDInteger < 10)
			currentGameID = "00000" + std::to_string(gameIDInteger);
		else if (gameIDInteger < 100)
			currentGameID = "0000" + std::to_string(gameIDInteger);
		else if (gameIDInteger < 1000)
			currentGameID = "000" + std::to_string(gameIDInteger);
		else if (gameIDInteger < 10000)
			currentGameID = "00" + std::to_string(gameIDInteger);
		else if (gameIDInteger < 100000)
			currentGameID = "0" + std::to_string(gameIDInteger);
		else
			currentGameID = std::to_string(gameIDInteger);
	}
	else
		std::cerr << "DEBUG: File could not be opened for ID read\n";
	ifs_.close();
}

void setNewIPJS()
{
	std::string IP = "";
	std::ifstream getIPFs_;
	std::fstream funcFileFs_, tempFuncFileFs_;
	//! Accessing file with info about current IP address
	getIPFs_.open("/home/root/files/wifiAutoconnect.txt", std::ifstream::in);

	if (getIPFs_.is_open())
	{
		getline(getIPFs_, IP);
		//! Splitting string to only get IP
		std::string newIP = "";
		bool colonReached = false;
		for (size_t i = 0; i < IP.length(); i++)
		{
			if (IP[i] == '=')
			{
				colonReached = true;
				i++; //! Skip equal sign
			}
			if (colonReached == true)
				newIP += IP[i];
		}
		newIP.pop_back(); //! Remove comma in end
		getIPFs_.close(); //! Cleanup

		//! Opening functionality.js and tempFunc for overwriting new IP
		funcFileFs_.open("/www/pages/functionality.js", std::fstream::in | std::fstream::out | std::fstream::app);
		if (!funcFileFs_.is_open())
		{
			std::cerr << "Could not open functionality.js\n";
		}
		tempFuncFileFs_.open("/www/pages/tempfunc.js", std::fstream::in | std::fstream::out | std::fstream::app);
		
		if (!tempFuncFileFs_.is_open())
		{
			std::cerr << "Could not open tempfunc.js\n";
		}

		//! Writing javascript before IP to temp file
		std::string lineToCopy = "";
		int currentLine = 0, lineNumber = 9;
		while (currentLine < lineNumber)
		{
			getline(funcFileFs_, lineToCopy);
			tempFuncFileFs_ << lineToCopy << std::endl;
			currentLine++;
		}

		//! Writing the new IP to the temp file
		std::string theIPLine = "    myRequest.open('GET', 'http://" + newIP + "/final.csv');";
		tempFuncFileFs_ << theIPLine << std::endl;

		//! Skipping ahead of the IP line
		currentLine = 0, lineNumber = 1;
		while (currentLine < lineNumber)
		{
			funcFileFs_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			++currentLine;
		}

		//! Writing rest of functionality.js to temp file
		lineToCopy = "";
		funcFileFs_.seekg(funcFileFs_.tellg());
		while (getline(funcFileFs_, lineToCopy))
		{
			tempFuncFileFs_ << lineToCopy << std::endl;
		}

		//! Closing both file streams
		tempFuncFileFs_.close();
		funcFileFs_.close();
		
		//! Deleting old functionality.js, replace with tempfunc.js
		if (remove("/www/pages/functionality.js") != 0)
			std::cerr << "Error deleting file" << std::endl;
		else
		{
			int result;
			std::string oldName = "", newName = "";
			oldName = "/www/pages/tempfunc.js";
			newName = "/www/pages/functionality.js";
			result = rename(oldName.c_str(), newName.c_str());
			if (result != 0)
				std::cerr << "Error renaming file" << std::endl;
		}
	}
	else
		std::cerr << "DEBUG: Could not open wifiAutoconnect.txt\n";
}