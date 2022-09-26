/*
 * demo: CAN-BUS Shield, receive all frames and print all fields id/type/data
 * to receive frame fastly, a poll in loop() is required.
 *
 * Copyright (C) 2020 Seeed Technology Co.,Ltd.
 */
#include <SPI.h>
#define CAN_2515
// #define CAN_2518FD

// Set SPI CS Pin according to your hardware



float StrToHex(char str[])
{
  return (float) strtol(str, NULL, 16);
}









// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;



#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#define MAX_DATA_SIZE 8
#endif

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR) {}

    #if MAX_DATA_SIZE > 8
    /*
     * To compatible with MCP2515 API,
     * default mode is CAN_CLASSIC_MODE
     * Now set to CANFD mode.
     */
    CAN.setMode(CAN_NORMAL_MODE);
    #endif

    while (CAN_OK != CAN.begin(CAN_250KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println(F("CAN init fail, retry..."));
        delay(100);
    }
    SERIAL_PORT_MONITOR.println(F("CAN init ok!"));
}

uint32_t id;
uint8_t  type; // bit0: ext, bit1: rtr
uint8_t  len;
byte cdata[MAX_DATA_SIZE] = {0};


char chunk1 [8];
char chunk2 [8];
char chunk3 [8];
char chunk4 [8];

char byte0 [4];
char byte1 [4];
char byte2 [4];
char byte3 [4];
char byte4 [4];
char byte5 [4];
char byte6 [4];
char byte7 [4];

//temperature values 
float PhaseA;
float PhaseB;
float PhaseC;
float GTDR; //gate driver board tem
float CBT; // control board temp
float MotorTemp;
float TQShudder;
float MotorAngle;
float MotorFreq;
float PhaseAcur;
float PhaseBcur;
float PhaseCcur;
float DCcur;
float DCvoltBus;
float Vout;
float V_AB;
float V_BC;



void loop() {
    // check if data coming
    if (CAN_MSGAVAIL != CAN.checkReceive()) {
        return;
    }

    char prbuf[32 + MAX_DATA_SIZE * 3];
    char dataBytes[24]; // holds data from the can ID
    int i, n;

    unsigned long t = millis();
    // read data, len: data length, buf: data buf
    CAN.readMsgBuf(&len, cdata);

    id = CAN.getCanId();
    type = (CAN.isExtendedFrame() << 0) |
           (CAN.isRemoteRequest() << 1);
    /*
     * MCP2515(or this driver) could not handle properly
     * the data carried by remote frame
     */

    n = sprintf(prbuf, "%04lu.%03d ", t / 1000, int(t % 1000));
    /* Displayed type:
     *
     * 0x00: standard data frame
     * 0x02: extended data frame
     * 0x30: standard remote frame
     * 0x32: extended remote frame
     */
    static const byte type2[] = {0x00, 0x02, 0x30, 0x32};
    n += sprintf(prbuf + n, "RX: [%08lX](%02X) ", (unsigned long)id, type2[type]);
    // n += sprintf(prbuf, "RX: [%08lX](%02X) ", id, type);


    for (i = 0; i < len; i++) {
        n += sprintf(prbuf + n, "%02X ", cdata[i]);
    }

    for (i = 0; i < 8; i++){
        sprintf(dataBytes + i*2, "%02X", cdata[i]);
    }


    sprintf(chunk1, "%02X", cdata[1]);
    sprintf(chunk1 + 2, "%02X", cdata[0]);

    sprintf(chunk2, "%02X", cdata[3]);
    sprintf(chunk2 + 2, "%02X", cdata[2]);

    sprintf(chunk3, "%02X", cdata[5]);
    sprintf(chunk3 + 2, "%02X", cdata[4]);

    sprintf(chunk4, "%02X", cdata[7]);
    sprintf(chunk4 + 2, "%02X", cdata[6]);


    sprintf(byte0, "%02X", cdata[0]);
    sprintf(byte1, "%02X", cdata[1]);

    sprintf(byte2, "%02X", cdata[2]);
    sprintf(byte3, "%02X", cdata[3]);

    sprintf(byte4, "%02X", cdata[4]);
    sprintf(byte5, "%02X", cdata[5]);

    sprintf(byte6, "%02X", cdata[6]);
    sprintf(byte7, "%02X", cdata[7]);


    if (id != 2000) {
        //SERIAL_PORT_MONITOR.println(dataBytes);
    }



    switch (id) {
        case 160: //    0x0A0 - Temperature #1 [hall A = byte 1 byte 2]

            ///////// PHASE A
            if(StrToHex(byte1) != 0) { //for some reason 0x00 casted to int is 489
                //temp is over 25.5 C
                PhaseA = StrToHex(chunk1)/10.0;
            } else {
                PhaseA = StrToHex(byte0)/10.0;
            }
            Serial.print("Phase A Temp: ");
            Serial.println(PhaseA);

            if(StrToHex(byte3) != 0) { 
                //temp is over 25.5 C
                PhaseB = StrToHex(chunk2)/10.0;
            } else {
                PhaseB = StrToHex(byte2)/10.0;
            }
            Serial.print("Phase B Temp: ");
            Serial.println(PhaseB);


            if(StrToHex(byte5) != 0) { 
                //temp is over 25.5 C
                PhaseC = StrToHex(chunk3)/10.0;
            } else {
                PhaseC = StrToHex(byte4)/10.0;
            }          
            Serial.print("Phase C Temp: ");
            Serial.println(PhaseC);

            if(StrToHex(byte7) != 0) { 
                //temp is over 25.5 C
                GTDR = StrToHex(chunk4)/10.0;
            } else {
                GTDR = StrToHex(byte6)/10.0;
            }
            Serial.print("Control Board Temp: ");
            Serial.println(GTDR);

            //SERIAL_PORT_MONITOR.println(prbuf);
            return;

        case 161: // 0x0A1 TEMPERATURES #2
        
            if(StrToHex(byte1) != 0) { 
                //temp is over 25.5 C
                CBT = StrToHex(chunk1)/10.0;

            } else {
                CBT = StrToHex(byte0)/10.0;
            }
            Serial.print("Control Board Temp: ");
            Serial.println(CBT);

            return; 

        case 162: // 0x0A2 Motor Temp and TQ shudder 
            //SERIAL_PORT_MONITOR.println(prbuf);

            if(StrToHex(byte5) != 0) {  //motor temp
                //
                MotorTemp = StrToHex(chunk3)/10.0;

            } else {
                MotorTemp = StrToHex(byte4)/10.0;
            }
            Serial.print("Motor Temp: ");
            Serial.println(MotorTemp);

            if(StrToHex(byte7) != 0) { //torque shudder 
                //
                TQShudder = StrToHex(chunk4)/10.0;

            } else {
                TQShudder = StrToHex(byte6)/10.0;
            }
            Serial.print("Torque Shudder: ");
            Serial.println(TQShudder);
            
            return; 
        
        case 165: // 0x0A5

            if(StrToHex(byte1) != 0) { 
                //
                MotorAngle = StrToHex(chunk1)/10.0;

            } else {
                MotorAngle = StrToHex(byte0)/10.0;
            }
            Serial.print("Motor Angle: ");
            Serial.println(MotorAngle);

            if(StrToHex(byte5) != 0) { 
                //
                MotorFreq = StrToHex(chunk3)/10.0;

            } else {
                MotorFreq = StrToHex(byte4)/10.0;
            }
            Serial.print("Motor Frequency: ");
            Serial.println(MotorFreq);

            //SERIAL_PORT_MONITOR.println(prbuf);
            return;

        case 166: //current of phase a b c and the dc bus current

            if(StrToHex(byte1) != 0) { 
                //
                PhaseAcur = StrToHex(chunk1)/10.0;

            } else {
                PhaseAcur = StrToHex(byte0)/10.0;
            }
            Serial.print("Phase A current: ");
            Serial.println(PhaseAcur);

            if(StrToHex(byte3) != 0) { 
                //
                PhaseAcur = StrToHex(chunk2)/10.0;

            } else {
                PhaseAcur = StrToHex(byte2)/10.0;
            }
            Serial.print("Phase B current: ");
            Serial.println(PhaseBcur);

            if(StrToHex(byte5) != 0) { 
                //
                PhaseAcur = StrToHex(chunk3)/10.0;

            } else {
                PhaseAcur = StrToHex(byte4)/10.0;
            }
            Serial.print("Phase C current: ");
            Serial.println(PhaseCcur);
            
            if(StrToHex(byte7) != 0) { 
                //
                DCcur = StrToHex(chunk4)/10.0;

            } else {
                DCcur = StrToHex(byte6)/10.0;
            }
            Serial.print("DC bus current: ");
            Serial.println(DCcur);
            
            SERIAL_PORT_MONITOR.println(prbuf);
            return;

        case 167: // 0x0A7 Voltage Information

            if(StrToHex(byte3) != 0) { 
                Vout = StrToHex(chunk2)/10.0;
            } else {
                Vout = StrToHex(byte2)/10.0;
            }
            Serial.print("Output Voltage: ");
            Serial.println(Vout);


            if(StrToHex(byte5) != 0) { 
                //temp is over 25.5 C
                V_AB = StrToHex(chunk3)/10.0;
            } else {
                V_AB = StrToHex(byte4)/10.0;
            }          
            Serial.print("Voltage AB: ");
            Serial.println(V_AB);

            if(StrToHex(byte7) != 0) { 
                //temp is over 25.5 C
                V_BC = StrToHex(chunk4)/10.0;
            } else {
                V_BC = StrToHex(byte6)/10.0;
            }
            Serial.print("Voltage BC: ");
            Serial.println(V_BC);

            SERIAL_PORT_MONITOR.println(prbuf);
            return;

        case 168: // flux information

            return;

        case 170: //Controller Internal States 

            return;

        case 171: //fault codes 

            return;

        case 173: //field weakening 

            return;

        case 176: //high speed messaging 

            return;




    }
}
// END FILE
