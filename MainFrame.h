﻿/***************************************************************
 * Name:      MainFrame.h
 * Purpose:   Defines Application Frame
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "cartographer/frame.h"

//(*Headers(MainFrame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/frame.h>
#include <wx/combobox.h>
#include <wx/statusbr.h>
//*)

#include <libpq-fe.h>

class MainFrame: public wxFrame
{
	public:

		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();

	private:
		cartographer::Frame *Cartographer;
		int images_[11];

		/* "Быстрые" точки */
		static const int count_ = 9;
		wxString names_[count_];
		int z_[count_];
		cartographer::coord coords_[count_];

		int big_font_;
		int small_font_;

		PGconn *pg_conn_;
		PGresult *pg_res_;

		void PgConnect();

		void Test(); /* Тестирование функций Картографера */

		void OnMapPaint(double z, int width, int height);

		void DrawImage(int id, const cartographer::coord &pt);
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
		void OnChoice1Select(wxCommandEvent& event);
		void OnZoomInButtonClick(wxCommandEvent& event);
		void OnZoomOutButtonClick(wxCommandEvent& event);
		void OnAnchorButtonClick(wxCommandEvent& event);
		//*)

		//(*Identifiers(MainFrame)
		static const long ID_COMBOBOX1;
		static const long ID_CHOICE1;
		static const long ID_PANEL2;
		static const long ID_PANEL1;
		static const long ID_MENU_QUIT;
		static const long ID_MENU_ABOUT;
		static const long ID_STATUSBAR1;
		static const long ID_ZOOMIN;
		static const long ID_ZOOMOUT;
		static const long ID_ANCHOR;
		static const long ID_TOOLBAR1;
		//*)

		//(*Declarations(MainFrame)
		wxToolBar* ToolBar1;
		wxToolBarToolBase* ToolBarItem3;
		wxPanel* Panel1;
		wxToolBarToolBase* ToolBarItem1;
		wxStatusBar* StatusBar1;
		wxComboBox* ComboBox1;
		wxPanel* Panel2;
		wxFlexGridSizer* FlexGridSizer1;
		wxChoice* Choice1;
		wxToolBarToolBase* ToolBarItem2;
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
