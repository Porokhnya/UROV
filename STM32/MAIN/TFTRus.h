#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "TFT_Includes.h"
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс поддержки русских шрифтов для GxTFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTRus
{
  public:
    TFTRus();
    void init(TFT_Class* uTft);

    int print(const char* str,int x, int y, word bgColor=TFT_BACK_COLOR, word fgColor=TFT_FONT_COLOR, bool computeTextWidthOnly=false);
    int getStringLength(const char* str);
    int textWidth(const char* str);
    
  private:
    TFT_Class* pDisplay;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
