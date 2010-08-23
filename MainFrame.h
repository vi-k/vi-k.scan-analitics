/***************************************************************
 * Name:      MainFrame.h
 * Purpose:   Defines Application Frame
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "cartographer/Painter.h"

#include <mylib.h>

//(*Headers(MainFrame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/combobox.h>
#include <wx/statusbr.h>
//*)

#include <libpq-fe.h>

class MainFrame: my::employer, public wxFrame
{
	public:
		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();

	private:
		cartographer::Painter *Cartographer;

		enum { NoAnchor, GpsAnchor, WiFiAnchor };
		int Anchor_;

		/* WiFiScan */
        my::worker::ptr WiFiScan_worker_;
        int WiFi_sock_;
        unsigned char WiFi_mac_[6];
		PGresult *WiFi_data_;
        int WiFi_min_power_, WiFi_max_power_;
        //cartographer::coord min_pt_, max_pt_;
		mutex WiFi_mutex_;

        void WiFiScanProc(my::worker::ptr this_worker);
        void UpdateWiFiData();

		/* Шрифты */
		int big_font_;
		int small_font_;

		/* Изображения */
		int gps_tracker_id_;
		int green_mark16_id_;
		int red_mark16_id_;
		int yellow_mark16_id_;

		/* Postgre */
		PGconn *pg_conn_;
		mutex pg_mutex_;

		bool PgConnect();

		/* GPS */
		my::worker::ptr GpsTracker_worker_;
		cartographer::coord Gps_pt_;
		double Gps_speed_;
		double Gps_azimuth_;
		double Gps_altitude_;
		bool Gps_ok_;
		mutex Gps_mutex_;

		void GpsTrackerProc(my::worker::ptr this_worker);


		void CheckerProc(my::worker::ptr this_worker);


		void Test(); /* Тестирование функций Картографера */

		void OnMapPaint(double z, const cartographer::size &screen_size);
		void StatusHandler(std::wstring &str);

		void UpdateButtons();

		//(*Handlers(MainFrame)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnComboBox1Select(wxCommandEvent& event);
		void OnZoomInButtonClick(wxCommandEvent& event);
		void OnZoomOutButtonClick(wxCommandEvent& event);
		void OnWiFiScanButtonClicked(wxCommandEvent& event);
		void OnGpsAnchorClick(wxCommandEvent& event);
		void OnGpsTrackerClick(wxCommandEvent& event);
		void OnWiFiAnchorClick(wxCommandEvent& event);
		//*)

		void OnIdle(wxIdleEvent& event);

		//(*Identifiers(MainFrame)
		static const long ID_COMBOBOX1;
		static const long ID_PANEL2;
		static const long ID_PANEL1;
		static const long ID_MENU_QUIT;
		static const long ID_MENU_ABOUT;
		static const long ID_STATUSBAR1;
		static const long ID_ZOOMIN;
		static const long ID_ZOOMOUT;
		static const long ID_GPSTRACKER;
		static const long ID_GPSANCHOR;
		static const long ID_WIFISCAN;
		static const long ID_WIFIANCHOR;
		static const long ID_TOOLBAR1;
		//*)

		//(*Declarations(MainFrame)
		wxToolBarToolBase* ToolBarItem4;
		wxToolBar* ToolBar1;
		wxToolBarToolBase* ToolBarItem3;
		wxPanel* Panel1;
		wxToolBarToolBase* ToolBarItem6;
		wxToolBarToolBase* ToolBarItem1;
		wxStatusBar* StatusBar1;
		wxComboBox* ComboBox1;
		wxToolBarToolBase* ToolBarItem5;
		wxPanel* Panel2;
		wxFlexGridSizer* FlexGridSizer1;
		wxToolBarToolBase* ToolBarItem2;
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
