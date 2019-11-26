#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "TFTMenu.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// экран номер 4 Меню часов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Screen4 : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new Screen4();
  }
    
protected:

    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
      Screen4();
  
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

