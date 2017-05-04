/*
 *  scope_INDI.cpp
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

#include "phd.h"
#include "config_ZTSH.h"
#include "scope_ztsh_hardware_comm.h"

#include <stdio.h>

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#ifdef GUIDE_ZTSH

ScopeZTSH::ScopeZTSH()
	: hwcomm (NULL)
{	
}

ScopeZTSH::~ScopeZTSH()
{
	
}

bool ScopeZTSH::Connect(void)
{
	bool connect_failed = false;

	wxString device = pConfig->Profile.GetString("/crao/ztsh/serial", _T(""));

	if (device.IsEmpty()) {
		wxMessageBox(_("Please select serial device in the mount configuration dialog"), _("Error"), wxOK | wxICON_ERROR);
		return true;
	}

#if defined (__linux__)
	device = wxString("/dev/") + device;
#elif defined (__WINDOWS__)
	device = wxString("\\\\.\\") + device;
#endif

	wxString parity = pConfig->Profile.GetString("/crao/ztsh/parity", "n");
	parity = parity.Capitalize();

	const char *parity_c = parity.mb_str();

	int baud = pConfig->Profile.GetInt("/crao/ztsh/baud", 9600);
	int dbits = pConfig->Profile.GetInt("/crao/ztsh/dbits", 8);
	int sbits = pConfig->Profile.GetInt("/crao/ztsh/sbits", 1);

	const char *device_c = device.mb_str();

	hwcomm = new ZtshHwComm();

	if (!hwcomm) {
		wxMessageBox(_("Failed to allocate memory for ZTSH hardware communicator"), _("Error"), wxOK | wxICON_ERROR);
		return true; // yes, means error
	}

	if (!hwcomm->ConfigureSerial(device_c, baud, parity_c[0], dbits, sbits)) {
		wxMessageBox(hwcomm->GetErrorText().c_str(), _("Error"), wxOK | wxICON_ERROR);
		return true;
	}

	hwcomm->ConfigureAdam( pConfig->Profile.GetInt("/crao/ztsh/adm_addr", 11),
							 pConfig->Profile.GetInt("/crao/ztsh/adm_pwr_channel", 0),
							 pConfig->Profile.GetInt("/crao/ztsh/adm_dec_minus_channel", 1),
							 pConfig->Profile.GetInt("/crao/ztsh/adm_dec_plus_channel", 2) );

	hwcomm->ConfigureInverters( pConfig->Profile.GetInt("/crao/ztsh/inv_hour_modbus_addr", 1),
								pConfig->Profile.GetInt("/crao/ztsh/inv_dec_modbus_addr", 2) );

	hwcomm->SetModbusDebug(pConfig->Profile.GetInt("/crao/ztsh/modbus_debug_mode", 0));
	hwcomm->SetModbusProtoRecovery(pConfig->Profile.GetInt("/crao/ztsh/modbus_protocol_recovery", 0));

	hwcomm->AdamEnableInverterPower();

	hwcomm->SetHourAxisSpeed(pConfig->Profile.GetInt("/crao/ztsh/inv_hour_norm_speed", 5035));

	Scope::Connect();

	return connect_failed;
}

bool ScopeZTSH::Disconnect(void)
{
	if (hwcomm) {
		hwcomm->StopDecAxis();
		hwcomm->AdamDisableInverterPower();

		delete hwcomm;
		hwcomm = NULL;
	}

	Scope::Disconnect();

	return true;
}

bool ScopeZTSH::HasSetupDialog(void) const
{
    return true;
}

void ScopeZTSH::EnumerateSerialDevices(std::vector<wxString>& devices)
{
#if defined (__linux__)

	DIR *ttydir;
	struct dirent *devdir;

	const char *topdir = "/sys/class/tty/";

	ttydir = opendir(topdir);

	if (!ttydir) {
		return;
	}

	while ((devdir = readdir(ttydir)) != NULL) {
		char path[100];
		int len = snprintf(path, sizeof(path) - 1, "%s/%s/device", topdir, devdir->d_name);
		path[len] = 0;

		DIR *ttydevdir = opendir(path);

		if (!ttydevdir) {
			continue;
		}

		devices.push_back(devdir->d_name);

		closedir(ttydevdir);
	}

	closedir(ttydir);

#elif defined (__WINDOWS__)
	/// TODO: implement me!
#endif
}

void ScopeZTSH::SetupDialog() 
{
    ZtshConfig *ztshDlg = new ZtshConfig(wxGetActiveWindow());

	ztshDlg->curr_device = pConfig->Profile.GetString("/crao/ztsh/serial", _T(""));
	ztshDlg->curr_baud = pConfig->Profile.GetInt("/crao/ztsh/baud", 9600);
	ztshDlg->curr_dbits = pConfig->Profile.GetInt("/crao/ztsh/dbits", 8);
	ztshDlg->curr_sbits = pConfig->Profile.GetInt("/crao/ztsh/sbits", 1);
	ztshDlg->curr_parity = pConfig->Profile.GetString("/crao/ztsh/parity", "none");
	ztshDlg->curr_adm_addr = pConfig->Profile.GetInt("/crao/ztsh/adm_addr", 11);
	ztshDlg->curr_adm_power_channel = pConfig->Profile.GetInt("/crao/ztsh/adm_pwr_channel", 0);
	ztshDlg->curr_adm_dec_plus_channel = pConfig->Profile.GetInt("/crao/ztsh/adm_dec_plus_channel", 2);
	ztshDlg->curr_adm_dec_minus_channel = pConfig->Profile.GetInt("/crao/ztsh/adm_dec_minus_channel", 1);
	ztshDlg->curr_inv_hour_modbus_addr = pConfig->Profile.GetInt("/crao/ztsh/inv_hour_modbus_addr", 1);
	ztshDlg->curr_inv_hour_low_speed = pConfig->Profile.GetInt("/crao/ztsh/inv_hour_low_speed", 4635);
	ztshDlg->curr_inv_hour_norm_speed = pConfig->Profile.GetInt("/crao/ztsh/inv_hour_norm_speed", 5035);
	ztshDlg->curr_inv_hour_high_speed = pConfig->Profile.GetInt("/crao/ztsh/inv_hour_high_speed", 6000);
	ztshDlg->curr_inv_dec_modbus_addr = pConfig->Profile.GetInt("/crao/ztsh/inv_dec_modbus_addr", 2);
	ztshDlg->curr_inv_dec_low_speed = pConfig->Profile.GetInt("/crao/ztsh/inv_dec_low_speed", 75);
	ztshDlg->curr_inv_dec_high_speed = pConfig->Profile.GetInt("/crao/ztsh/inv_dec_high_speed", 450);
	ztshDlg->curr_debug_mode = pConfig->Profile.GetInt("/crao/ztsh/modbus_debug_mode", 0);
	ztshDlg->curr_precovery_mode = pConfig->Profile.GetInt("/crao/ztsh/modbus_protocol_recovery", 0);

	EnumerateSerialDevices(ztshDlg->tty_devices);

    // initialize with actual values
	ztshDlg->LoadSettings();

    if (ztshDlg->ShowModal() == wxID_OK) {
		// if OK save the values to the current profile
		ztshDlg->SaveSettings();

		pConfig->Profile.SetString("/crao/ztsh/serial", ztshDlg->curr_device);
		pConfig->Profile.SetInt("/crao/ztsh/baud", ztshDlg->curr_baud);
		pConfig->Profile.SetInt("/crao/ztsh/dbits", ztshDlg->curr_dbits);
		pConfig->Profile.SetInt("/crao/ztsh/sbits", ztshDlg->curr_sbits);
		pConfig->Profile.SetString("/crao/ztsh/parity", ztshDlg->curr_parity);
		pConfig->Profile.SetInt("/crao/ztsh/adm_addr", ztshDlg->curr_adm_addr);
		pConfig->Profile.SetInt("/crao/ztsh/adm_pwr_channel", ztshDlg->curr_adm_power_channel);
		pConfig->Profile.SetInt("/crao/ztsh/adm_dec_plus_channel", ztshDlg->curr_adm_dec_plus_channel);
		pConfig->Profile.SetInt("/crao/ztsh/adm_dec_minus_channel", ztshDlg->curr_adm_dec_minus_channel);
		pConfig->Profile.SetInt("/crao/ztsh/inv_hour_modbus_addr", ztshDlg->curr_inv_hour_modbus_addr);
		pConfig->Profile.SetInt("/crao/ztsh/inv_hour_low_speed", ztshDlg->curr_inv_hour_low_speed);
		pConfig->Profile.SetInt("/crao/ztsh/inv_hour_norm_speed", ztshDlg->curr_inv_hour_norm_speed);
		pConfig->Profile.SetInt("/crao/ztsh/inv_hour_high_speed", ztshDlg->curr_inv_hour_high_speed);
		pConfig->Profile.SetInt("/crao/ztsh/inv_dec_modbus_addr", ztshDlg->curr_inv_dec_modbus_addr);
		pConfig->Profile.SetInt("/crao/ztsh/inv_dec_low_speed", ztshDlg->curr_inv_dec_low_speed);
		pConfig->Profile.SetInt("/crao/ztsh/inv_dec_high_speed", ztshDlg->curr_inv_dec_high_speed);
		pConfig->Profile.SetInt("/crao/ztsh/modbus_debug_mode", ztshDlg->curr_debug_mode);
		pConfig->Profile.SetInt("/crao/ztsh/modbus_protocol_recovery", ztshDlg->curr_precovery_mode);
    }

	ztshDlg->Destroy();

	delete ztshDlg;

	if (hwcomm) {
		hwcomm->SetModbusDebug(pConfig->Profile.GetInt("/crao/ztsh/modbus_debug_mode", 0));
		hwcomm->SetModbusProtoRecovery(pConfig->Profile.GetInt("/crao/ztsh/modbus_protocol_recovery", 0));
	}
}

bool ScopeZTSH::CanPulseGuide()
{
	return false;
}

static wxString PulseGuideFailedAlertEnabledKey()
{
	// we want the key to be under "/Confirm" so ConfirmDialog::ResetAllDontAskAgain() resets it, but we also want the setting to be per-profile
	return wxString::Format("/Confirm/%d/PulseGuideFailedAlertEnabled", pConfig->GetCurrentProfileId());
}

static void SuppressPulseGuideFailedAlert(long)
{
	pConfig->Global.SetBoolean(PulseGuideFailedAlertEnabledKey(), false);
}

Mount::MOVE_RESULT ScopeZTSH::Guide(GUIDE_DIRECTION direction, int duration)
{
	switch (direction) {
		case EAST:
			hwcomm->SetHourAxisSpeed(pConfig->Profile.GetInt("/crao/ztsh/inv_hour_high_speed", 6000));
			wxMilliSleep(duration);
			hwcomm->SetHourAxisSpeed(pConfig->Profile.GetInt("/crao/ztsh/inv_hour_norm_speed", 5035));

			break;

		case WEST:
			hwcomm->SetHourAxisSpeed(pConfig->Profile.GetInt("/crao/ztsh/inv_hour_low_speed", 4635));
			wxMilliSleep(duration);
			hwcomm->SetHourAxisSpeed(pConfig->Profile.GetInt("/crao/ztsh/inv_hour_norm_speed", 5035));

			break;

		case NORTH:
			hwcomm->SetDecAxisSpeed(DEC_DIRECTION_PLUS, pConfig->Profile.GetInt("/crao/ztsh/inv_dec_high_speed", 450));
			hwcomm->AdamRelayEnableDecPlus();
			wxMilliSleep(duration);
			hwcomm->StopDecAxis();
			hwcomm->AdamRelayDisableDecPlus();

			break;

		case SOUTH:
			hwcomm->SetDecAxisSpeed(DEC_DIRECTION_MINUS, pConfig->Profile.GetInt("/crao/ztsh/inv_dec_high_speed", 450));
			hwcomm->AdamRelayEnableDecMinus();
			wxMilliSleep(duration);
			hwcomm->StopDecAxis();
			hwcomm->AdamRelayDisableDecMinus();

			break;
	}

	return MOVE_OK;
}

#endif

