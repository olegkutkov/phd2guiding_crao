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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include "phd.h"
#include "scope_ztsh_coords.h"

ScopeZtshPosition::ScopeZtshPosition()
	: wxThread(wxTHREAD_JOINABLE)
	, sock (0)
	, thread_done (false)
{
}

ScopeZtshPosition::~ScopeZtshPosition()
{
	Disconnect();
}

void ScopeZtshPosition::OnExit()
{
}

wxThread::ExitCode ScopeZtshPosition::Entry()
{
	double ha = 22.9;
	double ra = 253.3;
	double dec = 13.4596;
/*
	while (!thread_done) {
		thread_done |= TestDestroy();

		if (Connect("10.1.1.142", 16050)) {
			break;
		}
	}
*/
	thread_done = TestDestroy();

	while (!thread_done) {
		usleep(500000);

		pFrame->ScheduleCoordsUpdate(ha += 0.1, ra += 0.1, dec += 0.1, rand(), rand());

		thread_done |= TestDestroy();
	}

	Disconnect();
}

bool ScopeZtshPosition::Connect(std::string host, int port)
{
	struct sockaddr_in server;

	std::cout << "Connecting to " << host << ":" << port << std::endl;
 
	sock = socket(AF_INET , SOCK_STREAM , 0);

	if (sock == -1) {
		last_error = std::string("Failed to create socket, error:\n") + strerror(errno);
		return false;
	}
 
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	struct timeval tv = {2, 0};
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof(tv));
 
	if (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0) {
		last_error = std::string("Failed to connect to the coords server, error:\n") + strerror(errno);
        return false;
	}

///

	WriteString("<HelloServer>");

	std::string srv_answer = ReadString();

	if (srv_answer != "<HelloClient>:") {
		last_error = "Bad answer from server: " + srv_answer;
		Disconnect();
		return false;
	}

	srv_answer = ReadString();

	if (srv_answer != "<NumberParameters>:[int32]") {
		last_error = "Bad answer from server: " + srv_answer;
		Disconnect();

		return false;
	}

	int srv_answer_int = ReadInt32();

	for (int i = 0; i < srv_answer_int; i++) {
		srv_answer = ReadString();

		if (srv_answer == "<ProtocolVersion>:[int32]") {
			int version = ReadInt32();
			/// log this in future...
		} else if (srv_answer == "<InstrumentName>:[string]") {
			instrument_name = ReadString();
		}
	}

	return true;
}

bool ScopeZtshPosition::Disconnect()
{
	if (sock <= 0) {
		return true;
	}

	WriteString("<ByeServer>");  
    close(sock);

	sock = 0;

	return true;
}

void ScopeZtshPosition::GetCoordsAndSpeed(double &ha, double &ra, double &dec, double &ra_speed, double &dec_speed)
{
	if (!sock) {
		ha = 0;
		ra = 0;
		dec = 0;
		ra_speed = 0;
		dec_speed = 0;

		return;
	}

	std::string srv_answer;

	WriteString("<" + instrument_name + ".Reflector.GetTelemetry>");

	ReadString();
	ReadString();
	int count = ReadInt32();

	for (int i = 0; i < count; i++) {
		srv_answer = ReadString();

		if (srv_answer == "<HourAngleDeg>:[float64]") {
			ha = ReadFloat64();
		} else if (srv_answer == "<RightAscensionDeg>:[float64]") {
			ra = ReadFloat64();
		} else if (srv_answer == "<DeclinationDeg>:[float64]") {
			dec = (ReadFloat64());
		} else if (srv_answer == "<HourAngleSpeedArcSecPerSec>:[float64]") {
			ra_speed = ReadFloat64();
		} else if (srv_answer == "<DeclinationSpeedArcSecPerSec>:[float64]") {
			dec_speed = ReadFloat64();
		}
	}
}

std::string ScopeZtshPosition::GetErrorText()
{
	return last_error;
}

char ScopeZtshPosition::SocketReadByte()
{
	char byte;

	recv(sock, &byte, sizeof(byte), 0);

	return byte;
}

int ScopeZtshPosition::Read7BitEncodedInt()
{
	int int32 = 0, shift = 0, byteval;

	while (((byteval = SocketReadByte()) & 128) != 0 & shift != 35)
	{
		int32 |= ((byteval & 127) << shift);
		shift += 7;
	}

	return (int32 | (byteval << shift));
}

std::string ScopeZtshPosition::ReadString()
{
	int bytes_count = Read7BitEncodedInt();

	char *buf = new char[bytes_count + 1];
	memset(buf, 0, bytes_count);

	recv(sock, (void*)buf, bytes_count, 0);

	buf[bytes_count] = '\0';

	std::string res = buf;

	delete[] buf;

	return res;
}

short ScopeZtshPosition::ReadInt16()
{
	short res;
	recv(sock, &res, sizeof(res), 0);

	return res;
}

int ScopeZtshPosition::ReadInt32()
{
	int res;
	recv(sock, &res, sizeof(res), 0);

	return res;
}

long ScopeZtshPosition::ReadInt64()
{
	long res;
	recv(sock, &res, sizeof(res), 0);

	return res;
}

float ScopeZtshPosition::ReadFloat32()
{
	float res;
	recv(sock, &res, sizeof(res), 0);

	return res;
}

double ScopeZtshPosition::ReadFloat64()
{
	unsigned char *buff = new unsigned char[8];
	recv(sock, buff, 8, 0);
	double res =  *(double*)buff;
	delete buff;

	return res;
}

bool ScopeZtshPosition::ReadBool()
{
	return SocketReadByte() == 1;
}

void ScopeZtshPosition::SockWriteByte(char byte)
{
	send(sock, &byte, 1, 0);
}

void ScopeZtshPosition::Write7BitEncodedInt(int int32)
{
	for (; int32 >= 128; int32 >>= 7) {
		SockWriteByte((unsigned char)(int32 | 128));
	}

	SockWriteByte((unsigned char)int32);
}

void ScopeZtshPosition::WriteString(std::string str)
{
	int str_len = str.length();

	Write7BitEncodedInt(str_len);

	send(sock, (void*)str.c_str(), str_len, 0);
}

