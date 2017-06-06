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

#define	LOOP_TIME	20	// milliseconds

unsigned long when;

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

  CartBot::GetInstance().ChangeState(&CartBot::powerOnState);
  when = millis() + LOOP_TIME;
}
  
void loop()
{
  unsigned long now = millis();
  if ((long)(now - when) >= 0) {
    when += LOOP_TIME;
    CartBot::GetInstance().Run();
  }
}

