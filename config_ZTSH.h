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

class ZtshConfig : public wxDialog {
public:
	ZtshConfig(wxWindow *parent);
	~ZtshConfig();

	void LoadSettings();
	void SaveSettings();

	std::vector<wxString> tty_devices;
	wxString curr_device;
	int curr_baud;
	int curr_dbits;
	int curr_sbits;
	wxString curr_parity;

	int curr_adm_addr;
	int curr_adm_power_channel;
	int curr_adm_dec_plus_channel;
	int curr_adm_dec_minus_channel;

	int curr_inv_hour_modbus_addr;
	int curr_inv_hour_low_speed;
	int curr_inv_hour_norm_speed;
	int curr_inv_hour_high_speed;

	int curr_inv_dec_modbus_addr;
	int curr_inv_dec_low_speed;
	int curr_inv_dec_high_speed;

	bool curr_debug_mode;
	bool curr_precovery_mode;

private:
	void OnMotCfgButton(wxCommandEvent& evt);

	wxComboBox *dev;
	wxComboBox *baud;
	wxComboBox *dbits;
	wxComboBox *sbits;
	wxComboBox *parity;

	wxTextCtrl *adm_addr;
	wxTextCtrl *adm_power_channel;
	wxTextCtrl *adm_dec_plus_channel;
	wxTextCtrl *adm_dec_minus_channel;

	wxTextCtrl *inv_hour_modbus_addr;
	wxTextCtrl *inv_hour_low_speed;
	wxTextCtrl *inv_hour_norm_speed;
	wxTextCtrl *inv_hour_high_speed;

	wxTextCtrl *inv_dec_modbus_addr;
	wxTextCtrl *inv_dec_low_speed;
	wxTextCtrl *inv_dec_high_speed;

	wxCheckBox *debug_checkbox;
	wxCheckBox *precovery_checkbox;

	DECLARE_EVENT_TABLE()
};

#endif

