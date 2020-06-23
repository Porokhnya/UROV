#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
#include "Drawing.h"
#include <SdFat.h>
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
     virtual void onDeactivate();  
    
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
   
    int clearButton, calculateButton, backButton, file1Button, file2Button, file3Button, file4Button, file5Button, grid_Button, countPulsesButton;


    int countPulses;
    String countPulsesCaption;
    
	  int touch_x_min;      // Запрет формировать точку по Х меньше предыдущей   
    
    Points chartPoints; // наш массив точек на экране
    Vector<uint32_t> pulsesList; // список длительностей импульсов

    void saveToFile(const char* fileName); // сохраняет список длительностей импульсов в файл
    void enableSaveButtons(bool en, bool redraw=false);
    void enableControlButtons(bool en, bool redraw=false);
    void writeToFile(SdFile& f, uint32_t rec);
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

