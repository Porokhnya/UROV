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
void CorePinScenario::update()
{
  if(!isEnabled || !actions->size())
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
        currentActionIndex = 0;

       action = &((*actions)[currentActionIndex]);
       digitalWrite(action->pin,action->level);
      
       timer = micros();
   }
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
