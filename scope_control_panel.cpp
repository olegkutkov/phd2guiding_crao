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

#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)

BEGIN_EVENT_TABLE(ScopeControlPanel, wxWindow)
    EVT_PAINT(ScopeControlPanel::OnPaint)
    EVT_BUTTON(START_STOP_HA_DRIVER, ScopeControlPanel::OnStartStopButton)
    EVT_BUTTON(CONFIGURE_MOVE_SPEEDS, ScopeControlPanel::OnSpeedEditButton)
    EVT_BUTTON(SCOPE_FORCE_RECALIBRATION, ScopeControlPanel::OnCalibrateButton)
END_EVENT_TABLE()

ScopeControlPanel::ScopeControlPanel(wxWindow *parent):
	wxWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, "Scope control")
{
	this->SetMaxSize(wxSize(280, 450));

	visible = false;
	mount_connected = false;
	ha_enabled = false;
	cam_deg = 0;

	SetBackgroundColour(*wxBLACK);

	wxGridSizer *sizer = new wxGridSizer(5,3,1,1);

	// north
	#include "icons/north_stopped.h"
	wxBitmap north_stopped_bmp(wxBITMAP_PNG_FROM_DATA(north_stopped));

	#include "icons/north_running.h"
	wxBitmap north_running_bmp(wxBITMAP_PNG_FROM_DATA(north_running));

	northButton = new wxBitmapButton(this, MGUIDE1_UP, north_stopped_bmp);
	northButton->SetBitmapSelected(north_running_bmp);

	northButton->Connect(MGUIDE1_UP, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnNorthDown));
	northButton->Connect(MGUIDE1_UP, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnNorthUp));

	// north

	// south
	#include "icons/south_stopped.h"
	wxBitmap south_stopped_bmp(wxBITMAP_PNG_FROM_DATA(south_stopped));

	#include "icons/south_running.h"
	wxBitmap south_running_bmp(wxBITMAP_PNG_FROM_DATA(south_running));

	southButton = new wxBitmapButton(this, MGUIDE1_DOWN, south_stopped_bmp);
	southButton->SetBitmapSelected(south_running_bmp);

	southButton->Connect(MGUIDE1_DOWN, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnSouthDown));
	southButton->Connect(MGUIDE1_DOWN, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnSouthUp));

	// south

	// east
	#include "icons/east_stopped.h"
	wxBitmap east_stopped_bmp(wxBITMAP_PNG_FROM_DATA(east_stopped));

	#include "icons/east_running.h"
	wxBitmap east_running_bmp(wxBITMAP_PNG_FROM_DATA(east_running));

	eastButton = new wxBitmapButton(this, MGUIDE1_RIGHT, east_stopped_bmp);
	eastButton->SetBitmapSelected(east_running_bmp);

	eastButton->Connect(MGUIDE1_RIGHT, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnEastDown));
	eastButton->Connect(MGUIDE1_RIGHT, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnEastUp));

	// east

	// west
	#include "icons/west_stopped.h"
	wxBitmap west_stopped_bmp(wxBITMAP_PNG_FROM_DATA(west_stopped));

	#include "icons/west_running.h"
	wxBitmap west_running_bmp(wxBITMAP_PNG_FROM_DATA(west_running));

	westButton = new wxBitmapButton(this, MGUIDE1_LEFT, west_stopped_bmp);
	westButton->SetBitmapSelected(west_running_bmp);

	westButton->Connect(MGUIDE1_LEFT, wxEVT_LEFT_DOWN, wxCommandEventHandler(ScopeControlPanel::OnWestDown));
	westButton->Connect(MGUIDE1_LEFT, wxEVT_LEFT_UP, wxCommandEventHandler(ScopeControlPanel::OnWestUp));

	// west

    sizer->AddStretchSpacer();
    sizer->Add(northButton, wxSizerFlags().Expand().Border(wxALL,6));
    sizer->AddStretchSpacer();

	sizer->Add(westButton, wxSizerFlags().Expand().Border(wxALL,6));

    sizer->Add(southButton, wxSizerFlags().Expand().Border(wxALL,6));
    sizer->Add(eastButton, wxSizerFlags().Expand().Border(wxALL,6));

	sizer->Add(0, 0, 1);
	sizer->Add(0, 0, 1);
	sizer->Add(0, 0, 1);
	sizer->Add(0, 0, 1);
	sizer->Add(0, 0, 1);

	sizer->Add(new wxButton(this, START_STOP_HA_DRIVER, wxT("Start/Stop")), wxSizerFlags().Expand().Border(wxALL,4));

//	sizer->Add(0, 0, 1);
//	sizer->Add(0, 0, 1);
//	sizer->Add(0, 0, 1);

	sizer->Add(new wxButton(this, CONFIGURE_MOVE_SPEEDS, wxT("Set speeds")), wxSizerFlags().Expand().Border(wxALL,4));
	sizer->Add(new wxButton(this, SCOPE_FORCE_RECALIBRATION, wxT("Calibration")), wxSizerFlags().Expand().Border(wxALL,4));

	wxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
	sizer2->Add(sizer, 0, wxEXPAND, 10);

	SetSizerAndFit(sizer2);
}

ScopeControlPanel::~ScopeControlPanel()
{
}



void ScopeControlPanel::OnNorthDown(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(NORTH, true);
	event.Skip();
}

void ScopeControlPanel::OnNorthUp(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(NORTH, false);
	event.Skip();
}

void ScopeControlPanel::OnSouthDown(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(SOUTH, true);
	event.Skip();
}

void ScopeControlPanel::OnSouthUp(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(SOUTH, false);
	event.Skip();
}

void ScopeControlPanel::OnEastDown(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(EAST, true);
	event.Skip();
}

void ScopeControlPanel::OnEastUp(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(EAST, false);
	event.Skip();
}

void ScopeControlPanel::OnWestDown(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(WEST, true);
	event.Skip();
}

void ScopeControlPanel::OnWestUp(wxCommandEvent &event)
{
	pFrame->ScheduleManualMove(WEST, false);
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

void ScopeControlPanel::SetMountConnected(bool connected)
{
	mount_connected = connected;
	ha_enabled = connected;

	if (visible) {
		Refresh();
	}
}

void ScopeControlPanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	wxAutoBufferedPaintDC dc(this);

#if defined (__APPLE__)
	const wxFont& smallFont = *wxSMALL_FONT;
#else
	const wxFont& smallFont = *wxSWISS_FONT;
#endif

	dc.SetBackground(wxColour(0,0,0));
	dc.Clear();

	const int ysize = this->GetSize().GetY();
	wxFont largeFont = smallFont.Scaled(1.25);
	dc.SetFont(largeFont);

	dc.SetTextForeground(*wxWHITE);

	dc.DrawText("Mount: ", 5, 105);

	if (!mount_connected) {
		dc.SetTextForeground(*wxRED);
		dc.DrawText("DISCONNECTED", 100, 105);
	} else {
		dc.SetTextForeground(*wxGREEN);
		dc.DrawText("CONNECTED", 100, 105);
	}

	dc.SetTextForeground(*wxWHITE);

	dc.DrawText("HA Driver: ", 5, 146);

	bool ha_driver_running = mount_connected && ha_enabled;

	if (!ha_driver_running) {
		dc.SetTextForeground(*wxRED);
		dc.DrawText("STOPPED", 100, 146);
	} else {
		dc.SetTextForeground(*wxGREEN);
		dc.DrawText("RUNNING", 100, 146);
	}
	
	dc.SetTextForeground(*wxWHITE);

	dc.DrawText("Camera angle: ", 5, 235);

	dc.DrawText(wxString::Format(wxT("%.2f deg"), cam_deg), 130, 235);
}


void ScopeControlPanel::SetCameraAngle(double ang)
{
	cam_deg = ang;

	if (visible) {
		Refresh();
	}
}

void ScopeControlPanel::SetState(bool is_active)
{
	visible = is_active;

	if (is_active) {
		Refresh();
	}
}

void ScopeControlPanel::OnStartStopButton(wxCommandEvent& evt)
{
	if (!mount_connected) {
		return;
	}

	if (ha_enabled) {
		pMount->StopHADriver();
	} else {
		pMount->StartHADriver();
	}

	ha_enabled = !ha_enabled;

	if (visible) {
		Refresh();
	}
}

void ScopeControlPanel::OnCalibrateButton(wxCommandEvent& evt)
{
	if (!mount_connected) {
		return;
	}

	if (!pCamera || !pCamera->Connected) {
		wxMessageBox(_T("Please connect camera before calibration"));
		return;
	}

	if (pFrame->pGuider->GetState() < STATE_SELECTED) {
		wxMessageBox(_T("Please select a guide star before attempting to calibrate"));
		return;
	}

	bool recalibrate = ConfirmDialog::Confirm(_("Are you sure you want force recalibration?"),
											"/force_recalibration_ok", _("Force Recalibration"));

	if (recalibrate) {
		pMount->ClearCalibration();
		pFrame->StartGuiding();
	}
}

void ScopeControlPanel::OnSpeedEditButton(wxCommandEvent& evt)
{
	
}

