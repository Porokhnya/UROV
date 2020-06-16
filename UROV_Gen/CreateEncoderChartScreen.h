#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
#include "Drawing.h"
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

     virtual void onActivate();  
    
protected:
     
    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);
	

private:
      CreateEncoderChartScreen();
	  void get_Point_Screen(TFTMenu* menu);
	  void create_Schedule(TFTMenu* menu);
    
	  void clear_Grid(TFTMenu* menu);
   void drawGrid(TFTMenu* menu);
   
    int clearButton, calculateButton, backButton, file1Button, file2Button, file3Button, mem1Button, mem2Button, mem3Button, grid_Button;
    
	  int touch_x_min;      // Запрет формировать точку по Х меньше предыдущей   
    
    Points chartPoints; // наш массив точек на экране
    Vector<uint32_t> pulsesList; // список длительностей импульсов
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

