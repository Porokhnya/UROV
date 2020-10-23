#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "Debounce.h"
//--------------------------------------------------------------------------------------------------
class RelayGuardClass : public Debounce
{
 public:
  
    RelayGuardClass();

    bool isTriggered();

    
};
//--------------------------------------------------------------------------------------------------
extern RelayGuardClass RelayGuard;
