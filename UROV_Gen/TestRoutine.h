#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------
typedef enum
{
  trmIdle,
  trmWaitRelayAllDone,
  trmWaitShult1Done,
  trmWaitShunt2Done,
  trmWaitProtectDone,
  trmWaitPulsesDone
  
} TestRoutineMachineState;
//--------------------------------------------------------------------------------------------------
class TestRoutineClass
{
  public:
    TestRoutineClass();
    
    void update();
    
    void start();
    void stop();
    
	  bool isDone();
  
  private:
  
	TestRoutineMachineState machineState;
  uint32_t lastMillis;
};
//--------------------------------------------------------------------------------------------------
extern TestRoutineClass TestRoutine;
//--------------------------------------------------------------------------------------------------

