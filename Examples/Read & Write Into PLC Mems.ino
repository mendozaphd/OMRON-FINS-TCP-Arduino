#include <Arduino.h>
#include <WiFi.h>
#include <LibFINS.h>

// WiFi network name and password:
const char *networkName = "TP-Link_3500";
const char *networkPswd = "32289459";

// IP address to send UDP data to:
//  either use the ip address of the server or
//  a network broadcast address
// also you can declare it directly in the begin() function,

const char *PLCAddress = "192.168.0.10";
const int udpPort = 9600;

//declare a FINS_TCP object to control the Omron PLC
FINS_TCP plc1;


void setup(){
  Serial.begin(115200);

  WiFi.disconnect();
  // We start by connecting to a WiFi network (TCP)
  WiFi.begin(networkName, networkPswd);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi..");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  while (!Serial)
    ;


  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Conecting to the PLC.");
  while (!plc1.init("192.168.0.10")) //fins port is 9600 by default
  {
    Serial.print(".");
  }
  Serial.println("Omron PLC Founded!");

//just read the time in the PLC and print it in the console
plc1.ClockRead();

//change the operation mode of the PLC if you want to write some memories or to forced bits
plc1.changePLCMode(MONITOR);

}

void loop(){


/* >>>>>WRITING 8 bits arrays into plc1<<<<< */

/*Declare some date sample into one array of unsigned 8 bits to send to the PLC*/
uint8_t hex_data_8bit_sample[] = {0x12, 0x34, 0x56, 0x78};
 
 /*Write consecutively 2 words (32 bits(2 bytes are required per word)) 
 starting in the 100 Adrres of an HR mem*/
plc1.MemoryAreaWrite(HR, 100, 2,hex_data_8bit_sample); 

//print the data sended
Serial.println("Word WRITED in HR100 Mem = 0x" + String(hex_data_8bit_sample[0], HEX) + String(hex_data_8bit_sample[1], HEX));
Serial.println("Word WRITED in HR101 Mem = 0x" + String(hex_data_8bit_sample[2], HEX) + String(hex_data_8bit_sample[3], HEX));

/*Declare an empty 16 bits array to get complete words from the plc1*/
uint16_t hex_data_16bit_read[2];

/*Read 2 words (32 bits)  from the same HR 100 memory to verify*/
plc1.MemoryAreaRead(HR,100,2,hex_data_16bit_read);

//print the data readed (it should be the same that we write)
Serial.println("Word READED in HR100 Mem = 0x" + String(hex_data_16bit_read[0], HEX));
Serial.println("Word READED in HR101 Mem = 0x" + String(hex_data_16bit_read[1], HEX));

delay(5000);


}