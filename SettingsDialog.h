#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//(*Headers(SettingsDialog)
#include <wx/dialog.h>
class wxPanel;
class wxSpinEvent;
class wxStdDialogButtonSizer;
class wxTextCtrl;
class wxNotebookEvent;
class wxStaticLine;
class wxNotebook;
class wxRadioButton;
class wxStaticText;
class wxFlexGridSizer;
class wxSpinCtrl;
class wxGridSizer;
//*)

class SettingsDialog: public wxDialog
{
	public:

		SettingsDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SettingsDialog();

		//(*Declarations(SettingsDialog)
		wxSpinCtrl* MaxPowerSpin;
		wxNotebook* Notebook1;
		wxRadioButton* LoadFromServerRB;
		wxSpinCtrl* MinPowerSpin;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
		wxStaticText* StaticText8;
		wxTextCtrl* ServerAddrText;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxPanel* Panel3;
		wxRadioButton* ColorsRelativeRB;
		wxStaticText* StaticText5;
		wxStaticLine* StaticLine1;
		wxPanel* Panel2;
		wxRadioButton* ColorsAbsolutRB;
		wxStaticText* StaticText4;
		wxRadioButton* CacheOnlyRB;
		//*)

		static void Open(wxWindow* parent);

	protected:

		//(*Identifiers(SettingsDialog)
		static const long ID_STATICTEXT3;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT1;
		static const long ID_PANEL3;
		static const long ID_PANEL1;
		static const long ID_STATICTEXT4;
		static const long ID_RADIOBUTTON3;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT6;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT8;
		static const long ID_RADIOBUTTON4;
		static const long ID_PANEL2;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(SettingsDialog)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
