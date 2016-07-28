///-----------------------------------------------------------------
/// Copyright © 2011 Rishi B
/// @file      arduinokit.cpp
/// @author    Rishi B
/// Created:   May 24 2011 11:23:31 AM
/// Updated:   August 23 2011 11:15:23 PM
/// @section   DESCRIPTION
///            arduinokit wxWidgets class implementation
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


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "arduinokit.h"
#endif

#include "wx/config.h"
#include "wx/confbase.h"
#include "wx/fileconf.h"
#include "wx/stdpaths.h"
#include "wx/wxprec.h"
#include "arduinokit.h"
#include "serial.h"
#include "toolkit.h"
#include "string.h"

//------------------------------------------------------------------------------
// Some definitions
//------------------------------------------------------------------------------
wxString firmata_name;
wxMenu *port_menu;
//keep track of actions
int action[5];

////Event Table Start
BEGIN_EVENT_TABLE(arduinokit,wxFrame)
	EVT_MENU(wxID_ABOUT, arduinokit::OnAbout)
	EVT_MENU(wxID_EXIT, arduinokit::OnQuit)
	EVT_MENU_RANGE(9000, 9999, arduinokit::OnPort) //try to read port when set
	EVT_IDLE(arduinokit::OnIdle) //read sensors all the time
	EVT_CLOSE(arduinokit::OnClose)
	EVT_MENU_OPEN(MyMenu::OnShowPortList)
	EVT_MENU_HIGHLIGHT(-1, MyMenu::OnHighlight)
	EVT_CLOSE(arduinokit::OnCloseWindow)
	EVT_BUTTON(ID_CALIBRATEA0,arduinokit::CalibrateA0Click)
    EVT_BUTTON(ID_SAVEA0CONFIG,arduinokit::SaveA0ConfigClick)
	EVT_BUTTON(ID_LOADA0CONFIG,arduinokit::LoadA0ConfigClick)
	EVT_COMMAND_SCROLL(ID_PIN3PWM,arduinokit::Pin3PWMScroll)
	EVT_TOGGLEBUTTON(ID_TOGGLEPIN3,arduinokit::TogglePin3Click)
	EVT_TOGGLEBUTTON(ID_TOGGLEPIN2,arduinokit::TogglePin2Click)
	EVT_BUTTON(ID_CALIBRATEA2,arduinokit::CalibrateA2Click)
	EVT_BUTTON(ID_SAVEA2CONFIG,arduinokit::SaveA2ConfigClick)
	EVT_BUTTON(ID_LOADA2CONFIG,arduinokit::LoadA2ConfigClick)
	EVT_BUTTON(ID_CALIBRATEA1,arduinokit::CalibrateA1Click)
	EVT_BUTTON(ID_LOADA1CONFIG,arduinokit::LoadA1ConfigClick)
	EVT_BUTTON(ID_SAVEA1CONFIG,arduinokit::SaveA1ConfigClick)
	EVT_TOGGLEBUTTON(ID_TOGGLEPIN4,arduinokit::TogglePin4Click)
	EVT_TOGGLEBUTTON(ID_TOGGLEACTION1,arduinokit::ToggleAction1Click)
	EVT_TOGGLEBUTTON(ID_TOGGLEACTION4,arduinokit::ToggleAction4Click)
	EVT_TOGGLEBUTTON(ID_TOGGLEACTION3,arduinokit::ToggleAction3Click)
	EVT_TOGGLEBUTTON(ID_TOGGLEACTION2,arduinokit::ToggleAction2Click)
	EVT_TOGGLEBUTTON(ID_TOGGLEPIN5,arduinokit::TogglePin5Click)
	EVT_COMMAND_SCROLL(ID_PIN5PWM,arduinokit::Pin5PWMScroll)

END_EVENT_TABLE()
////Event Table End


arduinokit::arduinokit(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style){

	#ifdef LOG_MSG_TO_WINDOW //for logging purposes
	wxLog::SetActiveTarget(new wxLogWindow(this, "Debug Messages"));
	#endif
	port.Set_baud(57600);
	wxMenuBar *menubar = new wxMenuBar;
	wxMenu *menu = new wxMenu;
	menu->Append( wxID_EXIT, _("Quit"), _(""));
	menu = new wxMenu;
	menubar->Append(menu, _("Select Port"));
	SetMenuBar(menubar);
	port_menu = menu;
	CreateStatusBar(1);

	CreateGUIControls();
	init_data();

}

arduinokit::~arduinokit(){
	//nothing to do here...
}

void arduinokit::init_data(void){
	init_pin();
	//tx_count = rx_count = 0;
	firmata_name = _("");
	UpdateStatus();
	Refresh();
}

void arduinokit::CreateGUIControls(){

	//May help to use a RAD tool to see the location of the objects or rename objects.
	//I've left them as default objects, because naming all of them seemed unnecessary
	SensorContainer = new wxStaticBox(this, ID_SENSORCONTAINER, wxT("Sensors/Inuput (Analog Only)"), wxPoint(5, 16), wxSize(628, 210));
	SensorContainer->SetToolTip(wxT("This is where you can configure, calibrate and test out your sensors."));

	WxStaticBox3 = new wxStaticBox(this, ID_WXSTATICBOX3, wxT("Output"), wxPoint(5, 232), wxSize(321, 285));
	WxStaticBox3->SetToolTip(wxT("This is where you can configure, calibrate and test out your sensors."));

	Actions = new wxStaticBox(this, ID_ACTIONS, wxT("Actions"), wxPoint(332, 232), wxSize(303, 285));
	Actions->SetToolTip(wxT("This is where you can configure, calibrate and test out your sensors."));

	sensor_pages = new wxNotebook(this, ID_SENSOR_PAGES, wxPoint(12, 34), wxSize(612, 180), wxNB_DEFAULT);

	SensorsPage1 = new wxPanel(sensor_pages, ID_SENSORSPAGE1, wxPoint(4, 24), wxSize(604, 152));
	sensor_pages->AddPage(SensorsPage1, wxT("Pin A0"));

	SensorType0 = new wxStaticText(SensorsPage1, ID_SENSORTYPE0, wxT("Type:"), wxPoint(5, 12), wxDefaultSize, 0, wxT("SensorType0"));

	wxArrayString arrayStringFor_A0SensorType;
	arrayStringFor_A0SensorType.Add(wxT("None"));
	arrayStringFor_A0SensorType.Add(wxT("IR"));
	arrayStringFor_A0SensorType.Add(wxT("Line"));
	arrayStringFor_A0SensorType.Add(wxT("Light"));
	arrayStringFor_A0SensorType.Add(wxT("Sound"));
	arrayStringFor_A0SensorType.Add(wxT("Other"));
	A0SensorType = new wxChoice(SensorsPage1, ID_A0SENSORTYPE, wxPoint(52, 10), wxSize(140, 21), arrayStringFor_A0SensorType, 0, wxDefaultValidator, wxT("A0SensorType"));
	A0SensorType->SetSelection(-1);

	A0SensorOn = new wxCheckBox(SensorsPage1, ID_A0SENSORENABLED, wxT("Enable"), wxPoint(237, 14), wxSize(97, 14), 0, wxDefaultValidator, wxT("A0SensorOn"));

	WxStaticBox2 = new wxStaticBox(SensorsPage1, ID_WXSTATICBOX2, wxT("Calibrate Sensor"), wxPoint(340, 8), wxSize(252, 141));

	CalibrateA0 = new wxButton(SensorsPage1, ID_CALIBRATEA0, wxT("Calibrate Sensor"), wxPoint(405, 115), wxSize(126, 20), 0, wxDefaultValidator, wxT("CalibrateA0"));

	LoadA0Config = new wxButton(SensorsPage1, ID_LOADA0CONFIG, wxT("Load Config"), wxPoint(158, 114), wxSize(123, 21), 0, wxDefaultValidator, wxT("LoadA0Config"));

	Range = new wxStaticText(SensorsPage1, ID_RANGE, wxT("Range (cm)"), wxPoint(349, 32), wxDefaultSize, 0, wxT("Range"));

	A0RangeStart = new wxTextCtrl(SensorsPage1, ID_A0RANGESTART, wxT(""), wxPoint(423, 29), wxSize(55, 20), 0, wxDefaultValidator, wxT("A0RangeStart"));

	to = new wxStaticText(SensorsPage1, ID_O, wxT("to"), wxPoint(487, 32), wxDefaultSize, 0, wxT("to"));

	A0RangeEnd = new wxTextCtrl(SensorsPage1, ID_A0RANGEEND, wxT(""), wxPoint(508, 28), wxSize(66, 21), 0, wxDefaultValidator, wxT("A0RangeEnd"));

	WxStaticText2 = new wxStaticText(SensorsPage1, ID_WXSTATICTEXT2, wxT("Reading at max range"), wxPoint(349, 57), wxDefaultSize, 0, wxT("WxStaticText2"));

	WxStaticText3 = new wxStaticText(SensorsPage1, ID_WXSTATICTEXT3, wxT("Reading at min range"), wxPoint(351, 83), wxDefaultSize, 0, wxT("WxStaticText3"));

	A0ReadingMax = new wxTextCtrl(SensorsPage1, ID_A0READINGMAX, wxT(""), wxPoint(508, 56), wxSize(55, 21), 0, wxDefaultValidator, wxT("A0ReadingMax"));

	A0ReadingMin = new wxTextCtrl(SensorsPage1, ID_A0READINGMIN, wxT(""), wxPoint(508, 81), wxSize(59, 21), 0, wxDefaultValidator, wxT("A0ReadingMin"));

	SaveA0Config = new wxButton(SensorsPage1, ID_SAVEA0CONFIG, wxT("Save Config"), wxPoint(12, 114), wxSize(119, 21), 0, wxDefaultValidator, wxT("SaveA0Config"));

	wxArrayString arrayStringFor_WxChoice3;
	WxChoice3 = new wxChoice(SensorsPage1, ID_WXCHOICE3, wxPoint(591, 157), wxSize(118, 21), arrayStringFor_WxChoice3, 0, wxDefaultValidator, wxT("WxChoice3"));
	WxChoice3->SetSelection(-1);

	A0SensorReading = new wxStaticText(SensorsPage1, ID_A0SENSORREADING, wxT("Reading: Enable Sensor First."), wxPoint(5, 43), wxDefaultSize, 0, wxT("A0SensorReading"));

	SensorsPage2 = new wxPanel(sensor_pages, ID_SENSORSPAGE2, wxPoint(4, 24), wxSize(604, 152));
	sensor_pages->AddPage(SensorsPage2, wxT("Pin A1"));

	wxArrayString arrayStringFor_A1SensorType;
	arrayStringFor_A1SensorType.Add(wxT("None"));
	arrayStringFor_A1SensorType.Add(wxT("IR"));
	arrayStringFor_A1SensorType.Add(wxT("Line"));
	arrayStringFor_A1SensorType.Add(wxT("Light"));
	arrayStringFor_A1SensorType.Add(wxT("Sound"));
	arrayStringFor_A1SensorType.Add(wxT("Other"));
	A1SensorType = new wxChoice(SensorsPage2, ID_A1SENSORTYPE, wxPoint(52, 10), wxSize(140, 21), arrayStringFor_A1SensorType, 0, wxDefaultValidator, wxT("A1SensorType"));
	A1SensorType->SetSelection(-1);

	SensorType = new wxStaticText(SensorsPage2, ID_SENSORTYPE, wxT("Type:"), wxPoint(5, 12), wxDefaultSize, 0, wxT("SensorType"));

	A1SensorOn = new wxCheckBox(SensorsPage2, ID_A1SENSORON, wxT("Enable"), wxPoint(237, 14), wxSize(97, 14), 0, wxDefaultValidator, wxT("A1SensorOn"));

	A1SensorReading = new wxStaticText(SensorsPage2, ID_A1SENSORREADING, wxT("Reading: Enable Sensor First."), wxPoint(5, 43), wxDefaultSize, 0, wxT("A1SensorReading"));

	SaveA1Config = new wxButton(SensorsPage2, ID_SAVEA1CONFIG, wxT("Save Config"), wxPoint(12, 114), wxSize(119, 21), 0, wxDefaultValidator, wxT("SaveA1Config"));

	LoadA1Config = new wxButton(SensorsPage2, ID_LOADA1CONFIG, wxT("Load Config"), wxPoint(158, 114), wxSize(123, 21), 0, wxDefaultValidator, wxT("LoadA1Config"));

	WxStaticBox6 = new wxStaticBox(SensorsPage2, ID_WXSTATICBOX6, wxT("Calibrate Sensor"), wxPoint(340, 8), wxSize(252, 141));

	CalibrateA1 = new wxButton(SensorsPage2, ID_CALIBRATEA1, wxT("Calibrate Sensor"), wxPoint(405, 115), wxSize(126, 20), 0, wxDefaultValidator, wxT("CalibrateA1"));

	A1ReadingMin = new wxTextCtrl(SensorsPage2, ID_A1RANGEMIN, wxT(""), wxPoint(508, 81), wxSize(59, 21), 0, wxDefaultValidator, wxT("A1ReadingMin"));

	WxStaticText7 = new wxStaticText(SensorsPage2, ID_WXSTATICTEXT7, wxT("Reading at min range"), wxPoint(351, 83), wxDefaultSize, 0, wxT("WxStaticText7"));

	WxStaticText8 = new wxStaticText(SensorsPage2, ID_WXSTATICTEXT8, wxT("Reading at max range"), wxPoint(349, 57), wxDefaultSize, 0, wxT("WxStaticText8"));

	WxStaticText9 = new wxStaticText(SensorsPage2, ID_WXSTATICTEXT9, wxT("Range (cm)"), wxPoint(349, 32), wxDefaultSize, 0, wxT("WxStaticText9"));

	A1RangeStart = new wxTextCtrl(SensorsPage2, ID_A1RANGESTART, wxT(""), wxPoint(423, 29), wxSize(55, 20), 0, wxDefaultValidator, wxT("A1RangeStart"));

	A1RangeEnd = new wxTextCtrl(SensorsPage2, ID_A1RANGEEND, wxT(""), wxPoint(508, 28), wxSize(66, 21), 0, wxDefaultValidator, wxT("A1RangeEnd"));

	A1ReadingMax = new wxTextCtrl(SensorsPage2, ID_A1READINGMAX, wxT(""), wxPoint(508, 56), wxSize(55, 21), 0, wxDefaultValidator, wxT("A1ReadingMax"));

	WxStaticText13 = new wxStaticText(SensorsPage2, ID_WXSTATICTEXT13, wxT("to"), wxPoint(487, 32), wxDefaultSize, 0, wxT("WxStaticText13"));

	SensorsPage3 = new wxPanel(sensor_pages, ID_SENSORSPAGE3, wxPoint(4, 24), wxSize(604, 152));
	sensor_pages->AddPage(SensorsPage3, wxT("Pin A2"));

	wxArrayString arrayStringFor_A2SensorType;
	arrayStringFor_A2SensorType.Add(wxT("None"));
	arrayStringFor_A2SensorType.Add(wxT("IR"));
	arrayStringFor_A2SensorType.Add(wxT("Line"));
	arrayStringFor_A2SensorType.Add(wxT("Light"));
	arrayStringFor_A2SensorType.Add(wxT("Sound"));
	arrayStringFor_A2SensorType.Add(wxT("Other"));
	A2SensorType = new wxChoice(SensorsPage3, ID_A2SENSORTYPE, wxPoint(52, 10), wxSize(140, 21), arrayStringFor_A2SensorType, 0, wxDefaultValidator, wxT("A2SensorType"));
	A2SensorType->SetSelection(-1);

	SensorType2 = new wxStaticText(SensorsPage3, ID_SENSORTYPE2, wxT("Type:"), wxPoint(5, 12), wxDefaultSize, 0, wxT("SensorType2"));

	A2SensorOn = new wxCheckBox(SensorsPage3, ID_A2SENSORON, wxT("Enable"), wxPoint(237, 14), wxSize(97, 14), 0, wxDefaultValidator, wxT("A2SensorOn"));

	WxStaticBox7 = new wxStaticBox(SensorsPage3, ID_WXSTATICBOX7, wxT("Calibrate Sensor"), wxPoint(340, 8), wxSize(252, 141));

	LoadA2Config = new wxButton(SensorsPage3, ID_LOADA2CONFIG, wxT("Load Config"), wxPoint(158, 114), wxSize(123, 21), 0, wxDefaultValidator, wxT("LoadA2Config"));

	SaveA2Config = new wxButton(SensorsPage3, ID_SAVEA2CONFIG, wxT("Save Config"), wxPoint(12, 114), wxSize(119, 21), 0, wxDefaultValidator, wxT("SaveA2Config"));

	A2RangeStart = new wxTextCtrl(SensorsPage3, ID_A2RANGESTART, wxT(""), wxPoint(423, 29), wxSize(55, 20), 0, wxDefaultValidator, wxT("A2RangeStart"));

	A2RangeEnd = new wxTextCtrl(SensorsPage3, ID_A2RANGEEND, wxT(""), wxPoint(508, 28), wxSize(66, 21), 0, wxDefaultValidator, wxT("A2RangeEnd"));

	WxStaticText11 = new wxStaticText(SensorsPage3, ID_WXSTATICTEXT11, wxT("Reading at max range"), wxPoint(349, 57), wxDefaultSize, 0, wxT("WxStaticText11"));

	A2ReadingMax = new wxTextCtrl(SensorsPage3, ID_A2READINGMAX, wxT(""), wxPoint(508, 56), wxSize(55, 21), 0, wxDefaultValidator, wxT("A2ReadingMax"));

	WxStaticText12 = new wxStaticText(SensorsPage3, ID_WXSTATICTEXT12, wxT("Reading at min range"), wxPoint(351, 83), wxDefaultSize, 0, wxT("WxStaticText12"));

	A2ReadingMin = new wxTextCtrl(SensorsPage3, ID_A2READINGMIN, wxT(""), wxPoint(508, 81), wxSize(59, 21), 0, wxDefaultValidator, wxT("A2ReadingMin"));

	CalibrateA2 = new wxButton(SensorsPage3, ID_CALIBRATEA2, wxT("Calibrate Sensor"), wxPoint(405, 115), wxSize(126, 20), 0, wxDefaultValidator, wxT("CalibrateA2"));

	WxStaticText14 = new wxStaticText(SensorsPage3, ID_WXSTATICTEXT14, wxT("Range (cm)"), wxPoint(349, 32), wxDefaultSize, 0, wxT("WxStaticText14"));

	WxStaticText15 = new wxStaticText(SensorsPage3, ID_WXSTATICTEXT15, wxT("to"), wxPoint(487, 32), wxDefaultSize, 0, wxT("WxStaticText15"));

	Outputs = new wxNotebook(this, ID_OUTPUTS, wxPoint(13, 249), wxSize(303, 260), wxNB_DEFAULT);

	Pin2 = new wxPanel(Outputs, ID_PIN2, wxPoint(4, 24), wxSize(295, 232));
	Outputs->AddPage(Pin2, wxT("Pin 2"));

	TogglePin2 = new wxToggleButton(Pin2, ID_TOGGLEPIN2, wxT("Turn On"), wxPoint(12, 72), wxSize(91, 21), 0, wxDefaultValidator, wxT("TogglePin2"));

	Type = new wxStaticText(Pin2, ID_TYPE, wxT("Type of device:"), wxPoint(12, 23), wxDefaultSize, 0, wxT("Type"));

	wxArrayString arrayStringFor_DevicePin2;
	arrayStringFor_DevicePin2.Add(wxT("Servo"));
	arrayStringFor_DevicePin2.Add(wxT("LED"));
	DevicePin2 = new wxChoice(Pin2, ID_DEVICEPIN2, wxPoint(109, 21), wxSize(145, 21), arrayStringFor_DevicePin2, 0, wxDefaultValidator, wxT("DevicePin2"));
	DevicePin2->SetSelection(-1);

	Pin3 = new wxPanel(Outputs, ID_PIN3, wxPoint(4, 24), wxSize(295, 232));
	Outputs->AddPage(Pin3, wxT("Pin 3 (PWM)"));

	WxStaticText5 = new wxStaticText(Pin3, ID_WXSTATICTEXT5, wxT("Type of device:"), wxPoint(12, 23), wxDefaultSize, 0, wxT("WxStaticText5"));

	wxArrayString arrayStringFor_DevicePin3;
	arrayStringFor_DevicePin3.Add(wxT("Servo"));
	arrayStringFor_DevicePin3.Add(wxT("LED"));
	arrayStringFor_DevicePin3.Add(wxT("Motor"));
	DevicePin3 = new wxChoice(Pin3, ID_DEVICEPIN3, wxPoint(109, 21), wxSize(145, 21), arrayStringFor_DevicePin3, 0, wxDefaultValidator, wxT("DevicePin3"));
	DevicePin3->SetSelection(-1);

	TogglePin3 = new wxToggleButton(Pin3, ID_TOGGLEPIN3, wxT("Turn On"), wxPoint(11, 155), wxSize(91, 21), 0, wxDefaultValidator, wxT("TogglePin3"));

	Pin3PWM = new wxSlider(Pin3, ID_PIN3PWM, 0, 0, 10, wxPoint(87, 79), wxSize(150, 45), wxSL_HORIZONTAL | wxSL_SELRANGE , wxDefaultValidator, wxT("Pin3PWM"));
	Pin3PWM->SetRange(0,10);
	Pin3PWM->SetValue(0);

	PWM3 = new wxStaticText(Pin3, ID_PWM3, wxT("PWM"), wxPoint(13, 81), wxDefaultSize, 0, wxT("PWM3"));

	EnablePWM3 = new wxCheckBox(Pin3, ID_ENABLEPWM3, wxT("Enable PWM"), wxPoint(10, 53), wxSize(97, 17), 0, wxDefaultValidator, wxT("EnablePWM3"));

	Pin4 = new wxPanel(Outputs, ID_PIN4, wxPoint(4, 24), wxSize(295, 232));
	Outputs->AddPage(Pin4, wxT("Pin 4"));

	Pin5 = new wxPanel(Outputs, ID_PIN5, wxPoint(4, 24), wxSize(295, 232));
	Outputs->AddPage(Pin5, wxT("Pin 5 (PWM)"));

	WxNotebook2 = new wxNotebook(this, ID_WXNOTEBOOK2, wxPoint(336, 250), wxSize(292, 259), wxNB_DEFAULT);

	Action1 = new wxPanel(WxNotebook2, ID_WXNOTEBOOKPAGE1, wxPoint(4, 24), wxSize(284, 231));
	WxNotebook2->AddPage(Action1, wxT("1"));

	WxStaticBox5 = new wxStaticBox(Action1, ID_WXSTATICBOX5, wxT("Processing"), wxPoint(2, 2), wxSize(279, 49));

	InputLabel = new wxStaticText(Action1, ID_INPUTLABEL, wxT("Input"), wxPoint(13, 24), wxDefaultSize, 0, wxT("InputLabel"));

	Processing1 = new wxStaticBox(Action1, ID_PROCESSING1, wxT("Processing"), wxPoint(3, 57), wxSize(279, 49));

	DistanceLabel = new wxStaticText(Action1, ID_DISTANCELABEL, wxT("Dist/Reading between:"), wxPoint(12, 78), wxDefaultSize, 0, wxT("DistanceLabel"));

	Action1Start = new wxTextCtrl(Action1, ID_ACTION1START, wxT(""), wxPoint(139, 77), wxSize(50, 19), 0, wxDefaultValidator, wxT("Action1Start"));

	andLabel = new wxStaticText(Action1, ID_ANDLABEL, wxT("and"), wxPoint(197, 79), wxDefaultSize, 0, wxT("andLabel"));

	Action1End = new wxTextCtrl(Action1, ID_ACTION1DISTEND, wxT(""), wxPoint(224, 77), wxSize(50, 20), 0, wxDefaultValidator, wxT("Action1End"));

	OutputLabel = new wxStaticText(Action1, ID_OUTPUTLABEL, wxT("Output"), wxPoint(7, 110), wxDefaultSize, 0, wxT("OutputLabel"));

	WxStaticText4 = new wxStaticText(Action1, ID_WXSTATICTEXT4, wxT("Action:"), wxPoint(7, 128), wxDefaultSize, 0, wxT("WxStaticText4"));

	Servo = new wxStaticText(Action1, ID_SERVO, wxT("Servo/Motor"), wxPoint(54, 128), wxDefaultSize, 0, wxT("Servo"));

	Event = new wxStaticText(Action1, ID_EVENT, wxT("Event"), wxPoint(8, 154), wxDefaultSize, 0, wxT("Event"));

	wxArrayString arrayStringFor_InputAction1;
	arrayStringFor_InputAction1.Add(wxT("Pin A0"));
	arrayStringFor_InputAction1.Add(wxT("Pin A1"));
	arrayStringFor_InputAction1.Add(wxT("Pin A2"));
	InputAction1 = new wxChoice(Action1, ID_INPUTACTION1, wxPoint(68, 22), wxSize(145, 21), arrayStringFor_InputAction1, 0, wxDefaultValidator, wxT("InputAction1"));
	InputAction1->SetSelection(-1);

	wxArrayString arrayStringFor_OutputAction1;
	arrayStringFor_OutputAction1.Add(wxT("Turn On Pin 2"));
	arrayStringFor_OutputAction1.Add(wxT("Turn Off Pin 2"));
	arrayStringFor_OutputAction1.Add(wxT("Turn On Pin 3"));
	arrayStringFor_OutputAction1.Add(wxT("Turn Off Pin 3"));
	arrayStringFor_OutputAction1.Add(wxT("PWM Pin 3"));
	arrayStringFor_OutputAction1.Add(wxT("Turn On Pin 4"));
	arrayStringFor_OutputAction1.Add(wxT("Turn Off Pin 4"));
	arrayStringFor_OutputAction1.Add(wxT("Turn On Pin 5"));
	arrayStringFor_OutputAction1.Add(wxT("Turn Off Pin 5"));
	arrayStringFor_OutputAction1.Add(wxT("PWM Pin 5"));
	OutputAction1 = new wxChoice(Action1, ID_OUTPUTACTION1, wxPoint(136, 124), wxSize(133, 21), arrayStringFor_OutputAction1, 0, wxDefaultValidator, wxT("OutputAction1"));
	OutputAction1->SetSelection(-1);

	ToggleAction1 = new wxToggleButton(Action1, ID_TOGGLEACTION1, wxT("Action ON"), wxPoint(78, 193), wxSize(141, 25), 0, wxDefaultValidator, wxT("ToggleAction1"));

	Action1File = new wxFilePickerCtrl(Action1, ID_ACTION1FILE, wxT(""), wxT("Select a file"), wxT("*.*"), wxPoint(49, 150), wxSize(185, 24), wxFLP_DEFAULT_STYLE, wxDefaultValidator, wxT("Action1File"));

	Action2 = new wxPanel(WxNotebook2, ID_WXNOTEBOOKPAGE2, wxPoint(4, 24), wxSize(284, 231));
	WxNotebook2->AddPage(Action2, wxT("2"));

	Action3 = new wxPanel(WxNotebook2, ID_WXNOTEBOOKPAGE3, wxPoint(4, 24), wxSize(284, 231));
	WxNotebook2->AddPage(Action3, wxT("3"));

	Action4 = new wxPanel(WxNotebook2, ID_WXNOTEBOOKPAGE4, wxPoint(4, 24), wxSize(284, 231));
	WxNotebook2->AddPage(Action4, wxT("4"));

	A2SensorReading = new wxStaticText(SensorsPage3, ID_A2SENSORREADING, wxT("Reading: Enable Sensor First."), wxPoint(5, 43), wxDefaultSize, 0, wxT("A2SensorReading"));

	WxStaticText17 = new wxStaticText(Pin5, ID_WXSTATICTEXT17, wxT("Type of device:"), wxPoint(12, 23), wxDefaultSize, 0, wxT("WxStaticText17"));

	wxArrayString arrayStringFor_DevicePin5;
	arrayStringFor_DevicePin5.Add(wxT("Servo"));
	arrayStringFor_DevicePin5.Add(wxT("LED"));
	arrayStringFor_DevicePin5.Add(wxT("Motor"));
	DevicePin5 = new wxChoice(Pin5, ID_DEVICEPIN5, wxPoint(109, 21), wxSize(145, 21), arrayStringFor_DevicePin5, 0, wxDefaultValidator, wxT("DevicePin5"));
	DevicePin5->SetSelection(-1);

	EnablePWM5 = new wxCheckBox(Pin5, ID_ENABLEPWM5, wxT("Enable PWM"), wxPoint(10, 53), wxSize(97, 17), 0, wxDefaultValidator, wxT("EnablePWM5"));

	WxStaticText18 = new wxStaticText(Pin5, ID_WXSTATICTEXT18, wxT("PWM"), wxPoint(13, 81), wxDefaultSize, 0, wxT("WxStaticText18"));

	Pin5PWM = new wxSlider(Pin5, ID_PIN5PWM, 0, 0, 10, wxPoint(87, 79), wxSize(150, 45), wxSL_HORIZONTAL | wxSL_SELRANGE , wxDefaultValidator, wxT("Pin5PWM"));
	Pin5PWM->SetRange(0,10);
	Pin5PWM->SetValue(0);

	TogglePin5 = new wxToggleButton(Pin5, ID_TOGGLEPIN5, wxT("Turn On"), wxPoint(11, 155), wxSize(91, 21), 0, wxDefaultValidator, wxT("TogglePin5"));

	WxStaticBox8 = new wxStaticBox(Action2, ID_WXSTATICBOX8, wxT("Processing"), wxPoint(2, 2), wxSize(279, 49));

	WxStaticText21 = new wxStaticText(Action2, ID_WXSTATICTEXT21, wxT("Input"), wxPoint(13, 24), wxDefaultSize, 0, wxT("WxStaticText21"));

	wxArrayString arrayStringFor_InputAction2;
	arrayStringFor_InputAction2.Add(wxT("Pin A0"));
	arrayStringFor_InputAction2.Add(wxT("Pin A1"));
	arrayStringFor_InputAction2.Add(wxT("Pin A2"));
	InputAction2 = new wxChoice(Action2, ID_INPUTACTION2, wxPoint(68, 22), wxSize(145, 21), arrayStringFor_InputAction2, 0, wxDefaultValidator, wxT("InputAction2"));
	InputAction2->SetSelection(-1);

	WxStaticBox9 = new wxStaticBox(Action2, ID_WXSTATICBOX9, wxT("Processing"), wxPoint(3, 57), wxSize(279, 49));

	WxStaticText22 = new wxStaticText(Action2, ID_WXSTATICTEXT22, wxT("Dist/Reading between:"), wxPoint(12, 78), wxDefaultSize, 0, wxT("WxStaticText22"));

	Action2Start = new wxTextCtrl(Action2, ID_ACTION2START, wxT(""), wxPoint(139, 77), wxSize(50, 19), 0, wxDefaultValidator, wxT("Action2Start"));

	WxStaticText23 = new wxStaticText(Action2, ID_WXSTATICTEXT23, wxT("and"), wxPoint(197, 79), wxDefaultSize, 0, wxT("WxStaticText23"));

	Action2End = new wxTextCtrl(Action2, ID_ACTION2END, wxT(""), wxPoint(224, 77), wxSize(50, 20), 0, wxDefaultValidator, wxT("Action2End"));

	WxStaticText24 = new wxStaticText(Action2, ID_WXSTATICTEXT24, wxT("Output"), wxPoint(7, 110), wxDefaultSize, 0, wxT("WxStaticText24"));

	WxStaticText25 = new wxStaticText(Action2, ID_WXSTATICTEXT25, wxT("Action:"), wxPoint(7, 128), wxDefaultSize, 0, wxT("WxStaticText25"));

	WxStaticText26 = new wxStaticText(Action2, ID_WXSTATICTEXT26, wxT("Servo/Motor"), wxPoint(54, 128), wxDefaultSize, 0, wxT("WxStaticText26"));

	wxArrayString arrayStringFor_OutputAction2;
	arrayStringFor_OutputAction2.Add(wxT("Turn On Pin 2"));
	arrayStringFor_OutputAction2.Add(wxT("Turn Off Pin 2"));
	arrayStringFor_OutputAction2.Add(wxT("Turn On Pin 3"));
	arrayStringFor_OutputAction2.Add(wxT("Turn Off Pin 3"));
	arrayStringFor_OutputAction2.Add(wxT("PWM Pin 3"));
	arrayStringFor_OutputAction2.Add(wxT("Turn On Pin 4"));
	arrayStringFor_OutputAction2.Add(wxT("Turn Off Pin 4"));
	arrayStringFor_OutputAction2.Add(wxT("Turn On Pin 5"));
	arrayStringFor_OutputAction2.Add(wxT("Turn Off Pin 5"));
	arrayStringFor_OutputAction2.Add(wxT("PWM Pin 5"));
	OutputAction2 = new wxChoice(Action2, ID_OUTPUTACTION2, wxPoint(136, 124), wxSize(133, 21), arrayStringFor_OutputAction2, 0, wxDefaultValidator, wxT("OutputAction2"));
	OutputAction2->SetSelection(-1);

	WxStaticText27 = new wxStaticText(Action2, ID_WXSTATICTEXT27, wxT("Event"), wxPoint(8, 154), wxDefaultSize, 0, wxT("WxStaticText27"));

	Action2File = new wxFilePickerCtrl(Action2, ID_ACTION2FILE, wxT(""), wxT("Select a file"), wxT("*.*"), wxPoint(49, 150), wxSize(185, 24), wxFLP_DEFAULT_STYLE, wxDefaultValidator, wxT("Action2File"));

	ToggleAction2 = new wxToggleButton(Action2, ID_TOGGLEACTION2, wxT("Action ON"), wxPoint(78, 193), wxSize(141, 25), 0, wxDefaultValidator, wxT("ToggleAction2"));

	WxStaticBox10 = new wxStaticBox(Action3, ID_WXSTATICBOX10, wxT("Processing"), wxPoint(2, 2), wxSize(279, 49));

	WxStaticText28 = new wxStaticText(Action3, ID_WXSTATICTEXT28, wxT("Input"), wxPoint(13, 24), wxDefaultSize, 0, wxT("WxStaticText28"));

	wxArrayString arrayStringFor_InputAction3;
	arrayStringFor_InputAction3.Add(wxT("Pin A0"));
	arrayStringFor_InputAction3.Add(wxT("Pin A1"));
	arrayStringFor_InputAction3.Add(wxT("Pin A2"));
	InputAction3 = new wxChoice(Action3, ID_INPUTACTION3, wxPoint(68, 22), wxSize(145, 21), arrayStringFor_InputAction3, 0, wxDefaultValidator, wxT("InputAction3"));
	InputAction3->SetSelection(-1);

	WxStaticBox11 = new wxStaticBox(Action3, ID_WXSTATICBOX11, wxT("Processing"), wxPoint(3, 57), wxSize(279, 49));

	WxStaticText29 = new wxStaticText(Action3, ID_WXSTATICTEXT29, wxT("Dist/Reading between:"), wxPoint(12, 78), wxDefaultSize, 0, wxT("WxStaticText29"));

	Action3Start = new wxTextCtrl(Action3, ID_ACTION3START, wxT(""), wxPoint(139, 77), wxSize(50, 19), 0, wxDefaultValidator, wxT("Action3Start"));

	WxStaticText30 = new wxStaticText(Action3, ID_WXSTATICTEXT30, wxT("and"), wxPoint(197, 79), wxDefaultSize, 0, wxT("WxStaticText30"));

	Action3End = new wxTextCtrl(Action3, ID_ACTION3END, wxT(""), wxPoint(224, 77), wxSize(50, 20), 0, wxDefaultValidator, wxT("Action3End"));

	WxStaticText31 = new wxStaticText(Action3, ID_WXSTATICTEXT31, wxT("Output"), wxPoint(7, 110), wxDefaultSize, 0, wxT("WxStaticText31"));

	WxStaticText32 = new wxStaticText(Action3, ID_WXSTATICTEXT32, wxT("Action:"), wxPoint(7, 128), wxDefaultSize, 0, wxT("WxStaticText32"));

	WxStaticText33 = new wxStaticText(Action3, ID_WXSTATICTEXT33, wxT("Servo/Motor"), wxPoint(54, 128), wxDefaultSize, 0, wxT("WxStaticText33"));

	wxArrayString arrayStringFor_OutputAction3;
	arrayStringFor_OutputAction3.Add(wxT("Turn On Pin 2"));
	arrayStringFor_OutputAction3.Add(wxT("Turn Off Pin 2"));
	arrayStringFor_OutputAction3.Add(wxT("Turn On Pin 3"));
	arrayStringFor_OutputAction3.Add(wxT("Turn Off Pin 3"));
	arrayStringFor_OutputAction3.Add(wxT("PWM Pin 3"));
	arrayStringFor_OutputAction3.Add(wxT("Turn On Pin 4"));
	arrayStringFor_OutputAction3.Add(wxT("Turn Off Pin 4"));
	arrayStringFor_OutputAction3.Add(wxT("Turn On Pin 5"));
	arrayStringFor_OutputAction3.Add(wxT("Turn Off Pin 5"));
	arrayStringFor_OutputAction3.Add(wxT("PWM Pin 5"));
	OutputAction3 = new wxChoice(Action3, ID_OUTPUTACTION3, wxPoint(136, 124), wxSize(133, 21), arrayStringFor_OutputAction3, 0, wxDefaultValidator, wxT("OutputAction3"));
	OutputAction3->SetSelection(-1);

	WxStaticText34 = new wxStaticText(Action3, ID_WXSTATICTEXT34, wxT("Event"), wxPoint(8, 154), wxDefaultSize, 0, wxT("WxStaticText34"));

	Action3File = new wxFilePickerCtrl(Action3, ID_ACTION3FILE, wxT(""), wxT("Select a file"), wxT("*.*"), wxPoint(49, 150), wxSize(185, 24), wxFLP_DEFAULT_STYLE, wxDefaultValidator, wxT("Action3File"));

	ToggleAction3 = new wxToggleButton(Action3, ID_TOGGLEACTION3, wxT("Action ON"), wxPoint(78, 193), wxSize(141, 25), 0, wxDefaultValidator, wxT("ToggleAction3"));

	WxStaticBox12 = new wxStaticBox(Action4, ID_WXSTATICBOX12, wxT("Processing"), wxPoint(2, 2), wxSize(279, 49));

	WxStaticText35 = new wxStaticText(Action4, ID_WXSTATICTEXT35, wxT("Input"), wxPoint(13, 24), wxDefaultSize, 0, wxT("WxStaticText35"));

	wxArrayString arrayStringFor_InputAction4;
	arrayStringFor_InputAction4.Add(wxT("Pin A0"));
	arrayStringFor_InputAction4.Add(wxT("Pin A1"));
	arrayStringFor_InputAction4.Add(wxT("Pin A2"));
	InputAction4 = new wxChoice(Action4, ID_INPUTACTION4, wxPoint(68, 22), wxSize(145, 21), arrayStringFor_InputAction4, 0, wxDefaultValidator, wxT("InputAction4"));
	InputAction4->SetSelection(-1);

	WxStaticBox13 = new wxStaticBox(Action4, ID_WXSTATICBOX13, wxT("Processing"), wxPoint(3, 57), wxSize(279, 49));

	WxStaticText36 = new wxStaticText(Action4, ID_WXSTATICTEXT36, wxT("Dist/Reading between:"), wxPoint(12, 78), wxDefaultSize, 0, wxT("WxStaticText36"));

	Action4Start = new wxTextCtrl(Action4, ID_ACTION4START, wxT(""), wxPoint(139, 77), wxSize(50, 19), 0, wxDefaultValidator, wxT("Action4Start"));

	WxStaticText37 = new wxStaticText(Action4, ID_WXSTATICTEXT37, wxT("and"), wxPoint(197, 79), wxDefaultSize, 0, wxT("WxStaticText37"));

	Action4End = new wxTextCtrl(Action4, ID_ACTION4END, wxT(""), wxPoint(224, 77), wxSize(50, 20), 0, wxDefaultValidator, wxT("Action4End"));

	WxStaticText38 = new wxStaticText(Action4, ID_WXSTATICTEXT38, wxT("Output"), wxPoint(7, 110), wxDefaultSize, 0, wxT("WxStaticText38"));

	WxStaticText39 = new wxStaticText(Action4, ID_WXSTATICTEXT39, wxT("Action:"), wxPoint(7, 128), wxDefaultSize, 0, wxT("WxStaticText39"));

	WxStaticText40 = new wxStaticText(Action4, ID_WXSTATICTEXT40, wxT("Servo/Motor"), wxPoint(54, 128), wxDefaultSize, 0, wxT("WxStaticText40"));

	wxArrayString arrayStringFor_OutputAction4;
	arrayStringFor_OutputAction4.Add(wxT("Turn On Pin 2"));
	arrayStringFor_OutputAction4.Add(wxT("Turn Off Pin 2"));
	arrayStringFor_OutputAction4.Add(wxT("Turn On Pin 3"));
	arrayStringFor_OutputAction4.Add(wxT("Turn Off Pin 3"));
	arrayStringFor_OutputAction4.Add(wxT("PWM Pin 3"));
	arrayStringFor_OutputAction4.Add(wxT("Turn On Pin 4"));
	arrayStringFor_OutputAction4.Add(wxT("Turn Off Pin 4"));
	arrayStringFor_OutputAction4.Add(wxT("Turn On Pin 5"));
	arrayStringFor_OutputAction4.Add(wxT("Turn Off Pin 5"));
	arrayStringFor_OutputAction4.Add(wxT("PWM Pin 5"));
	OutputAction4 = new wxChoice(Action4, ID_OUTPUTACTION4, wxPoint(136, 124), wxSize(133, 21), arrayStringFor_OutputAction4, 0, wxDefaultValidator, wxT("OutputAction4"));
	OutputAction4->SetSelection(-1);

	WxStaticText41 = new wxStaticText(Action4, ID_WXSTATICTEXT41, wxT("Event"), wxPoint(8, 154), wxDefaultSize, 0, wxT("WxStaticText41"));

	Action4File = new wxFilePickerCtrl(Action4, ID_ACTION4FILE, wxT(""), wxT("Select a file"), wxT("*.*"), wxPoint(49, 150), wxSize(185, 24), wxFLP_DEFAULT_STYLE, wxDefaultValidator, wxT("Action4File"));

	ToggleAction4 = new wxToggleButton(Action4, ID_TOGGLEACTION4, wxT("Action ON"), wxPoint(78, 193), wxSize(141, 25), 0, wxDefaultValidator, wxT("ToggleAction4"));

	wxArrayString arrayStringFor_DevicePin4;
	arrayStringFor_DevicePin4.Add(wxT("Servo"));
	arrayStringFor_DevicePin4.Add(wxT("LED"));
	DevicePin4 = new wxChoice(Pin4, ID_DEVICEPIN4, wxPoint(109, 21), wxSize(145, 21), arrayStringFor_DevicePin4, 0, wxDefaultValidator, wxT("DevicePin4"));
	DevicePin4->SetSelection(-1);

	WxStaticText19 = new wxStaticText(Pin4, ID_WXSTATICTEXT19, wxT("Type of device:"), wxPoint(12, 23), wxDefaultSize, 0, wxT("WxStaticText19"));
	TogglePin4 = new wxToggleButton(Pin4, ID_TOGGLEPIN4, wxT("Turn On"), wxPoint(12, 72), wxSize(91, 21), 0, wxDefaultValidator, wxT("TogglePin4"));

	SetTitle(wxT("Arduino Toolkit"));
	SetIcon(wxNullIcon);
	SetSize(8,8,650,558);
	Center();

}

void arduinokit::UpdateStatus(void){
	wxString status;
	if (port.Is_open()) {
		status.Printf(port.get_name() + _("    ") +
					  firmata_name + _("    Tx:%u Rx:%u"),
					  tx_count, rx_count);
	} else {
		status = _("Please choose a serial port");
	}
	SetStatusText(status);
}

void arduinokit::OnPort(wxCommandEvent &event){
	int id = event.GetId();
	wxString name = port_menu->FindItem(id)->GetLabel();
	check_port(name);
	UpdateStatus();
}

void arduinokit::OnIdle(wxIdleEvent &event){
	uint8_t buf[1024];
	int r;
	r = port.Input_wait(40);
	if (r > 0) {
		r = port.Read(buf, sizeof(buf));
		if (r < 0) {
			// error
			return;
		}
		if (r > 0) {
			// parse
			rx_count += r;
			for (int i=0; i < r; i++) {
				//printf("%02X ", buf[i]);
			}
			Parse(buf, r);
			UpdateStatus();
		}
	} else if (r < 0) {
		return;
	}
	event.RequestMore(true);
}

void arduinokit::Parse(const uint8_t *buf, int len){
	const uint8_t *p, *end;

	p = buf;
	end = p + len;
	for (p = buf; p < end; p++) {
		uint8_t msn = *p & 0xF0;
		if (msn == 0xE0 || msn == 0x90 || *p == 0xF9) {
			parse_command_len = 3;
			parse_count = 0;
		} else if (msn == 0xC0 || msn == 0xD0) {
			parse_command_len = 2;
			parse_count = 0;
		} else if (*p == START_SYSEX) {
			parse_count = 0;
			parse_command_len = sizeof(parse_buf);
		} else if (*p == END_SYSEX) {
			parse_command_len = parse_count + 1;
		} else if (*p & 0x80) {
			parse_command_len = 1;
			parse_count = 0;
		}
		if (parse_count < (int)sizeof(parse_buf)) {
			parse_buf[parse_count++] = *p;
		}
		if (parse_count == parse_command_len) {
			DoMessage();
			parse_count = parse_command_len = 0;
		}
	}
}

void arduinokit::DoMessage(void){

	read_analog(parse_buf, parse_count);
	int analog_ch;
	int analog_val;

	if (A0SensorOn->GetValue()) { //check if user wants this one enabled...
		analog_ch = 0;
		analog_val = pin_info[16].value;
		wxString val;
		val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
		A0SensorReading->SetLabel(val);
	}
	if (A1SensorOn->GetValue()) { //check if user wants this one enabled...
		analog_ch = 1;
		analog_val = pin_info[17].value;
		wxString val;
		val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
		A1SensorReading->SetLabel(val);
	}
	if (A2SensorOn->GetValue()) { //check if user wants this one enabled...
		analog_ch = 2;
		analog_val = pin_info[18].value;
		wxString val;
		val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
		A2SensorReading->SetLabel(val);
	}
	return;
	UpdateStatus();
}

void arduinokit::OnAbout( wxCommandEvent &event ){
    wxMessageDialog dialog( this, wxT("Arduino Tooklit. arduinokit.wordpress.com"),
        wxT("About ATK"), wxOK|wxICON_INFORMATION );
    //dialog.ShowModal();
}

void arduinokit::OnQuit( wxCommandEvent &event ){
     Close( true ); //exit gracefully
}

void arduinokit::OnCloseWindow( wxCloseEvent &event ){
    // if ! saved changes -> return
    Destroy();
}

void arduinokit::OnSize( wxSizeEvent &event ){
    //event.Skip( true );
}

//-------------------
// GUI EVENTS
//-------------------
void arduinokit::OnClose(wxCloseEvent& event){
	Destroy();
}

//------------------------------------------------------------------------------
// Calibrating the sensor
// Note: Once calibrated, the analog_val should be adjusted accordingly
// everywhere else
//------------------------------------------------------------------------------


void arduinokit::CalibrateA0Click(wxCommandEvent& event){
	//get config data
	int range_start = wxAtoi(A0RangeStart->GetValue());
    int range_end = wxAtoi(A0RangeEnd->GetValue());

	//save values into pin info
	pin_info[16].range_min = range_start;
	pin_info[16].range_max = range_end;

	int read_max = wxAtoi(A0ReadingMax->GetValue());
	int read_min = wxAtoi(A0ReadingMin->GetValue());

	float read_factor = fabs((read_max - read_min)/(float)(range_end - range_start));
	pin_info[16].range_factor = read_factor;
	printf("1cm = %f in reading read_max", pin_info[16].range_factor);

}

void arduinokit::CalibrateA1Click(wxCommandEvent& event){
	//get config data
	int range_start = wxAtoi(A1RangeStart->GetValue());
    int range_end = wxAtoi(A1RangeEnd->GetValue());

	//save values into pin info
	pin_info[17].range_min = range_start;
	pin_info[17].range_max = range_end;

	int read_max = wxAtoi(A1ReadingMax->GetValue());
	int read_min = wxAtoi(A1ReadingMin->GetValue());

	float read_factor = fabs((read_max - read_min)/(float)(range_end - range_start));
	pin_info[17].range_factor = read_factor;
	printf("1cm = %f in reading read_max", pin_info[17].range_factor);

}

void arduinokit::CalibrateA2Click(wxCommandEvent& event){
	//get config data
	int range_start = wxAtoi(A2RangeStart->GetValue());
    int range_end = wxAtoi(A2RangeEnd->GetValue());

	//save values into pin info
	pin_info[2].range_min = range_start;
	pin_info[2].range_max = range_end;

	int read_max = wxAtoi(A2ReadingMax->GetValue());
	int read_min = wxAtoi(A2ReadingMin->GetValue());

	float read_factor = fabs((read_max - read_min)/(float)(range_end - range_start));
	pin_info[2].range_factor = read_factor;
	printf("1cm = %f in reading read_max", pin_info[2].range_factor);
}


//------------------------------------------------------------------------------
// Save Config in an INI type file for safekeeping
//------------------------------------------------------------------------------


//SaveConfigClick - Save the data for the sensors
void arduinokit::SaveA0ConfigClick(wxCommandEvent& event){

	//delete previous config
	system("del a0.cfg");
	//write new config
	wxString ConfigFileName=wxT("a0.cfg");
    wxFileConfig *config = new wxFileConfig (wxEmptyString,
                        wxEmptyString,
                        ConfigFileName,
                        wxEmptyString,
                        wxCONFIG_USE_RELATIVE_PATH
                        );

    config->SetPath(wxT("/"));
    config->Write(wxT("/ReadingMax"), A0ReadingMax->GetValue());
	config->Write(wxT("/ReadingMin"), A0ReadingMin->GetValue());
	config->Write(wxT("/RangeStart"), A0RangeStart->GetValue());
	config->Write(wxT("/RangeEnd"), A0RangeEnd->GetValue());
	config->Flush();
	wxString status = wxT("Saved calibration configuration");
	SetStatusText(status);

}

void arduinokit::LoadA0ConfigClick(wxCommandEvent& event){
	wxString ConfigFileName=wxT("a0.cfg");
    wxFileConfig *config = new wxFileConfig (wxEmptyString,
											 wxEmptyString,
											 ConfigFileName,
											 wxEmptyString,
											 wxCONFIG_USE_RELATIVE_PATH
											 );
	double a0rm = 0, a0rmin = 5, a0rs = 5, a0re = 5;

	config->Read(wxT("/ReadingMax"), &a0rm);
	wxString ReadingMax = wxString::Format(wxT("%.0f"), a0rm);
	A0ReadingMax->SetValue(ReadingMax);

	config->Read(wxT("/ReadingMin"), &a0rmin);
	wxString ReadingMin = wxString::Format(wxT("%.0f"), a0rmin);
	A0ReadingMin->SetValue(ReadingMin);

	config->Read(wxT("/RangeStart"), &a0rs);
	wxString RangeStart = wxString::Format(wxT("%.0f"), a0rs);
	A0RangeStart->SetValue(RangeStart);

	config->Read(wxT("/RangeEnd"), &a0re);
	wxString RangeEnd = wxString::Format(wxT("%.0f"), a0re);
	A0RangeEnd->SetValue(RangeEnd);

	wxString status = wxT("Loaded configuration");
	SetStatusText(status);

}

void arduinokit::SaveA1ConfigClick(wxCommandEvent& event){
	//delete previous config
	system("del a1.cfg");
	//write new config
	wxString ConfigFileName=wxT("a1.cfg");
    wxFileConfig *config = new wxFileConfig (wxEmptyString,
											 wxEmptyString,
											 ConfigFileName,
											 wxEmptyString,
											 wxCONFIG_USE_RELATIVE_PATH
											 );

    config->SetPath(wxT("/"));
    config->Write(wxT("/ReadingMax"), A1ReadingMax->GetValue());
	config->Write(wxT("/ReadingMin"), A1ReadingMin->GetValue());
	config->Write(wxT("/RangeStart"), A1RangeStart->GetValue());
	config->Write(wxT("/RangeEnd"), A1RangeEnd->GetValue());
	config->Flush();
	wxString status = wxT("Saved calibration configuration");
	SetStatusText(status);
}


void arduinokit::LoadA1ConfigClick(wxCommandEvent& event){
	wxString ConfigFileName=wxT("a1.cfg");
    wxFileConfig *config = new wxFileConfig (wxEmptyString,
											 wxEmptyString,
											 ConfigFileName,
											 wxEmptyString,
											 wxCONFIG_USE_RELATIVE_PATH
											 );
	double a0rm = 0, a0rmin = 5, a0rs = 5, a0re = 5;

	config->Read(wxT("/ReadingMax"), &a0rm);
	wxString ReadingMax = wxString::Format(wxT("%.0f"), a0rm);
	A1ReadingMax->SetValue(ReadingMax);

	config->Read(wxT("/ReadingMin"), &a0rmin);
	wxString ReadingMin = wxString::Format(wxT("%.0f"), a0rmin);
	A1ReadingMin->SetValue(ReadingMin);

	config->Read(wxT("/RangeStart"), &a0rs);
	wxString RangeStart = wxString::Format(wxT("%.0f"), a0rs);
	A1RangeStart->SetValue(RangeStart);

	config->Read(wxT("/RangeEnd"), &a0re);
	wxString RangeEnd = wxString::Format(wxT("%.0f"), a0re);
	A1RangeEnd->SetValue(RangeEnd);

	wxString status = wxT("Loaded configuration");
	SetStatusText(status);
}


void arduinokit::SaveA2ConfigClick(wxCommandEvent& event){
	//delete previous config
	system("del a2.cfg");
	//write new config
	wxString ConfigFileName=wxT("a2.cfg");
    wxFileConfig *config = new wxFileConfig (wxEmptyString,
											 wxEmptyString,
											 ConfigFileName,
											 wxEmptyString,
											 wxCONFIG_USE_RELATIVE_PATH
											 );

    config->SetPath(wxT("/"));
    config->Write(wxT("/ReadingMax"), A2ReadingMax->GetValue());
	config->Write(wxT("/ReadingMin"), A2ReadingMin->GetValue());
	config->Write(wxT("/RangeStart"), A2RangeStart->GetValue());
	config->Write(wxT("/RangeEnd"), A2RangeEnd->GetValue());
	config->Flush();
	wxString status = wxT("Saved calibration configuration");
	SetStatusText(status);
}


void arduinokit::LoadA2ConfigClick(wxCommandEvent& event){
	wxString ConfigFileName=wxT("a2.cfg");
    wxFileConfig *config = new wxFileConfig (wxEmptyString,
											 wxEmptyString,
											 ConfigFileName,
											 wxEmptyString,
											 wxCONFIG_USE_RELATIVE_PATH
											 );
	double a0rm = 0, a0rmin = 5, a0rs = 5, a0re = 5;

	config->Read(wxT("/ReadingMax"), &a0rm);
	wxString ReadingMax = wxString::Format(wxT("%.0f"), a0rm);
	A2ReadingMax->SetValue(ReadingMax);

	config->Read(wxT("/ReadingMin"), &a0rmin);
	wxString ReadingMin = wxString::Format(wxT("%.0f"), a0rmin);
	A2ReadingMin->SetValue(ReadingMin);

	config->Read(wxT("/RangeStart"), &a0rs);
	wxString RangeStart = wxString::Format(wxT("%.0f"), a0rs);
	A2RangeStart->SetValue(RangeStart);

	config->Read(wxT("/RangeEnd"), &a0re);
	wxString RangeEnd = wxString::Format(wxT("%.0f"), a0re);
	A2RangeEnd->SetValue(RangeEnd);

	wxString status = wxT("Loaded configuration");
	SetStatusText(status);
}

//------------------------------------------------------------------------------
// Outputting to pins
//------------------------------------------------------------------------------


//Toggle Pins
void arduinokit::TogglePin2Click(wxCommandEvent& event){

	toggle_pin(2);
	UpdateStatus();
}

void arduinokit::TogglePin3Click(wxCommandEvent& event){
	toggle_pin(3);
	UpdateStatus();
}

void arduinokit::TogglePin4Click(wxCommandEvent& event){
	toggle_pin(4);
	UpdateStatus();
}

void arduinokit::TogglePin5Click(wxCommandEvent& event){
	toggle_pin(5);
	UpdateStatus();
}

//PWM for PWM enabled pins
void arduinokit::Pin3PWMScroll(wxScrollEvent& event){
	int pin = 3;
	//check if pwm is supposed to be enabled
	if (EnablePWM3->GetValue()){
	//get value
	int val = Pin3PWM->GetValue();
	set_pwm(val, pin);
	UpdateStatus();
	}
}

void arduinokit::Pin5PWMScroll(wxScrollEvent& event){
	int pin = 5;
	//check if pwm is supposed to be enabled
	if (EnablePWM5->GetValue()){
		//get value
		int val = Pin5PWM->GetValue();
		set_pwm(val, pin);
		UpdateStatus();
	}
}

void arduinokit::motor_action(int action_number){
	//motor action
	switch (action_number){
		case 1: //turn on pin 2

			//if off turn
			if(!pin_info[2].value)
				toggle_pin(2);

			break;

		case 2:
			//if on turn off...
			if(pin_info[2].value)
				toggle_pin(2);

			break;

		case 3:

			if(!pin_info[3].value)
				toggle_pin(3);

			break;

		case 4:
			if(pin_info[2].value)
				toggle_pin(2);

			break;

		case 5:
			//check if pwm is supposed to be enabled
			if (EnablePWM3->GetValue()){
				//get value
				int val = Pin3PWM->GetValue();
				set_pwm(val, 3);
				UpdateStatus();
			}
			break;

		case 6:
			if(!pin_info[4].value)
				toggle_pin(4);

			break;

		case 7:
			if(pin_info[4].value)
				toggle_pin(4);

			break;

		case 8:
			if(!pin_info[5].value)
				toggle_pin(5);

			break;
		case 9:
			if(pin_info[5].value)
				toggle_pin(5);

			break;
		case 10:
			if (EnablePWM5->GetValue()){
				//get value
				int val = Pin5PWM->GetValue();
				set_pwm(val, 3);
				UpdateStatus();
			}
			break;
		default:
			break;
	}

	return;

}

void arduinokit::ToggleAction1Click(wxCommandEvent& event){

	if (!action[1]){
		printf("Action 1 Started\n");
		float min = wxAtoi(Action1Start->GetValue());
		float max = wxAtoi(Action1End->GetValue());
		float this_val;
		action[1] = 1; //mark it as on
		//figure out type
		switch (InputAction1->GetCurrentSelection()){
			case 0: //pin a0
				printf("IR type Sensor Selected\n");
				//check if enabled
				if (A0SensorOn->GetValue()){
					read_analog(parse_buf, parse_count);
					int analog_ch;
					int analog_val;
					analog_ch = 0;
					analog_val = pin_info[16].value;
					wxString val;
					val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
					A0SensorReading->SetLabel(val);
					//if set and valid data
					this_val = (analog_val)/pin_info[16].range_factor;
					printf("range: %f\n %d \n %d\n %d", pin_info[16].range_factor, pin_info[16].range_max, pin_info[16].range_min, (int)this_val);

					if (pin_info[16].range_factor != 0 && !((int)this_val > pin_info[16].range_max) &&
													  !((int)this_val < pin_info[16].range_min)){
						if ( this_val >= min &&  this_val <= max) {//check if within range
							motor_action(OutputAction1->GetSelection());
							A0SensorReading->SetLabel(val);
							//file action
							if (Action1File->GetPath() != _("")){
								char open[200] = "open ";
								system(strcat(open, Action1File->GetPath().mb_str()));
							}
						}
					}
					else {
						wxMessageBox(_("Please calibrate the sensor or connect it"));
					}
				}

				break;

			default:
				break;

		} //end switch

	} //end if

	else {
		action[1] = 0;
		printf("Action 1 Off\n");
	}

}

void arduinokit::ToggleAction2Click(wxCommandEvent& event){
	if (!action[2]){
		printf("Action 1 Started\n");
		float min = wxAtoi(Action2Start->GetValue());
		float max = wxAtoi(Action2End->GetValue());
		float this_val;
		action[2] = 1; //mark it as on
		//figure out type
		switch (InputAction2->GetCurrentSelection()){
			case 0: //pin a0
				printf("IR type Sensor Selected\n");
				//check if enabled
				if (A0SensorOn->GetValue()){
					read_analog(parse_buf, parse_count);
					int analog_ch;
					int analog_val;
					analog_ch = 0;
					analog_val = pin_info[16].value;
					wxString val;
					val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
					A0SensorReading->SetLabel(val);
					//if set and valid data
					this_val = (analog_val)/pin_info[16].range_factor;
					printf("range: %f\n %d \n %d\n %d", pin_info[16].range_factor, pin_info[16].range_max, pin_info[16].range_min, (int)this_val);

					if (pin_info[16].range_factor != 0 && !((int)this_val > pin_info[16].range_max) &&
						!((int)this_val < pin_info[16].range_min)){
						if ( this_val >= min &&  this_val <= max) {//check if within range
							motor_action(OutputAction2->GetSelection());
							A0SensorReading->SetLabel(val);
							//file action
							if (Action2File->GetPath() != _("")){
								char open[200] = "open ";
								system(strcat(open, Action2File->GetPath().mb_str()));
							}
						}
					}
					else {
						wxMessageBox(_("Please calibrate the sensor or connect it"));
					}
				}

				break;

			default:
				break;

		} //end switch

	} //end if

	else {
		action[2] = 0;
		printf("Action 2 Off\n");
	}
}

void arduinokit::ToggleAction3Click(wxCommandEvent& event){
	if (!action[3]){
		printf("Action 3 Started\n");
		float min = wxAtoi(Action1Start->GetValue());
		float max = wxAtoi(Action1End->GetValue());
		float this_val;
		action[3] = 1; //mark it as on
		//figure out type
		switch (InputAction3->GetCurrentSelection()){
			case 0: //pin a0
				printf("IR type Sensor Selected\n");
				//check if enabled
				if (A0SensorOn->GetValue()){
					read_analog(parse_buf, parse_count);
					int analog_ch;
					int analog_val;
					analog_ch = 0;
					analog_val = pin_info[16].value;
					wxString val;
					val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
					A0SensorReading->SetLabel(val);
					//if set and valid data
					this_val = (analog_val)/pin_info[16].range_factor;
					printf("range: %f\n %d \n %d\n %d", pin_info[16].range_factor, pin_info[16].range_max, pin_info[16].range_min, (int)this_val);

					if (pin_info[16].range_factor != 0 && !((int)this_val > pin_info[16].range_max) &&
						!((int)this_val < pin_info[16].range_min)){
						if ( this_val >= min &&  this_val <= max) {//check if within range
							motor_action(OutputAction3->GetSelection());
							A0SensorReading->SetLabel(val);
							//file action
							if (Action3File->GetPath() != _("")){
								char open[200] = "open ";
								system(strcat(open, Action3File->GetPath().mb_str()));
							}
						}
					}
					else {
						wxMessageBox(_("Please calibrate the sensor or connect it"));
					}
				}

				break;

			default:
				break;

		} //end switch

	} //end if

	else {
		action[3] = 0;
		printf("Action 3 Off\n");
	}
}

void arduinokit::ToggleAction4Click(wxCommandEvent& event){
	if (!action[4]){
		printf("Action 4 Started\n");
		float min = wxAtoi(Action4Start->GetValue());
		float max = wxAtoi(Action4End->GetValue());
		float this_val;
		action[4] = 1; //mark it as on
		//figure out type
		switch (InputAction4->GetCurrentSelection()){
			case 0: //pin a0
				printf("IR type Sensor Selected\n");
				//check if enabled
				if (A0SensorOn->GetValue()){
					read_analog(parse_buf, parse_count);
					int analog_ch;
					int analog_val;
					analog_ch = 0;
					analog_val = pin_info[16].value;
					wxString val;
					val.Printf(_("A%d Sensor Reading: %d"), analog_ch, analog_val);
					A0SensorReading->SetLabel(val);
					//if set and valid data
					this_val = (analog_val)/pin_info[16].range_factor;
					printf("range: %f\n %d \n %d\n %d", pin_info[16].range_factor, pin_info[16].range_max, pin_info[16].range_min, (int)this_val);

					if (pin_info[16].range_factor != 0 && !((int)this_val > pin_info[16].range_max) &&
						!((int)this_val < pin_info[16].range_min)){
						if ( this_val >= min &&  this_val <= max) {//check if within range
							motor_action(OutputAction4->GetSelection());
							A0SensorReading->SetLabel(val);
							//file action
							if (Action4File->GetPath() != _("")){
								char open[200] = "open ";
								system(strcat(open, Action4File->GetPath().mb_str()));
							}
						}
					}
					else {
						wxMessageBox(_("Please calibrate the sensor or connect it"));
					}
				}

				break;

			default:
				break;

		} //end switch

	} //end if

	else {
		action[4] = 0;
		printf("Action 4 Off\n");
	}
}


//------------------------------------------------------------------------------
// Port Menu
//------------------------------------------------------------------------------

MyMenu::MyMenu(const wxString& title, long style) : wxMenu(title, style){
}

void MyMenu::OnShowPortList(wxMenuEvent &event){
	wxMenu *menu;
	wxMenuItem *item;
	int num, any=0;

	menu = event.GetMenu();
	if (menu != port_menu)
		return;

	menu->AppendRadioItem(9000, _(" (none)"));
	wxMenuItemList old_items = menu->GetMenuItems();
	num = old_items.GetCount();
	printf("number is %d", num);
	if (num > 0) {
			for (int i = num-1; i > 0; i--) {
				menu->Delete(old_items[i]);
			}

	}
	wxArrayString list = port.port_list();
	num = list.GetCount();
	if (num > 0) {

		for (int i=0; i < num; i++) {
			item = menu->AppendRadioItem(9001 + i, list[i]);
			if (port.Is_open() && port.get_name().IsSameAs(list[i])) {
				menu->Check(9001 + i, true);
				any = 1;
			}

		}
	}

	if (!any) menu->Check(9000, true);
}

void MyMenu::OnHighlight(wxMenuEvent &event){
}


//------------------------------------------------------------------------------
// Implement App
//------------------------------------------------------------------------------

IMPLEMENT_APP(arduinokitApp)

bool arduinokitApp::OnInit(){
    arduinokit* frame = new arduinokit(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}

int arduinokitApp::OnExit(){
	return 0;
}
