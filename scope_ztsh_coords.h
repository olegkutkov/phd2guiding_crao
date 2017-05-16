/*
 *  scope_ztsh_coords
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

#include <string>
#include <wx/thread.h>

class ScopeZtshPosition : public wxThread
{
public:
	ScopeZtshPosition();
	~ScopeZtshPosition();

	std::string GetErrorText();

	void GetScopePosAndSpeed(double &ha, double &ra, double &dec, double &rasp, double &decsp);
	double GetDeclination();

private:
	bool Connect(std::string host, int port);
	bool Disconnect();
	void UpdateCoordsAndSpeed();

	virtual void *Entry();
	void OnExit();

	char SocketReadByte();
	int Read7BitEncodedInt();
	std::string ReadString();
	short ReadInt16();
	int ReadInt32();
	long ReadInt64();
	float ReadFloat32();
	double ReadFloat64();
	bool ReadBool();
	void SockWriteByte(char byte);
	void Write7BitEncodedInt(int int32);
	void WriteString(std::string str);

	int sock;
	bool thread_done;
	double last_ha;
	double last_ra;
	double last_dec;
	double last_ra_speed;
	double last_dec_speed;

	std::string instrument_name;
	std::string last_error;
};

