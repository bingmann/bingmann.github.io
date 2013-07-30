// $Id: WMain.cpp 59 2007-07-17 14:43:23Z tb $

/*
 * STX Expression Parser Demo Program v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "WMain.h"

#include "ExpressionParser.h"

#include <map>

WMain::WMain()
    : WMain_wxg(NULL, -1, wxT(""))
{
    // set fixed monospace fonts for most controls
    
    wxFont sysoemfont = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);

    wxFont fixedfont(sysoemfont.GetPointSize(),
		     wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    textctrlExpression->SetFont(fixedfont);
    textctrlStringExpression->SetFont(fixedfont);
    textctrlResultType->SetFont(fixedfont);
    textctrlResultValue->SetFont(fixedfont);
    textctrlXmlTree->SetFont(fixedfont);

    gridVariables->SetLabelFont(fixedfont);
    gridVariables->SetDefaultCellFont(fixedfont);
    
    // xpm images for the buttons
    {
	#include "img/addrow.xpm"
	#include "img/delrow.xpm"

	buttonAddVariable->SetBitmapLabel( wxBitmap(addrow) );
	buttonDelVariable->SetBitmapLabel( wxBitmap(delrow) );
    }
    // xpm program images
    {
	#include "img/progicon16.xpm"
	#include "img/progicon32.xpm"

	wxIconBundle icons;
	icons.AddIcon(wxIcon(progicon16_xpm));
	icons.AddIcon(wxIcon(progicon32_xpm));

	SetIcons(icons);
    }

    SetSize(360, 480);
    textctrlExpression->SetFocus();

    splitterwindow->SetMinimumPaneSize(20);
    splitterwindow->SetSashSize(4);
    splitterwindow->SetSashGravity(0.5);
    splitterwindow->SetSashPosition( splitterwindow->GetSize().GetHeight() / 2 );
}

class VariableTable : public stx::BasicSymbolTable
{
public:
    typedef std::map<std::string, stx::AnyScalar> map_type;

    map_type			map;

    virtual stx::AnyScalar lookupVariable(const std::string &varname) const
    {
	map_type::const_iterator mi = map.find(varname);

	if (mi == map.end()) {
	    throw(stx::UnknownSymbolException(std::string("Unknown variable ") + varname));
	}

	return mi->second;
    }
};

void WMain::OnButtonEvaluate(wxCommandEvent &)
{
    if (notebookResults->GetSelection() == 0) {
	notebookResults->SetSelection(1);
    }

    // parse text into parse tree
    wxString text = textctrlExpression->GetValue();

    stx::ParseTree pt;

    try
    {
	pt = stx::parseExpression( std::string(text.mb_str()) );
	std::string strexpr = pt.toString();
	textctrlStringExpression->SetValue( wxString(strexpr.data(), wxConvUTF8, strexpr.size()) );
    }
    catch (stx::ExpressionParserException &e)
    {
	textctrlStringExpression->SetValue( wxString(e.what(), wxConvUTF8) );
	textctrlXmlTree->Clear();
	textctrlResultType->Clear();
	textctrlResultValue->Clear();
	return;
    }

    // load variable table
    VariableTable	vartable;
	
    for(int i = 0; i < gridVariables->GetNumberRows(); ++i)
    {
	wxString c0 = gridVariables->GetCellValue(i, 0);
	wxString c1 = gridVariables->GetCellValue(i, 1);

	vartable.map.insert( VariableTable::map_type::value_type(std::string(c0.mb_str()),
								 stx::AnyScalar().setAutoString( std::string(c1.mb_str()) )) );
    }

    // fill in xml tree
    std::string xmlstr = stx::parseExpressionXML( std::string(text.mb_str()) );
    textctrlXmlTree->SetValue( wxString(xmlstr.data(), wxConvUTF8, xmlstr.size()) );

    // evaluate parse tree with variables table
    try
    {
	stx::AnyScalar val = pt.evaluate(vartable);

	textctrlResultType->SetValue( wxString(val.getTypeString().c_str(), wxConvUTF8) );
	textctrlResultValue->SetValue( wxString(val.getString().c_str(), wxConvUTF8) );
    }
    catch (stx::ExpressionParserException &e)
    {
	textctrlResultType->SetValue( wxT("exception") );
	textctrlResultValue->SetValue( wxString(e.what(), wxConvUTF8) );
    }
}

void WMain::OnButtonAddVariable(wxCommandEvent &)
{
    gridVariables->AppendRows(1);
}

void WMain::OnButtonDelVariable(wxCommandEvent &)
{
    int selrow = gridVariables->GetGridCursorRow();
    if (selrow >= 0)
    {
	gridVariables->DeleteRows(selrow);
    }
    else
    {
	gridVariables->DeleteRows( gridVariables->GetNumberRows()-1 );
    }
}

BEGIN_EVENT_TABLE(WMain, wxFrame)

    EVT_BUTTON		(ID_BUTTON_EVALUATE,	WMain::OnButtonEvaluate)

    EVT_BUTTON		(ID_BUTTON_ADD_VARIABLE, WMain::OnButtonAddVariable)
    EVT_BUTTON		(ID_BUTTON_DEL_VARIABLE, WMain::OnButtonDelVariable)

END_EVENT_TABLE();

// *** Main Application

class AppParserDemo : public wxApp
{
public:
    bool 		OnInit();
};

IMPLEMENT_APP(AppParserDemo)

bool AppParserDemo::OnInit()
{
    wxInitAllImageHandlers();

    WMain* wm = new WMain();
    SetTopWindow(wm);
    wm->Show();

    return true;
}

