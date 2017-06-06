/*
** CartBot control software
** Stephen Tarr
** FRC Team 1425 "Error Code Xero"
**
** This code depends on F Malpartida's NewLiquidCrystal library:
** https://bitbucket.org/fmalpartida/new-liquidcrystal 
*/
#include <assert.h>
#include <string.h>
#include "Display.h"

byte Display::up[8] = {
    B00100,
    B01110,
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
};

byte Display::down[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
    B01110,
    B00100,
};

byte Display::left[8] = {
    B00000,
    B00100,
    B01100,
    B11100,
    B01100,
    B00100,
    B00000,
    B00000,
};

byte Display::right[8] = {
    B00000,
    B00100,
    B00110,
    B00111,
    B00110,
    B00100,
    B00000,
    B00000,
};

byte Display::bullet[8] = {
    B00000,
    B00000,
    B01110,
    B11111,
    B11111,
    B01110,
    B00000,
    B00000,
};

byte Display::vertical[8] = {
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
};

byte Display::horizontal[8] = {
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000,
    B00000,
};

Display::Display()
  : lcd(I2C_ADDR, EN_PIN, RW_PIN, RS_PIN,
        D4_PIN, D5_PIN, D6_PIN, D7_PIN,
	BACKLIGHT_PIN, BACKLIGHT_POL)
{
    InitDisplay();
}

Display::~Display()
{
    lcd.noBacklight();
}

void Display::InitDisplay()
{
    memset(msgText, 0, sizeof msgText);

    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.createChar(CHAR_UP, up);
    lcd.createChar(CHAR_DOWN, down);
    lcd.createChar(CHAR_LEFT, left);
    lcd.createChar(CHAR_RIGHT, right);
    lcd.createChar(CHAR_BULLET, bullet);
    lcd.createChar(CHAR_VERTICAL, vertical);
    lcd.createChar(CHAR_HORIZONTAL, horizontal);
    lcd.noAutoscroll();
    lcd.clear();
    lcd.noBlink();
    lcd.noCursor();
    lcd.display();
    lcd.backlight();
}

void Display::Print( int n, const char *msg )
{
    assert(n >= 0 && n < LCD_ROWS);
    assert(strlen(msg) == LCD_COLS);

    if (strcmp(msgText[n], msg) != 0) {
	strcpy(msgText[n], msg);
	lcd.setCursor(0,n);
	lcd.print(msg);
    }
}

void Display::Print( const char *msg0, const char *msg1, const char *msg2 )
{
    Print(0, msg0);
    Print(1, msg1);
    Print(2, msg2);
}

