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

unsigned long when;
bool blink_state;
int blink_count;

void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif
  
  pinMode( VBAT_PIN,       INPUT );
  pinMode( VENBL_PIN,      INPUT );
  pinMode( JOYX_PIN,       INPUT );
  pinMode( JOYY_PIN,       INPUT );

  pinMode( LEFTMOTOR_PIN,  OUTPUT );
  pinMode( RIGHTMOTOR_PIN, OUTPUT );
  pinMode( TEST_PIN,       INPUT_PULLUP );
  pinMode( BLINKY,         OUTPUT );

  CartBot::GetInstance().ChangeState(&CartBot::powerOnState);
  when = millis() + LOOP_TIME;
  blink_state = false;
  blink_count = 0;
}
  
void loop()
{
  unsigned long now = millis();
  if ((long)(now - when) >= 0) {
    when += LOOP_TIME;
    if (++blink_count > BLINK_CYCLES) {
      blink_state = !blink_state;
      digitalWrite(BLINKY, blink_state);
      blink_count = 0;
    }
    CartBot::GetInstance().Run();
  }
}

