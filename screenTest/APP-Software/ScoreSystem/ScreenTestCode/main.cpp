#include <osapi/ScoreSystem/LCD.hpp>

int main()
{
    LCD display;

    display.begin();

    // TESTING SCREEN
    display.stringWrite("ScreenTest");
    display.setCursor(0,2);
    display.stringWrite("APP");

    // TESTING CHARS
    display.setCursor(0,1);
    display.charWrite(126);
    display.charWrite(127);
    display.charWrite('a');
    display.charWrite('b');
    display.charWrite('c');
}