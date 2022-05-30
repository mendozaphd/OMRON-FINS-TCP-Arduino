
#ifndef _FINS_TCP_H_
#define _FINS_TCP_H_

#include <Arduino.h>
// #include <SPI.h>
// #include <EthernetENC.h>
#include <WiFi.h>
#include "FINS_constants.h"
//#include <vector>
//#include <Vector.h>
using namespace std;

const char dia_de_la_semana[7][10] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

class FINS_TCP
{
private:
    // struct sockaddr_in _serveraddr;
    uint16_t _port;
    const char *_ip;
    WiFiClient cliente;
    int finsCommandLen = 0;
    int finsResponseLen = 0;

    bool conectado = false;
    uint8_t cmdFS[16];
    uint8_t respFS[16];
    uint8_t respFinsData[2048];
    uint8_t cmdFins[22];
    uint8_t respFins[2048];
    uint8_t *Response;
    String lastError;
    uint8_t DA1__, SA1__;
    FinsErrors FrameSend(const uint8_t data[] = new uint8_t[0], int words = 0);
    void tcpFinsCommand(uint8_t ServiceID = 0x01);
    bool NodeAddressDataSend();

    bool Connect();
    void Close();
    int Send(const uint8_t command[], int cmdLen);
    int Receive(uint8_t response[], int respLen);
    void commandTypePrint(CommandType comand_type);

public:
    FINS_TCP();
    // FINS_TCP(WiFiClient &cliente_);

    // ~tcpTransport();

    bool init(const char *omron_plc_ip, uint16_t fins_port = 9600);

    /* Read 1 word from an specific memory area and address and return its value in one 16 bits unsigned word */
    uint16_t readWord(MemoryArea area, uint16_t address);

    /* Read Any Memory Area(words) and Write to ans_items 16 bits array and returns the error code of the operation
    Params...
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start read words
    No_words = number of words to read
    Ans_items = uint16_t array to save the data readed
    */
    FinsErrors MemoryAreaRead(MemoryArea area, uint16_t address, uint16_t no_words, uint16_t ans_items[]);

    /* Write to Any Memory Area from  uint16_t WORD and returns the error code of the operation
    Params...
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start writing words
    No_words = number of words to write
    Data = unsigned 16 bits word
    */
    FinsErrors MemoryAreaWrite(MemoryArea area, uint16_t start_address, uint16_t data);

    /* Write to Any Memory Area from  uint8_t array and returns the error code of the operation
    Params...
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start writing words
    No_words = number of words to write
    Data[] = array where is the uint8_t information(2 are requeired from 1 word(16 bits))
    */
    FinsErrors MemoryAreaWrite(MemoryArea area, uint16_t start_address, uint16_t number_of_words, uint8_t data[]);

    /* Write to Any Memory Area from  uint16_t array and returns the error code of the operation
    Params...
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start writing words
    No_words = number of words to write
    Data[] = array where is the uint8_t information
    */
    FinsErrors MemoryAreaWrite(MemoryArea area, uint16_t start_address, uint16_t number_of_words, uint16_t data[]);

    /* Write to Any Memory Area from signed int value and returns the error code of the operation
    >>>>>> (this operation uses just 1 word to store the int value) <<<<<<
    Params...
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start writing words
    No_words = number of words to write
    Int = int signed value of 16 bits
    */
    FinsErrors MemoryAreaWrite(MemoryArea area, uint16_t start_address, int int_value);

    /* Write to Any Memory Area from signed float value and returns the error code of the operation
    >>>>>> (this operation uses 2 consecutive words to store the float value) <<<<<<
    Params...
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start writing words
    No_words = number of words to write
    float_value = float value of 32 bits
    */
    FinsErrors MemoryAreaWrite(MemoryArea area, uint16_t start_address, float float_value);

    /* Write to Any Memory Area from signed double value and returns the error code of the operation
    >>>>>>> (this operation uses 4 consecutive words to store the double value) <<<<<<
    Memory Area = memory area to read (DM,H,CIO..)
    Address= Memory area to start writing words
    No_words = number of words to write
    float_value = float value of 64 bits
    */
    FinsErrors MemoryAreaWrite(MemoryArea area, uint16_t start_address, double double_value);

    /* Prints the time in the PLC  */
    FinsErrors ClockRead();

    /* Read the specific value of one bit in one word and returns it value 0 or 1
    Params...
     Memory Area = memory area where is the bit to read(DM,H,CIO..)
    Address= Memory address where is the bit
    Bit_position = number of bit of the word(0-15)
     */
    uint8_t readBit(MemoryArea area, uint16_t address, uint8_t bit_position);

    /* Write the specific value to one bit in one word and returns the error code of the operation
    Params...
    Memory Area = memory area where is the bit to write(DM,H,CIO..)
    Address= Memory address where is the bit to write
    Bit_position = number of bit of the word(0-15)
    Value = 0 or 1 to write
    >>Note. if the bit is used for a program, it will maybe not change, to ensure the change use forcedBit()<<
    */
    FinsErrors writeBit(MemoryArea area, uint16_t address, uint8_t bit_position, uint8_t value);

    /* Try to Toggle the value of one bit in one word and returns the error code of the operation
    Params...
    Memory Area = memory area where is the bit to write(DM,H,CIO..)
    Address= Memory address where is the bit to toggle
    BIt_position = number of bit of the word(0-15)
    Value = 0 or 1 to write
    >>Note. if the bit is used for a program, it will maybe not change, to ensure the change use toggleBitForced()<<
    */
    FinsErrors toggleBit(MemoryArea area, uint16_t address, uint8_t bit_position);

    /* Force to Toggle the value of one bit in one word and returns the error code of the operation
    Params...
    Memory Area = memory area where is the bit to write(DM,H,CIO..)
    Address= Memory address where is the bit to toggle
    BIt_position = number of bit of the word(0-15)
    Value = 0 or 1 to write
    Note. if the bit is used for a program, it will maybe not change, to ensure the change use toggleBitForced
    */
    FinsErrors toggleBitForced(MemoryArea area, uint16_t address, uint8_t bit_position);

    /*    Forced set or reset one bit from 1 memory address
    Params...
    Memory Area = memory area where is the bit to write(DM,H,CIO..)
    Address= Memory address where is the bit to toggle
    Bit_position = number of bit of the word(0-15)
    Force_mode = Forced_Reset to put the bit in 0 logic, Forced_Set  to put the bit un 1 logic
    */
    FinsErrors forcedBit(MemoryArea area, uint16_t address, uint8_t bit_position, Force_Mode set_reset);

    /*
    Cancel All forced Signals in PLC
    Note. Sometimes the canceled signals doest returns to it original state
    */
    FinsErrors cancelAllForced();

    /*
    Change the PLC status to another state
    PLC_MODE = MONITOR; RUN; STOP or PROGRAM
    */
    FinsErrors changePLCMode(PLC_MODE modo);

    /*Read the PLC status and returns the PLC mode PLC_MODE = MONITOR; RUN; STOP or PROGRAM
     */
    PLC_MODE PLCStatusRead();

    bool StopCPU(uint16_t ProgNum1, uint16_t ProgNum2);
    bool ClockWrite(uint16_t Year, uint16_t Month, uint16_t Date, uint16_t Hour, uint16_t Minute, uint16_t Second, uint16_t Day);
};

#endif
