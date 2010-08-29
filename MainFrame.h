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
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include <libpq-fe.h>

class macaddr
{
private:
	unsigned char mac_[6];

public:
	macaddr()
	{
		mac_[0] = 0; mac_[1] = 0; mac_[2] = 0;
		mac_[3] = 0; mac_[4] = 0; mac_[5] = 0;
	}

	macaddr(unsigned char mac0, unsigned char mac1, unsigned char mac2,
		unsigned char mac3, unsigned char mac4, unsigned char mac5)
	{
		mac_[0] = mac0; mac_[1] = mac1; mac_[2] = mac2;
		mac_[3] = mac3; mac_[4] = mac4; mac_[5] = mac5;
	}

	macaddr(unsigned char *mac)
	{
		mac_[0] = mac[0]; mac_[1] = mac[1]; mac_[2] = mac[2];
		mac_[3] = mac[3]; mac_[4] = mac[4]; mac_[5] = mac[5];
	}

	inline unsigned char* mac()
		{ return mac_; }

	inline const unsigned char* mac() const
		{ return mac_; }

	bool empty() const
	{
		return mac_[0] == 0 && mac_[1] == 0 && mac_[2] == 0
			&& mac_[3] == 0 && mac_[4] == 0 && mac_[5] == 0;
	}

	std::string to_string() const
	{
		char buf[18];

		snprintf( buf, sizeof(buf) / sizeof(*buf),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			mac_[0], mac_[1], mac_[2],
			mac_[3], mac_[4], mac_[5] );

		return std::string(buf);
	}

	std::wstring to_wstring() const
	{
		wchar_t buf[18];

		swprintf( buf, sizeof(buf) / sizeof(*buf),
			L"%02X:%02X:%02X:%02X:%02X:%02X",
			mac_[0], mac_[1], mac_[2],
			mac_[3], mac_[4], mac_[5] );

		return std::wstring(buf);
	}
};

class waiter
{
private:
	mutex mutex_;
	condition_variable cond_;

public:
	waiter() {}

	mutex& get_mutex()
		{ return mutex_; }

	void sleep()
	{
		unique_lock<mutex> lock(mutex_);
		cond_.wait(lock);
	}

	template<class Lock>
	void sleep(Lock &lock)
	{
		cond_.wait(lock);
	}

	void wake_up()
	{
		unique_lock<mutex> lock(mutex_);
		cond_.notify_all();
	}

	template<class Lock>
	void wake_up(Lock &)
	{
		cond_.notify_all();
	}
};

class myMessageBoxEvent: public wxEvent
{
private:
    wxString message_;
    wxString caption_;
    int style_;
    wxWindow *parent_;
    int x_;
    int y_;
    int *p_ret_;
    waiter *p_waiter_;

public:
    myMessageBoxEvent(wxEventType event_type)
		: wxEvent(wxID_ANY, event_type) {}

    myMessageBoxEvent(wxEventType event_type,
		const wxString &message, const wxString &caption,
		int style, wxWindow *parent, int x, int y)
        : wxEvent(wxID_ANY, event_type)
        , message_(message.c_str()) /* Копируем строки, а не */
        , caption_(caption.c_str()) /* создаём на них ссылки */
        , style_(style)
        , parent_(parent)
        , x_(x)
        , y_(y)
		, p_ret_(NULL)
		, p_waiter_(NULL) {}

	virtual wxEvent *Clone() const
	{
		return new myMessageBoxEvent(*this);
	}

    wxString GetMessage() const
		{ return message_; }

    wxString GetCaption() const
		{ return caption_; }

    int GetStyle() const
		{ return style_; }

    wxWindow* GetParent() const
		{ return parent_; }

    int GetX() const
		{ return x_; }

    int GetY() const
		{ return x_; }

	int* GetRetPtr() const
		{ return p_ret_; }

	void SetRetPtr(int *p_ret)
		{ p_ret_ = p_ret; }

	waiter* GetWaiterPtr() const
		{ return p_waiter_; }

	void SetWaiterPtr(waiter *p_waiter)
		{ p_waiter_ = p_waiter; }

	void Return(int ret)
	{
		if (p_ret_)
			*p_ret_ = ret;

		if (p_waiter_)
			p_waiter_->wake_up();
	}
};

typedef void (wxEvtHandler::*myMessageBoxEventFunction)(myMessageBoxEvent&);
#define myMessageBoxEventHandler(func) \
	wxEVENT_HANDLER_CAST(myMessageBoxEventFunction, func)

#define MY_EVT(event, func) \
    wx__DECLARE_EVT0(event, myMessageBoxEventHandler(func))

class MainFrame: my::employer, public wxFrame
{
	public:
		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();

		void ReloadSettings();

	private:
		wxToolBar* MainToolBar;

		cartographer::Painter *Cartographer;

		void OnMapChange(wxCommandEvent& event);

		enum { NoAnchor, GpsAnchor, WiFiAnchor };
		int Anchor_;

		/* WiFiScan */
        my::worker::ptr WiFiScan_worker_;
        macaddr WiFi_mac_;
		PGresult *WiFi_data_;
		bool WiFi_relative_mode_;
        int WiFi_min_power_, WiFi_max_power_;
        int WiFi_min_power_abs_, WiFi_max_power_abs_;
        cartographer::coord WiFi_max_power_pt_;
        //cartographer::coord min_pt_, max_pt_;
        bool WiFi_ok_;
		mutex WiFi_mutex_;

        void WiFiScanProc(my::worker::ptr this_worker);
        void UpdateWiFiData(const macaddr &mac);

		/* Шрифты */
		int big_font_;
		int small_font_;

		/* Изображения */
		int gps_tracker_id_;
		int green_mark_id_;
		int red_mark_id_;
		int yellow_mark_id_;

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
		bool Gps_test_;
		mutex Gps_mutex_;
		std::wstring Gps_buf_;

		void GpsTrackerProc(my::worker::ptr this_worker);


		/*
			Отправка сообщений
		*/

		/* Выдать сообщение на экран и дождаться результата */
		int myMessageBox(const wxString &message,
			const wxString &caption = "Message",
			int style = wxOK, wxWindow *parent = NULL,
			int x = wxDefaultCoord, int y = wxDefaultCoord);

		/* Выдать сообщение на экран и продолжить, не дожидаясь результата */
		void myMessageBoxDelayed(const wxString &message,
			const wxString &caption, int style, wxWindow *parent, int x, int y);


		void CheckerProc(my::worker::ptr this_worker);

		void Test(); /* Тестирование функций Картографера */

		void OnMapPaint(double z, const cartographer::size &screen_size);
		void StatusHandler(std::wstring &str);

		void UpdateButtons();

		//(*Handlers(MainFrame)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnSettings(wxCommandEvent& event);
		void OnZoomIn(wxCommandEvent& event);
		void OnZoomOut(wxCommandEvent& event);
		void OnGpsTracker(wxCommandEvent& event);
		void OnGpsAnchor(wxCommandEvent& event);
		void OnWiFiScan(wxCommandEvent& event);
		void OnWiFiAnchor(wxCommandEvent& event);
		//*)

		void OnMyMessageBox(myMessageBoxEvent& event);
		void OnIdle(wxIdleEvent& event);

		//(*Identifiers(MainFrame)
		static const long ID_PANEL1;
		static const long ID_SETTINGS;
		static const long ID_QUIT;
		static const long ID_MENUMAPS;
		static const long ID_ZOOMIN;
		static const long ID_ZOOMOUT;
		static const long ID_GPSTRACKER;
		static const long ID_GPSANCHOR;
		static const long ID_WIFISCAN;
		static const long ID_WIFIANCHOR;
		static const long ID_ABOUT;
		static const long ID_STATUSBAR1;
		//*)

		//(*Declarations(MainFrame)
		wxMenuItem* MenuSettings;
		wxMenuItem* MenuMapsNull;
		wxMenuItem* MenuZoomIn;
		wxMenuItem* MenuWifiScan;
		wxPanel* Panel1;
		wxMenuItem* MenuZoomOut;
		wxMenuItem* MenuWiFiAnchor;
		wxMenu* MenuMaps;
		wxStatusBar* StatusBar1;
		wxMenuItem* MenuGpsTracker;
		wxMenuItem* MenuGpsAnchor;
		wxFlexGridSizer* FlexGridSizer1;
		wxMenu* MenuView;
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
