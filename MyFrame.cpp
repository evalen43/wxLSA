#include "MyFrame.h"
#include <wx/wx.h>
#include  <wx/artprov.h> 
#include <wx/wfstream.h>
#include  <wx/textfile.h>  
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/app.h>
//#include <wx/textctrl.h>

#include "resources/mondrian.xpm"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{

#pragma region Menu



    /*File Menu
    ------------*/
    wxMenu* menuFile = new wxMenu;
    wxMenuItem* openItem = new wxMenuItem(menuFile, wxID_OPEN);
    wxMenuItem* quitItem = new wxMenuItem(menuFile, wxID_EXIT);
    wxMenuItem* saveItem = new wxMenuItem(menuFile, wxID_SAVE);
    saveItem->SetBitmap(wxArtProvider::GetBitmap("wxART_FILE_SAVE"));
    quitItem->SetBitmap(wxArtProvider::GetBitmap("wxART_QUIT"));
    openItem->SetBitmap(wxArtProvider::GetBitmap("wxART_FILE_OPEN"));
    menuFile->Append(openItem);
    menuFile->Append(saveItem);
    //menuFile->Append(ID_Hello, wxT("&Hello...\tCtrl-H",
    //    "Help string shown in status bar for this menu item"));
    menuFile->AppendSeparator();
    menuFile->Append(quitItem);

    /*Help Menu----
    ---------------*/
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    /*Menu Bar----
    ---------------*/
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    SetIcon(wxIcon("resources/mondrian_xpm"));
    //textctrl = new wxTextCtrl(this, -1, wxT("Log Screen"), wxPoint(-1, -1),
    //    wxSize(250, 150));

    /*File Menu Binding Events
    ---------------------------*/
    Bind(wxEVT_MENU, &MyFrame::OnOpenFile, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

#pragma endregion Menu

    /*Panel
    --------*/
#pragma region Panel

    wxPanel* panel = new wxPanel(this, -1);
    m_txt_in = new wxTextCtrl(panel, wxID_ANY, "This box shows the content of the input file....",
        wxDefaultPosition, wxSize(400, 550), wxTE_MULTILINE);
    m_txt_out = new wxTextCtrl(panel, wxID_ANY, "This box shows the results for the *.out file....",
        wxDefaultPosition, wxSize(400, 550), wxTE_MULTILINE);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);

    wxButton* ok = new wxButton(panel, ID_btn2, wxT("Open File"));
    wxButton* cancel = new wxButton(panel, ID_btn1, wxT("Exit"));
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnExit, this, ID_btn1);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::OnOpenFile, this, ID_btn2);

    hbox1->Add(new wxPanel(panel, -1));
    vbox->Add(hbox1, 1, wxEXPAND);

    hbox2->Add(m_txt_in);
    hbox2->Add(m_txt_out);
    hbox3->Add(ok);
    hbox3->Add(cancel);

    vbox->Add(hbox2, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
    vbox->Add(hbox3, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
    panel->SetSizer(vbox);

    Centre();
}

#pragma endregion Panel

/*-----------------------Methods------------------------------*/

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    if (wxMessageBox("Exit the program?", "Select YES to Quit", wxICON_QUESTION | wxYES_NO) == wxYES)
    {
        Close(true);
    }
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
        "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnOpenFile(wxCommandEvent& event)
{
    /* Create a new wxFileDialog dialog
    ---------------------------------------*/
    wxString wildcard = "XML Files (*.xml)|*.xml";
    wxFileDialog* openFileDialog = new wxFileDialog(this, wxT("Choose a file"), _(""), _(""), _("*.xml"), wxFD_OPEN);

    /*Display the dialog and transfer the contents to the wxTextCtrl on the main
    frame if the user doesn't cancel
    -----------------------------------------------------------*/

    if (openFileDialog->ShowModal() == wxID_CANCEL)
        return;
    filein = openFileDialog->GetPath();
    SetStatusText(filein);
    //wxLogMessage(wxT("File input %s"), filein);
    wxFileInputStream input_stream(filein);
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", filein);

    }

    wxString s;
    wxTextFile tfile(filein);
    tfile.Open();
    m_txt_in->Clear();
    for (s = tfile.GetFirstLine(); !tfile.Eof();
        s = tfile.GetNextLine())
    {
        //wxLogMessage(s);
        m_txt_in->WriteText(s + "\n");
    }
    tfile.Close();
    //ParseXml();
    openFileDialog->Destroy();
}
