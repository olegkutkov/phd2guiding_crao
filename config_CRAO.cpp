/*
 *  config_CRAO.cpp
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

#include "config_CRAO.h"
#include "config_CRAO_motors.h"

#define MOTCFG 201

#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)

CraoConfig::CraoConfig(wxWindow *parent, modbus_t *mbctx) :
	wxDialog(parent, wxID_ANY, _("CrAO Mount"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	modbus_ctx = mbctx;	
	is_connected = false;

    auto sizerLabelFlags  = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL;
    auto sizerButtonFlags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL;
    auto sizerSectionFlags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL;
    auto sizerTextFlags = wxALIGN_LEFT | wxALL | wxEXPAND;
    int border = 2;

    int pos = 0;
    wxGridBagSizer *gbs = new wxGridBagSizer(0, 25);
    wxBoxSizer *sizer;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Serial device configuration")),
             POS(pos, 1), SPAN(0, 0), sizerSectionFlags, border);

	pos++;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Device: ")),
             POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

    dev = new wxComboBox(this, wxID_ANY, _T(""));

    gbs->Add(dev, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);

	pos++;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Baud: ")),
             POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

    baud = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	baud->Append(_("1200"));
	baud->Append(_("4800"));
	baud->Append(_("9600"));
	baud->Append(_("19200"));
	baud->Append(_("38400"));
	baud->Append(_("57600"));
	baud->Append(_("115200"));

    gbs->Add(baud, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Data bits: ")),
             POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

    dbits = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	dbits->Append(_("7"));
	dbits->Append(_("8"));

    gbs->Add(dbits, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Stop bits: ")),
             POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

    sbits = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	sbits->Append(_("1"));
	sbits->Append(_("2"));

    gbs->Add(sbits, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);


	pos++;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Parity: ")),
             POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

    parity = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	parity->Append(_("none"));
	parity->Append(_("odd"));
	parity->Append(_("even"));

    gbs->Add(parity, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);

	pos++;

    gbs->Add(new wxStaticText(this, wxID_ANY, _("Modbus ID: ")),
             POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

    mbid = new wxTextCtrl(this, wxID_ANY);

    gbs->Add(mbid, POS(pos, 1), SPAN(1, 1), sizerTextFlags, border);

    pos++;
    gbs->Add(new wxStaticText(this, wxID_ANY, _T("")),
		 POS(pos, 0), SPAN(1, 1), sizerLabelFlags, border);

	pos++;

	gbs->Add(new wxButton(this, MOTCFG, _("Motors cfg")), POS(pos, 1), SPAN(1, 1), sizerButtonFlags, border);


    sizer = new wxBoxSizer(wxVERTICAL) ;
    sizer->Add(gbs);
    sizer->AddSpacer(10);
    sizer->Add(CreateButtonSizer(wxOK | wxCANCEL));
    sizer->AddSpacer(10);
    SetSizer(sizer);
    sizer->SetSizeHints(this) ;
    sizer->Fit(this) ;
}

void CraoConfig::LoadSettings()
{
	char str[80];

	for (size_t i = 0; i < tty_devices.size(); i++) {
		dev->Append(tty_devices[i]);
	}

	dev->SetValue(curr_device);

	sprintf(str, "%d", curr_baud);
	baud->SetValue(str);
	memset(str, 0, sizeof(str));

	sprintf(str, "%d", curr_dbits);
	dbits->SetValue(str);
	memset(str, 0, sizeof(str));

	sprintf(str, "%d", curr_sbits);
	sbits->SetValue(str);
	memset(str, 0, sizeof(str));

	parity->SetValue(curr_parity);

	sprintf(str, "%d", curr_mbid);
	mbid->Clear();
	mbid->WriteText(str);
}

BEGIN_EVENT_TABLE(CraoConfig, wxDialog)
EVT_BUTTON(MOTCFG, CraoConfig::OnMotCfgButton)
END_EVENT_TABLE()

CraoConfig::~CraoConfig()
{
	///
}

void CraoConfig::SaveSettings()
{
	curr_device = dev->GetValue();
	curr_baud = wxAtoi(baud->GetValue());
	curr_dbits = wxAtoi(dbits->GetValue());
	curr_sbits = wxAtoi(sbits->GetValue());
	curr_parity = parity->GetValue();
	curr_mbid = wxAtoi(mbid->GetLineText(0));
}

void CraoConfig::OnMotCfgButton(wxCommandEvent& evt)
{
	if (!is_connected) {
		wxMessageBox(_("Please connect to the Mount hardware before configuring motors"), _("Information"), wxOK | wxICON_INFORMATION);
		return;
	}

	CraoConfigMotors *mcfgDlg = new CraoConfigMotors(wxGetActiveWindow(), modbus_ctx);

	if (mcfgDlg->ShowModal() == wxID_OK) {
		///
	}

	mcfgDlg->Destroy();

	delete mcfgDlg;
}

void CraoConfig::SetConnectedState()
{
	is_connected = true;
}

