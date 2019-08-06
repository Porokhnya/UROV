#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------
class RS485;
//--------------------------------------------------------------------------------------------------
extern "C" {
  void ON_RS485_INCOMING_DATA(RS485* Sender);
}
//--------------------------------------------------------------------------------------------------
#define STX1 0xAB
#define STX2 0xBA
#define ETX1 0xDE
#define ETX2 0xAD
//--------------------------------------------------------------------------------------------------
typedef enum
{
  rs485Ping = 10, // пинг от мастера, в данных - 4 байта ID пакета пинга
  rs485Pong, // ответ мастеру на пинг, в ответ посылаем ему 4 байта ID пакета пинга + 1 байт флаг - есть данные по прерываниям или нет
  rs485InterruptDataRequest, // запрос данных по прерыванию (от мастера)
  rs485InterruptDataAnswer, // ответ данных по прерыванию (от слейва)
  rs485TextData, // просто текстовые данные
  rs485TestInterrupt, // запрос на генерацию тестового массива с прерываниями
  
} RS485PacketType;
//--------------------------------------------------------------------------------------------------
#pragma pack(push,1)
struct RS485Packet
{
  uint8_t stx1;
  uint8_t stx2;
  uint8_t packetType;
  uint16_t dataLength;
  uint8_t dataCrc;
  uint8_t etx1;
  uint8_t etx2;
  uint8_t packetCrc;

  RS485Packet()
  {
    stx1 =  STX1;
    stx2 =  STX2;

    etx1 =  ETX1;
    etx2 =  ETX2;

    dataLength = 0;
    dataCrc = 0;
    packetCrc = 0;   
  }
  
};
#pragma pack(pop)
//--------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  uint32_t pingID;  // ID пинга
  uint8_t hasGuardTriggered;  // флаг - сработала защита или нет
} RS485PongPacket;
#pragma pack(pop)
//--------------------------------------------------------------------------------------------------
class RS485
{
  public:
    RS485(Stream& s, uint8_t dePin, uint32_t receiveTimeout);
    ~RS485();    
    
    void begin();
    void update();

    void switchToSend();
    void switchToReceive();

    // отправляет пакет нужного типа
    void send(RS485PacketType packetType, const uint8_t* data, uint16_t dataLength);
 
    RS485Packet getDataReceived(uint8_t* &data);
    void clearReceivedData();
    
    uint8_t crc8(const uint8_t *addr, uint16_t len);

  private:
    
    void waitTransmitComplete();
    uint8_t dePin;
    Stream* workStream;

    uint8_t writePtr;
    RS485Packet rs485Packet;
    uint8_t* rsPacketPtr;
    uint32_t receiveTimeout;
   
    uint8_t* dataReceived;

    bool gotRS485Packet();
    bool processRS485Packet();
};
//--------------------------------------------------------------------------------------------------

