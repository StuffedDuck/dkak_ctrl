/*
  dkak_bluetooth.h - bluetooth control library
  Created by Andy Koezle, June 20, 2017.
  private

  TODO:

 	-	
*/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "dkak_bluetooth.h"
#include "dkak_stepper.h"

dkak_bluetooth::dkak_bluetooth() : btSerial(NULL) 
{
    btSerial_flag = 0;
}

//initialize dkak_bluetooth by supplying arduino pins
void dkak_bluetooth::init(byte ip_pin_rx, byte ip_pin_tx, char ip_delimiter)
{
    //directly initialized settings
    pin_rx = ip_pin_rx;
    pin_tx = ip_pin_tx;
    delimiter = ip_delimiter;
    inChar = "";
    btData = "";

    //arduino specific initialization
    if (NULL != btSerial) delete btSerial;
    btSerial = new SoftwareSerial(pin_rx, pin_tx);
    btSerial->begin(9600);
}

//set bluetooth to listen mode
String dkak_bluetooth::listen(bool report = true)
{
    inChar = "";
    btData = "";

    while (btSerial->available() > 0) // and inChar != delimiter)
    {
        delay(10);
        inChar = btSerial->read();
        btData.concat(inChar);
        btData.trim();
    }

    //report received command - ignore empty commands
    if (btData != "")
    {
        char buffer[128];   // Use an array large enough 
        snprintf(buffer, sizeof(buffer), "BT COMMAND ISSUED: %s", btData.c_str());
        Serial.println(buffer);
        Serial.println("");
    }
    return btData;
}