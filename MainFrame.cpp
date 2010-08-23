/***************************************************************
 * Name:      MainFrame.cpp
 * Purpose:   Code for Application Frame
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#include "MainFrame.h"

extern my::log main_log;
extern my::log debug_log;
extern wxFileConfig *MyConfig;

#include <string>
#include <sstream>

//(*InternalHeaders(MainFrame)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include "images/gps_tracker.c"
#include "images/green_mark16.c"
#include "images/red_mark16.c"
#include "images/yellow_mark16.c"

//#include <wx/filename.h>

//(*IdInit(MainFrame)
const long MainFrame::ID_COMBOBOX1 = wxNewId();
const long MainFrame::ID_PANEL2 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_MENU_QUIT = wxNewId();
const long MainFrame::ID_MENU_ABOUT = wxNewId();
const long MainFrame::ID_STATUSBAR1 = wxNewId();
const long MainFrame::ID_ZOOMIN = wxNewId();
const long MainFrame::ID_ZOOMOUT = wxNewId();
const long MainFrame::ID_GPSTRACKER = wxNewId();
const long MainFrame::ID_GPSANCHOR = wxNewId();
const long MainFrame::ID_WIFISCAN = wxNewId();
const long MainFrame::ID_WIFIANCHOR = wxNewId();
const long MainFrame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	EVT_IDLE(MainFrame::OnIdle)
	//(*EventTable(MainFrame)
	//*)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent,wxWindowID id)
	: my::employer("MainFrame_employer")
	, Cartographer(0)
	, Anchor_(NoAnchor)
	, WiFi_sock_(0)
	, WiFi_data_(NULL)
	, WiFi_min_power_(-100)
	, WiFi_max_power_(0)
	, MY_MUTEX_DEF(WiFi_mutex_,true)
	, big_font_(0)
	, small_font_(0)
	, gps_tracker_id_(0)
	, green_mark16_id_(0)
	, red_mark16_id_(0)
	, yellow_mark16_id_(0)
	, pg_conn_(NULL)
	, MY_MUTEX_DEF(pg_mutex_,true)
	, Gps_speed_(0.0)
	, Gps_azimuth_(0.0)
	, Gps_altitude_(0.0)
	, Gps_ok_(false)
	, MY_MUTEX_DEF(Gps_mutex_,true)
{
	MY_REGISTER_THREAD("Main");

	#undef _
	#define _(s) (L##s)

	//(*Initialize(MainFrame)
	wxMenuItem* MenuItem2;
	wxMenuItem* MenuItem1;
	wxMenu* Menu1;
	wxMenuBar* MenuBar1;
	wxMenu* Menu2;

	Create(parent, wxID_ANY, _("Scan Analitics"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(626,293));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(1);
	Panel2 = new wxPanel(this, ID_PANEL2, wxDefaultPosition, wxSize(616,61), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	ComboBox1 = new wxComboBox(Panel2, ID_COMBOBOX1, wxEmptyString, wxPoint(8,8), wxSize(208,24), 0, 0, wxCB_READONLY|wxCB_DROPDOWN, wxDefaultValidator, _T("ID_COMBOBOX1"));
	FlexGridSizer1->Add(Panel2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(616,331), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	MenuBar1 = new wxMenuBar();
	Menu1 = new wxMenu();
	MenuItem1 = new wxMenuItem(Menu1, ID_MENU_QUIT, _("Выход\tAlt-F4"), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(MenuItem1);
	MenuBar1->Append(Menu1, _("Файл"));
	Menu2 = new wxMenu();
	MenuItem2 = new wxMenuItem(Menu2, ID_MENU_ABOUT, _("О программе...\tF1"), wxEmptyString, wxITEM_NORMAL);
	Menu2->Append(MenuItem2);
	MenuBar1->Append(Menu2, _("Помощь"));
	SetMenuBar(MenuBar1);
	StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
	int __wxStatusBarWidths_1[1] = { -1 };
	int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
	StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
	StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
	SetStatusBar(StatusBar1);
	ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
	ToolBarItem1 = ToolBar1->AddTool(ID_ZOOMIN, _("ZoomIn"), wxBitmap(wxImage(_T("images/zoom-in-32.png"))), wxNullBitmap, wxITEM_NORMAL, _("Увеличить"), wxEmptyString);
	ToolBarItem2 = ToolBar1->AddTool(ID_ZOOMOUT, _("ZoomOut"), wxBitmap(wxImage(_T("images/zoom-out-32.png"))), wxNullBitmap, wxITEM_NORMAL, _("Уменьшить"), wxEmptyString);
	ToolBar1->AddSeparator();
	ToolBarItem3 = ToolBar1->AddTool(ID_GPSTRACKER, _("GpsTracker"), wxBitmap(wxImage(_T("images/gps_tracker.png"))), wxNullBitmap, wxITEM_CHECK, _("Gps"), wxEmptyString);
	ToolBarItem4 = ToolBar1->AddTool(ID_GPSANCHOR, _("GpsAnchor"), wxBitmap(wxImage(_T("images/ylw-pushpin32.png"))), wxNullBitmap, wxITEM_CHECK, _("Следить за Gps"), wxEmptyString);
	ToolBar1->AddSeparator();
	ToolBarItem5 = ToolBar1->AddTool(ID_WIFISCAN, _("WiFiScan"), wxBitmap(wxImage(_T("images/wifi.png"))), wxNullBitmap, wxITEM_CHECK, _("WiFi"), wxEmptyString);
	ToolBarItem6 = ToolBar1->AddTool(ID_WIFIANCHOR, _("WiFiAnchor"), wxBitmap(wxImage(_T("images/ylw-pushpin32.png"))), wxNullBitmap, wxITEM_CHECK, _("Следить за WiFi"), wxEmptyString);
	ToolBar1->Realize();
	SetToolBar(ToolBar1);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&MainFrame::OnComboBox1Select);
	Connect(ID_MENU_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnQuit);
	Connect(ID_MENU_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnAbout);
	Connect(ID_ZOOMIN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnZoomInButtonClick);
	Connect(ID_ZOOMOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnZoomOutButtonClick);
	Connect(ID_GPSTRACKER,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnGpsTrackerClick);
	Connect(ID_GPSANCHOR,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnGpsAnchorClick);
	Connect(ID_WIFISCAN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnWiFiScanButtonClicked);
	Connect(ID_WIFIANCHOR,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnWiFiAnchorClick);
	//*)

	setlocale(LC_NUMERIC, "C");

	{
		wxIcon FrameIcon;
		FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("images/cartographer.png"))));
		SetIcon(FrameIcon);
	}

	{
		int w, h;
		bool maximized;

		MyConfig->Read(L"/MainFrame/Width", &w, 400);
		MyConfig->Read(L"/MainFrame/Height", &h, 400);
		MyConfig->Read(L"/MainFrame/Maximized", &maximized, true);

		SetClientSize(w, h);
		Maximize(maximized);
	}

	Show(true);

	/* Создание Картографа на месте Panel1 */
	{
		delete Panel1;

		std::wstring error;

		bool only_cache
			= MyConfig->ReadBool(L"/Cartographer/only_cache", false);

		if (!only_cache)
		{
			wxString str = MyConfig->Read(L"/Cartographer/server_addr", L"");
			try
			{
				Cartographer = new cartographer::Painter(this,
					(const wchar_t*)str.c_str());
			}
			catch (my::exception &e)
			{
				/* Не удалось создать Картограф - возможно
					не получилось соединиться с сервером */
				error = e.message();
				only_cache = true;
			}
		}

		if (only_cache)
		{
			try
			{
				Cartographer = new cartographer::Painter(this, L"cache");
			}
			catch (my::exception &e)
			{
				if (error.empty())
					throw e;
				else
					throw my::exception(error);
			}
		}

		FlexGridSizer1->Add(Cartographer, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
		SetSizer(FlexGridSizer1);

		if (!error.empty())
		{
			wxMessageBox(L"Не удалось запустить Картограф"
				L" с заданными настройками. Картограф запущен"
				L" в режиме работы с кэшем.\n\nТекст ошибки:\n\n"
				+ error, L"Ошибка", wxOK | wxICON_ERROR,
				this);
		}

	} /* Создание Картографа */

	/* Создаём шрифты */
	big_font_ = Cartographer->CreateFont(
		wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );
	small_font_ = Cartographer->CreateFont(
		wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );

	/* Загружаем список карт */
	int maps_count = Cartographer->GetMapsCount();
	for( int i = 0; i < maps_count; ++i)
	{
		cartographer::map_info map = Cartographer->GetMapInfo(i);
		ComboBox1->Append(map.name);
	}

	/* Текущая карта */
	cartographer::map_info map = Cartographer->GetActiveMapInfo();
	ComboBox1->SetValue(map.name);

	/* Изображения */
	gps_tracker_id_ = Cartographer->LoadImageFromC(gps_tracker);
	Cartographer->SetImageCentralPoint(gps_tracker_id_, 15.5, 19.0);

	green_mark16_id_ = Cartographer->LoadImageFromC(green_mark16);
	red_mark16_id_ = Cartographer->LoadImageFromC(red_mark16);
	yellow_mark16_id_ = Cartographer->LoadImageFromC(yellow_mark16);


	/* Запускаем собственную прорисовку */
	Cartographer->SetPainter(
		boost::bind(&MainFrame::OnMapPaint, this, _1, _2));

	Cartographer->SetStatusHandler(
		boost::bind(&MainFrame::StatusHandler, this, _1));

	Cartographer->MoveTo(13,
		cartographer::DMSToDD( 48,28,48.77, 135,4,19.04 ));

	memset(WiFi_mac_, 0, sizeof(WiFi_mac_));
	WiFiScan_worker_ = new_worker( L"WiFiScan_worker");

	GpsTracker_worker_ = new_worker( L"GpsTracker_worker");

	/*
	WiFi_mac_[0] = 0;
	WiFi_mac_[1] = 0x10;
	WiFi_mac_[2] = 0xE7;
	WiFi_mac_[3] = 0xA4;
	WiFi_mac_[4] = 0x46;
	WiFi_mac_[5] = 0x9D;
	-*/

	UpdateWiFiData();

	//int a = PQisthreadsafe();

	boost::thread( boost::bind(
		&MainFrame::CheckerProc, this, new_worker( L"Checker_worker")) );
}

MainFrame::~MainFrame()
{
	/* Остановка картографера обязательно должна быть выполнена
		до удаления всех объектов, использующихся в обработчике OnMapPaint */
	Cartographer->Stop();


	lets_finish();

	if (WiFi_sock_)
		close(WiFi_sock_);

	dismiss(WiFiScan_worker_);
	dismiss(GpsTracker_worker_);

	#ifndef NDEBUG
	debug_wait_for_finish(L"MainFrame", posix_time::seconds(5));
	#endif

	wait_for_finish();

	if (WiFi_data_)
		PQclear(WiFi_data_);

	PQfinish(pg_conn_);

	//(*Destroy(MainFrame)
	//*)

	{
		bool maximized = IsMaximized();

		if (!maximized)
		{
			wxSize sz = GetClientSize();
			MyConfig->Write(L"/MainFrame/Width", sz.GetWidth());
			MyConfig->Write(L"/MainFrame/Height", sz.GetHeight());
		}

		MyConfig->Write(L"/MainFrame/Maximized", maximized);
	}
}

bool MainFrame::PgConnect()
{
	unique_lock<mutex> l(pg_mutex_);

	if (pg_conn_)
	{
		if (PQstatus(pg_conn_) == CONNECTION_OK)
			return true;

		PQfinish(pg_conn_);
		pg_conn_ = 0;
	}

	pg_conn_ = PQconnectdb(
		"hostaddr=127.0.0.1"
		" dbname=radioscan"
		" user=radioscan"
		" password=123");

	if (PQstatus(pg_conn_) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection to database failed: %s",
			PQerrorMessage(pg_conn_));
		PQfinish(pg_conn_);
		pg_conn_ = 0;
		return false;
	}

	return true;
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
	Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox( L"About...");
}

void MainFrame::OnComboBox1Select(wxCommandEvent& event)
{
	std::wstring str = (const wchar_t *)ComboBox1->GetValue().c_str();
	Cartographer->SetActiveMapByName(str);
}

void MainFrame::OnMapPaint(double z, const cartographer::size &screen_size)
{
	/*-
	int fields = PQnfields(pg_res_);
	for (i = 0; i < fields; i++)
		printf("%-15s ", PQfname(res, i));
	printf("\n\n");
	-*/

	if (z > 10.0)
	{
		unique_lock<mutex> lock(WiFi_mutex_, boost::try_to_lock);

		if (lock.owns_lock() && WiFi_data_)
		{
			int min_power = WiFi_min_power_;
			int max_power = WiFi_max_power_;
			int delta_power = max_power - min_power;

			if (delta_power == 0)
			{
				delta_power = 1;
				--min_power;
			}

			for (int i = 0; i < PQntuples(WiFi_data_); i++)
			{
				//char *mac = PQgetvalue(WiFi_data_, i, 0);
				char *power_s = PQgetvalue(WiFi_data_, i, 3);
				char *lat_s = PQgetvalue(WiFi_data_, i, 5);
				char *lon_s = PQgetvalue(WiFi_data_, i, 6);

				int power;
				cartographer::coord pt;

				sscanf(power_s, "%d", &power);
				sscanf(lat_s, "%lf", &pt.lat);
				sscanf(lon_s, "%lf", &pt.lon);

				double power_k = (power - min_power) / (double)delta_power;

				if (power_k < 0.0)
					power_k = 0.0;

				if (power_k > 1.0)
					power_k = 1.0;

				/*-
				if (power_k < 0.5)
				{
					Cartographer->DrawImage(green_mark16_id_, pt);
					Cartographer->DrawImage(yellow_mark16_id_, pt, 2.0 * power_k);
				}
				else
				{
					DrawImage(yellow_mark16_id_, pt);
					DrawImage(red_mark16_id_, pt, 2.0 * (power_k - 0.5));
				}
				-*/

				double red_k = power_k < 0.5 ? 1.0 : 2.0 * (1.0 - power_k);
				double green_k = power_k < 0.5 ? 2.0 * power_k : 1.0;

				Cartographer->DrawSimpleCircle(
					Cartographer->CoordToScreen(pt), 5.0, 2.0,
					cartographer::color(red_k, green_k, 0.0),
					cartographer::color(red_k, green_k, 0.0, 0.3));

				/*-
				pos.y += 5.0;

				std::wstring str = my::str::to_wstring(mac);
				cartographer::size sz = Cartographer->DrawText(small_font_,
					str, pos, cartographer::color(1.0, 1.0, 0.0),
					cartographer::ratio(0.5, 0.0), cartographer::ratio(1.0, 1.0));

				pos.y += sz.height;

				std::wstringstream out;
				out << L'(' << power << L')';
				Cartographer->DrawText(small_font_,
					out.str(), pos, cartographer::color(1.0, 1.0, 0.0),
					cartographer::ratio(0.5, 0.0), cartographer::ratio(1.0, 1.0));
				-*/
			}
		}
	}


	{
		unique_lock<mutex> lock(Gps_mutex_);

		if (Gps_ok_ && worked(GpsTracker_worker_))
		{
			cartographer::coord pt = Gps_pt_;
			double angle = Gps_azimuth_;
			lock.unlock();

			Cartographer->DrawImage(gps_tracker_id_, pt, 1.0, 1.0, angle);
		}
	}
}

void MainFrame::StatusHandler(std::wstring &str)
{
	debug_log << L"StatusHandler()" << debug_log;

	str += L" | GPS: ";

	if ( !worked(GpsTracker_worker_) )
		str += L"отключен";
	else
	{
		unique_lock<mutex> lock(Gps_mutex_);

		if (!Gps_ok_)
			str+= L"ошибка";
		else
		{
			wchar_t buf[400];

			int lat_sign, lon_sign;
			int lat_d, lon_d;
			int lat_m, lon_m;
			double lat_s, lon_s;

			DDToDMS( Gps_pt_,
				&lat_sign, &lat_d, &lat_m, &lat_s,
				&lon_sign, &lon_d, &lon_m, &lon_s );

			__swprintf(buf, sizeof(buf)/sizeof(*buf),
				L"%s%d°%02d\'%05.2f\"  %s%d°%02d\'%05.2f\""
				L"  %0.2fкм/ч  %0.1f°  %0.1fм",
				lat_sign < 0 ? L"-" : L"", lat_d, lat_m, lat_s,
				lon_sign < 0 ? L"-" : L"", lon_d, lon_m, lon_s,
				Gps_speed_, Gps_azimuth_, Gps_altitude_);

			str += buf;
		}
	}
}

void MainFrame::OnZoomInButtonClick(wxCommandEvent& event)
{
	Cartographer->ZoomIn();
}

void MainFrame::OnZoomOutButtonClick(wxCommandEvent& event)
{
	Cartographer->ZoomOut();
}

void MainFrame::OnWiFiScanButtonClicked(wxCommandEvent& event)
{
	unique_lock<mutex> lock(WiFi_mutex_);

	if (WiFiScan_worker_.use_count() != 2)
	{
		close(WiFi_sock_);
		ToolBar1->ToggleTool(ID_WIFISCAN, false);
	}
	else
	{
		struct sockaddr_un remote;

		if ((WiFi_sock_ = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		{
			WiFi_sock_ = 0;
			lock.unlock();

			wxMessageBox(L"Error in socket()", L"Error", wxOK | wxICON_ERROR);
			ToolBar1->ToggleTool(ID_WIFISCAN, false);
			return;
		}

		remote.sun_family = AF_UNIX;
		strcpy(remote.sun_path, "/tmp/wifiscan.sock");
		size_t len = strlen(remote.sun_path) + sizeof(remote.sun_family);

		if (connect(WiFi_sock_, (struct sockaddr *)&remote, len) == -1)
		{
			close(WiFi_sock_);
			WiFi_sock_ = 0;
			lock.unlock();

			wxMessageBox(L"Error in connect()", L"Error", wxOK | wxICON_ERROR);
			ToolBar1->ToggleTool(ID_WIFISCAN, false);
			return;
		}

		boost::thread( boost::bind(
			&MainFrame::WiFiScanProc, this, WiFiScan_worker_) );
	}
}

void MainFrame::WiFiScanProc(my::worker::ptr this_worker)
{
	MY_REGISTER_THREAD("WiFiScan");

	while (!finish())
	{
		unsigned char buf[100];

		int sz = recv(WiFi_sock_, buf, 6, 0);

		if (sz == -1)
			break;

		if (sz != 6)
			continue;

		{
			unique_lock<mutex> l(WiFi_mutex_);
			memcpy(WiFi_mac_, buf, 6);
		}

		UpdateWiFiData();
	}

	close(WiFi_sock_);
	WiFi_sock_ = 0;
}

void MainFrame::UpdateWiFiData()
{
	if (!PgConnect())
		return;

	wchar_t title[100] = { L"Scan Analitics" };

	unsigned char wifi_mac[6];

	{
		unique_lock<mutex> lock(WiFi_mutex_);

		if (WiFi_data_)
		{
			PQclear(WiFi_data_);
			WiFi_data_ = NULL;
		}

		memcpy(wifi_mac, WiFi_mac_, 6);
	}

	if (memcmp(wifi_mac, "\0\0\0\0\0\0", 6) != 0)
	{
		unique_lock<mutex> l(pg_mutex_);

		swprintf( title, sizeof(title) / sizeof(*title),
			L"Scan Analitics (%02X:%02X:%02X:%02X:%02X:%02X)",
			wifi_mac[0], wifi_mac[1], wifi_mac[2],
			wifi_mac[3], wifi_mac[4], wifi_mac[5] );

		char buf[200];
		snprintf( buf, sizeof(buf) / sizeof(*buf),
			"SELECT * FROM wifi_scan_data"
			" WHERE station_mac=\'%02X:%02X:%02X:%02X:%02X:%02X\'"
			" ORDER BY scan_power",
			wifi_mac[0], wifi_mac[1], wifi_mac[2],
			wifi_mac[3], wifi_mac[4], wifi_mac[5] );

		PGresult *wifi_data = PQexec(pg_conn_, buf);

		if (PQresultStatus(wifi_data) != PGRES_TUPLES_OK)
		{
			fprintf(stderr, "Command failed: %s", PQerrorMessage(pg_conn_));
			PQclear(wifi_data);
		}
		else
		{
			int count = PQntuples(wifi_data);

			int wifi_min_power = 0;
			int wifi_max_power = 0;

			for (int i = 0; i < count; i++)
			{
				char *power_s = PQgetvalue(wifi_data, i, 3);
				char *lat_s = PQgetvalue(wifi_data, i, 5);
				char *lon_s = PQgetvalue(wifi_data, i, 6);

				int power;
				cartographer::coord pt;

				sscanf(power_s, "%d", &power);
				sscanf(lat_s, "%lf", &pt.lat);
				sscanf(lon_s, "%lf", &pt.lon);

				if (i == 0 || power < wifi_min_power)
					wifi_min_power = power;

				if (i == 0 || power > wifi_max_power)
					wifi_max_power = power;

				if (Anchor_ == WiFiAnchor && i == count - 1)
					Cartographer->MoveTo(pt, cartographer::ratio(0.5, 0.5));
			}

			{
				unique_lock<mutex> lock(WiFi_mutex_);
				WiFi_data_ = wifi_data;
				WiFi_min_power_ = wifi_min_power;
				WiFi_max_power_ = wifi_max_power;
			}
		}
	}

	SetTitle(title);
}

void MainFrame::CheckerProc(my::worker::ptr this_worker)
{
	MY_REGISTER_THREAD("Checker");

	while (!finish())
	{
		boost::this_thread::sleep( posix_time::milliseconds(1000) );
		debug_log << L"CheckerProc()" << debug_log;
	}
}

void MainFrame::OnIdle(wxIdleEvent& event)
{
	UpdateButtons();

	static posix_time::ptime start = my::time::utc_now();
	if (my::time::utc_now() - start > posix_time::milliseconds(1000))
	{
		debug_log << L"OnIdle()" << debug_log;
		start = my::time::utc_now();
	}
}

void MainFrame::OnGpsTrackerClick(wxCommandEvent& event)
{
	unique_lock<mutex> lock(Gps_mutex_);

	if ( worked(GpsTracker_worker_) )
	{
		lets_finish(GpsTracker_worker_);
		ToolBar1->ToggleTool(ID_GPSTRACKER, false);
	}
	else
	{
		lets_finish(GpsTracker_worker_, false);

		boost::thread( boost::bind(
			&MainFrame::GpsTrackerProc, this, GpsTracker_worker_) );

		ToolBar1->ToggleTool(ID_GPSTRACKER, true);
	}
}

void MainFrame::GpsTrackerProc(my::worker::ptr this_worker)
{
	MY_REGISTER_THREAD("GpsTracker");

	int gpsd_sock = socket( AF_INET, SOCK_STREAM, 0);
	if( gpsd_sock < 0)
		return;

	sockaddr_in gpsd_addr;
	gpsd_addr.sin_family = AF_INET;
	gpsd_addr.sin_port = htons(2947);
	gpsd_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if( connect(gpsd_sock, (struct sockaddr *)&gpsd_addr,
		sizeof(gpsd_addr)) < 0)
	{
		close(gpsd_sock);
		return;
	}


	char buf[256];

	while ( !finish(this_worker) )
	{
		boost::this_thread::sleep( posix_time::milliseconds(1000) );

		debug_log << L"GpsTrackerProc()" << debug_log;

		int n = snprintf(buf, sizeof(buf) / sizeof(*buf), "pvta\r\n");
		if (send(gpsd_sock, buf, n, 0) != n)
			break;

		n = recv(gpsd_sock, buf, sizeof(buf) / sizeof(*buf) - 1, 0);
		if (n <= 0)
			break;

		buf[n] = 0;

		cartographer::coord gps_pt;
		double gps_speed;
		double gps_azimuth;
		double gps_altitude;
		bool gps_ok;

		sprintf(buf, "GPSD,P=48.5 135.1,V=2.5,T=45.0,A=100.0");

		n = sscanf(buf, "GPSD,P=%lf %lf,V=%lf,T=%lf,A=%lf",
			&gps_pt.lat, &gps_pt.lon,
			&gps_speed, &gps_azimuth, &gps_altitude);

		if (n != 5)
			gps_ok = false;
		else
		{
			gps_ok = true;
			gps_speed *= 1.852; /* Узлы в км/ч */
		}

		{
			unique_lock<mutex> lock(Gps_mutex_);

			Gps_ok_ = gps_ok;
			Gps_pt_ = gps_pt;
			Gps_speed_ = gps_speed;
			Gps_azimuth_ = gps_azimuth;
			Gps_altitude_ = gps_altitude;
		}

		if (gps_ok && Anchor_ == GpsAnchor)
			Cartographer->MoveTo(gps_pt, cartographer::ratio(0.5, 0.5));
	}

	close(gpsd_sock);
}

void MainFrame::OnGpsAnchorClick(wxCommandEvent& event)
{
	Anchor_ = GpsAnchor;
	UpdateButtons();
}

void MainFrame::OnWiFiAnchorClick(wxCommandEvent& event)
{
	Anchor_ = WiFiAnchor;
	UpdateButtons();
}

void MainFrame::UpdateButtons()
{
	/* GpsTracker */
	bool gps_enabled = worked(GpsTracker_worker_);
	ToolBar1->ToggleTool(ID_GPSTRACKER, gps_enabled);
	ToolBar1->EnableTool(ID_GPSANCHOR, gps_enabled);

	/* WiFiScan */
	bool wifi_enabled = worked(WiFiScan_worker_);
	ToolBar1->ToggleTool(ID_WIFISCAN, wifi_enabled);
	ToolBar1->EnableTool(ID_WIFIANCHOR, wifi_enabled);

	/* Кнопки слежения */
	switch (Anchor_)
	{
		case GpsAnchor:
			ToolBar1->ToggleTool(ID_WIFIANCHOR, false);

			if (!gps_enabled)
				Anchor_ = NoAnchor;
			else
				ToolBar1->ToggleTool(ID_GPSANCHOR, true);

			break;

		case WiFiAnchor:
			ToolBar1->ToggleTool(ID_GPSANCHOR, false);

			if (!wifi_enabled)
				Anchor_ = NoAnchor;
			else
				ToolBar1->ToggleTool(ID_WIFIANCHOR, true);

			break;
	}
}
