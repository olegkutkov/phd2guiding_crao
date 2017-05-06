/*
 *  scope_ztsh
 *  PHD Guiding
 *
 *  Module for ZTSH autogude system, Crimean astrophysical observatory
 *  All rights reserved.
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
#ifdef  GUIDE_ZTSH

#include <vector>

class ZtshHwComm;
class ScopeZtshPosition;

class ScopeZTSH : public Scope {
public:
	ScopeZTSH();
	~ScopeZTSH();

	bool     Connect(void);
    bool     Disconnect(void);
    bool     HasSetupDialog(void) const;
    void     SetupDialog();

    bool     CanReportPosition(void);
    double   GetDeclination(void);
    bool     GetGuideRates(double *pRAGuideRate, double *pDecGuideRate);
    bool     GetCoordinates(double *ra, double *dec, double *siderealTime);
    bool     GetSiteLatLong(double *latitude, double *longitude);

    MOVE_RESULT Guide(GUIDE_DIRECTION direction, int duration);

    bool   CanPulseGuide();

private:
	ZtshHwComm *hwcomm;
	ScopeZtshPosition *scope_pos;
	void DisplayMoveError(std::string dir);

	void EnumerateSerialDevices(std::vector<wxString>& devices);
};

#endif

