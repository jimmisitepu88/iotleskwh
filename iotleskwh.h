#include "Arduino.h"
#include "EEPROM.h"
#define EEPROM_SIZE 1000

#ifndef iotleskwh_h
#define iotleskwh_h

#define R1 0.51
#define R2 390
#define VREF 1.218
#define COEF_VOLT 73989
#define COEF_ARUS 305978
//#define RL 0.1961// 0.1963 ( error 0.09 )
//#define RL 0.1875
#define RL 0.2

class iotleskwh
{
private:
    /* data */
    byte req_data[3] = { 0x58, 0xAA }; // command request data
    byte req_reset[3] = {0x1C, 0x5A5A5A}; // command request reset
    byte buf_serial_meter[100];
    int c_meter = 0; 
    unsigned int raw_data = 0;
    unsigned int raw_data_kwh = 0;
    float _vol, _freq, _cur, _pwr, _en, _pf;
    byte addr_eep_kwh = 1; 
    byte addr_eep_token = 10;
    unsigned int eep_raw_data_kwh = 0;
    byte pinRelA = 32;
    byte pinRelB = 13;

public:
    iotleskwh(/* args */);
    virtual ~iotleskwh();
    void begin();
    void resetMeter();
    void bacaMeter();
    float bacaVolt();
    float bacaFreq();
    float bacaCur();
    float bacaWatt();
    float bacaKwh();
    float bacaPF();
    void relayON();
    void relayOFF();
    String idMeter();
};

#endif