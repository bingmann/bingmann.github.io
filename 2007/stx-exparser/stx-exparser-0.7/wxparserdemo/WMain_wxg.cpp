// -*- C++ -*- generated by wxGlade 0.5 on Tue Jun 12 19:49:49 2007 from /tdata/home/Desktop/stx-exparser/trunk/wxparserdemo/wxParserDemo.wxg

#include "WMain_wxg.h"


WMain_wxg::WMain_wxg(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: WMain_wxg::WMain_wxg
    panel_Main = new wxPanel(this, wxID_ANY);
    splitterwindow = new wxSplitterWindow(panel_Main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH);
    bottompane = new wxPanel(splitterwindow, wxID_ANY);
    notebookResults = new wxNotebook(bottompane, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    notebook_1_pane_3 = new wxPanel(notebookResults, wxID_ANY);
    notebook_1_pane_2 = new wxPanel(notebookResults, wxID_ANY);
    notebook_1_pane_1 = new wxPanel(notebookResults, wxID_ANY);
    toppane = new wxPanel(splitterwindow, wxID_ANY);
    panel_1 = new wxPanel(toppane, wxID_ANY);
    sizer_5_staticbox = new wxStaticBox(notebook_1_pane_2, -1, wxT("Parsed Expression String"));
    sizer_6_staticbox = new wxStaticBox(notebook_1_pane_2, -1, wxT("Evaluation Result"));
    sizer_2_staticbox = new wxStaticBox(panel_1, -1, wxT("Expression"));
    textctrlExpression = new wxTextCtrl(panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
    buttonEvaluate = new wxButton(panel_1, ID_BUTTON_EVALUATE, wxT("Evaluate"));
    gridVariables = new wxGrid(notebook_1_pane_1, ID_GRID_VARIABLES);
    buttonAddVariable = new wxBitmapButton(notebook_1_pane_1, ID_BUTTON_ADD_VARIABLE, wxNullBitmap);
    buttonDelVariable = new wxBitmapButton(notebook_1_pane_1, ID_BUTTON_DEL_VARIABLE, wxNullBitmap);
    textctrlStringExpression = new wxTextCtrl(notebook_1_pane_2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
    textctrlResultType = new wxTextCtrl(notebook_1_pane_2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    textctrlResultValue = new wxTextCtrl(notebook_1_pane_2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    textctrlXmlTree = new wxTextCtrl(notebook_1_pane_3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);

    set_properties();
    do_layout();
    // end wxGlade
}


void WMain_wxg::set_properties()
{
    // begin wxGlade: WMain_wxg::set_properties
    SetTitle(wxT("wxParserDemo"));
    gridVariables->CreateGrid(5, 2);
    gridVariables->SetRowLabelSize(0);
    gridVariables->SetColLabelValue(0, wxT("Name"));
    gridVariables->SetColLabelValue(1, wxT("Value"));
    buttonAddVariable->SetSize(buttonAddVariable->GetBestSize());
    buttonDelVariable->SetSize(buttonDelVariable->GetBestSize());
    // end wxGlade
}


void WMain_wxg::do_layout()
{
    // begin wxGlade: WMain_wxg::do_layout
    wxBoxSizer* sizer_Outside = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_Top = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_1_copy = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_7 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_6 = new wxStaticBoxSizer(sizer_6_staticbox, wxHORIZONTAL);
    wxStaticBoxSizer* sizer_5 = new wxStaticBoxSizer(sizer_5_staticbox, wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_2 = new wxStaticBoxSizer(sizer_2_staticbox, wxVERTICAL);
    sizer_2->Add(textctrlExpression, 1, wxALL|wxEXPAND, 4);
    sizer_2->Add(buttonEvaluate, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 4);
    panel_1->SetSizer(sizer_2);
    sizer_1->Add(panel_1, 1, wxALL|wxEXPAND, 4);
    toppane->SetSizer(sizer_1);
    sizer_3->Add(gridVariables, 1, wxLEFT|wxTOP|wxBOTTOM|wxEXPAND, 6);
    sizer_8->Add(5, 5, 1, wxADJUST_MINSIZE, 0);
    sizer_8->Add(buttonAddVariable, 0, wxALL, 6);
    sizer_8->Add(buttonDelVariable, 0, wxALL, 6);
    sizer_8->Add(5, 5, 1, wxADJUST_MINSIZE, 0);
    sizer_3->Add(sizer_8, 0, wxEXPAND, 0);
    notebook_1_pane_1->SetSizer(sizer_3);
    sizer_5->Add(textctrlStringExpression, 1, wxALL|wxEXPAND, 6);
    sizer_4->Add(sizer_5, 1, wxALL|wxEXPAND, 6);
    wxStaticText* label_1 = new wxStaticText(notebook_1_pane_2, wxID_ANY, wxT("Type"));
    sizer_6->Add(label_1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 6);
    sizer_6->Add(textctrlResultType, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 6);
    wxStaticText* label_2 = new wxStaticText(notebook_1_pane_2, wxID_ANY, wxT("Value"));
    sizer_6->Add(label_2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 6);
    sizer_6->Add(textctrlResultValue, 2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 6);
    sizer_4->Add(sizer_6, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 6);
    notebook_1_pane_2->SetSizer(sizer_4);
    sizer_7->Add(textctrlXmlTree, 1, wxALL|wxEXPAND, 6);
    notebook_1_pane_3->SetSizer(sizer_7);
    notebookResults->AddPage(notebook_1_pane_1, wxT("Variables"));
    notebookResults->AddPage(notebook_1_pane_2, wxT("Result"));
    notebookResults->AddPage(notebook_1_pane_3, wxT("XML Tree"));
    sizer_1_copy->Add(notebookResults, 1, wxALL|wxEXPAND, 4);
    bottompane->SetSizer(sizer_1_copy);
    splitterwindow->SplitHorizontally(toppane, bottompane);
    sizer_Top->Add(splitterwindow, 1, wxALL|wxEXPAND, 0);
    panel_Main->SetSizer(sizer_Top);
    sizer_Outside->Add(panel_Main, 1, wxEXPAND, 0);
    SetSizer(sizer_Outside);
    sizer_Outside->Fit(this);
    Layout();
    Centre();
    // end wxGlade
}

