#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// экран настроек графика прерываний для энкодера
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChartSettingsScreen : public AbstractTFTScreen, public KeyboardInputTarget
{
  public:

  static AbstractTFTScreen* create()
  {
    return new ChartSettingsScreen();
  }

  virtual void onKeyboardInput(bool enterPressed, const String& enteredValue);
  virtual void onActivate();  
    
protected:

    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
      ChartSettingsScreen();

    int genPeriodButton , pulseWidthButton, workTimeButton; 
    int backButton;
	int menu_height = 30;
	int button_gap = 5;
	int height_button = 29;
	int width_button = 80;
    
    String genPeriodVal  , pulseWidthVal, workTimeVal;
      int currentEditedButton;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

