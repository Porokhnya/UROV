#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <UTFT.h>
#include "TinyVector.h"
#include "InterruptHandler.h"
#include "UTFTMenu.h"
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  uint8_t R,G,B;

} RGBColor;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct _Point
{
  int X;
  int Y;

  _Point()
  {
    X = 0;
    Y = 0;
  }

  _Point(int _x, int _y)
  {
    X = _x;
    Y = _y;
  }

  bool operator== (const _Point& rhs)
  {
	  if (this == &rhs)
		  return true;

	  return(X == rhs.X) && (Y == rhs.Y);
  }
} Point;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<Point> Points;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AbstractTFTScreen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace Drawing
{
  void DrawGrid(int startX, int startY, int columnsCount, int rowsCount, int columnWidth, int rowHeight, RGBColor gridColor);
  void ComputeChart(const InterruptTimeList& list1, Points& serie1, uint16_t xPoints, uint16_t yPoints, int xCoord, int yCoord, int yStart,bool intervalsComputed=false);
  void DrawChart(AbstractTFTScreen* caller, const Points& serie1,uint16_t serie1Color=VGA_WHITE, int gridX = INTERRUPT_CHART_GRID_X_START, int gridY = INTERRUPT_CHART_GRID_Y_START, int colCount=6, int rowCount=4, int colWidth=INTERRUPT_CHART_X_POINTS/6, int rowHeight=INTERRUPT_CHART_Y_POINTS/4);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct _ChartSavedPixel
{
  int x;
  int y;

  bool operator==(const _ChartSavedPixel& rhs)
  {
	  if (this == &rhs)
		  return true;

	  return (x == rhs.x) && (y == rhs.y);
  }
  
} ChartSavedPixel;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<ChartSavedPixel> SavedPixelsList;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Chart;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChartSerie
{
  public:
    ChartSerie(Chart* parent, RGBColor color);
    ~ChartSerie();
    
    void setColor(RGBColor color)
    {
      serieColor = color;
    }

    void setPoints(uint16_t* pointsArray, uint16_t countOfPoints);
    uint16_t getPointsCount() {return pointsCount;}

    protected:

      friend class Chart;

      uint16_t getMaxYValue();

      void clearLine(UTFT* dc, uint16_t xPoint);
      void drawLine(UTFT* dc, uint16_t xPoint);
    

  private:

      void drawLine(UTFT* dc,uint16_t x, uint16_t y, uint16_t x2, uint16_t y2);

      uint16_t* points;
      uint16_t pointsCount;
      
      RGBColor serieColor;
      Chart* parentChart;

      SavedPixelsList savedPixels;
          
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<ChartSerie*> ChartSeries;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Chart
{
    public:
      Chart();
      ~Chart();

      void setCoords(uint16_t startX, uint16_t startY)
      {
        xCoord =  startX;
        yCoord = startY;
        
      }

      void setPoints(uint16_t pX, uint16_t pY);
      
      uint16_t getXCoord() { return xCoord; }
      uint16_t getYCoord() { return yCoord; }

       // очищает серии
      void clearSeries();
      
      // добавляет серию
      ChartSerie* addSerie(RGBColor color);

      // рисует все серии
      void draw();

     // прекращает отрисовку
     void stopDraw() { stopped = true; }

	 bool busy() {
		 return inDraw;
	 }

    protected:
    
      friend class ChartSerie;
      uint16_t getMaxYValue();
      
      uint16_t getYMax()
      {
        return yPoints;
      }

      uint16_t getXMax()
      {
        return xPoints;
      }


    private:

    bool stopped, inDraw;

    uint16_t xPoints, yPoints;
    byte* pixelsHits;

    uint16_t computedMaxYValue;
          
    uint16_t xCoord, yCoord;
    ChartSeries series;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

