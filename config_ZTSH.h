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

#ifndef _CONFIG_CRAO_H_
#define _CONFIG_CRAO_H_

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <vector>
#include <modbus/modbus.h>

class CraoConfig : public wxDialog {
public:
	CraoConfig(wxWindow *parent, modbus_t *mbctx);
	~CraoConfig();

	void LoadSettings();
	void SaveSettings();

	void SetConnectedState();

	std::vector<wxString> tty_devices;
	wxString curr_device;
	int curr_baud;
	int curr_dbits;
	int curr_sbits;
	wxString curr_parity;
	int curr_adm_mbid;
	int curr_adm_ra_channel;
	int curr_adm_dec_channel;

private:
	void OnMotCfgButton(wxCommandEvent& evt);

	wxComboBox *dev;
	wxComboBox *baud;
	wxComboBox *dbits;
	wxComboBox *sbits;
	wxComboBox *parity;
	wxTextCtrl *adm_mbid;
	wxTextCtrl *adm_ra_channel;
	wxTextCtrl *adm_dec_channel;;

	modbus_t *modbus_ctx;

	bool is_connected;

	DECLARE_EVENT_TABLE()
};

#endif

