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

static PowerOnState      CartBot::powerOnState;
static InitState         CartBot::initState;
static DisabledState     CartBot::disabledState;
static EnabledState      CartBot::enabledState;
static ControlFaultState CartBot::controlFaultState;
static BatteryFaultState CartBot::batteryFaultState;
static TestState         CartBot::testState;

CartBot& CartBot::GetInstance()
{
    static CartBot* bot = NULL;
    if (!bot) {
	bot = new CartBot();
    }
    return *bot;
}

Display& CartBot::GetDisplay()
{
    return GetInstance().display;
}

CartBot::CartBot()
  : currentState(nullptr),
    sampleIndex(0),
    joyx(0), joyy(0), vbat(0), venbl(0),
    motorsEnabled(false),
    leftMotor(), rightMotor(),
    display()
{
    for (int i = 0; i < NUM_SAMPLES; i++) {
	vbatSamples[i] = venblSamples[i] = VBAT_MAX;
    }
}

CartBot::~CartBot()
{
    ;
}


void CartBot::ChangeState( State *newState ) {
    // delete currentState;
    currentState = newState;
    currentState->ResetTimer();
    currentState->EnterState();
}

void CartBot::Run()
{
    ReadA2D();
    UpdateState();
    UpdateOutputs();
    UpdateDisplay();
}

void CartBot::UpdateState()
{
    currentState->UpdateState();
}

void CartBot::UpdateOutputs()
{
    currentState->UpdateOutputs();
}

void CartBot::UpdateDisplay()
{
    currentState->UpdateDisplay();
    ShowFuelGauge();
}

////////////////////////////////////////////////
//
// read analog inputs & update state variables
//
////////////////////////////////////////////////

void CartBot::ReadA2D()
{
    joyx = analogRead(JOYX_PIN);
    joyy = analogRead(JOYY_PIN);
    vbatSamples[sampleIndex] = analogRead(VBAT_PIN);
    venblSamples[sampleIndex] = analogRead(VENBL_PIN);
    if (++sampleIndex >= NUM_SAMPLES) {
	sampleIndex = 0;
    }

    unsigned long sumbat = 0;
    unsigned long sumenbl = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
	sumbat += (unsigned long) vbatSamples[i];
	sumenbl += (unsigned long) venblSamples[i];
    }
    vbat = (sumbat + NUM_SAMPLES/2) / NUM_SAMPLES;
    venbl = (sumenbl + NUM_SAMPLES/2) / NUM_SAMPLES;

#ifdef SERIAL_DEBUG
    static int slow = 0;
    if (++slow >= 100) {
	Serial.print(" x "); Serial.print(joyx);
	Serial.print(" y "); Serial.print(joyy);
	Serial.print(" b "); Serial.print(vbat);
	Serial.print(" e "); Serial.println(venbl);
	slow = 0;
    }
#endif
}

////////////////////////////////////////////////

int CartBot::GetJoyX() const
{
    return joyx;
}
 
int CartBot::GetJoyY() const
{
    return joyy;
}

int CartBot::GetVBat() const
{
    return vbat;
}

int CartBot::GetVEnbl() const
{
    return venbl;
}

bool CartBot::IsLowBattery() const
{
    return (vbat < VBAT_LOW);
}

bool CartBot::IsChargeNeeded() const
{
    return (vbat < VBAT_MIN);
}

bool CartBot::IsEnabled() const
{
    return (abs(venbl - vbat) < NUM_SAMPLES);
}

bool CartBot::IsJoystickCentered() const
{
    return (abs(joyx - 512) < DEADBAND) && (abs(joyy - 512) < DEADBAND);
}

////////////////////////////////////////////////

void CartBot::SetMotorSpeed(int left, int right)
{
    leftMotor.writeMicroseconds(left);
    if (!leftMotor.attached()) {
	leftMotor.attach(LEFTMOTOR_PIN, 1000, 2000);
    }

    rightMotor.writeMicroseconds(right);
    if (!rightMotor.attached()) {
	rightMotor.attach(RIGHTMOTOR_PIN, 1000, 2000);
    }
}

void CartBot::DisableMotors()
{
    leftMotor.writeMicroseconds(1500);
    leftMotor.detach();
    rightMotor.writeMicroseconds(1500);
    rightMotor.detach();
}

////////////////////////////////////////////////

void CartBot::ShowBatteryStatus()
{
    if (CartBot::GetInstance().IsLowBattery())
    {
	display.Print(2, "    Low Battery     ");
    }
    else
    {
	display.Print(2, "                    ");
    }
}

void CartBot::ShowFuelGauge()
{
    char fuel[21];

    int vbar = 20 * (vbat - VBAT_MIN) / (VBAT_MAX - VBAT_MIN);
    if (vbar < 0) vbar = 0;
    if (vbar > 19) vbar = 19;
    for (int i = 0; i <= vbar; i++) {
	fuel[i] = (i % 5 == 0) ? CHAR_VERTICAL : CHAR_HORIZONTAL;
    }
    for (int i = vbar+1; i < 20; i++) {
	fuel[i] = ' ';
    }
    fuel[20] = '\0';
    display.Print(3,fuel);
}

