/*
 BLACK F407VG
 Serial commanication: SerialUART1
 Upload method: STLink
 */

//#include <UTFT.h>

#include <GxTFT.h>         // Hardware-specific library // Настройки дисплея
#include <GxIO/STM32GENERIC/GxIO_STM32F4_FSMC_8bit/GxIO_STM32F4_FSMC_8bit.h>  // Версия для подключения FSMC "BLACK F407VE/ZE/ZG boards".
#include <GxCTRL/GxCTRL_LGDP4524/GxCTRL_LGDP4524.h>

#include "Fonts/GFXFF/gfxfont.h"
#include "SmallRusFont.h"
#include "BigRusFont.h"
#include <URTouch.h>

#define TFT_SMALL_FONT (&SmallRusFont) // какой шрифт юзаем
#define TFT_BIG_FONT (&BigRusFont) // какой шрифт юзаем

// Define the orientation of the touch screen. Further 
// information can be found in the instructions.
#define TOUCH_ORIENTATION  LANDSCAPE

GxIO_Class io; // #define GxIO_Class is in the selected header file
GxCTRL_Class controller(io);
GxTFT myGLCD(io,controller,220,176);


//
//URTouch  myTouch( PB13, PB12, PB15, PB14, PC5);


uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center;
uint32_t calx, caly, cals;
char buf[13];

uint32_t drawTimer = 0;
uint8_t drawCounter = 0;
void draw()
{
  static const uint16_t bgColors[] = {
    MAGENTA,
    RED,
    YELLOW,
    BLUE,
    GREEN,
    WHITE,
    BLACK
  };

  static const uint16_t fgColors[] = {
    BLACK,
    WHITE,
    BLACK,
    BLACK,
    BLACK,
    BLACK,
    WHITE
  };

  static const char* strings[] = {
    "BLACK ON MAGENTA",
    "WHITE ON RED",
    "BLACK ON YELLOW",
    "BLACK ON BLUE",
    "BLACK ON GREEN",
    "BLACK ON WHITE",
    "WHITE ON BLACK"
    
  };

  if(!(millis() - drawTimer >= 2000))
    return;


  uint16_t bgColor = bgColors[drawCounter];
  uint16_t fgColor = fgColors[drawCounter];
  const char* str = strings[drawCounter];


  Serial.println("TRY to draw string on TFT.");
  myGLCD.fillScreen(bgColor);  
  myGLCD.setTextColor(fgColor,bgColor);


  int dw = myGLCD.width();
  int dh = myGLCD.height();
  int tw = myGLCD.textWidth(str,1);
  int th = myGLCD.fontHeight(1);
  int left = (dw - tw)/2;
  int top = (dh - th)/2;
   
  
  myGLCD.drawString(str, left, top,1);   


    drawTimer = millis();
    drawCounter++;
    if(drawCounter >= sizeof(bgColors)/sizeof(bgColors[0]))
      drawCounter = 0;
}

void setup()
{
  Serial.begin(57600);
  delay(3000);
  Serial.println("Begin.");

  Serial.println("init TFT..."); Serial.flush();

  
  myGLCD.init();
  myGLCD.setRotation(3);
  myGLCD.setFreeFont(TFT_SMALL_FONT);

 // Serial.print("TFT ID: "); Serial.flush();
 // Serial.println(controller.readID(), HEX);
  


  
/*
 // myTouch.InitTouch(TOUCH_ORIENTATION);
 // myTouch.setPrecision(PREC_HI);
  dispx=myGLCD.width();
  dispy=myGLCD.height();
  text_y_center=(dispy/2)-6;


  myGLCD.fillRect(100,100,20,20,RED);
*/
  
  Serial.println("Started."); Serial.flush();
}
/*
void drawCrossHair(int x, int y, uint16_t color)
{
  myGLCD.drawRect(x-10, y-10, 20, 20,color);
  myGLCD.drawLine(x-5, y, x+5, y,color);
  myGLCD.drawLine(x, y-5, x, y+5,color);
}

void readCoordinates()
{
  int iter = 5000;
  int failcount = 0;
  int cnt = 0;
  uint32_t tx=0;
  uint32_t ty=0;
  boolean OK = false;
  
  while (OK == false)
  {
    myGLCD.setTextColor(WHITE,BLACK);
    myGLCD.drawString("*  PRESS  *", 100, text_y_center,1);
    while (myTouch.dataAvailable() == false) {}
    myGLCD.drawString("*  HOLD!  *", 100, text_y_center,1);
    while ((myTouch.dataAvailable() == true) && (cnt<iter) && (failcount<10000))
    {
      myTouch.calibrateRead();
      if (!((myTouch.TP_X==65535) || (myTouch.TP_Y==65535)))
      {
        tx += myTouch.TP_X;
        ty += myTouch.TP_Y;
        cnt++;
      }
      else
        failcount++;
    }
    if (cnt>=iter)
    {
      OK = true;
    }
    else
    {
      tx = 0;
      ty = 0;
      cnt = 0;
    }
    if (failcount>=10000)
      fail();
  }

  cx = tx / iter;
  cy = ty / iter;
}

void calibrate(int x, int y, int i)
{
  myGLCD.setTextColor(WHITE,BLACK);
  drawCrossHair(x,y,RED);
  myGLCD.setTextColor(WHITE,RED);
  readCoordinates();
  myGLCD.setTextColor(WHITE,BLACK);
  myGLCD.drawString("* RELEASE *", 100, text_y_center,1);
  myGLCD.setTextColor(LGRAY,BLACK);
  drawCrossHair(x,y,LGRAY);
  rx[i]=cx;
  ry[i]=cy;
  while (myTouch.dataAvailable() == true) {}
}

void waitForTouch()
{
  while (myTouch.dataAvailable() == true) {}
  while (myTouch.dataAvailable() == false) {}
  while (myTouch.dataAvailable() == true) {}
}

void toHex(uint32_t num)
{
  buf[0] = '0';
  buf[1] = 'x';
  buf[10] = 'U';
  buf[11] = 'L';
  buf[12] = 0;
  for (int zz=9; zz>1; zz--)
  {
    if ((num & 0xF) > 9)
      buf[zz] = (num & 0xF) + 55;
    else
      buf[zz] = (num & 0xF) + 48;
    num=num>>4;
  }
}

void startup()
{
//  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, dispx-1, 13,RED);
 // myGLCD.setColor(255, 255, 255);
  myGLCD.setTextColor(WHITE,BLACK);
  myGLCD.drawLine(0, 14, dispx-1, 14,WHITE);
  myGLCD.drawString("URTouch Calibration", 0, 1,1);
  //myGLCD.setBackColor(0, 0, 0);

  if (dispx==220)
  {  
    myGLCD.drawString("Use a stylus or something", 1, 30,1);
    myGLCD.drawString("similar to touch as close", 1, 42,1);
    myGLCD.drawString("to the center of the", 1, 54,1);
    myGLCD.drawString("highlighted crosshair as", 1, 66,1);
    myGLCD.drawString("possible. Keep as still as", 1, 78,1);
    myGLCD.drawString("possible and keep holding", 1, 90,1);
    myGLCD.drawString("until the highlight is", 1, 102,1);
    myGLCD.drawString("removed. Repeat for all", 1, 114,1);
    myGLCD.drawString("crosshairs in sequence.", 1, 126,1);
    myGLCD.drawString("Touch screen to continue", 1, 162,1);
  }
  else
  {
    myGLCD.drawString("INSTRUCTIONS", 1, 30,1);
    myGLCD.drawString("Use a stylus or something similar to", 1, 50,1);
    myGLCD.drawString("touch as close to the center of the", 1, 62,1);
    myGLCD.drawString("highlighted crosshair as possible. Keep", 1, 74,1);
    myGLCD.drawString("as still as possible and keep holding", 1, 86,1);
    myGLCD.drawString("until the highlight is removed. Repeat", 1, 98,1);
    myGLCD.drawString("for all crosshairs in sequence.", 1, 110,1);

    myGLCD.drawString("Further instructions will be displayed", 1, 134,1);
    myGLCD.drawString("when the calibration is complete.", 1, 146,1);

    myGLCD.drawString("Do NOT use your finger as a calibration", 1, 170,1);
    myGLCD.drawString("stylus or the result WILL BE imprecise.", 1, 182,1);

    myGLCD.drawString("Touch screen to continue", 1, 226,1);
  }

  waitForTouch();
  myGLCD.fillScreen(BLACK);
}

void done()
{
  myGLCD.fillScreen(BLACK);
  myGLCD.fillRect(0, 0, dispx-1, 13,RED);
  myGLCD.drawLine(0, 14, dispx-1, 14,WHITE);
  myGLCD.setTextColor(WHITE,BLACK);
  myGLCD.drawString("URTouch Calibration", 1, 1);
//  myGLCD.setBackColor(0, 0, 0);
  
  if (dispx==220)
  {  
    myGLCD.drawString("To use the new calibration", 1, 30,1);
    myGLCD.drawString("settings you must edit the", 1, 42,1);
    myGLCD.drawString("URTouchCD.h", 1, 54,1);
    myGLCD.drawString("file and change", 88, 54,1);
    myGLCD.drawString("the following values. The", 1, 66,1);
    myGLCD.drawString("values are located right", 1, 78,1);
    myGLCD.drawString("below the opening comment.", 1, 90,1);

    myGLCD.setFreeFont(TFT_BIG_FONT);

    
    myGLCD.drawString("CAL_X", 1, 110,1);
    myGLCD.drawString("CAL_Y", 1, 140,1);
    myGLCD.drawString("CAL_S", 1, 170,1);
    toHex(calx);
    Serial.print("#define CAL_X "); Serial.println(buf);
    myGLCD.drawString(buf, 75, 110,1);
    toHex(caly);
    Serial.print("#define CAL_Y "); Serial.println(buf);
    myGLCD.drawString(buf, 75, 140,1);
    toHex(cals);
    Serial.print("#define CAL_S "); Serial.println(buf);
    myGLCD.drawString(buf, 75, 170,1);

    myGLCD.setFreeFont(TFT_SMALL_FONT);
  }
  else
  {  
    myGLCD.drawString("CALIBRATION COMPLETE", 1, 30,1);
    myGLCD.drawString("To use the new calibration", 1, 50,1);
    myGLCD.drawString("settings you must edit the", 1, 62,1);
    myGLCD.drawString("URTouchCD.h", 1, 74,1);
    myGLCD.drawString("file and change", 88, 74,1);
    myGLCD.drawString("the following values.", 1, 86,1);
    myGLCD.drawString("The values are located right", 1, 98,1);
    myGLCD.drawString("below the opening comment in", 1, 110,1);
    myGLCD.drawString("the file.", 1, 122,1);
    myGLCD.drawString("CAL_X", 1, 150,1);
    myGLCD.drawString("CAL_Y", 1, 162,1);
    myGLCD.drawString("CAL_S", 1, 174,1);

    toHex(calx);
    Serial.print("#define CAL_X "); Serial.println(buf);
    myGLCD.drawString(buf, 75, 150);
    toHex(caly);
    Serial.print("#define CAL_Y "); Serial.println(buf);
    myGLCD.drawString(buf, 75, 162);
    toHex(cals);
    Serial.print("#define CAL_S "); Serial.println(buf);
    myGLCD.drawString(buf, 75, 174);
  }
  
}

void fail()
{
  myGLCD.fillScreen(BLACK);
  myGLCD.fillRect(0, 0, dispx-1, 13,RED);
  myGLCD.setTextColor(WHITE,RED);
  myGLCD.drawLine(0, 14, dispx-1, 14,RED);
  myGLCD.drawString("URTouch Calibration FAILED", 1, 1,1);
  myGLCD.setTextColor(WHITE,BLACK);
  
  myGLCD.drawString("Unable to read the position", 0, 30,1);
  myGLCD.drawString("of the press. This is a", 0, 42,1);
  myGLCD.drawString("hardware issue and can", 0, 54,1);
  myGLCD.drawString("not be corrected in", 0, 66,1);
  myGLCD.drawString("software.", 0, 78,1);
  
  while(true) {};
}
*/

void loop()
{
  draw();
  
/*  
  startup();
  
//  myGLCD.setColor(80, 80, 80);
  drawCrossHair(dispx-11, 10,LGRAY);
  drawCrossHair(dispx/2, 10,LGRAY);
  drawCrossHair(10, 10,LGRAY);
  drawCrossHair(dispx-11, dispy/2,LGRAY);
  drawCrossHair(10, dispy/2,LGRAY);
  drawCrossHair(dispx-11, dispy-11,LGRAY);
  drawCrossHair(dispx/2, dispy-11,LGRAY);
  drawCrossHair(10, dispy-11,LGRAY);
  myGLCD.setTextColor(WHITE,RED);
  myGLCD.drawString("***********", 100, text_y_center-12,1);
  myGLCD.drawString("***********", 100, text_y_center+12,1);

  calibrate(10, 10, 0);
  calibrate(10, dispy/2, 1);
  calibrate(10, dispy-11, 2);
  calibrate(dispx/2, 10, 3);
  calibrate(dispx/2, dispy-11, 4);
  calibrate(dispx-11, 10, 5);
  calibrate(dispx-11, dispy/2, 6);
  calibrate(dispx-11, dispy-11, 7);
  
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals=(long(dispx-1)<<12)+(dispy-1);
  else
    cals=(long(dispy-1)<<12)+(dispx-1);

  if (TOUCH_ORIENTATION == PORTRAIT)
    px = abs(((float(rx[2]+rx[4]+rx[7])/3)-(float(rx[0]+rx[3]+rx[5])/3))/(dispy-20));  // PORTRAIT
  else
    px = abs(((float(rx[5]+rx[6]+rx[7])/3)-(float(rx[0]+rx[1]+rx[2])/3))/(dispy-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    clx = (((rx[0]+rx[3]+rx[5])/3));  // PORTRAIT
    crx = (((rx[2]+rx[4]+rx[7])/3));  // PORTRAIT
  }
  else
  {
    clx = (((rx[0]+rx[1]+rx[2])/3));  // LANDSCAPE
    crx = (((rx[5]+rx[6]+rx[7])/3));  // LANDSCAPE
  }
  if (clx<crx)
  {
    clx = clx - (px*10);
    crx = crx + (px*10);
  }
  else
  {
    clx = clx + (px*10);
    crx = crx - (px*10);
  }
  
  if (TOUCH_ORIENTATION == PORTRAIT)
    py = abs(((float(ry[5]+ry[6]+ry[7])/3)-(float(ry[0]+ry[1]+ry[2])/3))/(dispx-20));  // PORTRAIT
  else
    py = abs(((float(ry[0]+ry[3]+ry[5])/3)-(float(ry[2]+ry[4]+ry[7])/3))/(dispx-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    cty = (((ry[5]+ry[6]+ry[7])/3));  // PORTRAIT
    cby = (((ry[0]+ry[1]+ry[2])/3));  // PORTRAIT
  }
  else
  {
    cty = (((ry[0]+ry[3]+ry[5])/3));  // LANDSCAPE
    cby = (((ry[2]+ry[4]+ry[7])/3));  // LANDSCAPE
  }
  if (cty<cby)
  {
    cty = cty - (py*10);
    cby = cby + (py*10);
  }
  else
  {
    cty = cty + (py*10);
    cby = cby - (py*10);
  }
  
  calx = (long(clx)<<14) + long(crx);
  caly = (long(cty)<<14) + long(cby);
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals = cals + (1L<<31);

  done();
  while(true) {}
  */
}
