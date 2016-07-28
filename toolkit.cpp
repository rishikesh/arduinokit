///-----------------------------------------------------------------
/// Copyright © 2011 Rishi B
/// @file      toolkit.cpp
/// @author    Rishi B 
/// Created:   May 24 2011 11:23:31 AM
/// Updated:   August 23 2011 11:15:23 PM
/// @section   DESCRIPTION
///            Firmata based code for arduinokit
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


#include "wx/wxprec.h"
#define __MAIN__
#include "toolkit.h"
#include "arduinokit.h"
//#include "serial.h"



void read_analog(uint8_t parse_buf[], int parse_count) { //read from analog pins

	uint8_t cmd = (parse_buf[0] & 0xF0);
	
	//printf("message, %d bytes, %02X\n", parse_count, parse_buf[0]);
	parse_buffer(parse_buf, parse_count);
	if (cmd == 0xE0 && parse_count == 3) {
		int analog_ch = (parse_buf[0] & 0x0F);
		int analog_val = parse_buf[1] | (parse_buf[2] << 7);
		for (int pin=0; pin<128; pin++) {
			if (pin_info[pin].analog_channel == analog_ch) {
				pin_info[pin].value = analog_val;
				//printf("Analog Channel: %d\n Analog Value: %d\n pin: %d\n", analog_ch, analog_val, pin);
				parse_buffer(parse_buf, parse_count);
				return; 
		
			}
		
		}
	}
}

void init_pin() { //initialize pin data
	
	for (int i=0; i < 128; i++) {
		pin_info[i].mode = 255;
		pin_info[i].analog_channel = 127;
		pin_info[i].supported_modes = 0;
		pin_info[i].value = 0;
		pin_info[i].range_factor = 0;
		pin_info[i].range_max = 1;
		pin_info[i].range_min = 1;
	}
	
	return;
}

void check_port(wxString name) { //check a port
	
	port.Close();
	//if (id == 9000) return;
	
	port.Open(name);
	port.Set_baud(57600);
	if (port.Is_open()) {
		//printf("port is open\n");
		//firmata_name = _("");
		rx_count = tx_count = 0;
		//UpdateStatus();
		uint8_t buf[3];
		buf[0] = START_SYSEX;
		buf[1] = REPORT_FIRMWARE; // read firmata name & version
		buf[2] = END_SYSEX;
		port.Write(buf, 3);
		tx_count += 3;
		wxWakeUpIdle(); //replace this with the idle function of your framework
	} else {
		printf("error opening port\n");
	}
	
	return;
}

	
void set_pwm(int val, int pin) {
    //set the PWM values according to the slider values
	if (pin <= 15 && val <= 16383) {
		uint8_t buf[3];
		buf[0] = 0xE0 | pin;
		buf[1] = val & 0x7F;
		buf[2] = (val >> 7) & 0x7F;
		port.Write(buf, 3);
		tx_count += 3;
	} else {
		uint8_t buf[12];
		int len=4;
		buf[0] = 0xF0;
		buf[1] = 0x6F;
		buf[2] = pin;
		buf[3] = val & 0x7F;
		if (val > 0x00000080) buf[len++] = (val >> 7) & 0x7F;
		if (val > 0x00004000) buf[len++] = (val >> 14) & 0x7F;
		if (val > 0x00200000) buf[len++] = (val >> 21) & 0x7F;
		if (val > 0x10000000) buf[len++] = (val >> 28) & 0x7F;
		buf[len++] = 0xF7;
		port.Write(buf, len); //send to board
		tx_count += len;
	}
	
}

void toggle_pin (int pin) {

	if (pin_info[pin].value)
		pin_info[pin].value = 0; 	
	else 
		pin_info[pin].value = 1;
	int port_num = pin / 8;
	int port_val = 0;
	for (int i=0; i<8; i++) {
		int p = port_num * 8 + i;
		if (pin_info[p].mode == MODE_OUTPUT || pin_info[p].mode == MODE_INPUT) {
			if (pin_info[p].value) {
				port_val |= (1<<i);
			}
		}
	}
	uint8_t buf[3];
	buf[0] = 0x90 | port_num;
	buf[1] = port_val & 0x7F;
	buf[2] = (port_val >> 7) & 0x7F;
	port.Write(buf, 3);
	tx_count += 3;

}
	
void parse_buffer(uint8_t parse_buf[], int parse_count) {
	if (parse_buf[0] == START_SYSEX && parse_buf[parse_count-1] == END_SYSEX) {
		// Sysex message
		if (parse_buf[1] == REPORT_FIRMWARE) {
			char name[140];
			int len=0;
			for (int i=4; i < parse_count-2; i+=2) {
				name[len++] = (parse_buf[i] & 0x7F)
				| ((parse_buf[i+1] & 0x7F) << 7);
			}
			name[len++] = '-';
			name[len++] = parse_buf[2] + '0';
			name[len++] = '.';
			name[len++] = parse_buf[3] + '0';
			name[len++] = 0;
			//firmata_name = _("name");
			uint8_t buf[80];
			len=0;
			buf[len++] = START_SYSEX;
			buf[len++] = ANALOG_MAPPING_QUERY; // read analog to pin # info
			buf[len++] = END_SYSEX;
			buf[len++] = START_SYSEX;
			buf[len++] = CAPABILITY_QUERY; // read capabilities
			buf[len++] = END_SYSEX;
			for (int i=0; i<16; i++) {
				buf[len++] = 0xC0 | i;  // report analog
				buf[len++] = 1;
				buf[len++] = 0xD0 | i;  // report digital
				buf[len++] = 1;
			}
			port.Write(buf, len);
			tx_count += len;
		} else if (parse_buf[1] == CAPABILITY_RESPONSE) {
			int pin, i, n;
			for (pin=0; pin < 128; pin++) {
				pin_info[pin].supported_modes = 0;
			}
			for (i=2, n=0, pin=0; i<parse_count; i++) {
				if (parse_buf[i] == 127) {
					pin++;
					n = 0;
					continue;
				}
				if (n == 0) {
					// first byte is supported mode
					pin_info[pin].supported_modes |= (1<<parse_buf[i]);
				}
				n = n ^ 1;
			}
			// send a state query for for every pin with any modes
			for (pin=0; pin < 128; pin++) {
				uint8_t buf[512];
				int len=0;
				if (pin_info[pin].supported_modes) {
					buf[len++] = START_SYSEX;
					buf[len++] = PIN_STATE_QUERY;
					buf[len++] = pin;
					buf[len++] = END_SYSEX;
				}
				port.Write(buf, len);
				tx_count += len;
			}
		} else if (parse_buf[1] == ANALOG_MAPPING_RESPONSE) {
			int pin=0;
			for (int i=2; i<parse_count-1; i++) {
				pin_info[pin].analog_channel = parse_buf[i];
				pin++;
			}
			return;
		} else if (parse_buf[1] == PIN_STATE_RESPONSE && parse_count >= 6) {
			int pin = parse_buf[2];
			pin_info[pin].mode = parse_buf[3];
			pin_info[pin].value = parse_buf[4];
			if (parse_count > 6) pin_info[pin].value |= (parse_buf[5] << 7);
			if (parse_count > 7) pin_info[pin].value |= (parse_buf[6] << 14);
		}
		return;
	}
}