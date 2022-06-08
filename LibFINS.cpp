#include <iostream>
#include <string.h>
#include "LibFINS.h"
#include <pthread.h>

#include <iterator>
using namespace std;

#define ANIO 0
#define MES 1
#define DIA 2
#define HORA 3
#define MINUTO 4
#define SEGUNDOS 5
#define DIA_DE_LA_SEMANA 6

FINS_TCP::FINS_TCP()
{
}

// FINS_TCP::FINS_TCP(WiFiClient &cliente_)
// {
//     cliente = &cliente_;
// }

bool FINS_TCP::init(const char *omron_plc_ip, uint16_t fins_port)
{
    _ip = omron_plc_ip;
    _port = fins_port;

    if (!Connect())
    {
        Serial.println("No se pudo conectar al PLC");
        Close();
        return false;
    }
    Close();

    // if (!NodeAddressDataSend())
    // {
    //     Serial.println("Hubo un problema en la respuesta del PLC");
    //     Close();
    //     return false;
    // }

    // Close();
    if (PLCStatusRead() != CPU_FAIL)
        return true;
    // FinsErrors clock = ClockRead();
    // if (clock == Completed_normally)
    //     return true;
    return false;
}

bool FINS_TCP::Connect()
{

    if (conectado)
        return true;
    if (cliente.connect(_ip, _port))
    {

        // conectado = true;
        //  Serial.print("Conectado ..");
        conectado = NodeAddressDataSend();
        // if (!conectado)
        //     Serial.println("Algo salio mal con la conexion");
        // Serial.println("Cliente conectado OKK-------<<<<<<<<<<<<<<");
    }
    else
    {
        conectado = false;
        Serial.println("Algo salio mal con la conexion(2)");
    }
    return conectado;
}

void FINS_TCP::Close()
{
    conectado = false;
    cliente.stop();
}

void FINS_TCP::tcpFinsCommand(uint8_t ServiceID)
{

    Response = &respFinsData[0];

    // Serial.println("TCP command 2");
    //---- COMMAND HEADER -------------------------------------------------------
    cmdFins[ICF] = 0x80; // 00 ICF Information control field
    cmdFins[RSC] = 0x00; // 01 RSC Reserved
    cmdFins[GTC] = 0x02; // 02 GTC Gateway count
    cmdFins[DNA] = 0x00; // 03 DNA Destination network address (0=local network)
    // cmdFins[DA1] = 0x00;      // 04 DA1 Destination node number
    cmdFins[DA1] = DA1__;
    cmdFins[DA2] = 0x00; // 05 DA2 Destination unit address
    cmdFins[SNA] = 0x00; // 06 SNA Source network address (0=local network)

    // cmdFins[SA1] = 0x00;      // 07 SA1 Source node number
    cmdFins[SA1] = SA1__;     // 07 SA1 Source node number
    cmdFins[SA2] = 0x00;      // 08 SA2 Source unit address
    cmdFins[SID] = ServiceID; // 09 SID Service ID
    //---- COMMAND --------------------------------------------------------------
    cmdFins[MC] = 0x00; // 10 MC Main command
    cmdFins[SC] = 0x00; // 11 SC Subcommand
    //---- PARAMS ---------------------------------------------------------------
    cmdFins[12] = 0x00; // 12 reserved area for additional params
    cmdFins[13] = 0x00; // depending on fins command
    cmdFins[14] = 0x00;
    cmdFins[15] = 0x00;
    cmdFins[16] = 0x00;
    cmdFins[17] = 0x00;
    cmdFins[18] = 0x00;
    cmdFins[19] = 0x00;
    cmdFins[20] = 0x00;
    cmdFins[21] = 0x00;

    fill_n(cmdFS, 16, 0);
    cmdFS[0] = 0x46; // 'F'
    cmdFS[1] = 0x49; // 'I'
    cmdFS[2] = 0x4E; // 'N'
    cmdFS[3] = 0x53; // 'S'
    cmdFS[10] = 0x00;
    cmdFS[11] = 0x02; // Command FS Sending=2 / Receiving=3

    fill_n(respFins, 2048, 0);
    fill_n(respFinsData, 2048, 0);

    finsCommandLen = 0;
    finsResponseLen = 0;
}

bool FINS_TCP::StopCPU(uint16_t Num1, uint16_t Num2)
{
    Serial.println("DETENIENDO EL CPU");
    cmdFins[MC] = 0x04;
    cmdFins[SC] = 0X02;
    cmdFins[F_PARAM] = (uint8_t)((Num1 >> 8) & 0XFF);
    cmdFins[F_PARAM + 1] = (uint8_t)(Num2 & 0XFF);

    finsCommandLen = 12;
    return FrameSend();
}

FinsErrors FINS_TCP::ClockRead()
{
    tcpFinsCommand();
    Serial.print("LEYENDO EL RELOJ.. ");
    cmdFins[MC] = 0x07;
    cmdFins[SC] = 0x01;

    finsCommandLen = 12;

    FinsErrors stat_ = FrameSend();
    if (stat_ != Completed_normally)
        return stat_;

    Serial.print("Fecha : " + String(dia_de_la_semana[respFinsData[DIA_DE_LA_SEMANA]]) + " - " + String(respFinsData[DIA], HEX) + "/" + String(respFinsData[MES], HEX) + "/20" + String(respFinsData[ANIO], HEX));
    Serial.println("  Hora = " + String(respFinsData[HORA], HEX) + ":" + String(respFinsData[MINUTO], HEX) + ":" + String(respFinsData[SEGUNDOS], HEX));

    return stat_;
}

bool FINS_TCP::ClockWrite(uint16_t Year, uint16_t Month, uint16_t Date, uint16_t Hour, uint16_t Minute, uint16_t Second, uint16_t Day)
{
    Serial.println("CONFIGURANDO EL RELOJ");
    tcpFinsCommand();
    cmdFins[MC] = 0x07;
    cmdFins[SC] = 0X02;

    cmdFins[F_PARAM] = (uint8_t)(Year);
    cmdFins[F_PARAM + 1] = (uint8_t)(Month);
    cmdFins[F_PARAM + 2] = (uint8_t)(Date);
    cmdFins[F_PARAM + 3] = (uint8_t)(Hour);
    cmdFins[F_PARAM + 4] = (uint8_t)(Minute);
    cmdFins[F_PARAM + 5] = (uint8_t)(Second);
    cmdFins[F_PARAM + 6] = (uint8_t)(Day);

    finsCommandLen = 19;
    return FrameSend();
}

FinsErrors FINS_TCP::MemoryAreaRead(MemoryArea area, uint16_t address, uint16_t no_words, uint16_t ans_items[])
{
    tcpFinsCommand();

    cmdFins[MC] = 0x01;
    cmdFins[SC] = 0x01;

    cmdFins[F_PARAM] = area;
    cmdFins[F_PARAM + 1] = (uint8_t)((address >> 8) & 0xFF);
    cmdFins[F_PARAM + 2] = (uint8_t)(address & 0xFF);
    // cmdFins[F_PARAM + 3] = (uint8_t)(bit_position);
    cmdFins[F_PARAM + 4] = (uint8_t)((no_words >> 8) & 0xFF);
    cmdFins[F_PARAM + 5] = (uint8_t)(no_words & 0xFF);
    finsCommandLen = 18;

    FinsErrors stat_ = FrameSend();

    if (stat_ != Completed_normally)
        return stat_;

    // convierte los datos de respuesta a enteros de 16 bits
    for (int i = 0; i < no_words; i++)
    {
        ans_items[i] = 0x00FF & respFinsData[2 * i];
        ans_items[i] <<= 8;
        ans_items[i] |= respFinsData[(2 * i) + 1];
    }

    return stat_;
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, int int_value)
{

    if (int_value >= -32768 && int_value <= 0xFFFF) // 32768
    {

        uint16_t en_hex;
        memcpy(&en_hex, &int_value, 2);
        return MemoryAreaWrite(area, start_address, en_hex);
    }
    Serial.println("Integer Number out of Range (data >= -32768 || data <= 0xFFFF)");
    return OUT_OF_RANGE;
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, float float_value)
{
    uint32_t en_hex;
    memcpy(&en_hex, &float_value, 4);
    uint16_t en_hex_[2] = {uint16_t(0xffff & (en_hex)), uint16_t(0xffff & (en_hex >> 16))};
    return MemoryAreaWrite(area, start_address, 2, en_hex_);
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, double double_value)
{
    uint64_t en_hex;
    memcpy(&en_hex, &double_value, 8);
    uint16_t en_hex_[4] = {uint16_t((0xffff) & (en_hex)), uint16_t(0xffff & (en_hex >> 16)), uint16_t(0xffff & (en_hex >> 32)), uint16_t(0xffff & (en_hex >> 48))};
    return MemoryAreaWrite(area, start_address, 4, en_hex_);
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, uint16_t data) // write just 1 16_bits word
{
    uint16_t data_arr[1] = {data};
    return MemoryAreaWrite(area, start_address, 1, data_arr);
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, uint16_t number_of_words, uint16_t data[])
{
    uint8_t _data[number_of_words * 2];

    for (int i = 0; i < number_of_words; i++)
    {
        _data[i * 2] = (0xFF & (data[i] >> 8));
        _data[(i * 2) + 1] = (data[i] & 0xFF);
    }
    return MemoryAreaWrite(area, start_address, number_of_words, _data);
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, uint16_t number_of_words, uint8_t data[])
{
    // unsigned long timmm= millis();
    tcpFinsCommand();
    // command & subcomand
    cmdFins[MC] = 0x01;
    cmdFins[SC] = 0x02;
    // memory area
    cmdFins[F_PARAM] = area;
    // address
    cmdFins[F_PARAM + 1] = (uint8_t)((start_address >> 8) & 0xFF);
    cmdFins[F_PARAM + 2] = (uint8_t)(start_address & 0xFF);
    // bit position
    // cmdFins[F_PARAM + 3] = bit_position;
    // count items
    cmdFins[F_PARAM + 4] = (uint8_t)((number_of_words >> 8) & 0xFF);
    cmdFins[F_PARAM + 5] = (uint8_t)(number_of_words & 0xFF);
    // set command lenght (12 + additional params)
    finsCommandLen = 18;
    // send the message
    // vector<uint8_t> w_data(data, data + count * 2);
    // Close();
    return FrameSend(data, (number_of_words * 2));
}

FinsErrors FINS_TCP::MemoryAreaWrite(MemoryArea area, uint16_t start_address, String text_message)
{
    uint8_t mess_len = text_message.length();
    if (mess_len % 2)
        mess_len++;

    uint16_t en_hex[mess_len / 2];

    for (size_t i = 0; i < (mess_len / 2); i++)
    {
        en_hex[i] = (0xFF & (text_message[i * 2])) << 8;
        en_hex[i] = en_hex[i] | (0xFF & text_message[(i * 2) + 1]);
    }


    return MemoryAreaWrite(area, start_address, mess_len / 2, en_hex);
}

uint8_t FINS_TCP::readBit(MemoryArea area, uint16_t address, uint8_t bit_position)
{
    uint16_t word_ = readWord(area, address);
    return (uint8_t)(((word_) >> (bit_position)) & 0x01);
}

uint16_t FINS_TCP::readWord(MemoryArea area, uint16_t address)
{
    uint16_t word_[1];
    MemoryAreaRead(area, address, 1, word_);
    return word_[0];
}

FinsErrors FINS_TCP::writeBit(MemoryArea area, uint16_t address, uint8_t bit_position, uint8_t value)
{
    uint16_t word__[1];
    word__[0] = readWord(area, address);
    bitWrite(word__[0], bit_position, value);
    return MemoryAreaWrite(area, address, 1, word__);
}

FinsErrors FINS_TCP::toggleBit(MemoryArea area, uint16_t address, uint8_t bit_position)
{
    uint16_t word__[1];
    word__[0] = readWord(area, address);
    if (bitRead(word__[0], bit_position))
        bitWrite(word__[0], bit_position, 0);
    else
        bitWrite(word__[0], bit_position, 1);

    return MemoryAreaWrite(area, address, 1, word__);
}

FinsErrors FINS_TCP::FrameSend(const uint8_t data[], int words)
{
    // unsigned long tim_ = millis();

    if (!Connect())
    {
        Serial.println("No se pudo conectar");
        Close();
        return No_response;
    }

    if (!cliente.connected())
    {
        Serial.println("No se encuentra conectado el cliente");
        Close();
        return No_response;
    }

    fill_n(respFS, 16, 0);

    int fsLen = finsCommandLen + 8;
    fsLen += words;

    cmdFS[6] = (uint8_t)((fsLen >> 8) & 0xFF);
    cmdFS[7] = (uint8_t)(fsLen & 0xFF);

    // send frame header
    int cmdFS_ = Send(cmdFS, 16);
    if (cmdFS_ != 16)
    {
        Serial.println("No se pudo escribir cmdFS, se escribieron " + String(cmdFS_) + " de 16 bytes");

        Close();
        return No_response;
    }

    int cmdFins_ = Send(cmdFins, finsCommandLen);
    if (cmdFins_ != finsCommandLen)
    {
        Serial.println("No se pudo escribir cmdFins, se escribieron " + String(cmdFins_) + " de " + String(finsCommandLen) + " bytes");
        Close();
        return No_response;
    }

    if (words > 0)
    {
        int datas = Send(data, words);
        // Serial.println("Hay datos extras;(0x" + String(words, HEX) + ")");
        if (datas != words)
        {
            Serial.println("No se pudieron escribir los datos extras, se escribieron " + String(datas) + " de " + String(words) + " bytes");
            Close();
            return No_response;
        }
    }

    int respFS_ = Receive(respFS, 16);
    if (respFS_ != 16)
    {
        Serial.println("No se pudo leer respFS, se recibieron " + String(respFS_) + " de 16 bytes");
        Close();
        return No_response;
    }

    // frame response

    finsResponseLen = (respFS[6] << 8) + respFS[7];
    finsResponseLen -= 8;

    // fins command response
    //
    int respFins_ = Receive(&respFins[0], 14);
    if (respFins_ < 14)
    {
        Serial.println(">>>  /*ILLEGAL RESPONSE LENGTH CHECK*/ bytes received=" + String(respFins_) + " of 14");
        Close();
        return No_response;
    }

    if ((cmdFins[3] != respFins[6]) || (cmdFins[4] != respFins[7]) || (cmdFins[5] != respFins[8]))
    { /*DESTINATION ADDRESS CHECK*/
        Serial.println(">>>>>> Illegal source address error");
        return Illegal_source_address_error;
    }

    if (cmdFins[9] != respFins[9])
    { /*SID CHECK*/
        Serial.println(">>>>>> Illegal SID error");
        return Illegal_SID_error;
    }

    // Serial.print("Resp Fins 0x");
    // for(int i = 0; i < 14; i++)
    // Serial.print("|=" + String(i)+ " =0x" + String(respFins[i]) );
    // Serial.println("<");

    // Serial.print("Fins Cmd 0x");
    // for(int i = 0; i < finsCommandLen; i++)
    // Serial.print("|=" + String(i)+ " =0x" + String(cmdFins[i]) );
    // Serial.println("<");

    uint16_t error_frm = 0x00FF & respFins[12]; // revisa la respuesta e busca de errores
    error_frm <<= 8;
    error_frm |= respFins[13];

    // if(respFins)

    if (error_frm != 0x00)
        Serial.println(">>>>>>> Hubo un error en el comando, error = 0x" + String(error_frm, HEX));

    uint16_t cmd_type = 0x00FF & respFins[10]; // tipo de comando
    cmd_type <<= 8;
    cmd_type |= respFins[11];

    // Serial.print("Error = 0x" + String(error_frm, HEX));
    // Serial.println("    CMD = 0x" + String(cmd_type, HEX));

    if (finsResponseLen > 14)
    {
        // fins command response data
        //
        // Serial.println("**********Data Request(hay mas datos)*************");
        int response_ = Receive(&respFinsData[0], finsResponseLen - 14);
        if (response_ != (finsResponseLen - 14))
        {
            Serial.println("Error al leer los datos de respuesta al comando, se recibieron " + String(response_) + " de " + String(finsResponseLen - 14) + " bytes");
            return No_response;
        }
    }

    Close();

    // Serial.println("tim =" + String(millis() - tim_) + " ms");
    return FinsErrors(error_frm);
}

bool FINS_TCP::NodeAddressDataSend()
{
    if (!cliente.connected())
    {
        Serial.println("No hay un cliente conectado NADS");
        Close();
        return false;
    }
    /* NODE ADDRESS DATA SEND buffer */
    uint8_t cmdNADS[] =
        {
            0x46, 0x49, 0x4E, 0x53, // 'F' 'I' 'N' 'S'
            0x00, 0x00, 0x00, 0x0C, // 12 Bytes expected
            0x00, 0x00, 0x00, 0x00, // NADS Command (0 Client to server, 1 server to client)
            0x00, 0x00, 0x00, 0x00, // Error code (Not used)
            0x00, 0x00, 0x00, 0x00  // Client node address, 0 = auto assigned
        };

    int cmdNADS_ = Send(cmdNADS, sizeof(cmdNADS));
    if (cmdNADS_ != sizeof(cmdNADS))
    {
        Serial.println("No se pudo escribir correctamente, se escribieron " + String(cmdNADS_) + " bytes");
        Close();
        return false;
    }

    uint8_t respNADS[24] = {0};
    int respNADS_ = Receive(respNADS, sizeof(respNADS));

    if (respNADS_ != sizeof(respNADS))
    {
        Serial.println("No Se recibieron los datos esperados, recv=" + String(respNADS_) + " bytes");
        Close();
        return false;
    }

    if (respNADS[15] != 0x00)
    {
        lastError = "NASD command error: ";
        lastError += respNADS[15];
        Serial.println(lastError);
        // no more actions
        //
        Close();
        return false;
    }

    if (respNADS[8] != 0 || respNADS[9] != 0 || respNADS[10] != 0 || respNADS[11] != 1)
    {
        lastError = "Error sending NADS command. ";
        lastError += respNADS[8];
        lastError += " ";
        lastError += respNADS[9];
        lastError += " ";
        lastError += respNADS[10];
        lastError += " ";
        lastError += respNADS[11];
        // String errorr = lastError;
        Serial.println(lastError);

        // no more actions
        //
        Close();

        return false;
    }
    cmdFins[DA1] = respNADS[23];
    cmdFins[SA1] = respNADS[19];
    DA1__ = respNADS[23];
    SA1__ = respNADS[19];

    // Serial.println("NodeAddDSend: DA1=0x" + String(cmdFins[DA1], HEX) + " SA1=0x" + String(cmdFins[SA1], HEX));
    //  Serial.println("Parece que todo bien con el NodeAddressDataSend");
    return true;
}

int FINS_TCP::Send(const uint8_t command[], int cmdLen)
{
    int writt = cliente.write(command, cmdLen);
    if (writt != cmdLen)
    {
        Serial.println("Hubo un problema al escribir =" + String(writt));
        return 0;
    }
    //  Serial.println("Escribi " + String(writt) + " de " + String(cmdLen) + " bytes 0x");
    return writt;
}

int FINS_TCP::Receive(uint8_t response[], int respLen)
{
    int maxloops = 0;

    // wait for the server's reply to become available
    // unsigned long timm = millis();
    if (!cliente.connected())
    {
        Serial.println("No hay un cliente conectado");
        Close();
        return 0;
    }

    while (!cliente.available() && maxloops < 1000)
    {
        maxloops++;
        delay(1); // delay 1 msec
    }

    int lei = cliente.read(response, respLen);
    if (!lei)
    {
        Serial.println("No habia datos en el buffer de entrada");
        return 0;
    }
    if (lei != respLen)
    {
        Serial.println("Error en la lectura >> Lei solamente " + String(lei) + " bytes de " + String(respLen));
    }
    // String fins_;
    // for (int i = 0; i < 4; i++)
    // for (int i = 0; i < 4; i++)
    //     fins_ += char(response[i]);

    // if (lei && (fins_ == "FINS"))
    // {
    //     timm = millis() - timm;
    //     int i = 0;
    //     Serial.print(" >>Lei=" + String(lei) + " de " + String(disp) + " tim= " + String(timm) + "ms=");
    //     // Serial.print(" >>Lei=" + String(lei) + " en tim= " + String(timm) + "ms=");
    //     for (; i < 4; i++)
    //         Serial.print(char(response[i]));
    //     Serial.print("<");
    //     Serial.print("  Length: 0x");
    //     for (; i < 8; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Reseved (0): 0x");
    //     for (; i < 11; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Data Type(): 0x");
    //     for (; i < 12; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Err_Code: 0x");
    //     for (; i < 16; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Data: 0x");
    //     for (; i < lei; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.println("<");
    //     Serial.print("  Raw=0x");
    //     for (int u = 0; u < lei; u++)
    //     {
    //         Serial.print(response[u], HEX);
    //     }
    //     Serial.println("<");
    // }
    // else
    // {
    //     Serial.print(">>Raw Response [" + String(lei) + "] of [" + String(disp) + "] bytes=");
    //     for (int u = 0; u < lei; u++)
    //     {
    //         Serial.print(response[u], HEX);
    //     }
    //     Serial.print("< = >");
    //     for (int u = 0; u < lei; u++)
    //     {
    //         Serial.print(response[u], HEX);
    //         Serial.print("|");
    //     }
    //     Serial.println("<");
    // }
    return lei;
}

FinsErrors FINS_TCP::forcedBit(MemoryArea area, uint16_t address, uint8_t bit_position, Force_Mode set_reset)
{
    // unsigned long timmm= millis();
    uint8_t area_ = (uint8_t)area - 128;

    tcpFinsCommand();
    // command & subcomand
    cmdFins[MC] = 0x23;
    cmdFins[SC] = 0x01;
    // no of bits/flags
    cmdFins[F_PARAM] = 0x00;
    cmdFins[F_PARAM + 1] = 0x01;
    // Set/reset specification
    cmdFins[F_PARAM + 2] = ((uint8_t)set_reset >> 8) & 0xFF;
    cmdFins[F_PARAM + 3] = (uint8_t)set_reset & 0xFF;
    // if (set_reset)
    //     cmdFins[F_PARAM + 3] = 0x01;
    // else
    //     cmdFins[F_PARAM + 3] = 0x00;
    // memory area code
    cmdFins[F_PARAM + 4] = area_;

    // Bit/flag
    cmdFins[F_PARAM + 5] = 0x00;
    cmdFins[F_PARAM + 6] = 0x00;
    cmdFins[F_PARAM + 7] = bit_position;
    // set command lenght (12 + additional params)
    finsCommandLen = 20;

    // send the message
    // vector<uint8_t> w_data(data, data + count * 2);
    // Close();
    return FrameSend();
}

FinsErrors FINS_TCP::cancelAllForced()
{
    tcpFinsCommand();
    // command & subcomand
    cmdFins[MC] = 0x23;
    cmdFins[SC] = 0x02;

    finsCommandLen = 12;
    return FrameSend();
}

FinsErrors FINS_TCP::changePLCMode(PLC_MODE modo)
{
    tcpFinsCommand(); // command & subcomand prepare
    cmdFins[MC] = 0x04;
    cmdFins[SC] = (0xFF & (modo >> 24));
    cmdFins[F_PARAM] = (0xFF & (modo >> 16));
    cmdFins[F_PARAM + 1] = (0xFF & (modo >> 8));
    finsCommandLen = 14;
    if (modo == RUN || modo == MONITOR)
    {
        cmdFins[F_PARAM + 2] = (0xFF & modo);
        finsCommandLen = 15;
    }
    // STOP = 0x02FFFF00,
    // PROGRAM = 0x02FFFF00,
    // RUN = 0x01FFFF04,
    // MONITOR = 0x01FFFF02

    return FrameSend();
}

FinsErrors FINS_TCP::toggleBitForced(MemoryArea area, uint16_t address, uint8_t bit_position)
{
    uint16_t word__;
    word__ = readWord(area, address);

    if (bitRead(word__, bit_position))
        return forcedBit(area, address, bit_position, Forced_Reset);
    else
        return forcedBit(area, address, bit_position, Forced_Set);

    return No_response;
}

PLC_MODE FINS_TCP::PLCStatusRead()
{
    tcpFinsCommand();
    // command & subcomand
    cmdFins[MC] = 0x06;
    cmdFins[SC] = 0x01;

    finsCommandLen = 12;

    FinsErrors stat_ = FrameSend();

    if (stat_ == Completed_normally)
    {
        Serial.println("Status (response)");

        Serial.print("PLC Program Status = ");
        Serial.println(bitRead(respFinsData[0], 0) ? "1: Run (user program is being executed)" : "0: Stop (user program is not being executed)");

        Serial.print("Battery status = ");
        Serial.println(bitRead(respFinsData[0], 2) ? "1: Battery present" : "0: No battery present");

        Serial.print("CPU status = ");
        Serial.println(bitRead(respFinsData[0], 7) ? "1: CPU on standby" : "0: Normal");

        Serial.print("CPU Mode = ");
        switch (respFinsData[1])
        {
        case 0x00:
            Serial.println(" PROGRAM ");
            break;
        case 0x02:
            Serial.println(" MONITOR ");
            break;
        case 0x04:
            Serial.println(" RUN ");
            break;
        }
        bool errors_ = false;
        for (size_t i = 2; i < 10; i++)
            if (respFinsData[i] != 0)
            {
                errors_ = true;
                break;
            }

        Serial.println(errors_ ? "One or More Errors are present in the PLC" : "Everithing is OK in the PLC");
    }
    else
        return CPU_FAIL;

    // Mode (response)
    // The CPU Unit operating mode is as follows:
    // 00: PROGRAM
    // 02: MONITOR
    // 04: RUN
    // Fatal error data

    switch (respFinsData[1])
    {
    case 0x00:
        return PROGRAM;
        break;
    case 0x02:
        return MONITOR;
        break;
    case 0x04:
        return RUN;
        break;
    }
    return CPU_FAIL;
}

// Serial.print("Resp Fins 0x");
// for(int i = 0; i < 14; i++)
// Serial.print("|=" + String(i)+ " =0x" + String(respFins[i]) );
// Serial.println("<");
