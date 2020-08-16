#pragma once
#include <wx/wx.h>


class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    //static wxTextCtrl* m_txt_out ;
private:
    void OnOpenFile(wxCommandEvent& event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxTextCtrl* m_txt_in;
    wxTextCtrl* m_txt_out;
    wxString filein;
    //wxTextCtrl* textctrl=NULL;
    //wxTextCtrl* m_txt_in=NULL;		/*Text Box for input file*/

};
enum
{
    ID_Hello = 1,
    ID_btn1 = 1001,
    ID_btn2 = 1002
};

