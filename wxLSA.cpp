// wxWidgets-micro-sample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "MyFrame.h"
class MyApp : public wxApp
{
public:

    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame(wxT("LSA"), wxPoint(20, 20), wxSize(900, 800));
    frame->Show(true);
    return true;
}
DECLARE_APP(MyApp);
