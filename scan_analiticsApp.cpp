/***************************************************************
 * Name:      scan_analiticsApp.cpp
 * Purpose:   Code for Application Class
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#include "stdafx.h"

#include "scan_analiticsApp.h"
#include "handle_exception.h"

//(*AppHeaders
#include "MainFrame.h"
#include <wx/image.h>
//*)

#include <mylib.h>

#include <string>
#include <fstream>
#include <exception>

wxFileConfig *MyConfig = NULL;

my::log main_log(L"main.log", my::log::multiline);
my::log debug_log(L"debug.log", my::log::clean);

IMPLEMENT_APP(scan_analiticsApp);

bool scan_analiticsApp::OnInit()
{
	MY_REGISTER_THREAD("Main");

	#if wxUSE_ON_FATAL_EXCEPTION
	wxHandleFatalExceptions(true);
	#endif

	main_log << L"Start" << main_log;
	debug_log << L"Start" << debug_log;

	/* Открываем файл настроек */
	{
		std::wstring path = fs::system_complete(L"scan_analitics.cfg").string();
		MyConfig = new wxFileConfig(wxEmptyString, wxEmptyString, path);
	}

	//(*AppInitialize
	bool wxsOK = true;
	wxInitAllImageHandlers();
	if (wxsOK)
	{
		MainFrame* Frame = new MainFrame(0);
		Frame->Show();
		SetTopWindow(Frame);
	}
	//*)
	return wxsOK;

}

int scan_analiticsApp::OnExit()
{
	delete MyConfig;
	MyConfig = NULL;

	main_log << L"Finish" << main_log;
	return 0;
}

bool scan_analiticsApp::OnExceptionInMainLoop()
{
	try
	{
		throw;
	}
	catch (std::exception &e)
	{
		handle_exception(&e, L"in App::OnExceptionInMainLoop", L"Ошибка");
	}
	catch (...)
	{
		handle_exception(0, L"in App::OnExceptionInMainLoop", L"Ошибка");
	}

	return true;
}

void scan_analiticsApp::OnUnhandledException()
{
	try
	{
		throw;
	}
	catch (std::exception &e)
	{
		handle_exception(&e, L"in App::OnUnhandledException", L"Ошибка");
	}
	catch (...)
	{
		handle_exception(0, L"in App::OnUnhandledException", L"Ошибка");
	}
}

void scan_analiticsApp::OnFatalException()
{
	try
	{
		throw;
	}
	catch (std::exception &e)
	{
		handle_exception(&e, L"in App::OnFatalException", L"Критическая ошибка");
	}
	catch (...)
	{
		handle_exception(0, L"in App::OnFatalException", L"Критическая ошибка");
	}
}
