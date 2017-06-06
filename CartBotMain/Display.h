#pragma once
/*
** CartBot control software
** Stephen Tarr - FRC Team 1425 "Error Code Xero"
**
** This code depends on F Malpartida's NewLiquidCrystal library:
** https://bitbucket.org/fmalpartida/new-liquidcrystal 
*/
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// LCD display
#define LCD_ROWS	4
#define LCD_COLS	20
#define I2C_ADDR	0x3F  // address of the PCF8574A I2C interface
#define EN_PIN		2 // these are PCF8574A pin numbers!
#define RW_PIN		1
#define RS_PIN		0
#define D4_PIN		4
#define D5_PIN		5
#define D6_PIN		6
#define D7_PIN		7
#define BACKLIGHT_PIN	3
#define BACKLIGHT_POL	POSITIVE

// custom characters
#define CHAR_UP		byte(0x01)
#define CHAR_DOWN	byte(0x02)
#define CHAR_LEFT	byte(0x03)
#define CHAR_RIGHT	byte(0x04)
#define CHAR_BULLET	byte(0x05)
#define CHAR_VERTICAL	byte(0x06)
#define CHAR_HORIZONTAL	byte(0x07)

class Display {
public:
    Display();
    ~Display();

    void InitDisplay();
    void ClearScreen();
    void Print( int n, const char *msg );
    void Print( const char *msg1, const char *msg2, const char *msg3 );

    LiquidCrystal_I2C lcd;

private:
    char msgText[LCD_ROWS][LCD_COLS+1];

    static byte up[8];
    static byte down[8];
    static byte left[8];
    static byte right[8];
    static byte bullet[8];
    static byte vertical[8];
    static byte horizontal[8];
};

