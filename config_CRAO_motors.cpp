/*
 *  config_CRAO_motors.cpp
 *  PHD Guiding
 *
 *  Copyright (c) 2017 Oleg Kutkov
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
 *    Neither the name of Craig Stark, Stark Labs nor the names of its 
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
 */

#include "config_CRAO_motors.h"

#define MREAD 202
#define MWRITE 203

#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)

CraoConfigMotors::CraoConfigMotors(wxWindow *parent, modbus_t *mbctx) :
	wxDialog(parent, wxID_ANY, _("CrAO Mount motors configuration"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	modbus_ctx = mbctx;

    auto sizerLabelFlags  = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL;
    auto sizerButtonFlags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL;
    auto sizerSectionFlags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL;
    auto sizerTextFlags = wxALIGN_LEFT | wxALL | wxEXPAND;
    int border = 2;

    int pos = 0;
    wxGridBagSizer *gbs = new wxGridBagSizer(0, 20);
    wxBoxSizer *sizer;

//	gbs->Add(new wxStaticText(this, wxID_ANY, _("Read/Write Mount hardware EEPROM configuration")),
//			POS(pos, 0), SPAN(0, 0), sizerSectionFlags, border);

	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _("RA acceleration factor: ")),
		POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	raaccel = new wxTextCtrl(this, wxID_ANY);
	gbs->Add(raaccel, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _("RA deceleration factor: ")),
		POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	radecel = new wxTextCtrl(this, wxID_ANY);
	gbs->Add(radecel, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _("DEC acceleration factor: ")),
		POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	decaccel = new wxTextCtrl(this, wxID_ANY);
	gbs->Add(decaccel, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _("DEC deceleration factor: ")),
		POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	decdecel = new wxTextCtrl(this, wxID_ANY);
	gbs->Add(decdecel, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _("RA max speed: ")),
		POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	ramaxspeed = new wxTextCtrl(this, wxID_ANY);
	gbs->Add(ramaxspeed, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _("DEC max speed: ")),
		POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	decmaxspeed = new wxTextCtrl(this, wxID_ANY);
	gbs->Add(decmaxspeed, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;
	gbs->Add(new wxStaticText(this, wxID_ANY, _T("")), POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	pos++;
    gbs->Add(new wxButton(this, MREAD, _("Read")), POS(pos, 1), SPAN(1, 1), sizerButtonFlags, border);

	pos++;
	gbs->Add(new wxButton(this, MWRITE, _("Write")), POS(pos, 1), SPAN(1, 1), sizerButtonFlags, border);


    sizer = new wxBoxSizer(wxVERTICAL) ;
    sizer->Add(gbs);
    sizer->AddSpacer(10);
    sizer->Add(CreateButtonSizer(wxOK));
    sizer->AddSpacer(10);
    SetSizer(sizer);
    sizer->SetSizeHints(this) ;
    sizer->Fit(this) ;
}

BEGIN_EVENT_TABLE(CraoConfigMotors, wxDialog) 
EVT_BUTTON(MREAD, CraoConfigMotors::OnReadButton)
EVT_BUTTON(MWRITE, CraoConfigMotors::OnWriteButton)
END_EVENT_TABLE()

CraoConfigMotors::~CraoConfigMotors()
{
	
}

void CraoConfigMotors::OnReadButton(wxCommandEvent& evt)
{
	uint16_t val;
	char str[80];

	raaccel->Disable();
	radecel->Disable();
	decaccel->Disable();
	decdecel->Disable();
	ramaxspeed->Disable();
	decmaxspeed->Disable();

	modbus_read_registers(modbus_ctx, 0, 1, &val);
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", val);
	raaccel->Clear();
	raaccel->WriteText(str);

	modbus_read_registers(modbus_ctx, 1, 1, &val);
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", val);
	radecel->Clear();
	radecel->WriteText(str);

	modbus_read_registers(modbus_ctx, 2, 1, &val);
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", val);
	decaccel->Clear();
	decaccel->WriteText(str);

	modbus_read_registers(modbus_ctx, 3, 1, &val);
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", val);
	decdecel->Clear();
	decdecel->WriteText(str);

	modbus_read_registers(modbus_ctx, 4, 1, &val);
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", val);
	ramaxspeed->Clear();
	ramaxspeed->WriteText(str);

	modbus_read_registers(modbus_ctx, 5, 1, &val);
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", val);
	decmaxspeed->Clear();
	decmaxspeed->WriteText(str);

	raaccel->Enable();
	radecel->Enable();
	decaccel->Enable();
	decdecel->Enable();
	ramaxspeed->Enable();
	decmaxspeed->Enable();
}

void CraoConfigMotors::OnWriteButton(wxCommandEvent& evt)
{
	
}

