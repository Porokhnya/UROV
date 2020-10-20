#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AT24CX.h"
#include "DS3231.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int raw;
  float voltage;
  
} VoltageData;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	rpBroken, // штанга поломана или находится в промежуточной позиции
	rpUp, // в верхней позиции
	rpDown // в нижней позиции
} RodDirection; // позиция штанги				
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SettingsClass
{
public:

  bool read8(uint32_t addr, uint8_t& val);
  bool read16(uint32_t addr, uint16_t& val);
  bool read32(uint32_t addr, uint32_t& val);

  void write8(uint32_t addr, uint8_t val);
  void write16(uint32_t addr, uint16_t val);
  void write32(uint32_t addr, uint32_t val);

	SettingsClass();

	void begin();

	void update();

  void reloadSettings();

  EEPROM_CLASS* getEEPROM() {return eeprom;}

	// возвращает настройку кол-ва импульсов
	uint16_t getPulses();

	// сохраняет настройку кол-ва импульсов
	void setPulses(uint16_t val);


 // работа с дельтами импульсов для сравнения эталона
 uint32_t getEthalonPulseDelta();
 void setEthalonPulseDelta(uint32_t val);

	// работа с дельтами импульсов
	uint8_t getPulsesDelta();
	void setPulsesDelta(uint8_t val);

	// работа с моторесурсом системы
	uint32_t getMotoresource();
	void setMotoresource(uint32_t val);

	uint32_t getMotoresourceMax();
	void setMotoresourceMax(uint32_t val);

	DS3231Temperature getTemperature() { return coreTemp; }

	void set3V3RawVoltage(uint16_t raw);
	void set5VRawVoltage(uint16_t raw);
	void set200VRawVoltage(uint16_t raw);

	VoltageData get3V3Voltage() { return voltage3V3; }
	VoltageData get5Vvoltage() { return voltage5V; }
	VoltageData get200Vvoltage() { return voltage200V; }

	String getUUID(const char* passedUUID);

	uint32_t getTransformerLowBorder();
	void setTransformerLowBorder(uint32_t val);

	uint32_t getTransformerHighBorder();
	void setTransformerHighBorder(uint32_t val);

	uint32_t getRelayDelay();
	void setRelayDelay(uint32_t rDelay);

	uint16_t getACSDelay();
	void setACSDelay(uint16_t rDelay);

	RodDirection getRodDirection() { return rodDirection; }
	void setRodDirection(RodDirection val) { rodDirection = val; }

  uint32_t getSkipCounter() {return skipCounter; }
  void setSkipCounter(uint32_t val);


  uint32_t getCurrentCoeff();
  void setCurrentCoeff(uint32_t c);

  uint8_t getAsuTpFlags();
  void setAsuTpFlags(uint8_t val);
    
  private:

    uint32_t ethalonPulseDelta;
    uint16_t channelPulses;
    uint8_t channelDelta;
    uint32_t motoresource, motoresourceMax;

    uint32_t transformerHighBorder, transformerLowBorder;

    uint32_t skipCounter;

    uint8_t asuTpFlags;

	  RodDirection rodDirection;

    EEPROM_CLASS* eeprom;
    DS3231Temperature coreTemp;
    uint32_t timer;


    VoltageData voltage3V3, voltage5V, voltage200V;

    uint32_t relayDelay;
    uint16_t acsDelay;

    uint32_t currentCoeff; // коэффициент по току
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
