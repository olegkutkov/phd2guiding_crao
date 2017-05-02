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

#include <stdio.h>

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef GUIDE_ZTSH

ScopeZTSH::ScopeZTSH()
	: ctx (NULL)
{	
}

ScopeZTSH::~ScopeZTSH()
{
	
}

bool ScopeZTSH::Connect(void)
{
	bool connect_failed = false;

	wxString device = pConfig->Profile.GetString("/crao/serial", _T(""));

	if (device.IsEmpty()) {
		wxMessageBox(_("Please select serial device in the mount configuration dialog"), _("Error"), wxOK | wxICON_ERROR);
		return true;
	}

#if defined (__linux__)
	device = wxString("/dev/") + device;
#elif defined (__WINDOWS__)
	device = wxString("\\\\.\\") + device;
#endif

	wxString parity = pConfig->Profile.GetString("/crao/parity", "n");
	parity = parity.Capitalize();

	const char *parity_c = parity.mb_str();

	int baud = pConfig->Profile.GetInt("/crao/baud", 9600);
	int dbits = pConfig->Profile.GetInt("/crao/dbits", 8);
	int sbits = pConfig->Profile.GetInt("/crao/sbits", 1);

	const char *device_c = device.mb_str();

	ctx = modbus_new_rtu(device_c, baud, parity_c[0], dbits, sbits);

	if (!ctx) {
		wxMessageBox(_("Failed to initialize modbus using current serial configuration"), _("Error"), wxOK | wxICON_ERROR);
		return true;
	}

	int mbid = pConfig->Profile.GetInt("/crao/mbid", 10);

	if (modbus_set_slave(ctx, mbid) != 0) {
		wxMessageBox(_("Failed to set selected modbus slave id"), _("Error"), wxOK | wxICON_ERROR);
		modbus_free(ctx);
		return true;
	}

	if (modbus_connect(ctx) != 0) {
		wxMessageBox(wxString("Modbus connection failed with error: ") + wxString(modbus_strerror(errno)), _("Error"), wxOK | wxICON_ERROR);
		modbus_free(ctx);
		return true;
	}

	uint16_t hw_answ;

	if (modbus_read_registers(ctx, 70, 1, &hw_answ) == -1) {
		wxMessageBox(wxString("Failed to communicate with the Mount hardware, modbus error: ") + wxString(modbus_strerror(errno)), _("Error"), wxOK | wxICON_ERROR);
		modbus_close(ctx);
		modbus_free(ctx);
		return true;
	}

	if (hw_answ != 100) {
		wxMessageBox(wxString("Failed to communicate with the Mount hardware, strange answer from device"), _("Error"), wxOK | wxICON_ERROR);
		modbus_close(ctx);
		modbus_free(ctx);
		return true;
	}

	Scope::Connect();

	return connect_failed;
}

bool ScopeZTSH::Disconnect(void)
{
	modbus_close(ctx);
	modbus_free(ctx);

	ctx = NULL;

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
	}

	closedir(ttydir);

#elif defined (__WINDOWS__)
	/// TODO: implement me!
#endif
}

void ScopeZTSH::SetupDialog() 
{
    CraoConfig *craoDlg = new CraoConfig(wxGetActiveWindow(), ctx);

	if (ctx) {
		craoDlg->SetConnectedState();
	}

	craoDlg->curr_device = pConfig->Profile.GetString("/crao/serial", _T(""));
	craoDlg->curr_baud = pConfig->Profile.GetInt("/crao/baud", 9600);
	craoDlg->curr_dbits = pConfig->Profile.GetInt("/crao/dbits", 8);
	craoDlg->curr_sbits = pConfig->Profile.GetInt("/crao/sbits", 1);
	craoDlg->curr_parity = pConfig->Profile.GetString("/crao/parity", "none");
	craoDlg->curr_mbid = pConfig->Profile.GetInt("/crao/mbid", 10);

	EnumerateSerialDevices(craoDlg->tty_devices);

    // initialize with actual values
	craoDlg->LoadSettings();

    if (craoDlg->ShowModal() == wxID_OK) {
		// if OK save the values to the current profile
		craoDlg->SaveSettings();

		pConfig->Profile.SetString("/crao/serial", craoDlg->curr_device);
		pConfig->Profile.SetInt("/crao/baud", craoDlg->curr_baud);
		pConfig->Profile.SetInt("/crao/dbits", craoDlg->curr_dbits);
		pConfig->Profile.SetInt("/crao/sbits", craoDlg->curr_sbits);
		pConfig->Profile.SetString("/crao/parity", craoDlg->curr_parity);
		pConfig->Profile.SetInt("/crao/mbid", craoDlg->curr_mbid);
    }

	craoDlg->Destroy();

	delete craoDlg;
}

bool ScopeZTSH::CanPulseGuide()
{
	return true;
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
			if (modbus_write_register(ctx, 11, duration) == -1)  {
				wxMessageBox(wxString("Failed to communicate with the Mount hardware, modbus error: ") + wxString(modbus_strerror(errno)), _("Error"), wxOK | wxICON_ERROR);
				pFrame->SuppressableAlert(PulseGuideFailedAlertEnabledKey(), _("PulseGuide command to mount has failed - guiding is likely to be ineffective."), SuppressPulseGuideFailedAlert, 0);

				return MOVE_STOP_GUIDING;
			}

			printf("Pulse RA+ for %i\n", duration);
            break;

        case WEST:
			if (modbus_write_register(ctx, 12, duration) == -1) {
				wxMessageBox(wxString("Failed to communicate with the Mount hardware, modbus error: ") + wxString(modbus_strerror(errno)), _("Error"), wxOK | wxICON_ERROR);
				pFrame->SuppressableAlert(PulseGuideFailedAlertEnabledKey(), _("PulseGuide command to mount has failed - guiding is likely to be ineffective."), SuppressPulseGuideFailedAlert, 0);

				return MOVE_STOP_GUIDING;
			}

			printf("Pulse RA- for %i\n", duration);
            break;

        case NORTH:
			if (modbus_write_register(ctx, 21, duration) == -1) {
				wxMessageBox(wxString("Failed to communicate with the Mount hardware, modbus error: ") + wxString(modbus_strerror(errno)), _("Error"), wxOK | wxICON_ERROR);
				pFrame->SuppressableAlert(PulseGuideFailedAlertEnabledKey(), _("PulseGuide command to mount has failed - guiding is likely to be ineffective."), SuppressPulseGuideFailedAlert, 0);

				return MOVE_ERROR;
			}

			printf("Pulse DEC+ for %i\n", duration);
            break;

        case SOUTH:
			if (modbus_write_register(ctx, 22, duration) == -1) {
				wxMessageBox(wxString("Failed to communicate with the Mount hardware, modbus error: ") + wxString(modbus_strerror(errno)), _("Error"), wxOK | wxICON_ERROR);
				pFrame->SuppressableAlert(PulseGuideFailedAlertEnabledKey(), _("PulseGuide command to mount has failed - guiding is likely to be ineffective."), SuppressPulseGuideFailedAlert, 0);

				return MOVE_ERROR;
			}

			printf("Pulse DEC- for %i\n", duration);
            break;

        case NONE:
		    printf("error ScopeZTSH::Guide NONE\n");
            break;
    }

    wxMilliSleep(duration);

	return MOVE_OK;
}

#endif

