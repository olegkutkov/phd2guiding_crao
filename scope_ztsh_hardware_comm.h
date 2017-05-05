/*
 *  scope_ztsh_hardware_comm
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

#include <modbus/modbus.h>
#include <string>

#define DEC_DIRECTION_PLUS 0x01
#define DEC_DIRECTION_MINUS 0x03

class ZtshHwComm {
public:
	ZtshHwComm();
	~ZtshHwComm();

	bool ConfigureSerial(const char* device, const int baud, const char parity, 
							const int dbits, const int sbits);

	void ConfigureAdam(const int addr, const int pwrch, const int dminch, const int dplusch);
	void ConfigureInverters(const int ra_addr, const int dec_addr);

	void SetModbusDebug(const bool debug);
	void SetModbusProtoRecovery(const bool recovery);

	bool AdamEnableInverterPower();
	bool AdamDisableInverterPower();

	bool AdamRelayEnableDecPlus();
	bool AdamRelayDisableDecPlus();
	bool AdamRelayEnableDecMinus();
	bool AdamRelayDisableDecMinus();

	bool SetHourAxisSpeed(const int speed);

	bool SetDecAxisSpeed(const uint16_t direction, const uint16_t speed);
	bool StopDecAxis();

	std::string GetErrorText();

private:
	bool DecAxisCmd(const uint16_t direction, const uint16_t speed);
	bool AdamCmd(const uint8_t channel, bool enable);

	modbus_t *ctx;

	int adam_pwr_ch;
	int adam_delta_min_ch;
	int adam_delta_plus_ch;
	int inv_ra_addr;
	int inv_dec_addr;
	bool adam_dump;

	char adam_addr[2];

	std::string last_error;
};

