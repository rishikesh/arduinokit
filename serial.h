///-----------------------------------------------------------------
/// Copyright © 2011 Rishi B
/// @file      serial.h
/// @author    Rishi B 
/// Created:   May 24 2011 11:23:31 AM
/// Updated:   August 23 2011 11:15:23 PM
/// @section   DESCRIPTION
///            Header for the Serial object
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

#ifndef __serial_h__
#define __serial_h__

#include <wx/wx.h>
#include <stdint.h>

/* WARNING: ONLY TESTED ON MAC OS X 10.6.8, I'm not sure at this point
 if it'll work on other OSs, but according to documentation, it should.
 */
#if !defined(LINUX) && !defined(WINDOWS) && !defined(MACOSX)
#define MACOSX
#endif

#if defined(LINUX)
#include <termios.h>
#elif defined(MACOSX)
#include <termios.h>
#elif defined(WINDOWS)
#include <windows.h>
#endif

class Serial
{
public:
	Serial();
	~Serial();
	wxArrayString port_list();
	int Open(const wxString& name);
	wxString error_message();
	int Set_baud(int baud);
	int Set_baud(const wxString& baud_str);
	int Read(void *ptr, int count);
	int Write(const void *ptr, int len);
	int Input_wait(int msec);
	void Input_discard(void);
	int Set_control(int dtr, int rts);
	void Output_flush();
	void Close(void);
	int Is_open(void);
	wxString get_name(void);
private:
	int port_is_open;
	wxString port_name;
	int baud_rate;
	wxString error_msg;
private:
#if defined(LINUX) || defined(MACOSX)
	int port_fd;
	struct termios settings_orig;
	struct termios settings;
#elif defined(WINDOWS)
	HANDLE port_handle;
	COMMCONFIG port_cfg_orig;
	COMMCONFIG port_cfg;
#endif
};

#endif // __serial_h__
