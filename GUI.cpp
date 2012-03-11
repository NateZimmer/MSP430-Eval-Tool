// Nathan Zimmerman MSP430 GUI EVAL 
// Version 0.3
// Copyright Nathan Zimmerman: Not authorized for commercial use or self promotion in any way. 
// Strictly for debug and learning purposes. 

//Sorry for the mess. 


#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/bookctrl.h>
#include <wx/utils.h>
#include <iostream>

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
wxTimer *DI_TIMER;
DWORD x;
DWORD n;
char buf[20];

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
	wxStaticText *m_text_display;
	wxStaticText *port_selected_text; 
	wxButton *Red_LED_Button;
	wxButton *Green_LED_Button;
	wxCheckBox *CP10D, *CP11D, *CP12D, *CP13D, *CP14D, *CP15D, *CP16D, *CP17D, *CP20D,*CP21D,*CP22D,*CP23D,*CP24D,*CP25D;
	wxCheckBox *Timer_Start;
	wxCheckBox *port_as_input;
	wxCheckBox *port_pullup_enable; 
	wxChoice *DI_PORT_SELECT; 


	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	
	void OnPageChanged(wxBookCtrlEvent &event );
	void OnButton1( wxCommandEvent &event );
	void OnButton2( wxCommandEvent &event );
	void OnListBoxDoubleClick( wxCommandEvent &event );
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnButtonConnect(wxCommandEvent &event);
	void OnChecked(wxCommandEvent &event);
	void OnStartTimer(wxCommandEvent &event);
	void OnDITimer(wxTimerEvent& event);
	void DI_INPUT(wxCommandEvent &event);
	void On_DI_Check(wxCommandEvent &event);
	


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

const int FILE_QUIT = wxID_EXIT;
const int HELP_ABOUT = wxID_ABOUT;

// Attach the event handlers. Put this after MyFrame declaration.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BOOKCTRL_PAGE_CHANGED(BOOKCTRL, MyFrame::OnPageChanged)
EVT_TIMER (ID_DI_TIMER, MyFrame::OnDITimer)
EVT_CHECKBOX(ID_Start_Timer, MyFrame::OnStartTimer)
EVT_BUTTON(BUTTON1, MyFrame::OnButton1)
EVT_BUTTON(BUTTON2, MyFrame::OnButton2)
EVT_LISTBOX_DCLICK(LISTBOX1,MyFrame::OnListBoxDoubleClick)
EVT_MENU(FILE_QUIT, MyFrame::OnQuit)
EVT_CHECKBOX(CHECKEDBOX, MyFrame::OnChecked)
EVT_CHECKBOX(ID_DI_CHECK, MyFrame::On_DI_Check)
EVT_MENU(HELP_ABOUT, MyFrame::OnAbout)
EVT_BUTTON(CONNECTBUTTON, MyFrame::OnButtonConnect)
EVT_CHOICE(ID_DI_INPUT, MyFrame::DI_INPUT)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame( _("MSP430G2553 EVAL TOOL V0.3"), wxPoint(50, 50),
		wxSize(600, 350) ); //AP Title
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, -1, title, pos, size)
{
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

	(void)new wxStaticText(panel, wxID_ANY, _T("Welcome to the Digital Outputs Tab. *NOTE: must first be connected to use this tab. Set any GPIO HIGH/LOW from this tab. P1.1 and P1.2 are protected for USB Comm. \n\nWill set all ports to either 1 or 0, no dynamic cross tab support yet implemented: "),wxPoint(5,5),wxSize(500,65));
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

	CP11D->SetValue (true);
	CP12D->SetValue (true);
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
	book->AddPage(panel, _T("Serial COM"), false);

	panel = new wxPanel(book);
	book->AddPage(panel, _T("Other"), false);


}

void MyFrame::OnPageChanged(wxBookCtrlEvent& event )
{
	int selOld = event.GetOldSelection();

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
static unsigned int p1ren=port1_ren_global; 
static unsigned int p1dir= port1_dir_global; 
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
		}
	}	
	
	
		if(check_pullup ==1)
		{
			p1ren |=(1<<test1);
				if(p1ren !=port1_ren_global)
				{
					P1REN(p1ren,false);
					P1OUT(p1ren,false);
				    port1_ren_global = p1ren;
				}
		}
		if(check_pullup ==0)
		{
			p1ren &=~(1<<test1);
				if(p1ren != port1_ren_global)
				{
					P1REN(~p1ren,true);
					P1OUT(~p1ren,true);
					port1_ren_global = p1ren;
				}
		}
	
}

}

//////TimerFunction////////

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
	msg.Printf( _T("About.\n")
		_T("Welcome to %s"), wxVERSION_STRING);

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
		Timer_Start->Enable(true);

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
unsigned int port2_out = 0; 
static unsigned int port1_previous = 0;
static unsigned int port2_previous = 0; 


if((CP10D->GetValue()) ==1 )
port1_out |= (1<<0);

if((CP11D->GetValue()) ==1 )
port1_out |= (1<<1);

if((CP12D->GetValue()) ==1 )
port1_out |= (1<<2);

if((CP13D->GetValue()) ==1 )
port1_out |= (1<<3);

if((CP14D->GetValue()) ==1 )
port1_out |= (1<<4);

if((CP15D->GetValue()) ==1 )
port1_out |= (1<<5);

if((CP16D->GetValue()) ==1 )
port1_out |= (1<<6);

if((CP17D->GetValue()) ==1 )
port1_out |= (1<<7);

if((CP20D->GetValue()) ==1 )
port2_out |= (1<<0);

if((CP21D->GetValue()) ==1 )
port2_out |= (1<<1);

if((CP22D->GetValue()) ==1 )
port2_out |= (1<<2);

if((CP23D->GetValue()) ==1 )
port2_out |= (1<<3);

if((CP24D->GetValue()) ==1 )
port2_out |= (1<<4);

if((CP25D->GetValue()) ==1 )
port2_out |= (1<<5);


if(port1_previous != port1_out)
{
P1DIR(port1_out,false);
P1OUT(port1_out,false);
P1OUT(~port1_out,true);
port1_dir_global =port1_out;
}
if(port2_previous != port2_out)
{
P2DIR(port2_out,false);
P2OUT(port2_out,false);
P2OUT(~port2_out,true);
}
port1_previous = port1_out; 
port2_previous = port2_out; 



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
	to.ReadIntervalTimeout = 20;
	to.ReadTotalTimeoutConstant = 20;
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