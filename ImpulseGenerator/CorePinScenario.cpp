//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CorePinScenario.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CorePinScenario
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CorePinScenario::CorePinScenario()
{
  actions = new CorePinActionsList;
  isEnabled = true;
  currentActionIndex = 0;
  timer = 0;
  loopMode = true;
  isdone = false;
  wantreset = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::setLoop(bool bLoop)
{
  loopMode = bLoop;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::reset()
{
  isdone = false;
  wantreset = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::resetTimer()
{
  timer = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CorePinScenario::enabled()
{
  return isEnabled;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::enable()
{
  isEnabled = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::disable()
{
  isEnabled = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::clear()
{
  delete actions;
  actions = new CorePinActionsList;
  currentActionIndex = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::add(CorePinAction action)
{
  actions->push_back(action); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CorePinScenario::isDone()
{
  return isdone;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::begin()
{
  resetTimer();
  currentActionIndex = 0;
  isdone = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CorePinScenario::update()
{
  if(!isEnabled || !actions->size() || isdone)
    return;

   CorePinAction* action = &((*actions)[currentActionIndex]);

   if(!timer) // first call
   {
    timer = micros();
   }
   
   if(micros() - timer > action->duration)
   {
      currentActionIndex++;
      
      if(currentActionIndex >= actions->size())
      {
          if(loopMode)
            currentActionIndex = 0;
          else
          {
            if(wantreset)
            {
              currentActionIndex = 0;
              wantreset = false;
            }
            else
            {
              currentActionIndex = actions->size()-1;
              isdone = true;
            }
          }
      }

       action = &((*actions)[currentActionIndex]);
       digitalWrite(action->pin,action->level);
             
       timer = micros();

   }
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
