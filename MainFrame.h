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

class MainFrame: public wxFrame, my::employer
{
	public:

		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();

	private:
		cartographer::Painter *Cartographer;

        my::worker::ptr WiFiScan_worker_;
        int WiFi_sock_;
        unsigned char WiFi_mac_[6];
		PGresult *WiFi_data_;
		mutex WiFi_mutex_;

        void WiFiScanProc(my::worker::ptr this_worker);
        void UpdateWiFiData();

		/* Шрифты */
		int big_font_;
		int small_font_;

		/* Изображения */
		int green_mark16_id_;
		int red_mark16_id_;
		int yellow_mark16_id_;


		PGconn *pg_conn_;
		mutex pg_mutex_;

		bool PgConnect();


		void Test(); /* Тестирование функций Картографера */

		void OnMapPaint(double z, int width, int height);

		void DrawImage(int id, const cartographer::coord &pt, double alpha = 1.0);
        void DrawSimpleCircle(const cartographer::point &pos,
            double r, double line_width, const cartographer::color &line_color,
            const cartographer::color &fill_color);

		void DrawCircle(const cartographer::coord &pt,
			double r, double line_width, const cartographer::color &line_color,
			const cartographer::color &fill_color);

		double DrawPath(const cartographer::coord &pt1,
			const cartographer::coord &pt2,
			double line_width, const cartographer::color &line_color,
			double *p_azimuth = NULL, double *p_rev_azimuth = NULL);

		cartographer::coord DrawPath(const cartographer::coord &pt,
			double azimuth, double distance,
			double line_width, const cartographer::color &line_color,
			double *p_rev_azimuth = NULL);

		//(*Handlers(MainFrame)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnComboBox1Select(wxCommandEvent& event);
		void OnZoomInButtonClick(wxCommandEvent& event);
		void OnZoomOutButtonClick(wxCommandEvent& event);
		void OnAnchorButtonClick(wxCommandEvent& event);
		void OnWiFiScanButtonClicked(wxCommandEvent& event);
		//*)

		//(*Identifiers(MainFrame)
		static const long ID_COMBOBOX1;
		static const long ID_PANEL2;
		static const long ID_PANEL1;
		static const long ID_MENU_QUIT;
		static const long ID_MENU_ABOUT;
		static const long ID_STATUSBAR1;
		static const long ID_ZOOMIN;
		static const long ID_ZOOMOUT;
		static const long ID_ANCHOR;
		static const long ID_WIFISCAN;
		static const long ID_TOOLBAR1;
		//*)

		//(*Declarations(MainFrame)
		wxToolBarToolBase* ToolBarItem4;
		wxToolBar* ToolBar1;
		wxToolBarToolBase* ToolBarItem3;
		wxPanel* Panel1;
		wxToolBarToolBase* ToolBarItem1;
		wxStatusBar* StatusBar1;
		wxComboBox* ComboBox1;
		wxPanel* Panel2;
		wxFlexGridSizer* FlexGridSizer1;
		wxToolBarToolBase* ToolBarItem2;
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
