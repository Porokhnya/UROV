#pragma once
//--------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  esWaitForTrig,
  esWaitForRelease,
  
} EndstopState;
//--------------------------------------------------------------------------------------------------------------------------------
class Endstop
{
  public:
  
    Endstop(uint8_t _pin, uint8_t _level,uint32_t _minInterval, uint32_t _maxInterval);
    void begin();
    void update();
    bool isTriggered() { return state; }

private:

  EndstopState machineState;
  uint8_t pin;
  uint8_t level;
  uint32_t minInterval;
  uint32_t maxInterval;
  uint32_t lastTrigTime;
  bool state;
  
};
//--------------------------------------------------------------------------------------------------------------------------------

