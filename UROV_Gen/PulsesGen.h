#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <SdFat.h>
#include <Arduino.h>
#include "TinyVector.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	igNothing,
//	igFile,
//	igEEPROM,
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
  
  bool isDone() { return done; }
  bool isRunning()
  {
    return (workMode != igNothing) && !done;
  }
	
	
  private:

  uint8_t pin;
  
  void pinConfig();
  void timerConfig();

  void timerStart();
  void timerStop();

  uint32_t getNextPauseTime(bool& done);

  void wipe();
  
  ImpulseGeneratorWorkMode workMode;
  bool done;
  
//	SdFile file;
  
//	int memAddress;
//  uint32_t memCount;

  Vector<uint32_t> internalList;
 
	const Vector<uint32_t>* pList;
  size_t listIterator;
	
	uint32_t lastMicros;
  uint32_t pauseTime;
  
//	uint8_t currentPinLevel;
  bool inUpdateFlag;
  ImpulseGeneratorMachineState machineState;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern ImpulseGeneratorClass ImpulseGeneratorA;
extern ImpulseGeneratorClass ImpulseGeneratorB;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
