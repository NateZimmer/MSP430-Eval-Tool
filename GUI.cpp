// Nathan Zimmerman MSP430 GUI EVAL 
// Version 0.4
// Copyright Nathan Zimmerman: Not authorized for commercial use or self promotion in any way. 
// Strictly for debug and learning purposes. 

//Sorry for the mess. Will be cleaned up when I finish


#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/bookctrl.h>
#include <wx/utils.h>
#include <iostream>

//File includes
#include <char_byte_to_hex.h>

//Global Variables
wxString comportnum;
HANDLE com_port =NULL;
int send_c = 0;
int send_d = 0;
bool connected = false; 
bool toggle_R = true; 
bool toggle_G = true; 
bool DO_mode = false;
unsigned int port1_out_global = 0; 
unsigned int port2_out_global = 0;
unsigned int port1_dir_global = 0;
unsigned int port2_dir_global = 0; 
unsigned int port1_ren_global = 0;
unsigned int port2_ren_global = 0; 


int port1ren = 0; 

wxString choices[] =
    {
        wxT("P1.0"), //0
        wxT("P1.1(Locked)"), //1
        wxT("P1.2(Locked)"), //2
        wxT("P1.3"), //3
        wxT("P1.4"), //4
		wxT("P1.5"), //5
		wxT("P1.6"), //6
		wxT("P1.7"), //7
		wxT("P2.0"), //8
		wxT("P2.1"), //9
		wxT("P2.2"), //10
		wxT("P2.3"), //11
		wxT("P2.4"), //12
		wxT("P2.5"), //13
		wxT("Select Port")
    };

wxString chipselect[] =
    {
        wxT("P1.0"), //0
        wxT("P1.3"), //1
        wxT("P1.4"), //2
		wxT("P2.0"), //3
		wxT("P2.1"), //4
		wxT("P2.2"), //5
		wxT("P2.3"), //6
		wxT("P2.4"), //7
		wxT("P2.5"), //8
		wxT("Chip Select")
    };

wxString bytes_to_send[] =
    {
        wxT("1"), //0
        wxT("2"), //1
        wxT("3"), //2
		wxT("4"), //3
		wxT("5"), //4
		wxT("6"), //5
		wxT("7"), //6
		wxT("8"), //7
		wxT("# of Bytes to Send")
    };

wxString baud_select[] =
    {
        wxT("1Mhz (unstable)"), //0
        wxT("500k (unstable)"), //1
        wxT("100k"), //2
		wxT("10k"), //3
		wxT("1k"), //4
		wxT("Select Baud")
    };


wxTimer *DI_TIMER;
DWORD x;
DWORD n;
char buf[20];
char tbuf[20];
//Global Functions

static unsigned OpenCom(wxString stuff, unsigned bps);

//Command Functions

void P1DIR(unsigned int port, bool set_zero);  
void P1OUT(unsigned int port, bool set_zero);
void P1REN(unsigned int port, bool set_zero);
void P1SEL(unsigned int port, bool set_zero);
void P1SEL2(unsigned int port, bool set_zero);
void P2DIR(unsigned int port, bool set_zero);
void P2OUT(unsigned int port, bool set_zero);
void P2REN(unsigned int port, bool set_zero);
void P2SEL(unsigned int port, bool set_zero);
void P2SEL2(unsigned int port, bool set_zero);
void PXIN();

//My App

class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
	wxBookCtrl *book;
	wxListBox  *listbox1;
	wxTextCtrl *textLog;
	wxTextCtrl *comport_txt;
	wxTextCtrl *recieved_D; 
	wxTextCtrl *port1DI, *port2DI,*portS; 
	wxStaticText *connection_status;
	wxStaticText *m_text_display,*status_text_spi;
	wxStaticText *port_selected_text; 
	wxButton *Red_LED_Button;
	wxButton *Green_LED_Button;
	wxCheckBox *CP10D, *CP11D, *CP12D, *CP13D, *CP14D, *CP15D, *CP16D, *CP17D, *CP20D,*CP21D,*CP22D,*CP23D,*CP24D,*CP25D;
	wxCheckBox *CP10O, *CP11O, *CP12O, *CP13O, *CP14O, *CP15O, *CP16O, *CP17O, *CP20O,*CP21O,*CP22O,*CP23O,*CP24O,*CP25O;
	wxCheckBox *Timer_Start;
	wxCheckBox *port_as_input;
	wxCheckBox *port_pullup_enable; 
	wxChoice *DI_PORT_SELECT; 
	wxChoice *select_baud, *select_bytes, *select_chip_select;
	wxButton *send_Packet;
	wxTextCtrl *send0, *send1, *send2, *send3, *send4, *send5, *send6, *send7, *recv0, *recv1, *recv2, *recv3, *recv4, *recv5, *recv6, *recv7;


	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	
	void OnPageChanged(wxBookCtrlEvent &event );
	void OnButton1( wxCommandEvent &event );
	void OnButton2( wxCommandEvent &event );
	void OnListBoxDoubleClick( wxCommandEvent &event );
	void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnButtonConnect(wxCommandEvent &event);
	void OnChecked(wxCommandEvent &event);
	void OnStartTimer(wxCommandEvent &event);
	void OnDITimer(wxTimerEvent& event);
	void DI_INPUT(wxCommandEvent &event);
	void On_DI_Check(wxCommandEvent &event);
	void On_send_Packet(wxCommandEvent &event);
	void On_select(wxCommandEvent &event);
	void On_chip(wxCommandEvent &event);
	


private:
	DECLARE_EVENT_TABLE()
};

// Declare some IDs. These are arbitrary.
const int ID_BOOK = 1000;
const int BOOKCTRL = 100;
const int BUTTON1 = 101;
const int BUTTON2 = 102;
const int LISTBOX1 = 103;
const int TEXTBOX1 = 104;
const int CHECKEDBOX = 231; 
const int CONNECTBUTTON = 107; 
const int ID_DI_TIMER = 240; 
const int ID_Start_Timer = 241; 
const int ID_DI_INPUT = 242; 
const int ID_DI_CHECK =243;
const int ID_send_Packet=244;
const int ID_select = 245; 
const int ID_chip=246;

const int FILE_QUIT = wxID_EXIT;
const int HELP_ABOUT = wxID_ABOUT;

// Attach the event handlers. Put this after MyFrame declaration.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BOOKCTRL_PAGE_CHANGED(BOOKCTRL, MyFrame::OnPageChanged)
EVT_TIMER (ID_DI_TIMER, MyFrame::OnDITimer)
EVT_CHECKBOX(ID_Start_Timer, MyFrame::OnStartTimer)
EVT_BUTTON(BUTTON1, MyFrame::OnButton1)
EVT_BUTTON(BUTTON2, MyFrame::OnButton2)
EVT_BUTTON(ID_send_Packet, MyFrame::On_send_Packet)
EVT_LISTBOX_DCLICK(LISTBOX1,MyFrame::OnListBoxDoubleClick)
EVT_MENU(FILE_QUIT, MyFrame::OnQuit)
EVT_CHECKBOX(CHECKEDBOX, MyFrame::OnChecked)
EVT_CHECKBOX(ID_DI_CHECK, MyFrame::On_DI_Check)
EVT_MENU(HELP_ABOUT, MyFrame::OnAbout)
EVT_BUTTON(CONNECTBUTTON, MyFrame::OnButtonConnect)
EVT_CHOICE(ID_DI_INPUT, MyFrame::DI_INPUT)
EVT_CHOICE(ID_select, MyFrame::On_select)
EVT_CHOICE(ID_chip, MyFrame::On_chip)
EVT_CLOSE(MyFrame::OnClose)

END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame( _("MSP430G2553 EVAL TOOL V0.4"), wxPoint(50, 50),
		wxSize(600, 400) ); //AP Title
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, -1, title, pos, size)
{
	int chkbx_height = 165; 
	DI_TIMER = new wxTimer(this, ID_DI_TIMER);
	SetIcon(wxICON(sample));
	wxMenu *fileMenu = new wxMenu;
	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(HELP_ABOUT, _T("&About...\tF1"),
		_T("Show about dialog"));
	fileMenu->Append(FILE_QUIT, _T("E&xit\tAlt-X"),
		_T("Quit this program"));
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(helpMenu, _T("&Help"));
	SetMenuBar(menuBar);
	CreateStatusBar(1);
	SetStatusText(_T("Developed By Nathan Zimmerman"), 0);


	book = new wxBookCtrl(this, BOOKCTRL);
	wxPanel *panel = new wxPanel(book);

	(void)new wxStaticText(panel, wxID_ANY, _T("Welcome to the MSP430 Eval Tool! Please connect your device. "),wxPoint(5,5),wxSize(450,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("Enter Comm Port # (Example: \"COM1\")"),wxPoint(15, 30), wxSize(250, 20));
	(void)new wxStaticText(panel, wxID_ANY, _T("Information: This tool is designed to function as a GUI to control specific port IO for the MSP430G2553 MCU. This GUI is meant to interface with the MSP430 Launchpad. Below are 2 example buttons that will toggle DO port 1.0 and 1.6. Leds on the launchpad should blink accordingly if set. "),wxPoint(5,110),wxSize(550,65));
	comport_txt = new wxTextCtrl(panel, wxID_ANY, _T(""), wxPoint(15, 50),wxSize(100, 25));
	recieved_D = new wxTextCtrl(panel, wxID_ANY, _T(""), wxPoint(350, 50),wxSize(100, 25));
	connection_status = new wxStaticText(panel, wxID_ANY, _T("Status: Not Connected : GUI Disconnected"),wxPoint(15, 80), wxSize(250, 20));
	m_text_display = new wxStaticText(panel, wxID_ANY, _T("Recieved RS232 Data:"),wxPoint(350, 30), wxSize(250, 20)); 
	Red_LED_Button = new wxButton( panel, BUTTON2,_T("Toggle Red LED"), wxPoint(220,185), wxSize(200,30) );
	Green_LED_Button = new wxButton( panel, BUTTON1,_T("Toggle Green LED"), wxPoint(10,185), wxSize(200,30) );

	Red_LED_Button->Enable(false);
	Green_LED_Button->Enable(false);

	new wxButton( panel, CONNECTBUTTON , _T("Connect"), wxPoint(120,50), wxSize(100,25) );

	book->AddPage(panel, _T("Connection"), true);

	panel = new wxPanel(book);

	(void)new wxStaticText(panel, wxID_ANY, _T("Digital Outputs Tab: Define any GPIO as an output else input. Set any GPIO output to 0/1. P1.1 and P1.2 are protected for USB Comm. \n\nDefine Ports as Digital Outputs: "),wxPoint(5,5),wxSize(500,65));
	CP10D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.0"),wxPoint(10, 80), wxSize(50, 20));
	CP11D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.1"),wxPoint(70, 80), wxSize(50, 20));
	CP12D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.2"),wxPoint(130, 80), wxSize(50, 20));
	CP13D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.3"),wxPoint(190, 80), wxSize(50, 20));
	CP14D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.4"),wxPoint(250, 80), wxSize(50, 20));
	CP15D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.5"),wxPoint(310, 80), wxSize(50, 20));
	CP16D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.6"),wxPoint(370, 80), wxSize(50, 20));
	CP17D = new wxCheckBox (panel, CHECKEDBOX, _T("P1.7"),wxPoint(430, 80), wxSize(50, 20));
	CP20D = new wxCheckBox (panel, CHECKEDBOX, _T("P2.0"),wxPoint(10, 110), wxSize(50, 20));
	CP21D = new wxCheckBox (panel, CHECKEDBOX, _T("P2.1"),wxPoint(70, 110), wxSize(50, 20));
	CP22D = new wxCheckBox (panel, CHECKEDBOX, _T("P2.2"),wxPoint(130, 110), wxSize(50, 20));
	CP23D = new wxCheckBox (panel, CHECKEDBOX, _T("P2.3"),wxPoint(190, 110), wxSize(50, 20));
	CP24D = new wxCheckBox (panel, CHECKEDBOX, _T("P2.4"),wxPoint(250, 110), wxSize(50, 20));
	CP25D = new wxCheckBox (panel, CHECKEDBOX, _T("P2.5"),wxPoint(310, 110), wxSize(50, 20));
	(void)new wxStaticText(panel, wxID_ANY, _T("Set Port Output to 0/1:"),wxPoint(5,140),wxSize(200,20));
	CP10O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.0"),wxPoint(10, chkbx_height), wxSize(50, 20));
	CP11O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.1"),wxPoint(70, chkbx_height), wxSize(50, 20));
	CP12O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.2"),wxPoint(130, chkbx_height), wxSize(50, 20));
	CP13O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.3"),wxPoint(190, chkbx_height), wxSize(50, 20));
	CP14O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.4"),wxPoint(250, chkbx_height), wxSize(50, 20));
	CP15O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.5"),wxPoint(310, chkbx_height), wxSize(50, 20));
	CP16O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.6"),wxPoint(370, chkbx_height), wxSize(50, 20));
	CP17O = new wxCheckBox (panel, CHECKEDBOX, _T("P1.7"),wxPoint(430, chkbx_height), wxSize(50, 20));
	CP20O = new wxCheckBox (panel, CHECKEDBOX, _T("P2.0"),wxPoint(10, chkbx_height+30), wxSize(50, 20));
	CP21O = new wxCheckBox (panel, CHECKEDBOX, _T("P2.1"),wxPoint(70, chkbx_height+30), wxSize(50, 20));
	CP22O = new wxCheckBox (panel, CHECKEDBOX, _T("P2.2"),wxPoint(130, chkbx_height+30), wxSize(50, 20));
	CP23O = new wxCheckBox (panel, CHECKEDBOX, _T("P2.3"),wxPoint(190, chkbx_height+30), wxSize(50, 20));
	CP24O = new wxCheckBox (panel, CHECKEDBOX, _T("P2.4"),wxPoint(250, chkbx_height+30), wxSize(50, 20));
	CP25O = new wxCheckBox (panel, CHECKEDBOX, _T("P2.5"),wxPoint(310, chkbx_height+30), wxSize(50, 20));

	CP11D->SetValue (true);
	CP12D->SetValue (true);
	CP11O->SetValue (true);
	CP12O->SetValue (true);
	CP10D->Enable(false);
	CP11D->Enable(false);
	CP12D->Enable(false);
	CP13D->Enable(false);
	CP14D->Enable(false);
	CP15D->Enable(false);
	CP16D->Enable(false);
	CP17D->Enable(false);
	CP20D->Enable(false);
	CP21D->Enable(false);
	CP22D->Enable(false);
	CP23D->Enable(false);
	CP24D->Enable(false);
	CP25D->Enable(false);
	CP10O->Enable(false);
	CP11O->Enable(false);
	CP12O->Enable(false);
	CP13O->Enable(false);
	CP14O->Enable(false);
	CP15O->Enable(false);
	CP16O->Enable(false);
	CP17O->Enable(false);
	CP20O->Enable(false);
	CP21O->Enable(false);
	CP22O->Enable(false);
	CP23O->Enable(false);
	CP24O->Enable(false);
	CP25O->Enable(false);

	book->AddPage(panel, _T("Digital Outputs"), false);
	panel = new wxPanel(book);
	
	(void)new wxStaticText(panel, wxID_ANY, _T("Digital Input Tab: Read 0/1 from any GPIO. Also has the capacity to set PD/PU."),wxPoint(5,5),wxSize(450,15));
	Timer_Start = new wxCheckBox (panel, ID_Start_Timer, _T("Start Active Reading:"),wxPoint(10, 30), wxSize(400, 20));
	Timer_Start->Enable(false);
	
	DI_PORT_SELECT = new wxChoice( panel, ID_DI_INPUT, wxPoint(10,60), wxSize(120,25), 15, choices );
	DI_PORT_SELECT->SetSelection(14);
	port_as_input = new wxCheckBox (panel,ID_DI_CHECK, _T("Define Port as Input"),wxPoint(150, 65), wxSize(150, 20));
	port_pullup_enable = new wxCheckBox (panel,ID_DI_CHECK, _T("Enable Internal Pull-Up"),wxPoint(300, 65), wxSize(150, 20));
	(void)new wxStaticText(panel, wxID_ANY, _T("Read Status of Port Selected:"),wxPoint(10,100),wxSize(180,15));
	portS = new wxTextCtrl(panel, wxID_ANY, _T("null"), wxPoint(200, 95),wxSize(50, 25));
	(void)new wxStaticText(panel, wxID_ANY, _T("Port 1 Read Status:"),wxPoint(10,160),wxSize(120,15));
	port1DI = new wxTextCtrl(panel, wxID_ANY, _T("null"), wxPoint(140, 155),wxSize(100, 25));
	(void)new wxStaticText(panel, wxID_ANY, _T("Port 2 Read Status:"),wxPoint(260,160),wxSize(120,15));
	port2DI = new wxTextCtrl(panel, wxID_ANY, _T("null"), wxPoint(390, 155),wxSize(100, 25));
	port_selected_text = new wxStaticText(panel, wxID_ANY, _T(""),wxPoint(10, 500), wxSize(250, 20));

	DI_PORT_SELECT->Enable(false);
	port_as_input->Enable(false);
	port_pullup_enable->Enable(false);
	portS->Enable(false);
	port1DI->Enable(false);
	port2DI->Enable(false);
	port_selected_text->Enable(false);


	book->AddPage(panel, _T("Digital Inputs"), false);

	panel = new wxPanel(book);
	book->AddPage(panel, _T("Analog Inputs"), false);

	panel = new wxPanel(book);
	unsigned int offset = 20;
	(void)new wxStaticText(panel, wxID_ANY, _T("SPI Master Tab: Hardware SPI ports are defined as follows: P1.5 -> SCLK, P1.6 --> MISO, P1.7 --> MOSI\n "),wxPoint(5,5),wxSize(550,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("Select Chip Select"),wxPoint(10,30),wxSize(120,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("Number Of Bytes To Send"),wxPoint(150,30),wxSize(150,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("Select Baud Rate"),wxPoint(310,30),wxSize(120,15));

	select_chip_select = new wxChoice( panel,ID_chip, wxPoint(10,30+offset), wxSize(120,25), 10, chipselect );
	select_chip_select->SetSelection(9);
	select_bytes = new wxChoice( panel,  ID_select, wxPoint(150,30+offset), wxSize(140,25), 9, bytes_to_send );
	select_bytes->SetSelection(8);
	select_baud = new wxChoice( panel,  wxID_ANY, wxPoint(310,30+offset), wxSize(120,25), 6, baud_select);
	select_baud->SetSelection(5);
	send_Packet = new wxButton( panel,ID_send_Packet,_T("Transmit Data Packet"), wxPoint(360,130+offset), wxSize(160,40) );
	(void)new wxStaticText(panel, wxID_ANY, _T("Send Buffer In Hex: \nMSB"),wxPoint(10,60+offset),wxSize(120,30));
	(void)new wxStaticText(panel, wxID_ANY, _T("LSB"),wxPoint(290,60+offset+15),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("7"),wxPoint(10,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("6"),wxPoint(50,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("5"),wxPoint(90,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("4"),wxPoint(130,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("3"),wxPoint(170,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("2"),wxPoint(210,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("1"),wxPoint(250,60+offset+30),wxSize(50,15));
	(void)new wxStaticText(panel, wxID_ANY, _T("0"),wxPoint(290,60+offset+30),wxSize(50,15));

	send7 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(10, 110+offset),wxSize(30, 25));
	send6 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(50, 110+offset),wxSize(30, 25));
	send5 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(90, 110+offset),wxSize(30, 25));
	send4 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(130, 110+offset),wxSize(30, 25));
	send3 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(170, 110+offset),wxSize(30, 25));
	send2 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(210, 110+offset),wxSize(30, 25));
	send1 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(250, 110+offset),wxSize(30, 25));
	send0 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(290, 110+offset),wxSize(30, 25));

	(void)new wxStaticText(panel, wxID_ANY, _T("Recieved Buffer In Hex: \nMSB"),wxPoint(10,140+offset),wxSize(200,35));

	recv7 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(10, 180+offset),wxSize(30, 25));
	recv6 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(50, 180+offset),wxSize(30, 25));
	recv5 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(90, 180+offset),wxSize(30, 25));
	recv4 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(130, 180+offset),wxSize(30, 25));
	recv3 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(170, 180+offset),wxSize(30, 25));
	recv2 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(210, 180+offset),wxSize(30, 25));
	recv1 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(250, 180+offset),wxSize(30, 25));
	recv0 = new wxTextCtrl(panel, wxID_ANY, _T("00"), wxPoint(290, 180+offset),wxSize(30, 25));
	
	send7->Enable(false);
	send6->Enable(false);
	send5->Enable(false);
	send4->Enable(false);
	send3->Enable(false);
	send2->Enable(false);
	send1->Enable(false);
	
	recv7->Enable(false);
	recv6->Enable(false);
	recv5->Enable(false);
	recv4->Enable(false);
	recv3->Enable(false);
	recv2->Enable(false);
	recv1->Enable(false);
	select_chip_select->Enable(false);
	select_bytes->Enable(false);
	select_baud->Enable(false);
	send_Packet->Enable(false);

	status_text_spi = new wxStaticText(panel, wxID_ANY, _T("Recieved RS232 Data:"),wxPoint(400, 210+500), wxSize(250, 20)); 






	book->AddPage(panel, _T("SPI Master COM"), false);

	panel = new wxPanel(book);
	book->AddPage(panel, _T("Other"), false);


} // end of gui

void MyFrame::On_chip(wxCommandEvent& WXUNUSED(event))
{
unsigned int chipselect_p = select_chip_select->GetSelection(); 
unsigned int p1dir = port1_dir_global; 
unsigned int p1out = port1_out_global;
unsigned int p2dir = port2_dir_global; 
unsigned int p2out = port2_out_global;

switch(chipselect_p)
{
	case 0:
		chipselect_p = 0x0;
		p1dir |= (1<<chipselect_p);
		P1DIR(p1dir,false);
		port1_dir_global =p1dir;
		p1out |= (1<<chipselect_p);
		P1OUT(p1out,false);	
		port1_out_global =p1out; 

		break;

	case 1:
		chipselect_p = 0x3;
		p1dir |= (1<<chipselect_p);
		P1DIR(p1dir,false);
		port1_dir_global =p1dir;
		p1out |= (1<<chipselect_p);
		P1OUT(p1out,false);	
		port1_out_global =p1out; 
		break;

	case 2:
		chipselect_p = 0x4;
		p1dir |= (1<<chipselect_p);
		P1DIR(p1dir,false);
		port1_dir_global =p1dir;
		p1out |= (1<<chipselect_p);
		P1OUT(p1out,false);	
		port1_out_global =p1out; 
		break;

	case 3:
		chipselect_p = (0x0);
		p2dir |= (1<<chipselect_p);
		P2DIR(p2dir,false);
		port2_dir_global =p2dir;
		p2out |= (1<<chipselect_p);
		P2OUT(p2out,false);	
		port2_out_global =p2out; 
		
		
		break;

	case 4:
		chipselect_p = (0x1);
		p2dir |= (1<<chipselect_p);
		P2DIR(p2dir,false);
		port2_dir_global =p2dir;
		p2out |= (1<<chipselect_p);
		P2OUT(p2out,false);	
		port2_out_global =p2out; 
		break;

	case 5:
		chipselect_p = (0x2);
		p2dir |= (1<<chipselect_p);
		P2DIR(p2dir,false);
		port2_dir_global =p2dir;
		p2out |= (1<<chipselect_p);
		P2OUT(p2out,false);	
		port2_out_global =p2out; 
		break;

	case 6:
		chipselect_p = (0x3);
		p2dir |= (1<<chipselect_p);
		P2DIR(p2dir,false);
		port2_dir_global =p2dir;
		p2out |= (1<<chipselect_p);
		P2OUT(p2out,false);	
		port2_out_global =p2out; 
		break;

	case 7:
		chipselect_p = (0x4);
		p2dir |= (1<<chipselect_p);
		P2DIR(p2dir,false);
		port2_dir_global =p2dir;
		p2out |= (1<<chipselect_p);
		P2OUT(p2out,false);	
		port2_out_global =p2out; 
		break;

	case 8:
		chipselect_p = (0x5);
		p2dir |= (1<<chipselect_p);
		P2DIR(p2dir,false);
		port2_dir_global =p2dir;
		p2out |= (1<<chipselect_p);
		P2OUT(p2out,false);	
		port2_out_global =p2out; 
		break;

}



}



void MyFrame::On_select(wxCommandEvent& WXUNUSED(event))
{
int select_byte_num =select_bytes->GetSelection();

switch(select_byte_num)
{
	case 0:
		send7->Enable(false), send6->Enable(false),send5->Enable(false),send4->Enable(false),send3->Enable(false),send2->Enable(false),send1->Enable(false);
		recv7->Enable(false);recv6->Enable(false);recv5->Enable(false);recv4->Enable(false);recv3->Enable(false);recv2->Enable(false);recv1->Enable(false);
		break;

	case 1:
		send7->Enable(false), send6->Enable(false),send5->Enable(false),send4->Enable(false),send3->Enable(false),send2->Enable(false),send1->Enable(true);
		recv7->Enable(false);recv6->Enable(false);recv5->Enable(false);recv4->Enable(false);recv3->Enable(false);recv2->Enable(false);recv1->Enable(true);
		break;

	case 2:
		send7->Enable(false), send6->Enable(false),send5->Enable(false),send4->Enable(false),send3->Enable(false),send2->Enable(true),send1->Enable(true);
		recv7->Enable(false);recv6->Enable(false);recv5->Enable(false);recv4->Enable(false);recv3->Enable(false);recv2->Enable(true);recv1->Enable(true);
		break;

	case 3:
		send7->Enable(false), send6->Enable(false),send5->Enable(false),send4->Enable(false),send3->Enable(true),send2->Enable(true),send1->Enable(true);
		recv7->Enable(false);recv6->Enable(false);recv5->Enable(false);recv4->Enable(false);recv3->Enable(true);recv2->Enable(true);recv1->Enable(true);
		break;

	case 4:
		send7->Enable(false), send6->Enable(false),send5->Enable(false),send4->Enable(true),send3->Enable(true),send2->Enable(true),send1->Enable(true);
		recv7->Enable(false);recv6->Enable(false);recv5->Enable(false);recv4->Enable(true);recv3->Enable(true);recv2->Enable(true);recv1->Enable(true);
		break;

	case 5:
		send7->Enable(false), send6->Enable(false),send5->Enable(true),send4->Enable(true),send3->Enable(true),send2->Enable(true),send1->Enable(true);
		recv7->Enable(false);recv6->Enable(false);recv5->Enable(true);recv4->Enable(true);recv3->Enable(true);recv2->Enable(true);recv1->Enable(true);
		break;

	case 6:
		send7->Enable(false), send6->Enable(true),send5->Enable(true),send4->Enable(true),send3->Enable(true),send2->Enable(true),send1->Enable(true);
		recv7->Enable(false);recv6->Enable(true);recv5->Enable(true);recv4->Enable(true);recv3->Enable(true);recv2->Enable(true);recv1->Enable(true);
		break;

	case 7:
		send7->Enable(true), send6->Enable(true),send5->Enable(true),send4->Enable(true),send3->Enable(true),send2->Enable(true),send1->Enable(true);
		recv7->Enable(true);recv6->Enable(true);recv5->Enable(true);recv4->Enable(true);recv3->Enable(true);recv2->Enable(true);recv1->Enable(true);
		break;

}



}



//////////ONCLOSE//////////

void MyFrame::On_send_Packet(wxCommandEvent& WXUNUSED(event))
{
wxString stringey;
unsigned int send_bytes[10];
unsigned int recv_bytes[10];
unsigned int *send_bytes_ptr;
unsigned int spi_command = 0xE;
unsigned int crystal = 0x8;
unsigned int chipselect = 0x16; 
unsigned int chipselect_p = select_chip_select->GetSelection(); 
unsigned int baudrate = select_baud->GetSelection();
unsigned int send_spi = 0x32; 
unsigned int data_length=select_bytes->GetSelection();
unsigned int i=0; 
send_bytes_ptr=send_bytes;

switch(chipselect_p)
{
	case 0:
		chipselect_p = 0x0;
		break;

	case 1:
		chipselect_p = 0x3;
		break;

	case 2:
		chipselect_p = 0x4;
		break;

	case 3:
		chipselect_p = (0x0 + 0x8);
		break;

	case 4:
		chipselect_p = (0x1 + 0x8);
		break;

	case 5:
		chipselect_p = (0x2 + 0x8);
		break;

	case 6:
		chipselect_p = (0x3 + 0x8);
		break;

	case 7:
		chipselect_p = (0x4 + 0x8);
		break;

	case 8:
		chipselect_p = (0x5 + 0x8);
		break;

}

stringey=send0->GetValue();
send_bytes[0] = Byte_To_HEX(stringey.mb_str());
stringey=send1->GetValue();
send_bytes[1] = Byte_To_HEX(stringey.mb_str());
stringey=send2->GetValue();
send_bytes[2] = Byte_To_HEX(stringey.mb_str());
stringey=send3->GetValue();
send_bytes[3] = Byte_To_HEX(stringey.mb_str());
stringey=send4->GetValue();
send_bytes[4] = Byte_To_HEX(stringey.mb_str());
stringey=send5->GetValue();
send_bytes[5] = Byte_To_HEX(stringey.mb_str());
stringey=send6->GetValue();
send_bytes[6] = Byte_To_HEX(stringey.mb_str());
stringey=send7->GetValue();
send_bytes[7] = Byte_To_HEX(stringey.mb_str());



i=0x0;

if(data_length==8)
{
data_length=0;
}

wxMilliSleep(1);
fflush(stdout);
wxMilliSleep(1);
CloseHandle(com_port);
wxMilliSleep(1);
OpenCom(comportnum, 9600);
wxMilliSleep(1);

while((data_length+1)>i)
{
wxMilliSleep(8);
WriteFile(com_port,  &(spi_command) , 1, &x, NULL); // Send Command for SPI function
wxMilliSleep(8);	
WriteFile(com_port,  &(i), 1, &x, NULL); //data1 -> case0;
wxMilliSleep(8);
WriteFile(com_port,  &(send_bytes[data_length - i]), 1, &x, NULL); //data2 -> 0 data;
i++;
}

wxMilliSleep(8);
WriteFile(com_port,  &(spi_command) , 1, &x, NULL); // Send Command for SPI function
wxMilliSleep(8);
WriteFile(com_port,  &(crystal), 1, &x, NULL); //data 1 -> case0x8;
wxMilliSleep(8);
crystal = 0x3;
WriteFile(com_port,  &(baudrate), 1, &x, NULL); //case0;


wxMilliSleep(8);
WriteFile(com_port,  &(spi_command) , 1, &x, NULL); // Send Command for SPI function
wxMilliSleep(8);
WriteFile(com_port,  &(chipselect), 1, &x, NULL); //data 1 -> case0x8;
wxMilliSleep(8);
crystal = 0x3;
WriteFile(com_port,  &(chipselect_p), 1, &x, NULL); //case0;


wxMilliSleep(8);
WriteFile(com_port,  &(spi_command) , 1, &x, NULL); // Send Command for SPI function
wxMilliSleep(8);
WriteFile(com_port,  &(send_spi), 1, &x, NULL); //data 1 -> case0x8;
wxMilliSleep(8);
crystal = 0x3;
WriteFile(com_port,  &(crystal), 1, &x, NULL); //case0;

buf[0]=0;buf[1]=0;buf[2]=0;buf[3]=0;buf[4]=0;
wxMilliSleep(1);
ReadFile(com_port, tbuf, sizeof(tbuf), &n, NULL);



recv0->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length]));

if((data_length -1)>=0)
recv1->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length -1]));

if((data_length -2)>=0)
recv2->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length-2]));


if((data_length -3)>=0)
recv3->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length-3]));

if((data_length -4)>=0)
recv4->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length-4]));

if((data_length -5)>=0)
recv5->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length-5]));

if((data_length -6)>=0)
recv6->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length-6]));

if((data_length -7)>=0)
recv7->SetLabel(wxString::Format(wxT("%x"), tbuf[data_length-7]));



status_text_spi->SetLabel(wxString::Format(wxT("%x"), tbuf[1]));


}


void MyFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
	DI_TIMER->Stop();
	wxMilliSleep(10);
	fflush(stdout);
	CloseHandle(com_port);
	Destroy();
}

///ON PAGE CHANG  EVENTS///////////

void MyFrame::OnPageChanged(wxBookCtrlEvent& event )
{
	int selOld = event.GetOldSelection();
	int cursel = event.GetSelection();


	if( cursel==1)
	{
	
		if((port1_dir_global & (1<<0))==(1<<0))
		CP10D->SetValue(true);
		else
		CP10D->SetValue(false);
		if((port1_dir_global & (1<<3))==(1<<3))
		CP13D->SetValue(true);
		else
		CP13D->SetValue(false);
		if((port1_dir_global & (1<<4))==(1<<4))
		CP14D->SetValue(true);
		else
		CP14D->SetValue(false);
		if((port1_dir_global & (1<<5))==(1<<5))
		CP15D->SetValue(true);
		else
		CP15D->SetValue(false);
		if((port1_dir_global & (1<<6))==(1<<6))
		CP16D->SetValue(true);
		else
		CP16D->SetValue(false);
		if((port1_dir_global & (1<<7))==(1<<7))
		CP17D->SetValue(true);
		else
		CP17D->SetValue(false);
		if((port2_dir_global & (1<<0))==(1<<0))
		CP20D->SetValue(true);
		else
		CP20D->SetValue(false);
		if((port2_dir_global & (1<<1))==(1<<1))
		CP21D->SetValue(true);
		else
		CP21D->SetValue(false);
		if((port2_dir_global & (1<<2))==(1<<2))
		CP22D->SetValue(true);
		else
		CP22D->SetValue(false);
		if((port2_dir_global & (1<<3))==(1<<3))
		CP23D->SetValue(true);
		else
		CP23D->SetValue(false);
		if((port2_dir_global & (1<<4))==(1<<4))
		CP24D->SetValue(true);
		else
		CP24D->SetValue(false);
		if((port2_dir_global & (1<<5))==(1<<5))
		CP25D->SetValue(true);
		else
		CP25D->SetValue(false);

		if((port1_out_global & (1<<0))==(1<<0))
		CP10O->SetValue(true);
		else
		CP10O->SetValue(false);
		if((port1_out_global & (1<<3))==(1<<3))
		CP13O->SetValue(true);
		else
		CP13O->SetValue(false);
		if((port1_out_global & (1<<4))==(1<<4))
		CP14O->SetValue(true);
		else
		CP14O->SetValue(false);
		if((port1_out_global & (1<<5))==(1<<5))
		CP15O->SetValue(true);
		else
		CP15O->SetValue(false);
		if((port1_out_global & (1<<6))==(1<<6))
		CP16O->SetValue(true);
		else
		CP16O->SetValue(false);
		if((port1_out_global & (1<<7))==(1<<7))
		CP17O->SetValue(true);
		else
		CP17O->SetValue(false);
		if((port2_out_global & (1<<0))==(1<<0))
		CP20O->SetValue(true);
		else
		CP20O->SetValue(false);
		if((port2_out_global & (1<<1))==(1<<1))
		CP21O->SetValue(true);
		else
		CP21O->SetValue(false);
		if((port2_out_global & (1<<2))==(1<<2))
		CP22O->SetValue(true);
		else
		CP22O->SetValue(false);
		if((port2_out_global & (1<<3))==(1<<3))
		CP23O->SetValue(true);
		else
		CP23O->SetValue(false);
		if((port2_out_global & (1<<4))==(1<<4))
		CP24O->SetValue(true);
		else
		CP24O->SetValue(false);
		if((port2_out_global & (1<<5))==(1<<5))
		CP25O->SetValue(true);
		else
		CP25O->SetValue(false);






	}


	if ( selOld == 2 )
	{
	if((Timer_Start->GetValue()) ==1 )
	{
    DI_TIMER->Stop(); 
	portS->Enable(false);
	port1DI->Enable(false);
	port2DI->Enable(false);
	Timer_Start->SetValue (false);
		wxMessageBox( _("Stoped Timer"),
			_("Comm Port"),
			wxOK | wxICON_INFORMATION, this );
	}
	}
}

//////////////CURRENTFUNCITON////////////////////////////////

void MyFrame::DI_INPUT(wxCommandEvent& WXUNUSED(event))
{
unsigned int test1 =DI_PORT_SELECT->GetSelection();
unsigned int shift1 = 0;

if (test1<=7)
{
	shift1= (1<<test1);
		if((port1_dir_global & shift1)==shift1)
		{	
			port_as_input->SetValue (false);	
		}
		else
		{
			port_as_input->SetValue (true);
		}
		if((port1_ren_global & shift1)==shift1)
		{
			port_pullup_enable->SetValue (true);
		}
		else
		{
			port_pullup_enable->SetValue (false);
		}
}

if (test1>=8)
{
	shift1= (1<<(test1 - 8));
}

port_selected_text->SetLabel(wxString::Format(wxT("%d"), test1));


}

//////


void MyFrame::On_DI_Check(wxCommandEvent& WXUNUSED(event))
{
unsigned int p1ren=port1_ren_global; 
unsigned int p1dir= port1_dir_global; 
unsigned int p1out=port1_out_global; 
int check_input = port_as_input->GetValue(); 
int check_pullup =port_pullup_enable->GetValue(); 
int test1 =DI_PORT_SELECT->GetSelection();


if(test1<=7)
{
	if(check_input ==1)
	{
		p1dir &=~(1<<test1);
		if(port1_dir_global != p1dir )
		{
			P1DIR(~p1dir,true);
			port_selected_text->SetLabel(wxString::Format(wxT("%d"), p1dir));
			port1_dir_global =p1dir; 
		}
	}	
	
	
		if(check_pullup ==1)
		{
			p1ren |=(1<<test1);
			p1out |=(1<<test1); 
				if((p1ren !=port1_ren_global)|(p1out !=port1_out_global))
				{
					P1REN(p1ren,false);
					P1OUT(p1ren,false);
				    port1_ren_global= p1ren;
					port1_out_global=p1out;
				}
		}
		if(check_pullup ==0)
		{
			p1ren &=~(1<<test1);
			p1out &=~(1<<test1);
				if(p1ren != port1_ren_global)
				{
					P1REN(~p1ren,true);
					P1OUT(~p1out,true);
					port1_ren_global = p1ren;
					port1_out_global =p1out;
				}
		}
	
}

}

//////TimerOutput Binary Function////////

void MyFrame::OnDITimer(wxTimerEvent& WXUNUSED(event))
{
int databit0 = 0; 
int databit1 = 0;
int c=0;
char txtbuf[10];
char databit0b[14];
char databit1b[14];
char *data0bptr;
char *data1bptr;
char *txtbuffptr;
int selected_p =0;
selected_p =DI_PORT_SELECT->GetSelection();

PXIN();
ReadFile(com_port, buf, sizeof(buf), &n, NULL);

databit0 = (char)buf[0];
databit1 = (char)buf[1];
while(c<8)
{
if((databit0 & (1<<(7-c)))==(1<<(7-c)))
{
databit0b[c] ='1';
}
else
{
databit0b[c] ='0';
}
c++;
}
databit0b[c]=0; 
c=0; 
data0bptr = databit0b;
while(c<8)
{
if((databit1 & (1<<(7-c)))==(1<<(7-c)))
{
databit1b[c] = '1';
}
else
{
databit1b[c] = '0';
}
c++;
}
databit1b[c]=0; 
data1bptr = databit1b;

port1DI->SetLabel(wxString::Format(wxT("%s"),data0bptr));
port2DI->SetLabel(wxString::Format(wxT("%s"),data1bptr));
	if (selected_p<=7)
	{
		sprintf(txtbuf,"%c",databit0b[(7-selected_p)]);
		txtbuffptr=txtbuf;
		portS->SetLabel(wxString::Format(wxT("%s"), txtbuffptr));
	}

}

void MyFrame::OnStartTimer(wxCommandEvent & WXUNUSED(event))
{
if((Timer_Start->GetValue()) ==1 )
{
DI_TIMER->Start(100);
portS->Enable(true);
port1DI->Enable(true);
port2DI->Enable(true);
}
if((Timer_Start->GetValue()) ==0 )
{
DI_TIMER->Stop();
portS->Enable(false);
port1DI->Enable(false);
port2DI->Enable(false);
}

}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	DI_TIMER->Stop();
	wxMilliSleep(10);
	fflush(stdout);
	CloseHandle(com_port);
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf(_T("A simple GUI created by Nathan Zimmerman for hardware/software development with the msp430g2553. For source, visit https://github.com/NateZimmer/MSP430-Eval-Tool "));

	wxMessageBox(msg, _T("About My Program"),
		wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnButton1(wxCommandEvent& WXUNUSED(event)) // Green LED
{	 
	toggle_G =!toggle_G;
	comportnum =comport_txt->GetValue();
	P1DIR((1<<6),false);
	P1OUT((1<<6),toggle_G);
	ReadFile(com_port, buf, sizeof(buf), &n, NULL);
	recieved_D ->SetValue(wxString::Format(wxT("%s"), buf));
}

void MyFrame::OnButton2(wxCommandEvent& WXUNUSED(event))
{
	toggle_R = !toggle_R; 
	comportnum =comport_txt->GetValue();
	P1DIR(0x1,false);
	P1OUT((0x1),toggle_R);
	ReadFile(com_port, buf, sizeof(buf), &n, NULL);
	recieved_D ->SetValue(wxString::Format(wxT("%s"), buf));
}

void MyFrame::OnListBoxDoubleClick( wxCommandEvent &event )
{
	*textLog << "ListBox double click string is: \n";
	*textLog << event.GetString();
	*textLog << "\n";
}

void MyFrame::OnButtonConnect(wxCommandEvent& WXUNUSED(event))
{
	comportnum =comport_txt->GetValue();
	send_c = 0x1;
	if(connected == true)
		{
		fflush(stdout);
		CloseHandle(com_port);
		wxMilliSleep(10);
		}
	OpenCom(comportnum, 9600);
	WriteFile(com_port,  &send_c, 1, &x, NULL);
	wxMilliSleep(10);
	send_c = 0x00; 
	WriteFile(com_port,  &send_c, 1, &x, NULL);
	wxMilliSleep(10);
	WriteFile(com_port,  &send_c, 1, &x, NULL);
	wxMilliSleep(1);
	ReadFile(com_port, buf, sizeof(buf), &n, NULL);
	//
	if(buf[0] =='C')
	{
		connection_status -> SetLabel(wxString::Format(wxT("%s"), "Status: Now Connected"));
		Red_LED_Button->Enable(true);
		Green_LED_Button->Enable(true);
		CP10D->Enable(true);
		CP11D->Enable(false);
		CP12D->Enable(false);
		CP13D->Enable(true);
		CP14D->Enable(true);
		CP15D->Enable(true);
		CP16D->Enable(true);
		CP17D->Enable(true);
		CP20D->Enable(true);
		CP21D->Enable(true);
		CP22D->Enable(true);
		CP23D->Enable(true);
		CP24D->Enable(true);
		CP25D->Enable(true);

		CP10O->Enable(true);
		CP11O->Enable(false);
		CP12O->Enable(false);
		CP13O->Enable(true);
		CP14O->Enable(true);
		CP15O->Enable(true);
		CP16O->Enable(true);
		CP17O->Enable(true);
		CP20O->Enable(true);
		CP21O->Enable(true);
		CP22O->Enable(true);
		CP23O->Enable(true);
		CP24O->Enable(true);
		CP25O->Enable(true);
		Timer_Start->Enable(true);
		select_chip_select->Enable(true);
		select_bytes->Enable(true);
		select_baud->Enable(true);
		send_Packet->Enable(true);


		DI_PORT_SELECT->Enable(true);
		port_as_input->Enable(true);
		port_pullup_enable->Enable(true);
		port_selected_text->Enable(true);
		//button_toggle_g->Enable(true);


	}
	else
	{
		recieved_D ->SetValue(wxString::Format(wxT("%s"), buf));
		wxMessageBox( _("Error: Failed Connection"),
			_("Comm Port"),
			wxOK | wxICON_INFORMATION, this );
		fflush(stdout);
		CloseHandle(com_port);
	}

	//m_text_display -> SetLabel(wxString::Format(wxT("%s"), buf));
	recieved_D ->SetValue(wxString::Format(wxT("%s"), buf));
	//fflush(stdout);
	//m_text_display -> SetLabel(wxString::Format(wxT("%s"), buf));
	//CloseHandle(com_port);

	connected = true; 
}

void MyFrame::OnChecked(wxCommandEvent& WXUNUSED(event))
{
unsigned int port1_out = 0;
unsigned int port1_dir = 0;
unsigned int port2_out = 0;
unsigned int port2_dir = 0; 

if((CP10O->GetValue()) ==1 )
port1_out |= (1<<0);

if((CP11O->GetValue()) ==1 )
port1_out |= (1<<1);

if((CP12O->GetValue()) ==1 )
port1_out |= (1<<2);

if((CP13O->GetValue()) ==1 )
port1_out |= (1<<3);

if((CP14O->GetValue()) ==1 )
port1_out |= (1<<4);

if((CP15O->GetValue()) ==1 )
port1_out |= (1<<5);

if((CP16O->GetValue()) ==1 )
port1_out |= (1<<6);

if((CP17O->GetValue()) ==1 )
port1_out |= (1<<7);

if((CP20O->GetValue()) ==1 )
port2_out |= (1<<0);

if((CP21O->GetValue()) ==1 )
port2_out |= (1<<1);

if((CP22O->GetValue()) ==1 )
port2_out |= (1<<2);

if((CP23O->GetValue()) ==1 )
port2_out |= (1<<3);

if((CP24O->GetValue()) ==1 )
port2_out |= (1<<4);

if((CP25O->GetValue()) ==1 )
port2_out |= (1<<5);

//////////End Out Reads/////////

if((CP10D->GetValue()) ==1 )
port1_dir |= (1<<0);

if((CP11D->GetValue()) ==1 )
port1_dir |= (1<<1);

if((CP12D->GetValue()) ==1 )
port1_dir |= (1<<2);

if((CP13D->GetValue()) ==1 )
port1_dir |= (1<<3);

if((CP14D->GetValue()) ==1 )
port1_dir |= (1<<4);

if((CP15D->GetValue()) ==1 )
port1_dir |= (1<<5);

if((CP16D->GetValue()) ==1 )
port1_dir |= (1<<6);

if((CP17D->GetValue()) ==1 )
port1_dir |= (1<<7);

if((CP20D->GetValue()) ==1 )
port2_dir |= (1<<0);

if((CP21D->GetValue()) ==1 )
port2_dir |= (1<<1);

if((CP22D->GetValue()) ==1 )
port2_dir |= (1<<2);

if((CP23D->GetValue()) ==1 )
port2_dir |= (1<<3);

if((CP24D->GetValue()) ==1 )
port2_dir |= (1<<4);

if((CP25D->GetValue()) ==1 )
port2_dir |= (1<<5);

/////End DIR Reads/////

if(port1_dir_global != port1_dir)
{
P1DIR(port1_dir,false); // Set 1s
P1DIR(~port1_dir,true); // Set 0s
port1_dir_global = port1_dir; 
}

if(port2_dir_global != port2_dir)
{
P2DIR(port2_dir,false); // Set 1s
P2DIR(~port2_dir,true); // Set 0s
port2_dir_global = port2_dir; 
}

if(port1_out_global != port1_out)
{
P1OUT(port1_out,false); //Set 1s
P1OUT(~port1_out,true); //Set 0s
port1_out_global =port1_out;
}
if(port2_out_global != port2_out)
{
P2OUT(port2_out,false); //Set 1s
P2OUT(~port2_out,true); //Set 0s
port2_out_global = port2_out; 
}




}


void P1DIR(unsigned int port, bool set_zero)
{
	int pstatus = 0; 
	unsigned int null = 0x2;
	wxMilliSleep(5);
	pstatus =WriteFile(com_port,  &(null) , 1, &x, NULL);
	if( pstatus == 0 )
	{
		fflush(stdout);
		CloseHandle(com_port);
		wxMessageBox( _("Error: Could not write"),
		_("Comm Port"),wxOK | wxICON_INFORMATION );
	}
	
	
	wxMilliSleep(5);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(5);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(5);
}

void P1OUT(unsigned int port, bool set_zero)
{
unsigned int null = 0x3;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P1REN(unsigned int port, bool set_zero)
{
unsigned int null = 0x4;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P1SEL(unsigned int port, bool set_zero)
{
unsigned int null = 0x5;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P1SEL2(unsigned int port, bool set_zero)
{
unsigned int null = 0x6;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P2DIR(unsigned int port, bool set_zero)
{
unsigned int null = 0x7;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P2OUT(unsigned int port, bool set_zero)
{
unsigned int null = 0x8;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P2REN(unsigned int port, bool set_zero)
{
unsigned int null = 0x9;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P2SEL(unsigned int port, bool set_zero)
{
unsigned int null = 0xA;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void P2SEL2(unsigned int port, bool set_zero)
{
unsigned int null = 0xB;
	wxMilliSleep(4);
	WriteFile(com_port,  &(null) , 1, &x, NULL);
	wxMilliSleep(10);
	if (set_zero ==true)
	{
		null = 0x1;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	else
	{
		null = 0x00;
		WriteFile(com_port, &(null), 1, &x, NULL);
	}
	wxMilliSleep(10);
	WriteFile(com_port,  &(port), 1, &x, NULL);
	wxMilliSleep(10);
}

void PXIN()
{
unsigned int null = 0xC;
wxMilliSleep(4);
WriteFile(com_port,  &(null) , 1, &x, NULL);
wxMilliSleep(10);
WriteFile(com_port,  &(null) , 1, &x, NULL);
wxMilliSleep(10);
WriteFile(com_port,  &(null) , 1, &x, NULL);
}


static unsigned OpenCom(wxString stuff, unsigned bps)
{
	int pstatus=0; 

	if((com_port = CreateFile("\\\\.\\" +stuff, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))==0)
	{
		fflush(stdout);
		CloseHandle(com_port);
		wxMessageBox( _("Error: Could not create"),
		_("Comm Port"),wxOK | wxICON_INFORMATION  );
		
	}

	DCB dcb;
	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = bps;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.fParity = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fBinary = TRUE;

	pstatus = SetCommState(com_port, &dcb);
	if( pstatus == 0 )
	{
		fflush(stdout);
		CloseHandle(com_port);
		wxMessageBox( _("Error: Could not set comm state"),
		_("Comm Port"),wxOK | wxICON_INFORMATION );
	}



	COMMTIMEOUTS to;
	to.ReadIntervalTimeout = 40;
	to.ReadTotalTimeoutConstant = 40;
	to.ReadTotalTimeoutMultiplier = 1;
	to.WriteTotalTimeoutMultiplier = 1;
	to.WriteTotalTimeoutConstant = 10;

	pstatus = SetCommTimeouts(com_port, &to);
	if( pstatus == 0 )
	{
		fflush(stdout);
		CloseHandle(com_port);
		wxMessageBox( _("Error: Could not set comm timeout"),
		_("Comm Port"),wxOK | wxICON_INFORMATION );
	}

	SetCommMask(com_port, 0);
	if( pstatus == 0 )
		{
			fflush(stdout);
			CloseHandle(com_port);
			wxMessageBox( _("Error: Could not set comm mask"),
			_("Comm Port"),wxOK | wxICON_INFORMATION );
		}


	return 0;

}

void digital_output_mode()
{
	if(DO_mode == false)
	{
		send_c = 0xFF; 
		//OpenCom(comportnum, 9600);

		WriteFile(com_port, &send_c , 1, &x, NULL);

		ReadFile(com_port, buf, sizeof(buf), &n, NULL);

		if(buf[0] =='R')
		{
		}
		else
		{
			wxMessageBox( _("Error: Failed Connection"),
				_("Comm Port"),
				wxOK | wxICON_INFORMATION);
		}
		DO_mode = true;
		send_c = 0; 

	}
}