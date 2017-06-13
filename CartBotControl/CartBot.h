#pragma once
/*
** CartBot control software
** Stephen Tarr - FRC Team 1425 "Error Code Xero"
**
** This code depends on F Malpartida's NewLiquidCrystal library:
** https://bitbucket.org/fmalpartida/new-liquidcrystal 
*/
#include <Servo.h>
#include "State.h"
#include "Display.h"

#define	NUM_SAMPLES	50	// for averaging

class CartBot {
private:
    CartBot();

public:
    static CartBot& GetInstance();
    static Display& GetDisplay();
    ~CartBot();

    int GetJoyX() const;
    int GetJoyY() const;
    int GetVBat() const;
    int GetVEnbl() const;

    bool IsLowBattery() const;
    bool IsChargeNeeded() const;
    bool IsEnabled() const;
    bool IsJoystickCentered() const;

    void ChangeState( State *newState );

    void SetMotorSpeed( int l, int r );
    void DisableMotors();

    void ShowBatteryStatus();
    void ShowFuelGauge();

    void Run();

public:
    static PowerOnState      powerOnState;
    static InitState         initState;
    static DisabledState     disabledState;
    static EnabledState      enabledState;
    static ControlFaultState controlFaultState;
    static BatteryFaultState batteryFaultState;
    static TestState         testState;

private:
    void ReadA2D();
    void UpdateState();
    void UpdateOutputs();
    void UpdateDisplay();

    // current operating mode/state
    State *currentState;

    // sample buffer for averaging
    int vbatSamples[NUM_SAMPLES];
    int venblSamples[NUM_SAMPLES];
    int sampleIndex;

    // inputs in A2D units (0..1023)
    int joyx, joyy, vbat, venbl;

    // outputs in microseconds (10000..20000)
    bool motorsEnabled;
    Servo leftMotor, rightMotor;

    // display
    Display display;
};

