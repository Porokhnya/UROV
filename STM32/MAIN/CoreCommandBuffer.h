#pragma once

#include <Arduino.h>
#include "TinyVector.h"
#include "InterruptHandler.h"
//--------------------------------------------------------------------------------------------------------------------------------------
// класс для накопления команды из потока
//--------------------------------------------------------------------------------------------------------------------------------------
class CoreCommandBuffer
{
private:
  Stream* pStream;
  String* strBuff;
public:
  CoreCommandBuffer(Stream* s);

  bool hasCommand();
  const String& getCommand() {return *strBuff;}
  void clearCommand() {delete strBuff; strBuff = new String(); }
  Stream* getStream() {return pStream;}

};
//--------------------------------------------------------------------------------------------------------------------------------------
extern CoreCommandBuffer Commands;
//--------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<char*> CommandArgsVec;
//--------------------------------------------------------------------------------------------------------------------------------------
class CommandParser // класс-парсер команды из потока
{
  private:
    CommandArgsVec arguments;
  public:
    CommandParser();
    ~CommandParser();

    void clear();
    bool parse(const String& command, bool isSetCommand);
    const char* getArg(size_t idx) const;
    size_t argsCount() const {return arguments.size();}
};
//--------------------------------------------------------------------------------------------------------------------------------------
class CommandHandlerClass // класс-обработчик команд из потока
{
  public:
  
    CommandHandlerClass();
    
    void handleCommands();
    void processCommand(const String& command,Stream* outStream);

	int getFreeMemory();
  bool getVER(Stream* pStream);

 private:
  void onUnknownCommand(const String& command, Stream* outStream);
  
  bool getFILE(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение файла
  bool getFILESIZE(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение размера файла
  bool setDELFILE(CommandParser& parser, Stream* pStream); // удаление файла
  bool setUPLOADFILE(CommandParser& parser, Stream* pStream); // закачка файла
  bool getLS(const char* commandPassed, const CommandParser& parser, Stream* pStream);  // получение списка файлов
  
  bool getPIN(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение состояния входа
  bool setPIN(CommandParser& parser, Stream* pStream); // установка состояния выхода
  int16_t getPinState(uint8_t pin); // получение состояния входа

  bool getFREERAM(const char* commandPassed, Stream* pStream); // получение свободной памяти

  bool setDATETIME(const char* param); // установка даты/времени
  bool getDATETIME(const char* commandPassed, Stream* pStream); // получение даты/времени
  void setCurrentDateTime(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second); // установка даты/времени

  bool setMOTORESOURCE_CURRENT(CommandParser& parser, Stream* pStream); // установка текущего моторесурса
  bool getMOTORESOURCE_CURRENT(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение текущего моторесурса

  bool setRLENGTH(CommandParser& parser, Stream* pStream); // установка величины перемещения привода
  bool getRLENGTH(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение величины перемещения привода
  
  bool setMOTORESOURCE_MAX(CommandParser& parser, Stream* pStream); // установка максимального моторесурса
  bool getMOTORESOURCE_MAX(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение максимального моторесурса

  bool setPULSES(CommandParser& parser, Stream* pStream); // установка кол-ва импульсов
  bool getPULSES(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение кол-ва импульсов

  bool setDELTA(CommandParser& parser, Stream* pStream); // установка дельты импульсов
  bool getDELTA(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение дельты импульсов

  //DEPRECATED: bool getINDUCTIVE(const char* commandPassed, const CommandParser& parser, Stream* pStream);
  bool getVOLTAGE(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение напряжения на входе

  bool getUUID(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение уникального ID контроллера
  bool getSDTEST(const char* commandPassed, const CommandParser& parser, Stream* pStream); // тестирование SD

  bool setTBORDERMAX(CommandParser& parser, Stream* pStream); // установка верхнего порога трансформатора
  bool getTBORDERMAX(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение верхнего порога трансформатора

  bool setTBORDERMIN(CommandParser& parser, Stream* pStream); // установка нижнего порога трансформатора
  bool getTBORDERMIN(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение нижнего порога трансформатора

  bool setTBORDERS(CommandParser& parser, Stream* pStream); // установка порогов трансформатора
  bool getTBORDERS(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение порогов трансформатора

  bool setRDELAY(CommandParser& parser, Stream* pStream); // установка задержки реле
  bool getRDELAY(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение задержки реле

  bool setCCOEFF(CommandParser& parser, Stream* pStream); // установка коэффициента тока
  bool getCCOEFF(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение коэффициента тока

  bool setECDELTA(CommandParser& parser, Stream* pStream); // получение дельты импульсов
  bool getECDELTA(const char* commandPassed, const CommandParser& parser, Stream* pStream); // установка дельты импульсов

  bool getEREC(const CommandParser& cParser, Stream* pStream); // запись эталона

  bool getLASTTRIG(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение информации о последнем срабатывании

  bool setSKIPCOUNTER(CommandParser& parser, Stream* pStream); // установка пропуска импульсов
  bool getSKIPCOUNTER(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение пропуска импульсов

  bool setASUTPFLAGS(CommandParser& parser, Stream* pStream); // установка флагов АСУ ТП
  bool getASUTPFLAGS(const char* commandPassed, const CommandParser& parser, Stream* pStream); // получение флагов АСУ ТП



  bool printBackSETResult(bool isOK, const char* command, Stream* pStream);
    
};
//--------------------------------------------------------------------------------------------------------------------------------------
extern CommandHandlerClass CommandHandler;
//--------------------------------------------------------------------------------------------------------------------------------------
// обработчик команды прерывания
//--------------------------------------------------------------------------------------------------------------------------------------
class ExternalEthalonCommandHandler : public InterruptEventSubscriber
{
public:

	ExternalEthalonCommandHandler();

	// вызывается, когда прерывания на нужном номере завершены, и накоплена статистика
	virtual void OnInterruptRaised(CurrentOscillData* oscData, EthalonCompareResult result);

	// возвращает true, если запись удалась
	bool beginRecord(uint32_t timeout);

	void saveList(EthalonDirection direction); // сохраняет список прерываний в файл


private:

	bool done;
	uint32_t timer; // таймер для отсчёта времени
	InterruptEventSubscriber* oldSubscriber; // старый подписчик
};
//--------------------------------------------------------------------------------------------------------------------------------------
