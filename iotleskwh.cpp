#include "iotleskwh.h"

iotleskwh::iotleskwh(){

}

iotleskwh::~iotleskwh(){

}

void iotleskwh::begin(){
    pinMode(pinRelA, OUTPUT);
    pinMode(pinRelB, OUTPUT);
    Serial1.begin(9600, SERIAL_8N1, 25, 26);
    if (!EEPROM.begin(EEPROM_SIZE)) {
      delay(1000);
      //ESP.restart();  
  }
  //Serial.println("found eeprom");
  eep_raw_data_kwh = EEPROM.readUInt(addr_eep_kwh);
  //Serial.print("cf: ");
  //Serial.println(eep_raw_data_kwh);
}

void iotleskwh::resetMeter(){
  //Serial.println("reset kwh");
  Serial1.write(req_reset, sizeof(req_reset));
  while(Serial1.available()){
    Serial.write(Serial1.read());
  }
  EEPROM.writeUInt(addr_eep_kwh, 0); 
  EEPROM.commit();
  eep_raw_data_kwh = EEPROM.readUInt(addr_eep_kwh);
}

String iotleskwh::idMeter(){
  String _str_meter = "";
  uint64_t _chipid;
  char chr_chipid[13];
  _chipid = ESP.getEfuseMac();
  uint16_t chip = (uint16_t)(_chipid >> 32);
  snprintf(chr_chipid, 13, "%04X%08X", chip, (uint32_t)_chipid);
  for ( int i = 0; i < 12; i++) {
    _str_meter += String(chr_chipid[i]);
  }
  return _str_meter;
}

void iotleskwh::relayOFF(){
  digitalWrite(pinRelA, LOW);
  digitalWrite(pinRelB, HIGH);
  delay(100);
  digitalWrite(pinRelA, LOW);
  digitalWrite(pinRelB, LOW);
}

void iotleskwh::relayON(){
  digitalWrite(pinRelA, HIGH);
  digitalWrite(pinRelB, LOW);
  delay(100);
  digitalWrite(pinRelA, LOW);
  digitalWrite(pinRelB, LOW);
}

void iotleskwh::bacaMeter(){
   Serial1.write(req_data, sizeof(req_data));
    while (Serial1.available()) {
    buf_serial_meter[c_meter] = Serial1.read();
    c_meter++;
    }

    raw_data = 0;
    raw_data = buf_serial_meter[4];
    raw_data |= buf_serial_meter[5] << 8;
    raw_data |= buf_serial_meter[6] << 16;
    _vol = raw_data * VREF * ((R2 * 5) + R1) / (COEF_VOLT * R1 * 1000);
    
    raw_data = 0;
    raw_data = buf_serial_meter[16];
    raw_data |= buf_serial_meter[17] << 8;
    _freq = 1000000 / float(raw_data);

    raw_data = 0;
    raw_data = buf_serial_meter[1];
    raw_data |= buf_serial_meter[2] << 8;
    raw_data |= buf_serial_meter[3] << 16;
    _cur = (raw_data * VREF) / (COEF_ARUS * RL) ;

    raw_data = 0;
    raw_data = buf_serial_meter[10];
    raw_data |= buf_serial_meter[11] << 8;
    raw_data |= buf_serial_meter[12] << 16;

    unsigned int _oneC = ~raw_data;
    raw_data = _oneC + 1;
    unsigned int _mask = 0b00000000111111111111111111111111;
    raw_data &= _mask;
    _pwr = (raw_data * VREF * VREF * ( (R2 * 5) + R1)) / (3537 * RL * R1 * 1000);

    raw_data = 0;
    raw_data = buf_serial_meter[13];
    raw_data |= buf_serial_meter[14] << 8;
    raw_data |= buf_serial_meter[15] << 16;

    if(raw_data < eep_raw_data_kwh){
        raw_data_kwh = raw_data + eep_raw_data_kwh;
    }else{
      raw_data_kwh = raw_data;
    }
    
    float a = (1638.4 * 256 * VREF * VREF * ((R2 * 5) + R1)) / 10000;
    float b = 360 * 3537 * RL * R1 * 1000;
    _en = (a / b) * raw_data_kwh;
    _pf = _pwr / (_cur * _vol);

    EEPROM.writeUInt(addr_eep_kwh, raw_data_kwh); 
    EEPROM.commit();

    c_meter = 0;

  }


  float iotleskwh::bacaVolt(){
     float _buf_meter;
    _buf_meter = _vol;
    return _buf_meter;
  }

  float iotleskwh::bacaCur(){
    float _buf_meter;
    _buf_meter = _cur;
    return _buf_meter;
  }

  float iotleskwh::bacaFreq(){
    float _buf_meter;
    _buf_meter = _freq;
    return _buf_meter;
  }

  float iotleskwh::bacaWatt(){
    float _buf_meter;
    _buf_meter = _pwr;
    return _buf_meter;
  }

  float iotleskwh::bacaKwh(){
    float _buf_meter;
    _buf_meter = _en;
    return _buf_meter;
  }

  float iotleskwh::bacaPF(){
    float _buf_meter;
    _buf_meter = _pf;
    return _buf_meter;
  }









