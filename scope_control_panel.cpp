/*
 *  scope_control_panel.cpp
 *  PHD Guiding
 *
 *  Created by Oleg Kutkov
 *  Copyright (c) 2017 Crimean Astrophysical observatory
 *  All rights reserved.
 *
 *  This source code is distributed under the following "BSD" license
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *    Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *    Neither the name of Bret McKee, Dad Dog Development Ltd, nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#include "phd.h"
#include "scope_control_panel.h"

BEGIN_EVENT_TABLE(ScopeControlPanel, wxWindow)
    EVT_PAINT(ScopeControlPanel::OnPaint)
    EVT_BUTTON(MGUIDE1_UP,ScopeControlPanel::OnButton)
    EVT_BUTTON(MGUIDE1_DOWN,ScopeControlPanel::OnButton)
    EVT_BUTTON(MGUIDE1_RIGHT,ScopeControlPanel::OnButton)
    EVT_BUTTON(MGUIDE1_LEFT,ScopeControlPanel::OnButton)
END_EVENT_TABLE()

ScopeControlPanel::ScopeControlPanel(wxWindow *parent):
	wxWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, "Scope control")
{
	visible = false;
	mount_connected = false;

	SetBackgroundColour(*wxBLACK);

	wxGridSizer *sizer = new wxGridSizer(3,3,0,0);

//	wxButton *NButton, *SButton, *EButton, *WButton;

//    NButton = new wxButton(this, MGUIDE1_UP, "North", wxPoint(-1,-1),wxSize(-1,-1));
//	NButton->Connect(MGUIDE1_UP, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnNorthDown));
//	NButton->Connect(MGUIDE1_UP, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnNorthUp));

//    SButton = new wxButton(this, MGUIDE1_DOWN, "South", wxPoint(-1,-1),wxSize(-1,-1));
//	SButton->Connect(MGUIDE1_DOWN, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnSouthDown));
//	SButton->Connect(MGUIDE1_DOWN, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnSouthUp));

//    EButton = new wxButton(this, MGUIDE1_RIGHT, "East", wxPoint(-1,-1),wxSize(-1,-1));
//	EButton->Connect(MGUIDE1_RIGHT, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnEastDown));
//	EButton->Connect(MGUIDE1_RIGHT, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnEastUp));

//  WButton = new wxButton(this, MGUIDE1_LEFT, "West", wxPoint(-1,-1),wxSize(-1,-1));
//	WButton->Connect(MGUIDE1_LEFT, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnWestDown));
//	WButton->Connect(MGUIDE1_LEFT, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnWestUp));

	// north
	#include "icons/north_stopped.h"
	wxBitmap north_stopped_bmp(wxBITMAP_PNG_FROM_DATA(north_stopped));

	#include "icons/north_running.h"
	wxBitmap north_running_bmp(wxBITMAP_PNG_FROM_DATA(north_running));

	northButton = new wxBitmapButton(this, MGUIDE1_UP, north_stopped_bmp);
	northButton->SetBitmapSelected(north_running_bmp);

	// north

	// south
	#include "icons/south_stopped.h"
	wxBitmap south_stopped_bmp(wxBITMAP_PNG_FROM_DATA(south_stopped));

	#include "icons/south_running.h"
	wxBitmap south_running_bmp(wxBITMAP_PNG_FROM_DATA(south_running));

	southButton = new wxBitmapButton(this, MGUIDE1_DOWN, south_stopped_bmp);
	southButton->SetBitmapSelected(south_running_bmp);

	// south

	// east
	#include "icons/east_stopped.h"
	wxBitmap east_stopped_bmp(wxBITMAP_PNG_FROM_DATA(east_stopped));

	#include "icons/east_running.h"
	wxBitmap east_running_bmp(wxBITMAP_PNG_FROM_DATA(east_running));

	eastButton = new wxBitmapButton(this, MGUIDE1_LEFT, east_stopped_bmp);
	eastButton->SetBitmapSelected(east_running_bmp);

	// east

	// west
	#include "icons/west_stopped.h"
	wxBitmap west_stopped_bmp(wxBITMAP_PNG_FROM_DATA(west_stopped));

	#include "icons/west_running.h"
	wxBitmap west_running_bmp(wxBITMAP_PNG_FROM_DATA(west_running));

	westButton = new wxBitmapButton(this, MGUIDE1_RIGHT, west_stopped_bmp);
	westButton->SetBitmapSelected(west_running_bmp);

	// west

    sizer->AddStretchSpacer();
    sizer->Add(northButton, wxSizerFlags().Expand().Border(wxALL,6));
    sizer->AddStretchSpacer();

	sizer->Add(westButton, wxSizerFlags().Expand().Border(wxALL,6));

    sizer->Add(southButton, wxSizerFlags().Expand().Border(wxALL,6));
    sizer->Add(eastButton, wxSizerFlags().Expand().Border(wxALL,6));

	wxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
	sizer2->Add(sizer, 0, wxEXPAND, 10);

	SetSizerAndFit(sizer2);

	//this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ScopeControlPanel::OnKeyDown), NULL, this);
}

ScopeControlPanel::~ScopeControlPanel()
{
}

void ScopeControlPanel::OnNorthDown(wxCommandEvent &event)
{
	std::cout << "N down" << std::endl;
	event.Skip();
}

void ScopeControlPanel::OnNorthUp(wxCommandEvent &event)
{
	std::cout << "N up" << std::endl;
	event.Skip();
}

void ScopeControlPanel::OnSouthDown(wxCommandEvent &event)
{
	std::cout << "S down" << std::endl;
	event.Skip();
}

void ScopeControlPanel::OnSouthUp(wxCommandEvent &event)
{
	std::cout << "s up" << std::endl;
	event.Skip();
}

void ScopeControlPanel::OnEastDown(wxCommandEvent &event)
{
	std::cout << "E down" << std::endl;
	event.Skip();
}

void ScopeControlPanel::OnEastUp(wxCommandEvent &event)
{
	std::cout << "E up" << std::endl;
	event.Skip();
}

void ScopeControlPanel::OnWestDown(wxCommandEvent &event)
{

	event.Skip();
}

void ScopeControlPanel::OnWestUp(wxCommandEvent &event)
{
	std::cout << "W up" << std::endl;
	event.Skip();
}

void ScopeControlPanel::KeyNorthDown()
{
	
}

void ScopeControlPanel::KeyNorthUp()
{
	
}

void ScopeControlPanel::KeySouthDown()
{
	
}

void ScopeControlPanel::KeySouthUp()
{
	
}

void ScopeControlPanel::KeyWestDown()
{
	
}

void ScopeControlPanel::KeyWestUp()
{
	
}

void ScopeControlPanel::KeyEastDown()
{
	
}

void ScopeControlPanel::KeyEastUp()
{
	
}


void ScopeControlPanel::SetMountConnected(bool state)
{
	mount_connected = state;
}

void ScopeControlPanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
//	wxAutoBufferedPaintDC dc(this);

//	dc.SetBackground(wxColour(0,0,0));
//	dc.Clear();
}

void ScopeControlPanel::SetState(bool is_active)
{
	visible = is_active;

	if (is_active) {
		Refresh();
	}
}


void ScopeControlPanel::OnButton(wxCommandEvent &evt)
{
	
}

