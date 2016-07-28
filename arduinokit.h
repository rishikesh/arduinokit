///-----------------------------------------------------------------
/// Copyright © 2011 Rishi B
/// @file      arduinokit.h
/// @author    Rishi B
/// Created:   May 24 2011 11:23:31 AM
/// Updated:   August 23 2011 11:15:23 PM
/// @section   DESCRIPTION
///            Header for the arduinokit app
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


#ifndef __arduinokit_H__
#define __arduinokit_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "arduinokit.h"
#endif


#ifndef WX_PRECOMP
#include <wx/wx.h>
#else
#include <wx/wxprec.h>
#endif

#include <stdint.h>
#include <wx/slider.h>
#include <wx/tglbtn.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/statbox.h>
#include <wx/filepicker.h>

#define LOG_MSG_TO_STDOUT
//#define LOG_MSG_TO_WINDOW

#if defined(LOG_MSG_TO_WINDOW)
#define printf(...) (wxLogMessage(__VA_ARGS__))
#elif defined(LOG_MSG_TO_STDOUT)
#else
#define printf(...)
#endif

// comment this out to enable lots of printing to stdout

////Dialog Style Start
#undef arduinokit_STYLE
#define arduinokit_STYLE wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End
const int ID_MENU = 1000;


class arduinokit : public wxFrame
{
private:
	//wxFlexGridSizer *grid;
	//wxScrolledWindow *scroll;
	int parse_count;
	int parse_command_len;
	uint8_t parse_buf[4096];
public:
	void init_data(void);
	void UpdateStatus(void);
	void Parse(const uint8_t *buf, int len);
	void DoMessage(void);
	void OnAbout(wxCommandEvent &event);
	void OnQuit(wxCommandEvent &event);
	void OnIdle(wxIdleEvent &event);
	void OnCloseWindow(wxCloseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnPort(wxCommandEvent &event);
	//void OnToggleButton(wxCommandEvent &event);
	//void OnSliderDrag(wxScrollEvent &event);
	//void OnModeChange(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();

public:
	arduinokit(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("arduinokit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = arduinokit_STYLE);
	virtual ~arduinokit();
	void CalibrateA0Click(wxCommandEvent& event);
	void CalibrateA1Click(wxCommandEvent& event);
	void CalibrateA2Click(wxCommandEvent& event);
	void arduinokitActivate(wxActivateEvent& event);
	void SaveA0ConfigClick(wxCommandEvent& event);
	void LoadA0ConfigClick(wxCommandEvent& event);
	void SaveA1ConfigClick(wxCommandEvent& event);
	void LoadA1ConfigClick(wxCommandEvent& event);
	void SaveA2ConfigClick(wxCommandEvent& event);
	void LoadA2ConfigClick(wxCommandEvent& event);
	void TogglePin2Click(wxCommandEvent& event);
	void TogglePin3Click(wxCommandEvent& event);
	void TogglePin4Click(wxCommandEvent& event);
	void TogglePin5Click(wxCommandEvent& event);
	void Pin3PWMScroll(wxScrollEvent& event);
	void Pin5PWMScroll(wxScrollEvent& event);
	void ToggleAction1Click(wxCommandEvent& event);
	void ToggleAction2Click(wxCommandEvent& event);
	void ToggleAction3Click(wxCommandEvent& event);
	void ToggleAction4Click(wxCommandEvent& event);

private:
	//Lot of random declarations here... need to be weeded later...
	wxToggleButton *TogglePin4;
	wxStaticText *WxStaticText19;
	wxChoice *DevicePin4;
	wxToggleButton *ToggleAction4;
	wxFilePickerCtrl *Action4File;
	wxStaticText *WxStaticText41;
	wxChoice *OutputAction4;
	wxStaticText *WxStaticText40;
	wxStaticText *WxStaticText39;
	wxStaticText *WxStaticText38;
	wxTextCtrl *Action4End;
	wxStaticText *WxStaticText37;
	wxTextCtrl *Action4Start;
	wxStaticText *WxStaticText36;
	wxStaticBox *WxStaticBox13;
	wxChoice *InputAction4;
	wxStaticText *WxStaticText35;
	wxStaticBox *WxStaticBox12;
	wxToggleButton *ToggleAction3;
	wxFilePickerCtrl *Action3File;
	wxStaticText *WxStaticText34;
	wxChoice *OutputAction3;
	wxStaticText *WxStaticText33;
	wxStaticText *WxStaticText32;
	wxStaticText *WxStaticText31;
	wxTextCtrl *Action3End;
	wxStaticText *WxStaticText30;
	wxTextCtrl *Action3Start;
	wxStaticText *WxStaticText29;
	wxStaticBox *WxStaticBox11;
	wxChoice *InputAction3;
	wxStaticText *WxStaticText28;
	wxStaticBox *WxStaticBox10;
	wxToggleButton *ToggleAction2;
	wxFilePickerCtrl *Action2File;
	wxStaticText *WxStaticText27;
	wxChoice *OutputAction2;
	wxStaticText *WxStaticText26;
	wxStaticText *WxStaticText25;
	wxStaticText *WxStaticText24;
	wxTextCtrl *Action2End;
	wxStaticText *WxStaticText23;
	wxTextCtrl *Action2Start;
	wxStaticText *WxStaticText22;
	wxStaticBox *WxStaticBox9;
	wxChoice *InputAction2;
	wxStaticText *WxStaticText21;
	wxStaticBox *WxStaticBox8;
	wxToggleButton *TogglePin5;
	wxSlider *Pin5PWM;
	wxStaticText *WxStaticText18;
	wxCheckBox *EnablePWM5;
	wxChoice *DevicePin5;
	wxStaticText *WxStaticText17;
	wxStaticText *A2SensorReading;
	wxPanel *Action4;
	wxPanel *Action3;
	wxPanel *WxAuiNoteBookPage2;
	wxPanel *Action2;
	wxFilePickerCtrl *Action1File;
	wxToggleButton *ToggleAction1;
	wxChoice *OutputAction1;
	wxChoice *InputAction1;
	wxStaticText *Event;
	wxStaticText *Servo;
	wxStaticText *WxStaticText4;
	wxStaticText *OutputLabel;
	wxTextCtrl *Action1End;
	wxStaticText *andLabel;
	wxTextCtrl *Action1Start;
	wxStaticText *DistanceLabel;
	wxStaticBox *Processing1;
	wxStaticText *InputLabel;
	wxStaticBox *WxStaticBox5;
	wxPanel *Action1;
	wxNotebook *WxNotebook2;
	wxPanel *Pin5;
	wxPanel *Pin4;
	wxCheckBox *EnablePWM3;
	wxStaticText *PWM3;
	wxSlider *Pin3PWM;
	wxToggleButton *TogglePin3;
	wxChoice *DevicePin3;
	wxStaticText *WxStaticText5;
	wxPanel *Pin3;
	wxChoice *DevicePin2;
	wxStaticText *Type;
	wxToggleButton *TogglePin2;
	wxPanel *Pin2;
	wxNotebook *Outputs;
	wxStaticText *WxStaticText15;
	wxStaticText *WxStaticText14;
	wxButton *CalibrateA2;
	wxTextCtrl *A2ReadingMin;
	wxStaticText *WxStaticText12;
	wxTextCtrl *A2ReadingMax;
	wxStaticText *WxStaticText11;
	wxTextCtrl *A2RangeEnd;
	wxTextCtrl *A2RangeStart;
	wxButton *SaveA2Config;
	wxButton *LoadA2Config;
	wxStaticBox *WxStaticBox7;
	wxCheckBox *A2SensorOn;
	wxStaticText *SensorType2;
	wxChoice *A2SensorType;
	wxPanel *SensorsPage3;
	wxStaticText *WxStaticText13;
	wxTextCtrl *A1ReadingMax;
	wxTextCtrl *A1RangeEnd;
	wxTextCtrl *A1RangeStart;
	wxStaticText *WxStaticText9;
	wxStaticText *WxStaticText8;
	wxStaticText *WxStaticText7;
	wxTextCtrl *A1ReadingMin;
	wxButton *CalibrateA1;
	wxStaticBox *WxStaticBox6;
	wxButton *LoadA1Config;
	wxButton *SaveA1Config;
	wxStaticText *A1SensorReading;
	wxCheckBox *A1SensorOn;
	wxStaticText *SensorType;
	wxChoice *A1SensorType;
	wxPanel *SensorsPage2;
	wxStaticText *A0SensorReading;
	wxChoice *WxChoice3;
	wxButton *SaveA0Config;
	wxTextCtrl *A0ReadingMin;
	wxTextCtrl *A0ReadingMax;
	wxStaticText *WxStaticText3;
	wxStaticText *WxStaticText2;
	wxTextCtrl *A0RangeEnd;
	wxStaticText *to;
	wxTextCtrl *A0RangeStart;
	wxStaticText *Range;
	wxButton *LoadA0Config;
	wxButton *CalibrateA0;
	wxStaticBox *WxStaticBox2;
	wxCheckBox *A0SensorOn;
	wxChoice *A0SensorType;
	wxStaticText *SensorType0;
	wxPanel *SensorsPage1;
	wxNotebook *sensor_pages;
	wxStaticBox *Actions;
	wxStaticBox *WxStaticBox3;
	wxStaticBox *SensorContainer;


private:
	enum
	{
		////GUI Enum Control ID Start
		////GUI Enum Control ID Start
		ID_TOGGLEPIN4 = 1697,
		ID_WXSTATICTEXT19 = 1696,
		ID_DEVICEPIN4 = 1695,
		ID_TOGGLEACTION4 = 1691,
		ID_ACTION4FILE = 1690,
		ID_WXSTATICTEXT41 = 1689,
		ID_OUTPUTACTION4 = 1688,
		ID_WXSTATICTEXT40 = 1687,
		ID_WXSTATICTEXT39 = 1686,
		ID_WXSTATICTEXT38 = 1685,
		ID_ACTION4END = 1684,
		ID_WXSTATICTEXT37 = 1683,
		ID_ACTION4START = 1682,
		ID_WXSTATICTEXT36 = 1681,
		ID_WXSTATICBOX13 = 1680,
		ID_INPUTACTION4 = 1679,
		ID_WXSTATICTEXT35 = 1678,
		ID_WXSTATICBOX12 = 1677,
		ID_TOGGLEACTION3 = 1571,
		ID_ACTION3FILE = 1570,
		ID_WXSTATICTEXT34 = 1569,
		ID_OUTPUTACTION3 = 1568,
		ID_WXSTATICTEXT33 = 1567,
		ID_WXSTATICTEXT32 = 1566,
		ID_WXSTATICTEXT31 = 1565,
		ID_ACTION3END = 1564,
		ID_WXSTATICTEXT30 = 1563,
		ID_ACTION3START = 1562,
		ID_WXSTATICTEXT29 = 1561,
		ID_WXSTATICBOX11 = 1560,
		ID_INPUTACTION3 = 1559,
		ID_WXSTATICTEXT28 = 1558,
		ID_WXSTATICBOX10 = 1557,
		ID_ACTION2FILE = 1450,
		ID_WXSTATICTEXT27 = 1449,
		ID_OUTPUTACTION2 = 1448,
		ID_WXSTATICTEXT26 = 1447,
		ID_WXSTATICTEXT25 = 1446,
		ID_WXSTATICTEXT24 = 1445,
		ID_ACTION2END = 1444,
		ID_WXSTATICTEXT23 = 1443,
		ID_ACTION2START = 1442,
		ID_WXSTATICTEXT22 = 1441,
		ID_WXSTATICBOX9 = 1440,
		ID_INPUTACTION2 = 1439,
		ID_WXSTATICTEXT21 = 1438,
		ID_WXSTATICBOX8 = 1437,
		ID_TOGGLEPIN5 = 1310,
		ID_PIN5PWM = 1309,
		ID_WXSTATICTEXT18 = 1308,
		ID_ENABLEPWM5 = 1307,
		ID_DEVICEPIN5 = 1306,
		ID_WXSTATICTEXT17 = 1305,
		ID_A2SENSORREADING = 1289,
		ID_WXNOTEBOOKPAGE4 = 1042,
		ID_WXNOTEBOOKPAGE3 = 1041,
		ID_WXAUINOTEBOOKPAGE1 = 1040,
		ID_WXNOTEBOOKPAGE2 = 1039,
		ID_ACTION1FILE = 1083,
		ID_TOGGLEACTION2 = 1075,
		ID_TOGGLEACTION1 = 1074,
		ID_OUTPUTACTION1 = 1072,
		ID_INPUTACTION1 = 1071,
		ID_EVENT = 1061,
		ID_SERVO = 1059,
		ID_WXSTATICTEXT4 = 1053,
		ID_OUTPUTLABEL = 1051,
		ID_ACTION1DISTEND = 1050,
		ID_ANDLABEL = 1049,
		ID_ACTION1START = 1048,
		ID_DISTANCELABEL = 1047,
		ID_PROCESSING1 = 1052,
		ID_INPUTLABEL = 1044,
		ID_WXSTATICBOX5 = 1054,
		ID_WXNOTEBOOKPAGE1 = 1038,
		ID_WXNOTEBOOK2 = 1043,
		ID_PIN5 = 1042,
		ID_PIN4 = 1041,
		ID_ENABLEPWM3 = 1082,
		ID_PWM3 = 1081,
		ID_PIN3PWM = 1080,
		ID_TOGGLEPIN3 = 1079,
		ID_DEVICEPIN3 = 1078,
		ID_WXSTATICTEXT5 = 1077,
		ID_PIN3 = 1039,
		ID_DEVICEPIN2 = 1070,
		ID_TYPE = 1064,
		ID_TOGGLEPIN2 = 1063,
		ID_PIN2 = 1038,
		ID_OUTPUTS = 1037,
		ID_WXSTATICTEXT15 = 1287,
		ID_WXSTATICTEXT14 = 1286,
		ID_CALIBRATEA2 = 1281,
		ID_A2READINGMIN = 1280,
		ID_WXSTATICTEXT12 = 1279,
		ID_A2READINGMAX = 1278,
		ID_WXSTATICTEXT11 = 1277,
		ID_A2RANGEEND = 1276,
		ID_A2RANGESTART = 1275,
		ID_SAVEA2CONFIG = 1274,
		ID_LOADA2CONFIG = 1273,
		ID_WXSTATICBOX7 = 1288,
		ID_A2SENSORON = 1272,
		ID_SENSORTYPE2 = 1271,
		ID_A2SENSORTYPE = 1270,
		ID_SENSORSPAGE3 = 1006,
		ID_WXSTATICTEXT13 = 1282,
		ID_A1READINGMAX = 1202,
		ID_A1RANGEEND = 1201,
		ID_A1RANGESTART = 1200,
		ID_WXSTATICTEXT9 = 1199,
		ID_WXSTATICTEXT8 = 1198,
		ID_WXSTATICTEXT7 = 1197,
		ID_A1RANGEMIN = 1196,
		ID_CALIBRATEA1 = 1195,
		ID_WXSTATICBOX6 = 1203,
		ID_LOADA1CONFIG = 1194,
		ID_SAVEA1CONFIG = 1193,
		ID_A1SENSORREADING = 1192,
		ID_A1SENSORON = 1191,
		ID_SENSORTYPE = 1190,
		ID_A1SENSORTYPE = 1189,
		ID_SENSORSPAGE2 = 1004,
		ID_A0SENSORREADING = 1068,
		ID_WXCHOICE3 = 1066,
		ID_SAVEA0CONFIG = 1036,
		ID_A0READINGMIN = 1032,
		ID_A0READINGMAX = 1031,
		ID_WXSTATICTEXT3 = 1030,
		ID_WXSTATICTEXT2 = 1029,
		ID_A0RANGEEND = 1028,
		ID_O = 1027,
		ID_A0RANGESTART = 1026,
		ID_RANGE = 1025,
		ID_LOADA0CONFIG = 1016,
		ID_CALIBRATEA0 = 1013,
		ID_WXSTATICBOX2 = 1024,
		ID_A0SENSORENABLED = 1011,
		ID_A0SENSORTYPE = 1008,
		ID_SENSORTYPE0 = 1007,
		ID_SENSORSPAGE1 = 1002,
		ID_SENSOR_PAGES = 1001,
		ID_ACTIONS = 1034,
		ID_WXSTATICBOX3 = 1033,
		ID_SENSORCONTAINER = 1023,
		////GUI Enum Control ID End
		ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
	};

private:
	void CreateGUIControls();
	void OnClose(wxCloseEvent& event);
	//for motor actions
	void motor_action(int action_number);
};


class MyMenu: public wxMenu
{
public:
	MyMenu(const wxString& title = wxEmptyString, long style = 0);
	void OnShowPortList(wxMenuEvent &event);
	void OnHighlight(wxMenuEvent &event);
};


class arduinokitApp : public wxApp
{
public:
	bool OnInit();
	int OnExit();
};



#endif
