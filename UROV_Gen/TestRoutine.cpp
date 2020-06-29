//--------------------------------------------------------------------------------------------------
#include "TestRoutine.h"
#include "Relay.h"
#include "Screen1.h"
#include "PulsesGen.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------
TestRoutineClass TestRoutine;
//--------------------------------------------------------------------------------------------------
TestRoutineClass::TestRoutineClass()
{
  machineState = trmIdle;
  lastMillis = 0;
}
//--------------------------------------------------------------------------------------------------
bool TestRoutineClass::isDone()
{
  return (machineState == trmIdle);
}
//--------------------------------------------------------------------------------------------------
void TestRoutineClass::start()
{
  stop();

  Relay_LineALL.on();
  machineState = trmWaitRelayAllDone;
  lastMillis = millis();
}
//--------------------------------------------------------------------------------------------------
void TestRoutineClass::stop()
{
  Relay_LineALL.off();
  Relay_Shunt1.off();
  Relay_Shunt2.off();
  Relay_Alarm.off();

  machineState = trmIdle;
}
//--------------------------------------------------------------------------------------------------
void TestRoutineClass::update()
{
    switch(machineState)
    {
      case trmIdle:
      {
        
      }
      break;

      case trmWaitRelayAllDone:
      {
          if(millis() - lastMillis >= RELAY_LINE_ALL_WAIT_INTERVAL)
          {
            Relay_Shunt1.on();
            machineState = trmWaitShult1Done;
            lastMillis = millis();
          }
      }
      break;

      case trmWaitShult1Done:
      {
          if(millis() - lastMillis >= RELAY_SHUNT1_WAIT_INTERVAL)
          {
            Relay_Shunt2.on();
            machineState = trmWaitShunt2Done;
            lastMillis = millis();
          }
      }
      break;

      case trmWaitShunt2Done:
      {
         if(millis() - lastMillis >= RELAY_SHUNT2_WAIT_INTERVAL)
          {
            Relay_Alarm.on();
            machineState = trmWaitProtectDone;
            lastMillis = millis();
          }
      }
      break;

      case trmWaitProtectDone:
      {
         if(millis() - lastMillis >= RELAY_PROTECT_WAIT_INTERVAL)
          {
            startGeneratePulses();
            machineState = trmWaitPulsesDone;
            lastMillis = millis();
          }
      }
      break;

      case trmWaitPulsesDone:
      {
        if(ImpulseGeneratorB.isDone())
        {
          machineState = trmIdle;
        }
      }
      break;
        
    } // switch
}
//--------------------------------------------------------------------------------------------------

