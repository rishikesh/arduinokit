///-----------------------------------------------------------------
/// Copyright © 2011 Rishi B
/// @file      toolkit.h
/// @author    Rishi B 
/// Created:   May 24 2011 11:23:31 AM
/// Updated:   August 23 2011 11:15:23 PM
/// @section   DESCRIPTION
///            Header containing function prototypes for toolkit.cpp
///			   
///				
///------------------------------------------------------------------
/*
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License <http://www.gnu.org/licenses/> for more 
 *  details.
 */

#ifndef __toolkit_h__
#define __toolkit_h__

#include <wx/wx.h>
#include <stdint.h>
#include "serial.h"


#define MODE_INPUT    0x00
#define MODE_OUTPUT   0x01
#define MODE_ANALOG   0x02
#define MODE_PWM      0x03
#define MODE_SERVO    0x04
#define MODE_SHIFT    0x05
#define MODE_I2C      0x06

#define START_SYSEX             0xF0 // start a MIDI Sysex message
#define END_SYSEX               0xF7 // end a MIDI Sysex message
#define PIN_MODE_QUERY          0x72 // ask for current and supported pin modes
#define PIN_MODE_RESPONSE       0x73 // reply with current and supported pin modes
#define PIN_STATE_QUERY         0x6D
#define PIN_STATE_RESPONSE      0x6E
#define CAPABILITY_QUERY        0x6B
#define CAPABILITY_RESPONSE     0x6C
#define ANALOG_MAPPING_QUERY    0x69
#define ANALOG_MAPPING_RESPONSE 0x6A
#define REPORT_FIRMWARE         0x79 // report name and version of the firmware

#ifndef __MAIN__
extern unsigned int tx_count;
extern unsigned int rx_count; 

extern Serial port;

typedef struct { //Store data for each pin.
	uint8_t mode;
	uint8_t analog_channel;
	uint64_t supported_modes;
	uint32_t value;
	float range_factor; //Has units of [Reading/CM] for IR sensors  
	uint16_t range_max;
	uint16_t range_min; 
} pin_t;
extern pin_t pin_info[128];
#else
unsigned int tx_count;
unsigned int rx_count; 

Serial port;

typedef struct { //Store data for each pin.
	uint8_t mode;
	uint8_t analog_channel;
	uint64_t supported_modes;
	uint32_t value;
	uint16_t range_factor; //Has units of [Reading/CM] for IR sensors  
	uint16_t range_max;
	uint16_t range_min; 
} pin_t;
pin_t pin_info[128];
#endif



//some function prototypes for firmata based functions
void read_analog(uint8_t parse_buf[], int parse_count); //read a pin
void init_pin(); //initialize a pin
void check_port(wxString name); //check a port
void set_pwm(int val, int pin); //write pwm settings to a port
void toggle_pin (int pin);
void parse_buffer(uint8_t parse_buf[], int parse_count); //figure out which pin does what

#endif
