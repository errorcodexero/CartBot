#pragma once
/*
** CartBot control software
** Stephen Tarr - FRC Team 1425 "Error Code Xero"
**
** This code depends on F Malpartida's NewLiquidCrystal library:
** https://bitbucket.org/fmalpartida/new-liquidcrystal 
*/

// analog input pins
#define JOYX_PIN	0
#define JOYY_PIN	1
#define VBAT_PIN	2
#define VENBL_PIN	3

// A2D values based on 10k/5.1k divider, 5.00V reference
#define	VBAT_MIN	726	// 10.5V
#define	VBAT_LOW	774	// 11.2V
#define	VBAT_MAX	1023	// 14.8V

#define	DEADBAND	85	// half-width of joystick neutral zone
#define	FAST		300	// threshold for "fast forward" display

// digital pins
#define	LEFTMOTOR_PIN	3	// should use 9 for Servo output
#define	RIGHTMOTOR_PIN	5	// should use 10 for Servo output
#define	TEST_PIN	6
#define	BLINKY		13

// min/max servo pulse widths in microseconds
#define	FORWARD_LIMIT	1800
#define	REVERSE_LIMIT	1300

// cycle times
#define	LOOP_TIME	20	// main loop - milliseconds
#define	BLINK_CYCLES	25	// multiples of LOOP_TIME
#define	DEBOUNCE_TIME	5	// multiples of LOOP_TIME

