#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// экран создания графика прерываний для энкодера
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CreateEncoderChartScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new CreateEncoderChartScreen();
  }
    
protected:

    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
      CreateEncoderChartScreen();

      int backButton;
  
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

