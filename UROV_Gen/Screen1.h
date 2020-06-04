#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// экран номер 1
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Screen1 : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new Screen1();
  }
  
  void requestToDrawChart( uint16_t* points1,   uint16_t* points2,  uint16_t* points3, uint16_t pointsCount);

   virtual void onActivate();
   virtual void onDeactivate();


protected:
  
    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:

    bool relAll_State, relLineA_State, relLineB_State, relLineC_State, relShunt1_State, relShunt2_State;
    void drawRelayState(TFTMenu* menu, bool anyway=false);

    int settingsButton,relAllButton, shunt1Button, shunt2Button;

    Screen1();

    void drawChart(); // рисуем наши графики
    bool canDrawChart;
    bool inDrawingChart;
    bool canLoopADC;
    
	  int getFreeMemory();
	  int oldsecond;

    void drawTime(TFTMenu* menu);
    
  	uint16_t* points1;
    uint16_t* points2;
    uint16_t* points3;
   
	  // НАШ ГРАФИК ДЛЯ ЭКРАНА
	  Chart chart;
	  ChartSerie* serie1;
	  ChartSerie* serie2;
	  ChartSerie* serie3;

   uint16_t getSynchroPoint(uint16_t* points, uint16_t pointsCount);
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern Screen1* mainScreen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
