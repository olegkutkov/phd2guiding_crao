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
//	SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->visible = false;

	SetBackgroundColour(*wxBLACK);

	wxGridSizer *sizer = new wxGridSizer(3,3,0,0);

	wxButton *NButton1, *SButton1, *EButton1, *WButton1;

    NButton1 = new wxButton(this, MGUIDE1_UP, "North", wxPoint(-1,-1),wxSize(-1,-1));
    SButton1 = new wxButton(this, MGUIDE1_DOWN, "South", wxPoint(-1,-1),wxSize(-1,-1));
    EButton1 = new wxButton(this, MGUIDE1_RIGHT, "West", wxPoint(-1,-1),wxSize(-1,-1));
    WButton1 = new wxButton(this, MGUIDE1_LEFT, "East", wxPoint(-1,-1),wxSize(-1,-1));

    sizer->AddStretchSpacer();
    sizer->Add(NButton1,wxSizerFlags().Expand().Border(wxALL,6));
    sizer->AddStretchSpacer();
    sizer->Add(WButton1,wxSizerFlags().Expand().Border(wxALL,6));
    sizer->AddStretchSpacer();
    sizer->Add(EButton1,wxSizerFlags().Expand().Border(wxALL,6));
    sizer->AddStretchSpacer();
    sizer->Add(SButton1,wxSizerFlags().Expand().Border(wxALL,6));

	wxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
	sizer2->Add(sizer, 0, wxEXPAND, 10);

	SetSizerAndFit(sizer2);
}

ScopeControlPanel::~ScopeControlPanel()
{
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

