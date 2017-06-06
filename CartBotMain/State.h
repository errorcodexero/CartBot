#pragma once
/*
** CartBot control software
** Stephen Tarr - FRC Team 1425 "Error Code Xero"
**
** This code depends on F Malpartida's NewLiquidCrystal library:
** https://bitbucket.org/fmalpartida/new-liquidcrystal 
*/

class State {
public:
    State();
    virtual ~State();
    void ResetTimer();
    virtual void EnterState() = 0;
    virtual void UpdateState() = 0;
    virtual void UpdateOutputs() = 0;
    virtual void UpdateDisplay() = 0;

private:
    unsigned long startTime;	// in milliseconds

protected:
    unsigned long TimeInState();
};

class PowerOnState : public State {
public:
    PowerOnState();
    virtual ~PowerOnState();
    virtual void EnterState();
    virtual void UpdateState();
    virtual void UpdateOutputs();
    virtual void UpdateDisplay();
};

class InitState : public State {
public:
    InitState();
    virtual ~InitState();
    virtual void EnterState();
    virtual void UpdateState();
    virtual void UpdateOutputs();
    virtual void UpdateDisplay();
};

class DisabledState : public State {
public:
    DisabledState();
    virtual ~DisabledState();
    virtual void EnterState();
    virtual void UpdateState();
    virtual void UpdateOutputs();
    virtual void UpdateDisplay();
};

class EnabledState : public State {
public:
    EnabledState();
    virtual ~EnabledState();
    virtual void EnterState();
    virtual void UpdateState();
    virtual void UpdateOutputs();
    virtual void UpdateDisplay();
private:
    int forward;
    int turn;
    int leftSpeed;
    int rightSpeed;
};

class ControlFaultState : public State {
public:
    ControlFaultState();
    virtual ~ControlFaultState();
    virtual void EnterState();
    virtual void UpdateState();
    virtual void UpdateOutputs();
    virtual void UpdateDisplay();
};

class BatteryFaultState : public State {
public:
    BatteryFaultState();
    virtual ~BatteryFaultState();
    virtual void EnterState();
    virtual void UpdateState();
    virtual void UpdateOutputs();
    virtual void UpdateDisplay();
};

