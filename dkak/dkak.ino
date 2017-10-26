//import custom libraries
#include <dkak_stepper.h>
#include <dkak_bluetooth.h>
#include <dkak_texscan.h>
#include <EEPROM.h>

//global variables
dkak_texscan dkak_ctrl; //class coordinating all stepper modules
dkak_bluetooth bt_hc05;   //bluetooth
String bt_command;

void setup()
{ 
    //SERIAL SETUP - includes delay to avoid serial monitor being filled with NULs
    Serial.begin(9600);
    delay(2000);

    //BLUETOOTH SETUP - uno and mega only support softwareserial on specific pins (2+3 used for uno)
    const byte pin_led = 13;
    const byte pin_rx = 50; //2 - uno
    const byte pin_tx = 51; //3 - uno

    bt_hc05.init(pin_rx, pin_tx, ",");

    //STEPPER SETUP
    //constant values - stepper
    const int delay_usec = 500;
    const int lead_mm = 8;
    const int stepsPerRev = 400;

    //constant values - arduino
    const byte x_pin_pul = 5;
    const byte x_pin_dir = 6;
    const byte x_pin_ena = 7;
    const byte x_pin_limitHome = 8;
    const byte x_pin_limit = 9;

    const byte xled_pin_pul = 10; 
    const byte xled_pin_dir = 11;
    const byte xled_pin_ena = 12;

    const byte y_pin_pul = 34;
    const byte y_pin_dir = 35;
    const byte y_pin_ena = 36;
    const byte y_pin_limitHome = 37;
    const byte y_pin_limit = 38;

    const byte yled_pin_pul = 2; 
    const byte yled_pin_dir = 3;
    const byte yled_pin_ena = 4;

    //init stepper - x translation
    dkak_stepper stepper_x = dkak_stepper(x_pin_pul, x_pin_dir, x_pin_ena); 
    stepper_x.set_name("STEPPER_X_TRANSL");
    stepper_x.set_pulsedelay(delay_usec);
    stepper_x.set_pins_limit(x_pin_limit, x_pin_limitHome);
    stepper_x.set_mode(1, stepsPerRev, lead_mm);    //1 == translation mode

    //init stepper - x led
    dkak_stepper stepper_xled = dkak_stepper(xled_pin_pul, xled_pin_dir, xled_pin_ena);
    stepper_xled.set_name("STEPPER_X_LED");
    stepper_xled.set_pulsedelay(delay_usec);
    stepper_xled.set_mode(2, stepsPerRev);          //2 == rotation mode

    //init stepper - y translation
    dkak_stepper stepper_y = dkak_stepper(y_pin_pul, y_pin_dir, y_pin_ena); 
    stepper_y.set_name("STEPPER_Y_TRANSL");
    stepper_y.set_pulsedelay(delay_usec);
    stepper_y.set_pins_limit(y_pin_limit, y_pin_limitHome);
    stepper_y.set_mode(1, stepsPerRev, lead_mm);    //1 == translation mode

    //init stepper - y led
    dkak_stepper stepper_yled = dkak_stepper(yled_pin_pul, yled_pin_dir, yled_pin_ena);
    stepper_yled.set_name("STEPPER_Y_LED");
    stepper_yled.set_pulsedelay(delay_usec);
    stepper_yled.set_mode(2, stepsPerRev);          //2 == rotation mode

    //DKAK_TEXSCAN SETUP
    //constant values - current prototype
    const int path_distance = 80;

    //init dkak_texscan
    dkak_ctrl.init(stepper_x, stepper_xled, stepper_y, stepper_yled, path_distance);

    //enable steppers
    stepper_x.enable();
    stepper_xled.enable();
    stepper_y.enable();
    stepper_yled.enable();

    //DKAK_TEXSCAN - check for home positions and reset if neccessary/possible - TODO: expand to second stepper unit and add master/slave arduino communication
    String delimiter = "################################################################";
    Serial.println(delimiter);
    String dkak_msg;
    if (!stepper_x.isHome())
    {
        //update dkak state if home position was reached
        if (dkak_ctrl.stepper_transl_x.movehome()) dkak_msg += "DONE - WAITING FOR COMMANDS"; 
        else dkak_msg += "FAILED - X COULDNT RESET";
    }

    //report dkak state 
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "DKAK TEXSCAN - initialization test: %s", dkak_msg.c_str());
    Serial.println(buffer);
    Serial.println(delimiter);
}

void loop()
{
    //listen for bt commands, ignore empty commands
    bt_command = bt_hc05.listen();
    if (bt_command != "")
    {
        //interpret command
        bool exec_command = dkak_ctrl.exec_cmd(bt_command);
    }

    delay(500);
}
