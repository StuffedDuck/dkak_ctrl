/*
  dkak_bluetooth.h - bluetooth control library
  Created by Andy Koezle, June 20, 2017.
  private
*/
  
#ifndef dkak_bluetooth_h
#define dkak_bluetooth_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "dkak_stepper.h"

class dkak_bluetooth
{
  private:
    int btSerial_flag;
    byte pin_rx, pin_tx;
    char delimiter;
    char inChar;
    String btData;
    SoftwareSerial* btSerial;
  public:
    dkak_bluetooth();
    void init(byte, byte, char);  //initialize bluetooth component
    String listen(bool = true);              //listens for bluetooth commands, returns command
};

#endif

