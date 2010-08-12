/***************************************************************
 * Name:      scan_analiticsApp.h
 * Purpose:   Defines Application Class
 * Author:    vi.k (vi.k@mail.ru)
 * Created:   2010-07-16
 * Copyright: vi.k ()
 * License:
 **************************************************************/

#ifndef APP_H
#define APP_H

#include <wx/app.h>

class scan_analiticsApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
		virtual bool OnExceptionInMainLoop();
	    virtual void OnUnhandledException();
	    virtual void OnFatalException();
};

#endif // APP_H
