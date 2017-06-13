/*
** CartBot control software
** Stephen Tarr
** FRC Team 1425 "Error Code Xero"
**
** This code depends on F Malpartida's NewLiquidCrystal library:
** https://bitbucket.org/fmalpartida/new-liquidcrystal 
*/
#include "CartBot.h"
#include "Hardware.h"

#define	POWER_ON_TIME	5000	// milliseconds
#define	INIT_TIME	2000

#define	DEBUG_MOTORS

State::State()
{
    ResetTimer();
}

State::~State()
{
    ;
}

void State::ResetTimer()
{
    startTime = millis();
}

unsigned long State::TimeInState()
{
    unsigned long now = millis();
    return (long)(now - startTime);
}

////////////////////////////////////////
//
// PowerOn:
// - display splash screen
// - wait 5 seconds
//
////////////////////////////////////////

PowerOnState::PowerOnState()
{
    ;
}

PowerOnState::~PowerOnState()
{
    ;
}

void PowerOnState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("PowerOnState");
#endif
    CartBot::GetInstance().DisableMotors();
    CartBot::GetDisplay().Print(
	"WILSONVILLE ROBOTICS",
	"   FRC TEAM 1425    ",
	"  ERROR CODE XERO   "
    );
}

void PowerOnState::UpdateState()
{
    if (!digitalRead(TEST_PIN))
    {
#ifdef SERIAL_DEBUG
	Serial.println("PowerOnState -> TestState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::testState);
    }

    if (TimeInState() > POWER_ON_TIME)
    {
#ifdef SERIAL_DEBUG
	Serial.println("PowerOnState -> InitState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::initState);
    }
}

void PowerOnState::UpdateOutputs()
{
    ;
}

void PowerOnState::UpdateDisplay()
{
    ;
}

////////////////////////////////////////
//
// Init:
// - wait for 3 seconds with all controls inactive
// - if enable is pressed or joystick isn't centered,
//	go to ControlFault state
//
////////////////////////////////////////

InitState::InitState()
{
    ;
}

InitState::~InitState()
{
    ;
}

void InitState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("InitState");
#endif
    CartBot::GetInstance().DisableMotors();
    CartBot::GetDisplay().Print(
	" CHECKING CONTROLS  ",
	"     please wait    ",
	"                    "
    );
}

void InitState::UpdateState()
{
    if (CartBot::GetInstance().IsChargeNeeded())
    {
#ifdef SERIAL_DEBUG
	Serial.println("InitState -> BatteryFaultState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::batteryFaultState);
    }
    else if (CartBot::GetInstance().IsEnabled() ||
	     ! CartBot::GetInstance().IsJoystickCentered())
    {
#ifdef SERIAL_DEBUG
	Serial.println("InitState -> ControlFaultState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::controlFaultState);
    }
    else if (TimeInState() > INIT_TIME)
    {
#ifdef SERIAL_DEBUG
	Serial.println("InitState -> DisabledState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::disabledState);
    }
}

void InitState::UpdateOutputs()
{
    ;
}

void InitState::UpdateDisplay()
{
    CartBot::GetInstance().ShowBatteryStatus();
}

////////////////////////////////////////
//
// Disabled:
// - if joystick isn't centered, go to ControlFault state
// - if user presses enable button, go to Enabled state
//
////////////////////////////////////////

DisabledState::DisabledState()
{
    ;
}

DisabledState::~DisabledState()
{
    ;
}

void DisabledState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("DisabledState");
#endif
    CartBot::GetInstance().DisableMotors();

    CartBot::GetDisplay().Print(
	"       READY        ",
	"push button to drive",
	"                    "
    );
}

void DisabledState::UpdateState()
{
    if (CartBot::GetInstance().IsChargeNeeded())
    {
#ifdef SERIAL_DEBUG
	Serial.println("DisabledState -> BatteryFaultState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::batteryFaultState);
    }
    else if (!CartBot::GetInstance().IsJoystickCentered())
    {
#ifdef SERIAL_DEBUG
	Serial.println("DisabledState -> ControlFaultState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::controlFaultState);
    }
    else if (CartBot::GetInstance().IsEnabled())
    {
#ifdef SERIAL_DEBUG
	Serial.println("DisabledState -> EnabledState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::enabledState);
    }
}

void DisabledState::UpdateOutputs()
{
    ;
}

void DisabledState::UpdateDisplay()
{
    CartBot::GetInstance().ShowBatteryStatus();
}

////////////////////////////////////////
//
// Enabled:
// - if user releases enable button, go to Disabled state
// - drive the robot from the joystick
//
////////////////////////////////////////

EnabledState::EnabledState()
{
    ;
}

EnabledState::~EnabledState()
{
    ;
}

void EnabledState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("EnabledState");
#endif
    CartBot::GetInstance().SetMotorSpeed( 15000, 15000 );
    CartBot::GetDisplay().Print(
    	"                    ",
    	"                    ",
    	"                    "
    );
}

void EnabledState::UpdateState()
{
    if (CartBot::GetInstance().IsChargeNeeded())
    {
#ifdef SERIAL_DEBUG
	Serial.println("EnabledState -> BatteryFaultState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::batteryFaultState);
    }
    else if (!CartBot::GetInstance().IsEnabled())
    {
#ifdef SERIAL_DEBUG
	Serial.println("EnabledState -> DisabledState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::disabledState);
    }
}

void EnabledState::UpdateOutputs()
{
    int joyx = CartBot::GetInstance().GetJoyX();
    int joyy = CartBot::GetInstance().GetJoyY();

    if (joyy > 512 + DEADBAND) {
	forward = joyy - (512 + DEADBAND);
    } else if (joyy < 512 - DEADBAND) {
	forward = joyy - (512 - DEADBAND);  // reverse motion
    } else {
	forward = 0;
    }

    if (joyx > 512 + DEADBAND) {
	turn = joyx - (512 + DEADBAND);
    } else if (joyx < 512 - DEADBAND) {
	turn = joyx - (512 - DEADBAND);
    } else {
	turn = 0;
    }

    leftSpeed = 1500 + forward + turn / 3;
    if (leftSpeed > FORWARD_LIMIT) leftSpeed = FORWARD_LIMIT;
    if (leftSpeed < REVERSE_LIMIT) leftSpeed = REVERSE_LIMIT;

    rightSpeed = 1500 + forward - turn / 3;
    if (rightSpeed > FORWARD_LIMIT) rightSpeed = FORWARD_LIMIT;
    if (rightSpeed < REVERSE_LIMIT) rightSpeed = REVERSE_LIMIT;

    CartBot::GetInstance().SetMotorSpeed( leftSpeed, rightSpeed );
}

void EnabledState::UpdateDisplay()
{
    LiquidCrystal_I2C &lcd = CartBot::GetDisplay().lcd;

#ifdef DEBUG_MOTORS
    lcd.setCursor(0,0);
    lcd.print(leftSpeed);

    lcd.setCursor(16,0);
    lcd.print(rightSpeed);
#endif

    lcd.setCursor(10,0);
    lcd.write((forward > FAST) ? CHAR_UP : ' ');

    lcd.setCursor(9,1);
    lcd.write((turn < 0) ? CHAR_LEFT : ' ');
    lcd.write((forward > 0) ? CHAR_UP :
    	      (forward < 0) ? CHAR_DOWN :
	      CHAR_BULLET);
    lcd.write((turn > 0) ? CHAR_RIGHT : ' ');

    CartBot::GetInstance().ShowBatteryStatus();
}

////////////////////////////////////////
//
// ControlFault:
// - display "hands off" message
// - when user releases all controls, go to Init state
//
////////////////////////////////////////

ControlFaultState::ControlFaultState()
{
    ;
}

ControlFaultState::~ControlFaultState()
{
    ;
}

void ControlFaultState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("ControlFaultState");
#endif
    CartBot::GetInstance().DisableMotors();
    CartBot::GetDisplay().Print(
	"      DISABLED      ",
	"                    ",
	"                    "
    );
}

void ControlFaultState::UpdateState()
{
    if (CartBot::GetInstance().IsChargeNeeded())
    {
#ifdef SERIAL_DEBUG
	Serial.println("ControlFaultState -> BatteryFaultState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::batteryFaultState);
    }
    else if (! CartBot::GetInstance().IsEnabled() &&
	     CartBot::GetInstance().IsJoystickCentered())
    {
#ifdef SERIAL_DEBUG
	Serial.println("ControlFaultState -> InitState");
#endif
	CartBot::GetInstance().ChangeState(&CartBot::initState);
    }
}

void ControlFaultState::UpdateOutputs()
{
    if (CartBot::GetInstance().IsEnabled()) {
	CartBot::GetDisplay().Print(1, " release the button ");
    } else if (! CartBot::GetInstance().IsJoystickCentered()) {
	CartBot::GetDisplay().Print(1, "release the joystick");
    } else { // "can't happen"
	CartBot::GetDisplay().Print(1, " release the kraken ");
    }
}

void ControlFaultState::UpdateDisplay()
{
    ;
}

////////////////////////////////////////
//
// BatteryFault:
// - display "hands off" message
// - when user releases all controls, go to Init state
//
////////////////////////////////////////

BatteryFaultState::BatteryFaultState()
{
    ;
}

BatteryFaultState::~BatteryFaultState()
{
    ;
}

void BatteryFaultState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("BatteryFaultState");
#endif
    CartBot::GetInstance().DisableMotors();
    CartBot::GetDisplay().Print(
	"  BATTERY TOO LOW   ",
	"  Recharge battery  ",
	"  before operating  "
    );
}

void BatteryFaultState::UpdateState()
{
    ;
}

void BatteryFaultState::UpdateOutputs()
{
    ;
}

void BatteryFaultState::UpdateDisplay()
{
    ;
}

////////////////////////////////////////
//
// Test:
// - display analog inputs and PWM outputs
// - when user presses test-mode button,
//	cycle whether the display shows A/D counts,
//	measured voltage (based on 5.00V ref.) or
//	calculated voltage
//
////////////////////////////////////////

TestState::TestState()
{
    ;
}

TestState::~TestState()
{
    ;
}

void TestState::EnterState()
{
#ifdef SERIAL_DEBUG
    Serial.println("TestState");
#endif
    CartBot::GetInstance().DisableMotors();
    CartBot::GetDisplay().Print(
    	"Vbat xx.x Venbl xx.x",
	"JoyX xx.x JoyY  xx.x",
	"Left x.xx Right x.xx"
    );
    displayMode = 0;
    buttonPressed = true;
    debounce = 0;
    leftSpeed = rightSpeed = 1500;
}

void TestState::UpdateState()
{
    if (debounce) {
	--debounce;
    }
    if (!digitalRead(TEST_PIN)) {	// input low == pressed
	if (!buttonPressed) {		// wasn't previously pressed
	    if (debounce == 0) {	// if we're past the debounce time
		if (++displayMode > 2)	// advance mode
		    displayMode = 0;
	    }
	    buttonPressed = true;	// record button press
	    debounce = DEBOUNCE_TIME;	// (re)start the debounce timer
	}
    } else {				// input high == released
	if (buttonPressed) {		// was previously pressed
	    buttonPressed = false;	// record button release
	    debounce = DEBOUNCE_TIME;	// (re)start the debounce timer
	}
    }
}

void TestState::UpdateOutputs()
{
    int joyx = CartBot::GetInstance().GetJoyX();
    int joyy = CartBot::GetInstance().GetJoyY();
    if (joyx < 512 + DEADBAND) {
	leftSpeed = (joyy < 512 - DEADBAND) ? 1000
	     : (joyy > 512 + DEADBAND) ? 2000
	     : 1500;
    } else {
	leftSpeed = 1500;
    }
    if (joyx > 512 - DEADBAND) {
	rightSpeed = (joyy < 512 - DEADBAND) ? 1000
	     : (joyy > 512 + DEADBAND) ? 2000
	     : 1500;
    } else {
	rightSpeed = 1500;
    }

    CartBot::GetInstance().SetMotorSpeed( leftSpeed, rightSpeed );
}

void itoa4( char *buf, int n )
{
    int d = n % 10;
    buf[3] = '0' + d;
    n /= 10;
    if (n) {
	d = n % 10;
	buf[2] = '0' + d;
	n /= 10;
	if (n) {
	    d = n % 10;
	    buf[1] = '0' + d;
	    n /= 10;
	    if (n) {
		d = n % 10;
		buf[0] = '0' + d;
		n /= 10;
		if (n) {
		    // out of range
		    buf[0] = buf[1] = buf[2] = buf[3] = '-';
		}
	    } else {
		buf[0] = ' ';
	    }
	} else {
	    buf[1] = buf[0] = ' ';
	}
    } else {
	buf[2] = buf[1] = buf[0] = ' ';
    }
}

void ftoa2x1( char *buf, float f )
{
    int n = (f * 10. + 0.5);
    int d = n % 10;
    buf[3] = '0' + d;
    buf[2] = '.';
    n /= 10;
    d = n % 10;
    buf[1] = '0' + d;
    n /= 10;
    if (n) {
	d = n % 10;
	buf[0] = '0' + d;
	n /= 10;
	if (n) {
	    // out of range
	    buf[0] = buf[1] = buf[2] = buf[3] = '-';
	}
    } else {
	buf[0] = ' ';
    }
}

void ftoa1x2( char *buf, float f )
{
    int n = (f * 100. + 0.5);
    int d = n % 10;
    buf[3] = '0' + d;
    n /= 10;
    d = n % 10;
    buf[2] = '0' + d;
    buf[1] = '.';
    n /= 10;
    d = n % 10;
    buf[0] = '0' + d;
    n /= 10;
    if (n) {
	// out of range
	buf[0] = buf[1] = buf[2] = buf[3] = '-';
    }
}

void TestState::UpdateDisplay()
{
    char line1[21];
    char line2[21];
    char line3[21];

    strcpy(line1, "Vbat xx.x Venbl xx.x");
    strcpy(line2, "JoyX xx.x JoyY  xx.x");
    strcpy(line3, "Left x.xx Right x.xx");

    switch (displayMode) {
    case 0:	// display raw A/D counts
	itoa4( line1 + 5, CartBot::GetInstance().GetVBat() );
	itoa4( line1 + 16, CartBot::GetInstance().GetVEnbl() );
	itoa4( line2 + 5, CartBot::GetInstance().GetJoyX() );
	itoa4( line2 + 16, CartBot::GetInstance().GetJoyY() );
	break;
    case 1:	// display raw input voltage based on 5.00V ref
	ftoa1x2( line1 + 5, CartBot::GetInstance().GetVBat() * 5.00 / 1024. );
	ftoa1x2( line1 + 16, CartBot::GetInstance().GetVEnbl() * 5.00 / 1024.);
	ftoa1x2( line2 + 5, CartBot::GetInstance().GetJoyX() * 5.00 / 1024. );
	ftoa1x2( line2 + 16, CartBot::GetInstance().GetJoyY() * 5.00 / 1024.);
	break;
    case 2:	// display calculated input voltage based on divider
	ftoa2x1( line1 + 5, CartBot::GetInstance().GetVBat() * 5.00 / 1024. * (15.1e3 / 5.1e3));
	ftoa2x1( line1 + 16, CartBot::GetInstance().GetVEnbl() * 5.00 / 1024. * (15.1e3 / 5.1e3));
	ftoa2x1( line2 + 5, CartBot::GetInstance().GetJoyX() * 100.0 / 1024.);
	ftoa2x1( line2 + 16, CartBot::GetInstance().GetJoyY() * 100.0 / 1024.);
	break;
    }
    ftoa1x2( line3 + 5, leftSpeed / 1000. );
    ftoa1x2( line3 + 16, rightSpeed / 1000. );

    CartBot::GetDisplay().Print( line1, line2, line3 );
}

////////////////////////////////////////
