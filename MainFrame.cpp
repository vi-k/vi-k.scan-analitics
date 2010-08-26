/***************************************************************
 * Name:      MainFrame.cpp
 * Purpose:   Code for Application Frame
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/
bool ppp = false;
#include "MainFrame.h"
#include "SettingsDialog.h"

extern my::log main_log;
extern my::log debug_log;
extern wxFileConfig *MyConfig;

my::log gps_log(L"gps.log", my::log::clean);

#include <string>
#include <sstream>

//(*InternalHeaders(MainFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

/* Загрузка картинок */
namespace images
{
	#include "images/cartographer.c"
	#include "images/zoom_in.c"
	#include "images/zoom_out.c"
	#include "images/gps_tracker.c"
	#include "images/wifi.c"
	#include "images/anchor.c"
	#include "images/green_mark.c"
	#include "images/red_mark.c"
	#include "images/yellow_mark.c"
}


wxDEFINE_EVENT(MY_MESSAGEBOX, myMessageBoxEvent);

//(*IdInit(MainFrame)
const long MainFrame::ID_COMBOBOX1 = wxNewId();
const long MainFrame::ID_PANEL2 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_MENUITEM3 = wxNewId();
const long MainFrame::ID_MENU_QUIT = wxNewId();
const long MainFrame::ID_MENUITEM2 = wxNewId();
const long MainFrame::ID_MENUITEM1 = wxNewId();
const long MainFrame::ID_ZOOMIN = wxNewId();
const long MainFrame::ID_ZOOMOUT = wxNewId();
const long MainFrame::ID_GPSTRACKER = wxNewId();
const long MainFrame::ID_GPSANCHOR = wxNewId();
const long MainFrame::ID_WIFISCAN = wxNewId();
const long MainFrame::ID_WIFIANCHOR = wxNewId();
const long MainFrame::ID_MENU_ABOUT = wxNewId();
const long MainFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	MY_EVT(MY_MESSAGEBOX, MainFrame::OnMyMessageBox)
	EVT_IDLE(MainFrame::OnIdle)
	//(*EventTable(MainFrame)
	//*)
END_EVENT_TABLE()

wxImage LoadImageFromRaw(const unsigned char *data,
	int width, int height, bool with_alpha)
{
	wxImage image(width, height);

	if (with_alpha)
		image.InitAlpha();

	unsigned char *dest_rgb = image.GetData();
	unsigned char *dest_a = image.GetAlpha();

	const unsigned char *end = data
		+ width * height * (with_alpha ? 4 : 3);

	if (with_alpha)
	{
		while (data != end)
		{
			*dest_rgb++ = *data++;
			*dest_rgb++ = *data++;
			*dest_rgb++ = *data++;
			*dest_a++   = *data++;
		}
	}
	else
	{
		while (data != end)
		{
			*dest_rgb++ = *data++;
			*dest_rgb++ = *data++;
			*dest_rgb++ = *data++;
		}
	}

	return image;
}

template<class C_ImageStruct>
wxImage LoadImageFromC(const C_ImageStruct &st)
{
	return LoadImageFromRaw(st.pixel_data,
		st.width, st.height, st.bytes_per_pixel == 4);
}

wxBitmap LoadBitmapFromRaw(const unsigned char *data,
	int width, int height, bool with_alpha)
{
	return wxBitmap( LoadImageFromRaw(data, width, height, with_alpha) );
}

template<class C_ImageStruct>
wxBitmap LoadBitmapFromC(const C_ImageStruct &st)
{
	return LoadBitmapFromRaw(st.pixel_data,
		st.width, st.height, st.bytes_per_pixel == 4);
}

MainFrame::MainFrame(wxWindow* parent,wxWindowID id)
	: my::employer("MainFrame_employer")
	, Cartographer(0)
	, Anchor_(NoAnchor)
	, WiFi_data_(NULL)
	, WiFi_relative_mode_(true)
	, WiFi_min_power_(-100)
	, WiFi_max_power_(10)
	, WiFi_min_power_abs_(-100)
	, WiFi_max_power_abs_(10)
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
	, Gps_test_(false)
	, MY_MUTEX_DEF(Gps_mutex_,true)
{
	#undef _
	#define _(s) (L##s)

	//(*Initialize(MainFrame)
	wxMenuItem* MenuItem2;
	wxMenuItem* MenuItem1;
	wxMenu* Menu1;
	wxMenuBar* MainMenu;
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
	MainMenu = new wxMenuBar();
	Menu1 = new wxMenu();
	MenuItem5 = new wxMenuItem(Menu1, ID_MENUITEM3, _("Настройки..."), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(MenuItem5);
	Menu1->AppendSeparator();
	MenuItem1 = new wxMenuItem(Menu1, ID_MENU_QUIT, _("Выход\tAlt-F4"), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(MenuItem1);
	MainMenu->Append(Menu1, _("Файл"));
	Menu3 = new wxMenu();
	MenuItem3 = new wxMenu();
	MenuItem4 = new wxMenuItem(MenuItem3, ID_MENUITEM2, _("Карта"), wxEmptyString, wxITEM_NORMAL);
	MenuItem3->Append(MenuItem4);
	Menu3->Append(ID_MENUITEM1, _("Карты"), MenuItem3, wxEmptyString);
	Menu3->AppendSeparator();
	Menu4 = new wxMenuItem(Menu3, ID_ZOOMIN, _("Увеличить масштаб"), wxEmptyString, wxITEM_NORMAL);
	Menu3->Append(Menu4);
	Menu5 = new wxMenuItem(Menu3, ID_ZOOMOUT, _("Уменьшить масштаб"), wxEmptyString, wxITEM_NORMAL);
	Menu3->Append(Menu5);
	Menu3->AppendSeparator();
	MenuItem6 = new wxMenuItem(Menu3, ID_GPSTRACKER, _("Загружать данные с Gps"), wxEmptyString, wxITEM_CHECK);
	Menu3->Append(MenuItem6);
	MenuItem7 = new wxMenuItem(Menu3, ID_GPSANCHOR, _("Следить за Gps"), wxEmptyString, wxITEM_CHECK);
	Menu3->Append(MenuItem7);
	Menu3->AppendSeparator();
	MenuItem8 = new wxMenuItem(Menu3, ID_WIFISCAN, _("Загружать данные с WiFi-сканера"), wxEmptyString, wxITEM_CHECK);
	Menu3->Append(MenuItem8);
	MenuItem9 = new wxMenuItem(Menu3, ID_WIFIANCHOR, _("Следить за данными WiFi-сканера"), wxEmptyString, wxITEM_CHECK);
	Menu3->Append(MenuItem9);
	MainMenu->Append(Menu3, _("Вид"));
	Menu2 = new wxMenu();
	MenuItem2 = new wxMenuItem(Menu2, ID_MENU_ABOUT, _("О программе...\tF1"), wxEmptyString, wxITEM_NORMAL);
	Menu2->Append(MenuItem2);
	MainMenu->Append(Menu2, _("Помощь"));
	SetMenuBar(MainMenu);
	StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
	int __wxStatusBarWidths_1[1] = { -1 };
	int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
	StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
	StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
	SetStatusBar(StatusBar1);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&MainFrame::OnComboBox1Select);
	Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnSettings);
	Connect(ID_MENU_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnQuit);
	Connect(ID_ZOOMIN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnZoomIn);
	Connect(ID_ZOOMOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnZoomOut);
	Connect(ID_GPSTRACKER,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnGpsTracker);
	Connect(ID_GPSANCHOR,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnGpsAnchor);
	Connect(ID_WIFISCAN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnWiFiScan);
	Connect(ID_WIFIANCHOR,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnWiFiAnchor);
	Connect(ID_MENU_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnAbout);
	//*)

	setlocale(LC_NUMERIC, "C");

	{
		wxIcon FrameIcon;
		FrameIcon.CopyFromBitmap(LoadBitmapFromC(images::cartographer));
		SetIcon(FrameIcon);
	}

	MainToolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER);
	MainToolBar->AddTool(ID_ZOOMIN, L"ZoomIn", LoadBitmapFromC(images::zoom_in), L"Увеличить");
	MainToolBar->AddTool(ID_ZOOMOUT, L"ZoomOut", LoadBitmapFromC(images::zoom_out), L"Уменьшить");
	MainToolBar->AddSeparator();
	MainToolBar->AddTool(ID_GPSTRACKER, L"GpsTracker", LoadBitmapFromC(images::gps_tracker), L"Gps", wxITEM_CHECK);
	MainToolBar->AddTool(ID_GPSANCHOR, L"GpsAnchor", LoadBitmapFromC(images::anchor), L"Следить за Gps", wxITEM_CHECK);
	MainToolBar->AddSeparator();
	MainToolBar->AddTool(ID_WIFISCAN, L"WiFiScan", LoadBitmapFromC(images::wifi), L"WiFi", wxITEM_CHECK);
	MainToolBar->AddTool(ID_WIFIANCHOR, L"WiFiAnchor", LoadBitmapFromC(images::anchor), L"Следить за WiFi", wxITEM_CHECK);
	MainToolBar->Realize();
	SetToolBar(MainToolBar);

	ReloadSettings();

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
			= MyConfig->ReadBool(L"/Cartographer/OnlyCache", false);

		if (!only_cache)
		{
			wxString str = MyConfig->Read(L"/Cartographer/ServerAddr", L"");
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
	gps_tracker_id_ = Cartographer->LoadImageFromC(images::gps_tracker);
	Cartographer->SetImageCentralPoint(gps_tracker_id_, 15.5, 19.0);

	green_mark16_id_ = Cartographer->LoadImageFromC(images::green_mark);
	red_mark16_id_ = Cartographer->LoadImageFromC(images::red_mark);
	yellow_mark16_id_ = Cartographer->LoadImageFromC(images::yellow_mark);


	/* Запускаем собственную прорисовку */
	Cartographer->SetPainter(
		boost::bind(&MainFrame::OnMapPaint, this, _1, _2));

	Cartographer->SetStatusHandler(
		boost::bind(&MainFrame::StatusHandler, this, _1));

	Cartographer->MoveTo(13,
		cartographer::DMSToDD( 48,28,48.77, 135,4,19.04 ));

	//UpdateWiFiData( macaddr(0, 0x10, 0xE7, 0xA4, 0x46, 0x9D) );
	WiFiScan_worker_ = new_worker( L"WiFiScan_worker");

	GpsTracker_worker_ = new_worker( L"GpsTracker_worker");

	boost::thread( boost::bind(
		&MainFrame::CheckerProc, this, new_worker( L"Checker_worker")) );
}

MainFrame::~MainFrame()
{
	/* Остановка картографера обязательно должна быть выполнена
		до удаления всех объектов, использующихся в обработчике OnMapPaint */
	Cartographer->Stop();


	lets_finish();

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
	//wxMessageBox( L"About...");

	int ret = myMessageBox(L"Test", L"Title", wxOK | wxICON_ERROR, this);
	ret = ret - ret;
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
			int min_power = WiFi_relative_mode_ ?
				WiFi_min_power_ : WiFi_min_power_abs_;
			int max_power = WiFi_relative_mode_ ?
				WiFi_max_power_ : WiFi_max_power_abs_;
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

				double green_k = power_k < 0.5 ? 1.0 : 2.0 * (1.0 - power_k);
				double red_k = power_k < 0.5 ? 2.0 * power_k : 1.0;

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

	if (ppp)
	{
		Cartographer->DrawText(small_font_,
			L"TestTestTest", cartographer::point(screen_size) / 2.0,
			cartographer::color(1.0, 1.0, 0.0),
			cartographer::ratio(0.5, 0.5), cartographer::ratio(1.0, 1.0));
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
				L"%s%d°%02d\'%05.2f\" %s%d°%02d\'%05.2f\""
				L" %0.2fкм/ч %0.1f° %0.1fм (%ls)",
				lat_sign < 0 ? L"-" : L"", lat_d, lat_m, lat_s,
				lon_sign < 0 ? L"-" : L"", lon_d, lon_m, lon_s,
				Gps_speed_, Gps_azimuth_, Gps_altitude_,
				Gps_buf_.c_str());

			str += buf;
		}
	}
}

void MainFrame::CheckerProc(my::worker::ptr this_worker)
{
	MY_REGISTER_THREAD("Checker");

	while (!finish())
	{
		debug_log << L"CheckerProc()" << debug_log;
		timed_sleep( this_worker, posix_time::milliseconds(1000) );
	}
}

void MainFrame::OnIdle(wxIdleEvent& event)
{
	static posix_time::ptime start = my::time::utc_now();
	if (my::time::utc_now() - start > posix_time::milliseconds(100))
	{
		debug_log << L"OnIdle()" << debug_log;
		UpdateButtons();
		start = my::time::utc_now();
	}
}

void MainFrame::UpdateButtons()
{
	int anchor = Anchor_;

	/* GpsTracker */
	bool gps_enabled = worked(GpsTracker_worker_);
	MainToolBar->ToggleTool(ID_GPSTRACKER, gps_enabled);
	MainToolBar->EnableTool(ID_GPSANCHOR, gps_enabled);
	MainToolBar->ToggleTool(ID_GPSANCHOR, gps_enabled && anchor == GpsAnchor);

	/* WiFiScan */
	bool wifi_enabled = worked(WiFiScan_worker_);
	MainToolBar->ToggleTool(ID_WIFISCAN, wifi_enabled);
	MainToolBar->EnableTool(ID_WIFIANCHOR, wifi_enabled);
	MainToolBar->ToggleTool(ID_WIFIANCHOR, wifi_enabled && anchor == WiFiAnchor);
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
	SettingsDialog::Open(this);
	ReloadSettings();
}

void MainFrame::ReloadSettings()
{
	{
		unique_lock<mutex> lock(WiFi_mutex_);
		WiFi_relative_mode_ = MyConfig->ReadBool(L"/Colors/RelativeMode", true);
		WiFi_min_power_abs_ = MyConfig->ReadLong(L"/Colors/MinPower", -100);
		WiFi_max_power_abs_ = MyConfig->ReadLong(L"/Colors/MaxPower", 10);
	}

	Gps_test_ = MyConfig->ReadBool(L"/Cartographer/GpsTest", false);
}

void MainFrame::OnZoomIn(wxCommandEvent& event)
{
	Cartographer->ZoomIn();
}

void MainFrame::OnZoomOut(wxCommandEvent& event)
{
	Cartographer->ZoomOut();
}

void MainFrame::OnGpsTracker(wxCommandEvent& event)
{
	unique_lock<mutex> lock(Gps_mutex_);

	if ( worked(GpsTracker_worker_) )
	{
		lets_finish(GpsTracker_worker_);
	}
	else
	{
		cancel_finish(GpsTracker_worker_);
		boost::thread( boost::bind(
			&MainFrame::GpsTrackerProc, this, GpsTracker_worker_) );
	}
}

void MainFrame::OnGpsAnchor(wxCommandEvent& event)
{
	if (Anchor_ == GpsAnchor)
		Anchor_ = NoAnchor;
	else
		Anchor_ = GpsAnchor;

	UpdateButtons();
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
		std::string gps_buf(buf);

		if (Gps_test_)
			sprintf(buf, "GPSD,P=48.5 135.1,V=2.5,T=45.0,A=100.0");

		char *ptr = buf;
		while (*ptr)
		{
			if (*ptr == '?')
				*ptr = '0';
			ptr++;
		}

		n = sscanf(buf, "GPSD,P=%lf %lf,V=%lf,T=%lf,A=%lf",
			&gps_pt.lat, &gps_pt.lon,
			&gps_speed, &gps_azimuth, &gps_altitude);

		if (n != 5)
			gps_ok = false;
		else
		{
			gps_ok = !(gps_pt.lat == 0.0 && gps_pt.lon == 0.0);
			gps_speed *= 1.852; /* Узлы в км/ч */
		}

		{
			unique_lock<mutex> lock(Gps_mutex_);

			Gps_ok_ = gps_ok;
			Gps_pt_ = gps_pt;
			Gps_speed_ = gps_speed;
			Gps_azimuth_ = gps_azimuth;
			Gps_altitude_ = gps_altitude;
			Gps_buf_ = my::str::to_wstring(gps_buf);
			gps_log << Gps_buf_ << gps_log;
		}

		if (gps_ok && Anchor_ == GpsAnchor)
			Cartographer->MoveTo(gps_pt, cartographer::ratio(0.5, 0.5));

		timed_sleep( this_worker, posix_time::milliseconds(1000) );
	}

	close(gpsd_sock);
}

void MainFrame::OnWiFiScan(wxCommandEvent& event)
{
	unique_lock<mutex> lock(WiFi_mutex_);

	if ( worked(WiFiScan_worker_) )
	{
		lets_finish(WiFiScan_worker_);
	}
	else
	{
		cancel_finish(WiFiScan_worker_);
		boost::thread( boost::bind(
			&MainFrame::WiFiScanProc, this, WiFiScan_worker_) );
	}
}

void MainFrame::OnWiFiAnchor(wxCommandEvent& event)
{
	if (Anchor_ == WiFiAnchor)
		Anchor_ = NoAnchor;
	else
	{
		Anchor_ = WiFiAnchor;

		cartographer::coord wifi_max_power_pt;
		bool wifi_ok;

		{
			unique_lock<mutex> lock(WiFi_mutex_);
			wifi_ok = WiFi_ok_;
			wifi_max_power_pt = WiFi_max_power_pt_;
		}

		if (wifi_ok)
			Cartographer->MoveTo(wifi_max_power_pt,
				cartographer::ratio(0.5, 0.5));
	}

	UpdateButtons();
}

void MainFrame::WiFiScanProc(my::worker::ptr this_worker)
{
	MY_REGISTER_THREAD("WiFiScan");

	int wifi_sock;

	wifi_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (wifi_sock < 0 )
	{
		myMessageBox(L"Не удалось создать сокет", L"Ошибка",
			wxOK | wxICON_ERROR);
		return;
	}

	sockaddr_un remote;
	remote.sun_family = AF_UNIX;

	strcpy(remote.sun_path, "/tmp/wifiscan.sock");
	size_t len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(wifi_sock, (struct sockaddr *)&remote, len) == -1)
	{
		close(wifi_sock);
		int ret = myMessageBox(L"Не удалось соединиться с WiFi-сканером", L"Ошибка",
			wxOK | wxICON_ERROR);
		ret = ret;
		return;
	}

	while ( !finish(this_worker) )
	{
		debug_log << L"WiFiScanProc()" << debug_log;

		macaddr mac;

		int sz = recv(wifi_sock, mac.mac(), 6, 0);

		if (sz == -1)
			break;

		if (sz != 6)
			continue;

		UpdateWiFiData(mac);
	}

	close(wifi_sock);
}

void MainFrame::UpdateWiFiData(const macaddr &mac)
{
	if (!PgConnect())
		return;

	std::wstring title(L"Scan Analitics");

	{
		unique_lock<mutex> lock(WiFi_mutex_);

		if (WiFi_data_)
		{
			PQclear(WiFi_data_);
			WiFi_data_ = NULL;
		}
	}

	if ( !mac.empty() )
	{
		unique_lock<mutex> lock(pg_mutex_);

		title = L"Scan Analitics (" + mac.to_wstring() + L")";

		std::string query
			= "SELECT * FROM wifi_scan_data WHERE station_mac=\'"
				+ mac.to_string() + "\' ORDER BY scan_power";

		PGresult *wifi_data = PQexec(pg_conn_, query.c_str());

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
			cartographer::coord wifi_max_power_pt;
			bool wifi_ok = false;

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

				if (i == count - 1)
				{
					wifi_ok = true;
					wifi_max_power_pt = pt;
				}
			}

			{
				unique_lock<mutex> lock(WiFi_mutex_);
				WiFi_mac_ = mac;
				WiFi_data_ = wifi_data;
				WiFi_min_power_ = wifi_min_power;
				WiFi_max_power_ = wifi_max_power;
				WiFi_max_power_pt_ = wifi_max_power_pt;
				WiFi_ok_ = wifi_ok;
			}

			lock.unlock();

			if (Anchor_ == WiFiAnchor && wifi_ok)
				Cartographer->MoveTo(wifi_max_power_pt,
					cartographer::ratio(0.5, 0.5));
		}
	}

	SetTitle(title);
}

int MainFrame::myMessageBox(const wxString &message,
	const wxString &caption, int style, wxWindow *parent, int x, int y)
{
	myMessageBoxEvent *event = new myMessageBoxEvent(
		MY_MESSAGEBOX, message, caption, style, parent, x, y );

	int ret;
	event->SetRetPtr(&ret);

	waiter w;
	event->SetWaiterPtr(&w);

	unique_lock<mutex> lock( w.get_mutex() );

	QueueEvent(event);

	w.sleep(lock);

	return ret;
}

void MainFrame::myMessageBoxDelayed(const wxString &message,
	const wxString &caption, int style, wxWindow *parent, int x, int y)
{
	myMessageBoxEvent *event = new myMessageBoxEvent(
		MY_MESSAGEBOX, message, caption, style, parent, x, y );

	QueueEvent(event);
}

void MainFrame::OnMyMessageBox(myMessageBoxEvent& event)
{
	int ret = wxMessageBox( event.GetMessage(), event.GetCaption(),
		event.GetStyle(), event.GetParent(), event.GetX(), event.GetY() );

	event.Return(ret);
}
