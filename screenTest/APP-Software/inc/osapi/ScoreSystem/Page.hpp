#ifndef PAGE_H_
#define PAGE_H_
#include <osapi/ScoreSystem/LCD.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using std::string;
using std::vector;


/*! These are written in the .txt file we load in.
they explain what cursor is being used, and it's functionality */
struct cursorCoord
{
    uint8_t line_; 
    uint8_t row_;  
    uint8_t dir_; //! 0 for right-arroy, 1 for left-arrow, 2 for cursor, 3 for noCursor 
    uint8_t type_; //! 0 for next/enter/on ---- 1 for delete/off ---- 2 for enter Team name. Rest follows the pageEvent table. Check it for what the values represent
};

//! Enum for what state is returned to ScoreSystemCrtl. It's handled in the handleState() function.
enum pageEvent
{
    noUpdate = 0,
    nextPage = 1,
    teamNameEntered = 2,
    syncMusic = 3,
    fullGame = 4,
    halfGame = 5,
    placeCupsExit = 6,
    startGame = 7,
    quickPlay = 8,
    reArrangeCups = 9,
    team1Rearrange = 10,
    team2Rearrange = 11,
    doneRearrange = 12,
    calibrate = 13
};


class Page
{
public:
	/*! Constructor for Page. It opens up the specified file, and reads it in.
	File needs to be a specific style, where the cursorcoords is in the first line, comma seperated with a colon indicating the end.
	*/
	Page(std::string filename);

	/*! Checks the current button position and checks what action it's supposed to do, according to the type.
	*/
	void buttonPressed(LCD *display, string &returnString, unsigned int &state_);
   

	/*! Handles when the button is rotated right. Here it checks if a char is being selected, since it either needs
	to rotate the cursor or change the char appropriately*/ 
	void buttonRight(LCD *display);


	/*! Handles when the button is rotated left. Here it checks if a char is being selected, since it either needs
	to rotate the cursor or change the char appropriately*/
	void buttonLeft(LCD *display);


	/*! Displays the screen. Scrolls through the pageText and writes it to the screen. 
	Takes 2 string arguments, which is either both team names, or the IP adress, and displays them accordingly.
	Remember to check if the page has space for the strings before you pass the strings. */
	void displayScreen(LCD *display, string *name1 = nullptr, string *name2 = nullptr);


	//! Resets the page. Cursorposition and teamname is reset. 
	void resetPage();

    void getNameRFID(LCD *display, string *name);


private:
    std::vector<std::string> pageText; //! Page Text to be displayed. 
    int8_t cursorPos_ = 0; //! Current cursor position
    uint8_t nrCursorPos_; 

    char teamNameArr[16]; //! Used for team naming schemes
    bool teamEnter_ = false; //! Used for team naming schemes
    bool selectingChar = false; //! Used for team naming schemes
    bool rearrangePage = false;
    char currentChar = 'a'; //! Used for team naming schemes

    std::vector<struct cursorCoord> possibleCursorPos; //! Cursor positions
};

#endif