#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "TFTMenu.h"
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
    Screen1();

    bool isRS485Online;
    void drawRS485State(TFTMenu* menu);

    void drawSDSpeedInfo(TFTMenu* menu);

    uint16_t oldChannel1Current, oldChannel2Current, oldChannel3Current;
    String oldCurrentString1,oldCurrentString2,oldCurrentString3;
    void drawCurrent(TFTMenu* menu);

    int last3V3Voltage, last5Vvoltage, last200Vvoltage;
    //void drawVoltage(TFTMenu* menu);

	//DEPRECATED: void drawInductiveSensors(TFTMenu* menu);
	//DEPRECATED: uint8_t inductiveSensorState1, inductiveSensorState2, inductiveSensorState3;

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
