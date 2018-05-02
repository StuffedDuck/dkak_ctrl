/*
  dkak_stepper.cpp - stepper control library
  Created by Andy Koezle, November 10, 2016.
  private

  TODO:

	-	int range?
	-	include microsteps 
	-	adjust arduino code to byte values instead of int? for pins
	-	check for delay/pulse function and meaning
	- 	check for char memory - use in current state or reserve memory in class attribute?
	- 	write function for reports
*/

#include "Arduino.h"
#include "dkak_stepper.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTORS >> MEMBER INITILIZATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
dkak_stepper::dkak_stepper(byte ip_pin_pul, byte ip_pin_dir, byte ip_pin_ena) : pin_pul(ip_pin_pul), pin_dir(ip_pin_dir), pin_ena(ip_pin_ena)
{
	//directly initialized settings
	pin_pul = ip_pin_pul;
	pin_dir = ip_pin_dir;
	pin_ena = ip_pin_ena;

	//arduino specific initialization
	pinMode(pin_pul, OUTPUT);
	pinMode(pin_dir, OUTPUT);
	pinMode(pin_ena, OUTPUT);

	//activate enable pin
	digitalWrite(pin_ena, HIGH);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SETUP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//enable stepper
void dkak_stepper::enable() //CONTINUE HERE - doesnt work - removes jitter on startup
{
	digitalWrite(pin_ena, LOW);
}

//set dkak_stepper name
void dkak_stepper::set_pulsedelay(int delay)
{
	delay_usec = delay;
}

//set dkak_stepper name
void dkak_stepper::set_mode(int ip_mode, int ip_stepsPerRev, int ip_lead_mm)
{
	//directly initialized settings
	mode = ip_mode;
	stepsPerRev = ip_stepsPerRev;
	lead_mm = ip_lead_mm;

	//values derived from input
	if (stepsPerRev != 0) mm_perstep = lead_mm / stepsPerRev;
	deg_perstep = 360.0 / ip_stepsPerRev;
}

//set dkak_stepper name
void dkak_stepper::set_name(String ip_name)
{
	name = ip_name;
}

//initialize optional limit switch pins
void dkak_stepper::set_pins_limit(byte ip_pin_limit, byte ip_pin_limitHome)
{
	pin_limit = ip_pin_limit;
	pin_limitHome = ip_pin_limitHome;

	pinMode(pin_limitHome, OUTPUT);
	pinMode(pin_limit, OUTPUT);

	digitalWrite(pin_limitHome, HIGH);
	digitalWrite(pin_limit, HIGH);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//USAGE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//movement cycle
void dkak_stepper::cycle()
{
	digitalWrite(pin_pul, HIGH);
	delayMicroseconds(delay_usec);
	digitalWrite(pin_pul, LOW);
	delayMicroseconds(delay_usec);
}

//check for stepper being in home position
bool dkak_stepper::isHome()
{
	if (digitalRead(pin_limitHome) == HIGH) return true;
	else return false;
}

//move dkak_stepper by value in mm - input values determine direction (+/-), steps are calulated based on attributes, returns step validation string + serial feedback
bool dkak_stepper::move(int distance_mm)
{
  	set_dir(distance_mm);
  	int steps_total = ceil(abs(distance_mm) / mm_perstep);
  	int steps = step(steps_total);
  	pos_current += distance_mm;
  	bool validation = validate_steps(steps, steps_total);
  	return validation;
}

//move dkak_stepper to home position
bool dkak_stepper::movehome(bool report = true)
{
	//set direction
	digitalWrite(pin_dir, HIGH);

	//keep moving until limit switch is triggered
	int steps_taken = 0;
	while (!triggeredLimit(1, false))
	{
		++steps_taken;
		if (!triggeredLimit(2)) cycle();
		else 
		{
			return false;
			break;
		}
	}

	//upper loop didn't break
	pos_current = 0;
	errorfree = true;
	state = "OK";
	serial_feedback(4, steps_taken);
	return errorfree;
}

//report to serial
void dkak_stepper::serial_feedback(int msg_mode, int steps_taken = 0, int steps_req = 0)
{
	char buffer[128];   // Use an array large enough
	int limit_state = digitalRead(pin_limit);
	int limitHome_state = digitalRead(pin_limitHome);

	//prepare report message
	switch (msg_mode)
	{
		//home switch triggered
		case 1:
		{
			snprintf(buffer, sizeof(buffer), "%s(%s) - triggered home switch", name.c_str(), state.c_str());
			break;
		}
		//normal cycle unexpectedly interrupted by limits (2 || 3)
		case 2:
		case 3:
		{
			snprintf(buffer, sizeof(buffer), "%s(%s) - normal cycle hit limits: LMT - %i, LMTH - %i", name.c_str(), state.c_str(), limit_state, limitHome_state);
			break;
		}
		//cycle feedback - supplying only steps_taken implies home position movement //CONTINUE HERE - SHOULDNT REQ BREAK
		case 4:
		{
			if (steps_req == 0)
			{
				snprintf(buffer, sizeof(buffer), "%s(%s) - cycled # steps back home: %i", name.c_str(), state.c_str(), steps_taken);
			}
			else snprintf(buffer, sizeof(buffer), "%s(%s) - cycled steps: %i / %i", name.c_str(), state.c_str(), steps_taken, steps_req);
			break;
		}
	}

	Serial.println(buffer);
}

//rotate dkak_stepper by degrees - same as move but uses degrees as basis for step calculation - kept seperately for readibility when issuing stepper commands
bool dkak_stepper::rotate(double deg)
{
	set_dir(deg);
	int steps_total = ceil(abs(deg) / deg_perstep);
	int steps = step(steps_total);
	rot_current += deg;
	bool validation = validate_steps(steps, steps_total);
  	return validation;
}

//set direction - cw or ccw
void dkak_stepper::set_dir(auto inval)
{
	if (inval > 0) digitalWrite(pin_dir, HIGH);  
  	else digitalWrite(pin_dir, LOW);
}

//run steps
int dkak_stepper::step(int steps_total)
{
	//run steps
	int steps;
	for (steps = 1; steps < steps_total; steps++)
	{
		if (mode != 2)
		{
			if (!triggeredLimit()) cycle();
			else 
			{
				errorfree = false;
				break;
			}
		}
		else cycle();
	}

	return steps;
}

//check if limit switches were triggered - mode1: check for home limit state mode2: check for turning point limit state default: check both
bool dkak_stepper::triggeredLimit(int move_mode, bool report = true) //TODO: add report mode and adjust to common formatting
{
	switch (move_mode)
	{
		//home limit switch triggered
		case 1: 
		{
			if (digitalRead(pin_limitHome) == HIGH) 
			{
				if (report) serial_feedback(move_mode);
				return true;
			}
			else return false; 
		}
		//turning point limit switch triggered
		case 2:
		{
			if (digitalRead(pin_limit) == HIGH) 
			{
				state = "ERROR_HOMECYCLE_HIT_LIMIT2";
				errorfree = false;
				if (report) serial_feedback(move_mode);
				return true;
			}
			else return false; 
		}
		//default mode used when neither limit switch should be triggered - trigger error state
		default:
		{
			if (digitalRead(pin_limitHome) == HIGH || digitalRead(pin_limit) == HIGH)
			{
				state = "ERROR_CYCLE_HIT_LIMITS";
				errorfree = false;
				if (report) serial_feedback(move_mode);
				return true;
			}
			else return false;
		}
	} 
}

//validate step count 
bool dkak_stepper::validate_steps(int steps_taken, int steps_req, bool report = true)
{
 	//return true on success, false if loop was interrupted
 	if (steps_taken == steps_req) errorfree = true;
	else errorfree = false;

	//report if required
	serial_feedback(4, steps_taken, steps_req);

	return errorfree;
}