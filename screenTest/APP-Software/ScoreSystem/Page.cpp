#include <osapi/ScoreSystem/Page.hpp>

Page::Page(std::string filename)
{
	std::ifstream file;
	file.open(filename);

	string coords;

	if (file.is_open())
	{
		getline(file, coords); // Getting first line, which is coords for the cursor positions
		string line;

		while (getline(file, line))
		{
			pageText.push_back(line);
		}
		file.close();
	}


	uint8_t pos1 = 0;
	uint8_t pos2 = 0;

	while (coords.find(';', pos1) != std::string::npos)
	{
		struct cursorCoord cc;
		pos2 = coords.find(';', pos1);

		string line = coords.substr(pos1, pos2);
		//cout << line << endl;
		sscanf(line.c_str(), "%hhu, %hhu, %hhu, %hhu", &cc.line_, &cc.row_, &cc.dir_, &cc.type_);

		possibleCursorPos.push_back(cc);
		if ((int)cc.type_ == 2)
		{
			teamEnter_ = true;
		}
		if ((int)cc.type_ == 10 ||(int)cc.type_ == 11)
		{
			rearrangePage = true;
		}
		pos1 = pos2 + 1;
	}

	//teamNamePtr = &possibleCursorPos[0]; // Only relevant if teamEnter is active

}

void Page::buttonPressed(LCD *display, string &returnString, unsigned int &state_)
{
	if (possibleCursorPos[cursorPos_].type_ == 0)
	{
		state_ = pageEvent::nextPage;

		if (teamEnter_)
		{
			state_ = pageEvent::teamNameEntered;
			returnString = "";
			for (int i = 0; i < possibleCursorPos[0].row_; i++)
				returnString += teamNameArr[i];
		}

		return;
	}

	if (possibleCursorPos[cursorPos_].type_ > 2)
	{
		state_ = possibleCursorPos[cursorPos_].type_;
		return;
	}


	//! It needs to check if we're scrolling through chars, since it then needs to deselect it. 
	//! Updated after accepttest, since a stray line of code was commented out, enabling the possibility of reaching unwanted places when deleting
	if (possibleCursorPos[cursorPos_].type_ == 2)
	{
		if (!selectingChar)
		{
			if(possibleCursorPos[0].row_ >= 15) return;
			selectingChar = true;
			display->charWrite(currentChar);
			display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
		}
		else if (selectingChar)
		{
			selectingChar = false;
			teamNameArr[possibleCursorPos[cursorPos_].row_] = currentChar;
			currentChar = 'a';
			if ((possibleCursorPos[cursorPos_].row_++) >= 15) 
			{
				possibleCursorPos[cursorPos_].row_ = 15;
				buttonLeft(display);
			}

			display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
		}
	}
	else if (possibleCursorPos[cursorPos_].type_ == 1)
	{
		if (possibleCursorPos[0].row_ != 0) possibleCursorPos[0].row_--;
		display->setCursor(possibleCursorPos[0].row_, possibleCursorPos[0].line_);
		display->charWrite(' ');
		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
		display->charWrite(126 + possibleCursorPos[cursorPos_].dir_);
	}
	return;
}

void Page::buttonRight(LCD *display)
{
	if (possibleCursorPos.size() != 0 && !selectingChar)
	{
		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
		if (!(possibleCursorPos[cursorPos_].row_ == 15 && possibleCursorPos[cursorPos_].dir_ == 2))
			display->charWrite(32); // Clearing out the cursor

		cursorPos_++;
		if (unsigned(cursorPos_) >= possibleCursorPos.size()) cursorPos_ = 0;

		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);

		if (possibleCursorPos[cursorPos_].dir_ == 2) display->cursor();
		else if (possibleCursorPos[cursorPos_].dir_ == 3) return;
		else
		{
			display->noCursor();
			display->charWrite(126 + possibleCursorPos[cursorPos_].dir_); // Setting the new cursor 
		}
	}
	else if (selectingChar)
	{
		currentChar++;
		if (currentChar == ' ' + 1) currentChar = 'a';
		else if (currentChar > 'z') currentChar = ' ';
		display->charWrite(currentChar);
		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
	}
	if(rearrangePage)
	{
		if (cursorPos_ == 0) I2C_reg::getInstance().setArduinoMessage(0x06);
		else if (cursorPos_ == 1) I2C_reg::getInstance().setArduinoMessage(0b01100000);
	}
}

void Page::buttonLeft(LCD *display)
{
	if (possibleCursorPos.size() != 0 && !selectingChar)
	{
		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
		if (!(possibleCursorPos[cursorPos_].row_ == 15 && possibleCursorPos[cursorPos_].dir_ == 2))
			display->charWrite(32); // Clearing out the cursor

		cursorPos_--;
		if (cursorPos_ < 0) cursorPos_ = possibleCursorPos.size() - 1;

		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);

		if (possibleCursorPos[cursorPos_].dir_ == 2) display->cursor();
		else if (possibleCursorPos[cursorPos_].dir_ == 3) return;
		else
		{
			display->noCursor();
			display->charWrite(126 + possibleCursorPos[cursorPos_].dir_); // Setting the new cursor
		}
	}
	else if (selectingChar)
	{
		currentChar--;
		if (currentChar == ' ' - 1) currentChar = 'z';
		else if (currentChar < 'a') currentChar = ' ';
		display->charWrite(currentChar);
		display->setCursor(possibleCursorPos[cursorPos_].row_, possibleCursorPos[cursorPos_].line_);
	}
	if(rearrangePage)
	{
		if (cursorPos_ == 0) I2C_reg::getInstance().setArduinoMessage(0x06);
		else if(cursorPos_ == 1) I2C_reg::getInstance().setArduinoMessage(0b01100000);
	}
}

void Page::displayScreen(LCD *display, string *name1, string *name2)
{
	display->clear();
	usleep(5000);
	display->clear();
	usleep(5000);
	for (uint8_t i = 0; i < 4; i++)
	{
		display->setCursor(0, i);
		if (i == 2 && name1 != nullptr) display->stringWrite(*name1);
		else if (i == 3 && name2 != nullptr) display->stringWrite(*name2);
		else display->stringWrite(pageText[i]);
	}

	if (possibleCursorPos.size() != 0)
	{
		display->setCursor(possibleCursorPos[0].row_, possibleCursorPos[0].line_);
		if (possibleCursorPos[0].dir_ == 2) display->cursor();
		else if (possibleCursorPos[0].dir_ == 3) return;
		else
		{
			display->noCursor();
			display->charWrite(126 + possibleCursorPos[0].dir_);
		}
	}
	if(rearrangePage)
	{
		I2C_reg::getInstance().setArduinoMessage(0x06);
	}
}

void Page::getNameRFID(LCD *display, string *name1)
{
	if(!teamEnter_) return;
	std::cout << "hello2 " << *name1 << std::endl;
	display->setCursor(0, 1);
	display->stringWrite(*name1);
	possibleCursorPos[0].row_ = name1->length();
	for(uint8_t i = 0; i < name1->length(); i++)
	{
		teamNameArr[i] = name1->at(i);
	}
	for(uint8_t i = name1->length(); i < 16; i++)
	{
		teamNameArr[i] = 0;
		display->charWrite(32);
	}
	display->charWrite(32);
	buttonLeft(display);
	
}

void Page::resetPage()
{
	cursorPos_ = 0;


	if (teamEnter_)
	{
		possibleCursorPos[0].row_ = 0;
		for (int i = 0; i < 16; i++)
		{
			teamNameArr[i] = 0;
		}
	}
}