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

  bool read8(uint32_t addr, uint8_t& val);    // читаем один байт из памяти
  bool read16(uint32_t addr, uint16_t& val); // читаем два байта из памяти
  bool read32(uint32_t addr, uint32_t& val); // читаем четыре байта из памяти

  void write8(uint32_t addr, uint8_t val);  // пишем один байт в память
  void write16(uint32_t addr, uint16_t val);// пишем два байта в память
  void write32(uint32_t addr, uint32_t val);// пишем четыре байта в память

	SettingsClass();  // конструктор

	void begin(); // начинаем работу

	void update(); // обновляем внутреннее состояние

  void reloadSettings(); // перезагружаем настройки

  AT24CX* getEEPROM() {return eeprom;} // возвращаем указатель на класс работы с памятью

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

	DS3231Temperature getTemperature() { return coreTemp; } // возвращаем температуру системы

  // устанавливаем значение вольтажа
	void set3V3RawVoltage(uint16_t raw);
	void set5VRawVoltage(uint16_t raw);
	void set200VRawVoltage(uint16_t raw);

  // возвращаем значение вольтажа
	VoltageData get3V3Voltage() { return voltage3V3; }
	VoltageData get5Vvoltage() { return voltage5V; }
	VoltageData get200Vvoltage() { return voltage200V; }

	String getUUID(const char* passedUUID); // возвращаем уникальный ID контроллера

  // работа с порогами трансформатора
	uint32_t getTransformerLowBorder();
	void setTransformerLowBorder(uint32_t val);

	uint32_t getTransformerHighBorder();
	void setTransformerHighBorder(uint32_t val);

  // работа с задержкой реле
	uint32_t getRelayDelay();
	void setRelayDelay(uint32_t rDelay);

	uint16_t getACSDelay();
	void setACSDelay(uint16_t rDelay);

  // работа с направлением движения штанги
	RodDirection getRodDirection() { return rodDirection; }
	void setRodDirection(RodDirection val) { rodDirection = val; }

  // работа с пропуском импульсов
  uint32_t getSkipCounter()  __attribute__((always_inline)) {return skipCounter; }
  void setSkipCounter(uint32_t val);

  // работа с коэффициентом тока
  uint32_t getCurrentCoeff();
  void setCurrentCoeff(uint32_t c);

  // работа с флагами АСУ ТП
  uint8_t getAsuTpFlags();
  void setAsuTpFlags(uint8_t val);

  // работа с временем ожидания окончания импульсов
  uint32_t getMaxIdleTime() { return maxIdleTime; }
  void setMaxIdleTime(uint32_t val);

  // работа с величиной перемещения привода
  uint32_t getRodMoveLength() { return rodMoveLength; }
  void setRodMoveLength(uint32_t val);

  // работа со slave ID modbus
  uint8_t getModbusSlaveID() { return modbus_slave_id; }
  void setModbusSlaveID(uint8_t val);
    
  private:

    uint32_t ethalonPulseDelta;
    uint16_t channelPulses;
    uint8_t channelDelta;
    uint32_t motoresource, motoresourceMax;

    uint32_t transformerHighBorder, transformerLowBorder;
    uint32_t maxIdleTime;
    uint32_t rodMoveLength;

    uint32_t skipCounter;

    uint8_t asuTpFlags;

	  RodDirection rodDirection;

    uint8_t modbus_slave_id;

    AT24CX* eeprom;
    DS3231Temperature coreTemp;
    uint32_t timer;


    VoltageData voltage3V3, voltage5V, voltage200V;

    uint32_t relayDelay;
    uint16_t acsDelay;

    uint32_t currentCoeff; // коэффициент по току

    bool modbusHasSetupDone;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
