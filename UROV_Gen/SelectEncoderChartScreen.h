#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
#include "Drawing.h"
#include <SdFat.h>
#include "TinyVector.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// экран выбора графика прерываний для энкодера
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  lcmFromFile,
  lcmFromMemory,
  
} LoadChartMode;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SelectEncoderChartScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new SelectEncoderChartScreen();
  }

     virtual void onActivate();  
     virtual void onDeactivate();  
    
protected:
     
    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);
	

private:
      SelectEncoderChartScreen();   

	  void clear_Grid(TFTMenu* menu);
	  void drawGrid(TFTMenu* menu);
      int backButton, file1Button, file2Button, file3Button, file4Button, file5Button, file_selection;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LoadEncoderChartScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new LoadEncoderChartScreen();
  }

     virtual void onActivate();  
     virtual void onDeactivate();  


     void LoadChart(LoadChartMode mode, int index);
    
protected:
     
    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);
  

private:
      LoadEncoderChartScreen();   

      int doneButton;
      LoadChartMode loadMode;
      int loadIndex;

      Vector<const char*> lines;

      void drawLines(TFTMenu* menu, int color);

      void prepareFromFile();
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

