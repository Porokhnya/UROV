#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <SdFat.h>
#include <Arduino.h>
#include "TinyVector.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	igNothing,
	igExternalList, // внешний список
  igInternalList, // внутренний список
	
} ImpulseGeneratorWorkMode;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  onHighLevel, // обрабатываем высокий уровень на пине
  onBetweenPulses, // находимся в паузе между импульсами
  
} ImpulseGeneratorMachineState;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ImpulseGeneratorClass
{
  public:

  ImpulseGeneratorClass(uint8_t pin);
	
	void prepare(const String& fileName);
	void prepare(int memAddr);
	void prepare(const Vector<uint32_t>& list);

  void start();
	void stop();
	
	void update();
  
  bool isDone() { return stopped; }
  bool isRunning()
  {
    return ( (workMode != igNothing) && !stopped );
  }
	
	
  private:

  uint8_t pin;
  bool pinInited;
  
  void pinConfig();
  void pinWrite(uint8_t level);

  uint32_t getNextPauseTime();

  void wipe();
  
  ImpulseGeneratorWorkMode workMode;
  bool done;
  bool stopped;
  
  Vector<uint32_t> internalList;
 
	const Vector<uint32_t>* pList;
  size_t listIterator;
	
	uint32_t lastMicros;
  uint32_t pauseTime;
  
  bool inUpdateFlag;
  ImpulseGeneratorMachineState machineState;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern ImpulseGeneratorClass ImpulseGeneratorA;
extern ImpulseGeneratorClass ImpulseGeneratorB;
extern void genUpdate();
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
