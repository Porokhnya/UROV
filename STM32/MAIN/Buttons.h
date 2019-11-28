#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "CoreButton.h"
//--------------------------------------------------------------------------------------------------
class ButtonsList
{
  public:
    ButtonsList();
    void begin();
    void update();

  private:
  
    Button redButton;
    Button blueButton;
    Button yellowButton;

    bool inited;
    
};
//--------------------------------------------------------------------------------------------------
extern ButtonsList Buttons;
//--------------------------------------------------------------------------------------------------

