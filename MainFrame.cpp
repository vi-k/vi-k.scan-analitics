/***************************************************************
 * Name:      MainFrame.cpp
 * Purpose:   Code for Application Frame
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#include "MainFrame.h"
#include "SettingsDialog.h"

extern my::log main_log;
extern wxFileConfig *MyConfig;

my::log gps_log(L"gps.log", my::log::clean | my::log::single | my::log::nothread);
my::log wifi_log(L"wifi.log", my::log::clean | my::log::single | my::log::nothread);

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
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_SETTINGS = wxNewId();
const long MainFrame::ID_QUIT = wxNewId();
const long MainFrame::ID_MENUMAPS = wxNewId();
const long MainFrame::ID_ZOOMIN = wxNewId();
const long MainFrame::ID_ZOOMOUT = wxNewId();
const long MainFrame::ID_GPSTRACKER = wxNewId();
const long MainFrame::ID_GPSANCHOR = wxNewId();
const long MainFrame::ID_WIFISCAN = wxNewId();
const long MainFrame::ID_WIFIANCHOR = wxNewId();
const long MainFrame::ID_ABOUT = wxNewId();
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
	: my::employer(L"MainFrame_employer", false)
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
	, green_mark_id_(0)
	, red_mark_id_(0)
	, yellow_mark_id_(0)
	, pg_conn_(NULL)
	, MY_MUTEX_DEF(pg_mutex_,true)
	, Gps_test_(false)
	, MY_MUTEX_DEF(Gps_mutex_,true)
{
	#undef _
	#define _(s) (L##s)

	//(*Initialize(MainFrame)
	wxMenu* MenuHelp;
	wxMenuItem* MenuAbout;
	wxMenuBar* MainMenu;
	wxMenu* MenuFile;
	wxMenuItem* MenuQuit;

	Create(parent, wxID_ANY, _("Scan Analitics"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(626,293));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer1 = new wxFlexGridSizer(1, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(616,331), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	MainMenu = new wxMenuBar();
	MenuFile = new wxMenu();
	MenuSettings = new wxMenuItem(MenuFile, ID_SETTINGS, _("Настройки..."), wxEmptyString, wxITEM_NORMAL);
	MenuFile->Append(MenuSettings);
	MenuFile->AppendSeparator();
	MenuQuit = new wxMenuItem(MenuFile, ID_QUIT, _("Выход\tAlt-F4"), wxEmptyString, wxITEM_NORMAL);
	MenuFile->Append(MenuQuit);
	MainMenu->Append(MenuFile, _("Файл"));
	MenuView = new wxMenu();
	MenuMaps = new wxMenu();
	MenuMapsNull = new wxMenuItem(MenuMaps, 0, _("Нет карт"), wxEmptyString, wxITEM_RADIO);
	MenuMaps->Append(MenuMapsNull);
	MenuView->Append(ID_MENUMAPS, _("Карты"), MenuMaps, wxEmptyString);
	MenuView->AppendSeparator();
	MenuZoomIn = new wxMenuItem(MenuView, ID_ZOOMIN, _("Увеличить масштаб"), wxEmptyString, wxITEM_NORMAL);
	MenuView->Append(MenuZoomIn);
	MenuZoomOut = new wxMenuItem(MenuView, ID_ZOOMOUT, _("Уменьшить масштаб"), wxEmptyString, wxITEM_NORMAL);
	MenuView->Append(MenuZoomOut);
	MenuView->AppendSeparator();
	MenuGpsTracker = new wxMenuItem(MenuView, ID_GPSTRACKER, _("Загружать данные с Gps"), wxEmptyString, wxITEM_CHECK);
	MenuView->Append(MenuGpsTracker);
	MenuGpsAnchor = new wxMenuItem(MenuView, ID_GPSANCHOR, _("Следить за Gps"), wxEmptyString, wxITEM_CHECK);
	MenuView->Append(MenuGpsAnchor);
	MenuView->AppendSeparator();
	MenuWifiScan = new wxMenuItem(MenuView, ID_WIFISCAN, _("Загружать данные с WiFi-сканера"), wxEmptyString, wxITEM_CHECK);
	MenuView->Append(MenuWifiScan);
	MenuWiFiAnchor = new wxMenuItem(MenuView, ID_WIFIANCHOR, _("Следить за данными WiFi-сканера"), wxEmptyString, wxITEM_CHECK);
	MenuView->Append(MenuWiFiAnchor);
	MainMenu->Append(MenuView, _("Вид"));
	MenuHelp = new wxMenu();
	MenuAbout = new wxMenuItem(MenuHelp, ID_ABOUT, _("О программе...\tF1"), wxEmptyString, wxITEM_NORMAL);
	MenuHelp->Append(MenuAbout);
	MainMenu->Append(MenuHelp, _("Помощь"));
	SetMenuBar(MainMenu);
	StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
	int __wxStatusBarWidths_1[1] = { -1 };
	int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
	StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
	StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
	SetStatusBar(StatusBar1);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_SETTINGS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnSettings);
	Connect(ID_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnQuit);
	Connect(ID_ZOOMIN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnZoomIn);
	Connect(ID_ZOOMOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnZoomOut);
	Connect(ID_GPSTRACKER,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnGpsTracker);
	Connect(ID_GPSANCHOR,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnGpsAnchor);
	Connect(ID_WIFISCAN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnWiFiScan);
	Connect(ID_WIFIANCHOR,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnWiFiAnchor);
	Connect(ID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnAbout);
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

	Cartographer->Bind(wxEVT_MOTION, &MainFrame::OnMapMouseMove, this, wxID_ANY);

	/* Создаём шрифты */
	big_font_ = Cartographer->CreateFont(
		wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );
	small_font_ = Cartographer->CreateFont(
		wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );

	/* Загружаем список карт */
	{
		MenuMaps->Delete(MenuMapsNull);
		cartographer::map_info active_map = Cartographer->GetActiveMapInfo();

		long maps_count = Cartographer->GetMapsCount();

		for(long i = 0; i < maps_count; ++i)
		{
			cartographer::map_info map = Cartographer->GetMapInfo(i);

			wxMenuItem *item = new wxMenuItem(MenuMaps, i,
				map.name + L"\t" + my::num::to_wstring(i + 1),
				wxEmptyString, wxITEM_RADIO);

			MenuMaps->Append(item);

			if (active_map.sid == map.sid)
				item->Check();

			MenuMaps->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMapChange, this, i);
		}
	}

	/* Изображения */
	gps_tracker_id_ = Cartographer->LoadImageFromC(images::gps_tracker);
	Cartographer->SetImageCentralPoint(gps_tracker_id_, 15.5, 19.0);

	green_mark_id_ = Cartographer->LoadImageFromC(images::green_mark);
	red_mark_id_ = Cartographer->LoadImageFromC(images::red_mark);
	yellow_mark_id_ = Cartographer->LoadImageFromC(images::yellow_mark);


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
		&MainFrame::CheckerProc, this, new_worker(L"Checker_worker")) );
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
	myMessageBox(L"About...");
}

void MainFrame::OnMapPaint(double z, const cartographer::size &screen_size)
{
	/*-
	int fields = PQnfields(pg_res_);
	for (i = 0; i < fields; i++)
		printf("%-15s ", PQfname(res, i));
	printf("\n\n");
	-*/

	#if 0
	{
		cartographer::coord pt;
		for (pt.lat = -88.0; pt.lat < 88; pt.lat += 1.0)
			for (pt.lon = -180.0; pt.lon < 180.0; pt.lon += 1.0)
			{
				glColor4d(1.0, 1.0, 1.0, 1.0);

				glBindTexture(GL_TEXTURE_2D, 2);
				glBegin(GL_QUADS);
					glTexCoord2d(0.0, 0.0); glVertex3d(-100, -100, 0);
					glTexCoord2d(1.0, 0.0); glVertex3d(-200, -100, 0);
					glTexCoord2d(1.0, 1.0); glVertex3d(-200, -200, 0);
					glTexCoord2d(0.0, 1.0); glVertex3d(-100, -200, 0);
				glEnd();

				//Cartographer->CoordToScreen(pt);
				//Cartographer->DrawImage(green_mark_id_, pt);
			}
	}
	#endif

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

				if (power_k < 0.5)
				{
					Cartographer->DrawImage(green_mark_id_, pt);
					Cartographer->DrawImage(yellow_mark_id_, pt, 1.0, 2.0 * power_k);
				}
				else
				{
					Cartographer->DrawImage(yellow_mark_id_, pt);
					Cartographer->DrawImage(red_mark_id_, pt, 1.0, 2.0 * (power_k - 0.5));
				}

				/*-
				double green_k = power_k < 0.5 ? 1.0 : 2.0 * (1.0 - power_k);
				double red_k = power_k < 0.5 ? 2.0 * power_k : 1.0;

				Cartographer->DrawSimpleCircle(
					Cartographer->CoordToScreen(pt), 5.0, 2.0,
					cartographer::color(red_k, green_k, 0.0),
					cartographer::color(red_k, green_k, 0.0, 0.3));
				-*/

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


	/* Gps */
	Gps_params_st gps;
	copy(Gps_params_, &gps, Gps_mutex_);

	if (gps.ok && worked(GpsTracker_worker_))
	{
		Cartographer->DrawImage(gps_tracker_id_, gps.pt,
			1.0, 1.0, gps.azimuth);
	}
}

void MainFrame::StatusHandler(std::wstring &str)
{
	my::scope sc(L"StatusHandler()");

	str += L" | GPS: ";

	if ( !worked(GpsTracker_worker_) )
		str += L"отключен";
	else
	{
		Gps_params_st gps;
		copy(Gps_params_, &gps, Gps_mutex_);

		if (!gps.ok)
			str+= L"ошибка";
		else
		{
			wchar_t buf[400];

			int lat_sign, lon_sign;
			int lat_d, lon_d;
			int lat_m, lon_m;
			double lat_s, lon_s;

			DDToDMS( gps.pt,
				&lat_sign, &lat_d, &lat_m, &lat_s,
				&lon_sign, &lon_d, &lon_m, &lon_s );

			__swprintf(buf, sizeof(buf)/sizeof(*buf),
				L"%s%d°%02d\'%05.2f\" %s%d°%02d\'%05.2f\""
				L" %0.2fкм/ч %0.1f° %0.1fм %0.1fкм (%ls)",
				lat_sign < 0 ? L"-" : L"", lat_d, lat_m, lat_s,
				lon_sign < 0 ? L"-" : L"", lon_d, lon_m, lon_s,
				gps.speed, gps.azimuth, gps.altitude, gps.distance,
				gps.test_buf.c_str());

			str += buf;
		}
	}
}

void MainFrame::CheckerProc(my::worker::ptr this_worker)
{
	MY_REGISTER_THREAD(L"Checker");

	while (!finish())
	{
		main_log << L"CheckerProc()" << main_log;
		timed_sleep( this_worker, posix_time::milliseconds(1000) );
	}
}

void MainFrame::OnIdle(wxIdleEvent& event)
{
	static posix_time::ptime start = my::time::utc_now();
	if (my::time::utc_now() - start > posix_time::milliseconds(100))
	{
		my::scope sc(L"OnIdle()");
		UpdateButtons();
		start = my::time::utc_now();
	}
}

void MainFrame::UpdateButtons()
{
	int anchor = Anchor_;
	wxMenuBar *MainMenu = GetMenuBar();

	/* GpsTracker */
	bool gps_enabled = worked(GpsTracker_worker_);
	MainMenu->Check(ID_GPSTRACKER, gps_enabled);
	MainMenu->Enable(ID_GPSANCHOR, gps_enabled);
	MainMenu->Check(ID_GPSANCHOR, gps_enabled && anchor == GpsAnchor);
	MainToolBar->ToggleTool(ID_GPSTRACKER, gps_enabled);
	MainToolBar->EnableTool(ID_GPSANCHOR, gps_enabled);
	MainToolBar->ToggleTool(ID_GPSANCHOR, gps_enabled && anchor == GpsAnchor);

	/* WiFiScan */
	bool wifi_enabled = worked(WiFiScan_worker_);
	MainMenu->Check(ID_WIFISCAN, wifi_enabled);
	MainMenu->Enable(ID_WIFIANCHOR, wifi_enabled);
	MainMenu->Check(ID_WIFIANCHOR, wifi_enabled && anchor == WiFiAnchor);
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
	MY_REGISTER_THREAD(L"GpsTracker");

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
		int n = snprintf(buf, sizeof(buf) / sizeof(*buf), "padvt\r\n");
		if (send(gpsd_sock, buf, n, 0) != n)
			break;

		n = recv(gpsd_sock, buf, sizeof(buf) / sizeof(*buf) - 1, 0);
		if (n <= 0)
			break;

		buf[n] = 0;

		cartographer::coord pt;

		Gps_params_st gps;
		copy(Gps_params_, &gps, Gps_mutex_);

		gps.test_buf = my::str::to_wstring(buf);

		gps_log << gps.test_buf << gps_log;

		if (Gps_test_)
			strcpy(buf, Gps_test_buf_.c_str());

		main_log << buf << main_log;

		n = sscanf(buf, "GPSD,P=%lf %lf,A=%lf",
			&pt.lat, &pt.lon, &gps.altitude);

		if (n < 3)
			gps.altitude = std::numeric_limits<double>::quiet_NaN();

		if (n >= 2 && !(pt.lat == 0.0 && pt.lon == 0.0))
		{
			posix_time::ptime now;

			const char *ptr = strstr(buf, ",D=");
			if (ptr)
				my::time::get(ptr + 3, (size_t)-1, now, 'T');

			if (gps.ok)
			{
				double azimuth;
				double distance = cartographer::Inverse(gps.pt, pt, &azimuth) / 1000.0;

				if (distance < 0.001) /* 1m */
					now = posix_time::not_a_date_time;
				else
				{
					gps.azimuth = azimuth;
					gps.distance += distance;
					gps.speed = distance
						/ my::time::div(now - gps.timestamp, posix_time::hours(1));
				}
			}

			if (!now.is_special())
			{
				gps.ok = true;
				gps.pt = pt;
				gps.timestamp = now;
			}

			if (Anchor_ == GpsAnchor)
				Cartographer->MoveTo(pt, cartographer::ratio(0.5, 0.5));
		}

		copy(gps, &Gps_params_, Gps_mutex_);

		timed_sleep( this_worker, posix_time::milliseconds(500) );
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
	MY_REGISTER_THREAD(L"WiFiScan");

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
		macaddr mac;
		int sz;

		{
			my::scope sc(L"recv()", L"WiFiScanProc():");
			sz = recv(wifi_sock, mac.mac(), 6, 0);
			sc.add(L" sz=" + my::num::to_wstring(sz));
			if (sz == 6)
				sc.add(L" mac=" + mac.to_wstring());
		}

		if (sz == -1)
			break;

		if (sz != 6)
			continue;

		wifi_log << mac.to_wstring() << wifi_log;

		UpdateWiFiData(mac);
	}

	close(wifi_sock);
}

void MainFrame::UpdateWiFiData(const macaddr &mac)
{
	my::scope sc(L"UpdateWiFiData()");

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
			= "SELECT * FROM wifi_scan_data"
			" WHERE station_mac=\'" + mac.to_string() + "\'"
			" AND NOT (scan_lat=0.0 AND scan_long=0.0)"
			" ORDER BY scan_power";

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
	if (wxThread::IsMain())
		return wxMessageBox(message, caption, style, parent, x, y);

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
	if (wxThread::IsMain())
		wxMessageBox(message, caption, style, parent, x, y);

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

void MainFrame::OnMapChange(wxCommandEvent &event)
{
	Cartographer->SetActiveMapByIndex( event.GetId() );
}

void MainFrame::OnMapMouseMove(wxMouseEvent& event)
{
	if (Gps_test_)
	{
		cartographer::point pos( event.GetX(), event.GetY() );
		cartographer::coord pt = Cartographer->ScreenToCoord(pos);

		char buf[200];
		sprintf(buf, "GPSD,P=%f %f,A=100.0", pt.lat, pt.lon);
		Gps_test_buf_ = buf;
		Gps_test_buf_ += my::time::to_string(
			my::time::utc_now(), ",D=%Y-%m-%dT%H:%M:%S%fZ");
	}

	event.Skip(true);
}
