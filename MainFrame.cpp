﻿/***************************************************************
 * Name:      MainFrame.cpp
 * Purpose:   Code for Application Frame
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#include "MainFrame.h"

#include <string>
#include <sstream>

//(*InternalHeaders(MainFrame)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include "images/green_mark16.c"
#include "images/red_mark16.c"
#include "images/yellow_mark16.c"

//#include <wx/filename.h>

//(*IdInit(MainFrame)
const long MainFrame::ID_COMBOBOX1 = wxNewId();
const long MainFrame::ID_CHOICE1 = wxNewId();
const long MainFrame::ID_PANEL2 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_MENU_QUIT = wxNewId();
const long MainFrame::ID_MENU_ABOUT = wxNewId();
const long MainFrame::ID_STATUSBAR1 = wxNewId();
const long MainFrame::ID_ZOOMIN = wxNewId();
const long MainFrame::ID_ZOOMOUT = wxNewId();
const long MainFrame::ID_ANCHOR = wxNewId();
const long MainFrame::ID_WIFISCAN = wxNewId();
const long MainFrame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	//(*EventTable(MainFrame)
	//*)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent,wxWindowID id)
	: Cartographer(0)
	, WiFi_sock_(0)
	, WiFi_data_(0)
	, big_font_(0)
	, small_font_(0)
{
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
	Choice1 = new wxChoice(Panel2, ID_CHOICE1, wxPoint(232,8), wxSize(192,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
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
	ToolBarItem3 = ToolBar1->AddTool(ID_ANCHOR, _("Anchor"), wxBitmap(wxImage(_T("images/binoculars-32.png"))), wxNullBitmap, wxITEM_CHECK, _("Следить"), wxEmptyString);
	ToolBarItem4 = ToolBar1->AddTool(ID_WIFISCAN, _("WiFiScan"), wxBitmap(wxImage(_T("images/wifi.png"))), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString);
	ToolBar1->Realize();
	SetToolBar(ToolBar1);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&MainFrame::OnComboBox1Select);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MainFrame::OnChoice1Select);
	Connect(ID_MENU_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnQuit);
	Connect(ID_MENU_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnAbout);
	Connect(ID_ZOOMIN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnZoomInButtonClick);
	Connect(ID_ZOOMOUT,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnZoomOutButtonClick);
	Connect(ID_ANCHOR,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnAnchorButtonClick);
	Connect(ID_WIFISCAN,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnWiFiScanButtonClicked);
	//*)

    setlocale(LC_NUMERIC, "C");

	{
		wxIcon FrameIcon;
		FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("images/cartographer.png"))));
		SetIcon(FrameIcon);
	}

	SetClientSize(400, 400);
	Maximize(true);
	Show(true);

	/* Обязательно обнуляем, а то получим ошибку, т.к. картинки начнут
		использоваться ещё до того, как мы успеем их загрузить */
	for (int i = 0; i < count_; ++i)
		images_[i]= 0;

	Cartographer = new cartographer::Painter(this, L"cache");
	Cartographer = new cartographer::Painter(this, L"172.16.19.1");
	Cartographer = new cartographer::Painter(this, L"127.0.0.1");
	delete Panel1;
	FlexGridSizer1->Add(Cartographer, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);

	big_font_ = Cartographer->CreateFont(
		wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );
	small_font_ = Cartographer->CreateFont(
		wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );

	/* Список карт */
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
	green_mark16_id_ = Cartographer->LoadImageFromC(green_mark16);
	red_mark16_id_ = Cartographer->LoadImageFromC(red_mark16);
	yellow_mark16_id_ = Cartographer->LoadImageFromC(yellow_mark16);


	/* Метки - не забыть изменить размер массива (!),
		когда надо будет добавить ещё */
	images_[0] = Cartographer->LoadImageFromFile(L"images/blu-blank.png");
	Cartographer->SetImageCentralPoint(images_[0], 31.5, 64.0);

	images_[1] = Cartographer->LoadImageFromFile(L"images/back.png");
	Cartographer->SetImageCentralPoint(images_[1], -1.0, 15.5);

	images_[2] = Cartographer->LoadImageFromFile(L"images/forward.png");
	Cartographer->SetImageCentralPoint(images_[2], 32.0, 15.5);

	images_[3] = Cartographer->LoadImageFromFile(L"images/up.png");
	Cartographer->SetImageCentralPoint(images_[3], 15.5, -1.0);

	images_[4] = Cartographer->LoadImageFromFile(L"images/down.png");
	Cartographer->SetImageCentralPoint(images_[4], 15.5, 31.0);

	images_[5] = Cartographer->LoadImageFromFile(L"images/flag.png");
	Cartographer->SetImageCentralPoint(images_[5], 3.5, 31.0);

	images_[6] = Cartographer->LoadImageFromFile(L"images/write.png");
	Cartographer->SetImageCentralPoint(images_[6], 1.0, 31.0);

	images_[7] = Cartographer->LoadImageFromFile(L"images/ylw-pushpin.png");
	Cartographer->SetImageCentralPoint(images_[7], 18.0, 63.0);
	Cartographer->SetImageScale(images_[7], 0.5, 0.5);

	images_[8] = Cartographer->LoadImageFromFile(L"images/wifi.png");
	Cartographer->SetImageCentralPoint(images_[8], 15.5, 35.0);

	images_[9] = Cartographer->LoadImageFromFile(L"images/blue_star.png");
	Cartographer->SetImageCentralPoint(images_[9], 7.0, 8.0);

	images_[10] = Cartographer->LoadImageFromFile(L"images/green_star.png");
	Cartographer->SetImageCentralPoint(images_[10], 7.0, 8.0);

	/* Места для быстрого перехода */
	names_[0] = L"Хабаровск";
	z_[0] = 12;
	coords_[0] = cartographer::DMSToDD( 48,28,48.77, 135,4,19.04 );

	names_[1] = L"Владивосток";
	z_[1] = 13;
	coords_[1] = cartographer::DMSToDD( 43,7,17.95, 131,55,34.4 );

	names_[2] = L"Магадан";
	z_[2] = 12;
	coords_[2] = cartographer::DMSToDD( 59,33,41.79, 150,50,19.87 );

	names_[3] = L"Якутск";
	z_[3] = 10;
	coords_[3] = cartographer::DMSToDD( 62,4,30.33, 129,45,24.39 );

	names_[4] = L"Южно-Сахалинск";
	z_[4] = 12;
	coords_[4] = cartographer::DMSToDD( 46,57,34.28, 142,44,18.58 );

	names_[5] = L"Петропавловск-Камчатский";
	z_[5] = 13;
	coords_[5] = cartographer::DMSToDD( 53,4,11.14, 158,37,9.24 );

	names_[6] = L"Бикин";
	z_[6] = 11;
	coords_[6] = cartographer::DMSToDD( 46,48,47.59, 134,14,55.71 );

	names_[7] = L"Благовещенск";
	z_[7] = 14;
	coords_[7] = cartographer::DMSToDD( 50,16,55.96, 127,31,46.09 );

	names_[8] = L"Биробиджан";
	z_[8] = 14;
	coords_[8] = cartographer::DMSToDD( 48,47,52.55, 132,55,5.13 );

	for (int i = 0; i < count_; ++i)
		Choice1->Append(names_[i]);

	Cartographer->SetPainter(
		boost::bind(&cartographerFrame::OnMapPaint, this, _1, _2, _3));

	Cartographer->MoveTo(z_[0], coords_[0]);

	memset(WiFi_mac_, 0, sizeof(WiFi_mac_));
	WiFiScan_worker_ = new_worker( L"WiFiScan_worker_");

	WiFi_mac_[0] = 0;
	WiFi_mac_[1] = 0x10;
	WiFi_mac_[2] = 0xE7;
	WiFi_mac_[3] = 0xA4;
	WiFi_mac_[4] = 0x46;
	WiFi_mac_[5] = 0x9D;

	UpdateWiFiData();

	Test();
}

MainFrame::~MainFrame()
{
	/* Остановка картографера обязательно должна быть выполнена
		до удаления всех объектов, использующихся в обработчике OnMapPaint */
	Cartographer->Stop();


	lets_finish();

	if (WiFi_sock_)
		shutdown(WiFi_sock_, 0);

	dismiss(WiFiScan_worker_);

	#ifndef NDEBUG
	debug_wait_for_finish(L"MainFrame", posix_time::seconds(5));
	#endif

	wait_for_finish();

	if (WiFi_data_)
		PQclear(WiFi_data_);

	PQfinish(pg_conn_);

	//(*Destroy(MainFrame)
	//*)
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

void MainFrame::DrawImage(int id, const cartographer::coord &pt, double alpha)
{
	if (id == 0)
		return;

	cartographer::point pos = Cartographer->CoordToScreen(pt);
	const double z = Cartographer->GetActiveZ();

	glColor4d(1.0, 1.0, 1.0, alpha);
	Cartographer->DrawImage(id, pos, z < 6.0 ? z / 6.0 : 1.0);
}

void MainFrame::DrawCircle(const cartographer::coord &pt,
	double r, double line_width, const cartographer::color &line_color,
	const cartographer::color &fill_color)
{
	const double z = Cartographer->GetActiveZ();

	cartographer::point pt_pos = Cartographer->CoordToScreen(pt);

	/* Сначала заполняем, потом рисуем окружность */
	for (int n = 0; n < 2; ++n)
	{
		if (n == 0)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
			glColor4dv(&fill_color.r);
		}
		else
		{
			glLineWidth( z >= 6.0 ? line_width : line_width * (z / 6.0) );
			glBegin(GL_LINE_LOOP);
			glColor4dv(&line_color.r);
		}

		const double step = 3.0;
		for (double a = step / 2.0; a < 360.0; a += step)
		{
			cartographer::coord ptN = cartographer::Direct(pt, a, r);
			cartographer::point ptN_pos = Cartographer->CoordToScreen(ptN);
			glVertex3d(ptN_pos.x, ptN_pos.y, 0);
		}
		glEnd();
	}
}

void MainFrame::DrawSimpleCircle(const cartographer::point &pos,
	double r, double line_width, const cartographer::color &line_color,
	const cartographer::color &fill_color)
{
	/* Сначала заполняем, потом рисуем окружность */
	for (int n = 0; n < 2; ++n)
	{
		if (n == 0)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
			glColor4dv(&fill_color.r);
		}
		else
		{
			glLineWidth(line_width);
			glBegin(GL_LINE_LOOP);
			glColor4dv(&line_color.r);
		}

		const double step = M_PI / 18.0;
		for (double a = step / 2.0; a < 2.0 * M_PI; a += step)
		{
			double x = r * cos(a);
			double y = r * sin(a);
			glVertex3d(pos.x + x, pos.y + y, 0);
		}
		glEnd();
	}
}

double MainFrame::DrawPath(const cartographer::coord &pt1,
	const cartographer::coord &pt2,
	double line_width, const cartographer::color &line_color,
	double *p_azimuth, double *p_rev_azimuth)
{
	/* Находим расстояние и начальный азимут */
	double azimuth;
	double distance = cartographer::Inverse(pt1, pt2, &azimuth, NULL, 1000.0);

	if (p_azimuth)
		*p_azimuth = azimuth;

	DrawPath(pt1, azimuth, distance, line_width, line_color, p_rev_azimuth);

	return distance;
}

cartographer::coord MainFrame::DrawPath(const cartographer::coord &pt,
	double azimuth, double distance,
	double line_width, const cartographer::color &line_color,
	double *p_rev_azimuth)
{
	const double z = Cartographer->GetActiveZ();

	glLineWidth( z >= 6.0 ? line_width : line_width * (z / 6.0) );
	glBegin(GL_LINE_STRIP);
	glColor4dv(&line_color.r);

	cartographer::coord ptN;
	cartographer::point ptN_pos;

	/* Делим путь на равные промежутки и вычисляем координаты узлов */
	for (int i = 0; i <= 100; ++i)
	{
		double d = distance / 100.0 * i;

		/* Сохраняем старое значение */
		cartographer::coord ptP = ptN;
		cartographer::point ptP_pos = ptN_pos;

		/* Получаем новое */
		ptN = cartographer::Direct(pt, azimuth, d, p_rev_azimuth);
		ptN_pos = Cartographer->CoordToScreen(ptN);

		/* Проверяем переход с одной стороны карты на другую */
		if (ptP.lon * ptN.lon < 0.0 && abs(ptN.lon) > 170.0)
		{
			/* Ищем среднюю точку, на которой произошёл переход */
			const double k = (180.0 - abs(ptP.lon)) / (360.0 - abs(ptN.lon - ptP.lon));
			const double mid_lat = ptP.lat + (ptN.lat - ptP.lat) * k;
			cartographer::coord ptM_N(mid_lat, ptN.lon < 0.0 ? -180.0 : 180.0);
			cartographer::coord ptM_P(mid_lat, ptP.lon < 0.0 ? -180.0 : 180.0);

			cartographer::point ptM_N_pos = Cartographer->CoordToScreen(ptM_N);
			cartographer::point ptM_P_pos = Cartographer->CoordToScreen(ptM_P);

			/* Дочерчиваем линию на предыдущей стороне */
			glVertex3d(ptM_P_pos.x, ptM_P_pos.y, 0);
			glEnd();

			/* ... и переходим на новую сторону */
			glBegin(GL_LINE_STRIP);
			glColor4dv(&line_color.r);
			glVertex3d(ptM_N_pos.x, ptM_N_pos.y, 0);

			#if 0
			/* Позиция новой точки на обратной стороне карты */
			cartographer::point ptN_pos_ = Cartographer->CoordToScreen( cartographer::coord(
				ptN.lat, ptN.lon < 0.0 ? ptN.lon + 360.0 : ptN.lon - 360.0) );

			/* Позиция старой точки на обратной стороне карты */
			cartographer::point ptP_pos_ = Cartographer->CoordToScreen( cartographer::coord(
				ptP.lat, ptP.lon < 0.0 ? ptP.lon + 360.0 : ptP.lon - 360.0) );

			/* Дочерчиваем линию на предыдущей стороне */
			glVertex3d(ptN_pos_.x, ptN_pos_.y, 0);
			glEnd();

			/* ... и переходим на новую сторону */
			glBegin(GL_LINE_STRIP);
			glColor4dv(&line_color.r);
			glVertex3d(ptP_pos_.x, ptP_pos_.y, 0);
			#endif
		}

		glVertex3d(ptN_pos.x, ptN_pos.y, 0);
	}
	glEnd();

	return ptN;
}

void MainFrame::OnMapPaint(double z, int width, int height)
{
	/*-
	int fields = PQnfields(pg_res_);
	for (i = 0; i < fields; i++)
		printf("%-15s ", PQfname(res, i));
	printf("\n\n");
	-*/

	if (z > 10.0)
	{
		unique_lock<mutex> l(WiFi_mutex_);
		if (WiFi_data_)
		{
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

				cartographer::point pos = Cartographer->CoordToScreen(pt);

				double power_k = (power + 100.0) / 110.0;

				if (power_k < 0.0)
					power_k = 0.0;

				if (power_k > 1.0)
					power_k = 1.0;

				if (power_k < 0.5)
				{
					DrawImage(green_mark16_id_, pt);
					DrawImage(yellow_mark16_id_, pt, 2.0 * power_k);
				}
				else
				{
					DrawImage(yellow_mark16_id_, pt);
					DrawImage(red_mark16_id_, pt, 2.0 * (power_k - 0.5));
				}

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
}

void MainFrame::OnChoice1Select(wxCommandEvent& event)
{
	int i = Choice1->GetCurrentSelection();
	Cartographer->MoveTo(z_[i], coords_[i]);
}

void MainFrame::OnZoomInButtonClick(wxCommandEvent& event)
{
	Cartographer->ZoomIn();
}

void MainFrame::OnZoomOutButtonClick(wxCommandEvent& event)
{
	Cartographer->ZoomOut();
}

void MainFrame::OnAnchorButtonClick(wxCommandEvent& event)
{
}

void MainFrame::OnWiFiScanButtonClicked(wxCommandEvent& event)
{
	unique_lock<mutex> lock(WiFi_mutex_);

	if (WiFiScan_worker_.use_count() != 2)
	{
		shutdown(WiFi_sock_, 0);
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
			shutdown(WiFi_sock_, 0);
			WiFi_sock_ = 0;
			lock.unlock();

			wxMessageBox(L"Error in connect()", L"Error", wxOK | wxICON_ERROR);
			ToolBar1->ToggleTool(ID_WIFISCAN, false);
			return;
		}

        boost::thread( boost::bind(
            &MainFrame::WiFiScanProc, this, WiFiScan_worker_) );

        ToolBar1->ToggleTool(ID_WIFISCAN, true);
	}
}

void MainFrame::WiFiScanProc(my::worker::ptr this_worker)
{
	while (!finish())
	{
		unsigned char buf[100];

		int sz = recv(WiFi_sock_, buf, 6, 0);

		if (sz < 6)
		{
			//fprintf(stderr, "Connection closed");
			break;
		}

		{
		    unique_lock<mutex> l(WiFi_mutex_);
            memcpy(WiFi_mac_, buf, 6);
		}

		UpdateWiFiData();
    }

    shutdown(WiFi_sock_, 0);
    WiFi_sock_ = 0;
}

void MainFrame::UpdateWiFiData()
{
	if (!PgConnect())
		return;

    wchar_t title[100] = { L"Scan Analitics" };

	unique_lock<mutex> lock(WiFi_mutex_);

	if (WiFi_data_)
		PQclear(WiFi_data_);

    if (memcmp(WiFi_mac_, "\0\0\0\0\0\0", 6) != 0)
    {
        swprintf( title, sizeof(title) / sizeof(*title),
            L"Scan Analitics (%02X:%02X:%02X:%02X:%02X:%02X)",
            WiFi_mac_[0], WiFi_mac_[1], WiFi_mac_[2],
            WiFi_mac_[3], WiFi_mac_[4], WiFi_mac_[5] );

        char buf[200];
        snprintf( buf, sizeof(buf) / sizeof(*buf),
            "SELECT * FROM wifi_scan_data"
            " WHERE station_mac=\'%02X:%02X:%02X:%02X:%02X:%02X\'",
            WiFi_mac_[0], WiFi_mac_[1], WiFi_mac_[2],
            WiFi_mac_[3], WiFi_mac_[4], WiFi_mac_[5] );

        WiFi_data_ = PQexec(pg_conn_, buf);

        if (PQresultStatus(WiFi_data_) != PGRES_TUPLES_OK)
        {
            fprintf(stderr, "Command failed: %s", PQerrorMessage(pg_conn_));
            PQclear(WiFi_data_);
            WiFi_data_ = 0;
        }
    }

    lock.unlock();
    SetTitle( (wchar_t*)title);
    //SetTitle(L"bbbbbbb123");
}
