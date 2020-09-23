#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "TFTMenu.h"
#include "InterruptHandler.h"
#include "Drawing.h"
#include "EthalonComparer.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
//  uint8_t channelNum;
  uint16_t chartColor;
  uint16_t compareColor;
  uint16_t foreCompareColor;
  const char* compareCaption;
  
} EthalonCompareBox;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//typedef Vector<EthalonCompareBox> EthalonCompareBoxes;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// экран с графиком прерываний
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class InterruptScreen : public AbstractTFTScreen, public InterruptEventSubscriber
{
  public:

  static AbstractTFTScreen* create();  

  void OnInterruptRaised(CurrentOscillData* oscData, EthalonCompareResult result);
    
protected:

    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

    virtual void onDeactivate();

private:
    InterruptScreen();

    void drawTime(TFTMenu* menu);

	uint32_t channelMotoresourcePercents;
	word motoresourceLastFontColor;
	uint32_t motoresourceBlinkTimer;
    void computeMotoresource();
    void drawMotoresource(TFTMenu* menu);

	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	CurrentOscillData* oscillData; // данные по току
	// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

  EthalonCompareBox compareBox;

    unsigned long startSeenTime;

    bool canAcceptInterruptData;

    void drawCompareResult(TFTMenu* menu);

//    Points serie;

    uint32_t timerDelta;  
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern InterruptScreen* ScreenInterrupt;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
