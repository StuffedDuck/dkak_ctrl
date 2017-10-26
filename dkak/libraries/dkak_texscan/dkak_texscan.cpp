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
void dkak_texscan::init(dkak_stepper ip_stepper_transl_x, dkak_stepper ip_stepper_led_x, dkak_stepper ip_stepper_transl_y, dkak_stepper ip_stepper_led_y, int ip_path_distance)
{
    stepper_transl_x = ip_stepper_transl_x;
    stepper_led_x = ip_stepper_led_x;
    stepper_transl_y = ip_stepper_transl_y;
    stepper_led_y = ip_stepper_led_y;
    path_distance = ip_path_distance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//USAGE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//check for errors from all components
bool dkak_texscan::errorfree()
{
    return (stepper_transl_x.errorfree && stepper_led_x.errorfree && stepper_transl_y.errorfree && stepper_led_y.errorfree);
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

            //rotate x +deg 
            if (errorfree())
            {  
                stepper_led_x.rotate(rot_val);
                serial_feedback(1);
                delay(1000);
            }

            //move +x
            if (errorfree())
            {  
                stepper_transl_x.move(-path_distance);
                serial_feedback(1);
                delay(1000);
            }

            //rotate x -2*deg
            if (errorfree())
            {  
                stepper_led_x.rotate(2 * -rot_val);
                serial_feedback(1);
                delay(1000);
            }

            //move x home
            if (errorfree())
            {  
                stepper_transl_x.movehome(true);          //TODO: shouldn't require "true", since value has default
                serial_feedback(1);
                delay(1000);
            }

            //rotate x to neutral
            if (errorfree())
            {  
                stepper_led_x.rotate(rot_val);
                serial_feedback(1);
                delay(1000);
            }

            //rotate y +deg 
            if (errorfree())
            {  
                stepper_led_y.rotate(rot_val);
                serial_feedback(1);
                delay(1000);
            }

            //move +y
            if (errorfree())
            {  
                stepper_transl_y.move(-path_distance);
                serial_feedback(1);
                delay(1000);
            }
        }
        else if (bt_command == "sx_reset")
        {
            stepper_transl_x.movehome();
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
            String x_pos_str = String(stepper_transl_x.pos_current);
            String x_rot_str = String(stepper_led_x.rot_current);
            String y_pos_str = String(stepper_transl_y.pos_current);
            String y_rot_str = String(stepper_led_y.rot_current);   //CONTINUE HERE - adjust feedback to include both stepper sides
            snprintf(   buffer, sizeof(buffer), "%s(X - %s/%s, Y - %s/%s) - transform: X - %s/%s, Y - %s/%s",
                        "DKAK_TEXSCAN", stepper_transl_x.state.c_str(), stepper_led_x.state.c_str(), stepper_transl_y.state.c_str(), stepper_led_y.state.c_str(),
                        x_pos_str.c_str(), x_rot_str.c_str(), y_pos_str.c_str(), y_rot_str.c_str());
            break;
        }
        case 2:
        {
            snprintf(buffer, sizeof(buffer), "%s(%s, %s) - can't execute command, in errorstate", "STEPPER_X_UNIT", stepper_transl_x.state.c_str(), stepper_led_x.state.c_str());
        }

    }
    Serial.println(buffer);
    Serial.println("");   
}