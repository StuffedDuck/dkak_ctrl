/*
  dkak_texscan.cpp - stepper control library
  Created by Andy Koezle, June 24, 2017.
  private

  TODO:

 	-	implement exec_cmds for stepper resetting (currently being reset automatically)
*/

#include "Arduino.h"
#include "dkak_stepper.h"
#include "dkak_texscan.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTORS >> MEMBER INITILIZATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//initialize dkak_texscan by supplying steppers (translation and led) and led relais information
void dkak_texscan::init(dkak_stepper ip_stepper_transl, dkak_stepper ip_stepper_led, int ip_path_distance)
{
    stepper_transl = ip_stepper_transl;
    stepper_led = ip_stepper_led;
    path_distance = ip_path_distance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//USAGE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//check for errors from all components
bool dkak_texscan::errorfree()
{
    return (stepper_transl.errorfree && stepper_led.errorfree);
}

//execute bluetooth command
bool dkak_texscan::exec_cmd(String bt_command)
{
    //bool canExec = False//CONTINUE HERE: adjust with pointers
    bool ignoreError =  true;//if (canIgnoreError) canIgnoreError else errorfree();
    if (ignoreError)
    {
        if (bt_command == "sx_scan")
        {
            double rot_val = 90.0;

            //rotate +deg
            if (errorfree())
            {  
                stepper_led.rotate(rot_val);
                serial_feedback(1);
                delay(1000);
            }

            //move +x
            if (errorfree())
            {  
                stepper_transl.move(-path_distance);
                serial_feedback(1);
                delay(1000);
            }

            //rotate -2*deg
            if (errorfree())
            {  
                stepper_led.rotate(2 * -rot_val);
                serial_feedback(1);
                delay(1000);
            }

            //move x home
            if (errorfree())
            {  
                stepper_transl.movehome(true);          //TODO: shouldn't require "true", since value has default
                serial_feedback(1);
                delay(1000);
            }

            //rotate to neutral
            if (errorfree())
            {  
                stepper_led.rotate(rot_val);
                serial_feedback(1);
                delay(1000);
            }
        }
        else if (bt_command == "sx_reset")
        {
            stepper_transl.movehome();
        }
    }
    else serial_feedback(2);

    //report success/failure
    if (errorfree()) return true;
    else return false;
}

//reports x- and y-units position and rotation to serial
void dkak_texscan::serial_feedback(int msg_mode)
{
    char buffer[128];
    switch (msg_mode)
    {
        case 1:
        {
            String pos_str = String(stepper_transl.pos_current);
            String rot_str = String(stepper_led.rot_current);
            snprintf(buffer, sizeof(buffer), "%s(%s, %s) - transform: %s - %s", "STEPPER_X_UNIT", stepper_transl.state.c_str(), stepper_led.state.c_str(), pos_str.c_str(), rot_str.c_str());
            break;
        }
        case 2:
        {
            snprintf(buffer, sizeof(buffer), "%s(%s, %s) - can't execute command, in errorstate", "STEPPER_X_UNIT", stepper_transl.state.c_str(), stepper_led.state.c_str());
        }

    }
    Serial.println(buffer);
    Serial.println("");   
}