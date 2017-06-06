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
	"    SAFETY CHECK    ",
	"     Hands off!     ",
	"        ...         "
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
	"        READY       ",
	" Press either button",
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

    lcd.setCursor(0,0);
    lcd.print(leftSpeed);

    lcd.setCursor(16,0);
    lcd.print(rightSpeed);

    lcd.setCursor(10,0);
    lcd.write((forward>FAST)?CHAR_UP:' ');

    lcd.setCursor(9,1);
    lcd.write((turn<0)?CHAR_LEFT:' ');
    lcd.write((forward>0)?CHAR_UP:(forward<0)?CHAR_DOWN:CHAR_BULLET);
    lcd.write((turn>0)?CHAR_RIGHT:' ');

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
	"    SAFETY CHECK    ",
	"     Hands off!     ",
	"Release the controls"
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
    ;
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

