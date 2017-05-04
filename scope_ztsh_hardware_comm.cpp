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

#include <errno.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>

#include "scope_ztsh_hardware_comm.h"

#define RA_INVERTER_HZ_REG 61451

#define DEC_INVERTER_ROTATION_REG 0x01
#define DEC_INVERTER_DATA_LEN 2

#define ADAM_ASCII_COMMAND_START '#'
#define ADAM_ASCII_COMMAND_OUT_CTRL '1'
#define ASCII_NUM_START 48
#define ADAM_ASCII_COMMAND_END '\r'

#define ADAM_CHANNEL_POWER_RELAY 0
#define ADAM_CHANNEL_DELTA_PLUS_RELAY 1
#define ADAM_CHANNEL_DELTA_MINUS_RELAY 2

static uint16_t dex_inverter_data[DEC_INVERTER_DATA_LEN];

ZtshHwComm::ZtshHwComm()
	: ctx(NULL)
	, adam_pwr_ch(0)
	, adam_delta_min_ch(0)
	, adam_delta_plus_ch(0)
	, inv_ra_addr(0)
	, inv_dec_addr(0)
	, adam_dump(false)
{	
}

ZtshHwComm::~ZtshHwComm()
{
	if (ctx) {
		modbus_close(ctx);
		modbus_free(ctx);
		ctx = NULL;
	}	
}

bool ZtshHwComm::ConfigureSerial(const char* device, const int baud, const char parity,
									const int dbits, const int sbits)
{
	ctx = modbus_new_rtu(device, baud, parity, dbits, sbits);

	if (!ctx) {
		last_error = std::string("Failed to initialize modbus using current serial configuration: ")
						+ modbus_strerror(errno);

		return false;
	}

	if (modbus_connect(ctx) == -1) {
		last_error = std::string("Modbus connection failed with error: ") + modbus_strerror(errno);

		return false;
	}

	return true;
}


void ZtshHwComm::ConfigureAdam(const int addr, const int pwrch, const int dminch, const int dplusch)
{
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << addr;
	adam_addr[0] = stream.str()[0];
	adam_addr[1] = stream.str()[1];

	adam_pwr_ch = pwrch;
	adam_delta_min_ch = dminch;
	adam_delta_plus_ch = dplusch;
}

void ZtshHwComm::SetModbusDebug(const bool debug)
{
	modbus_set_debug(ctx, debug);
	adam_dump = debug;
}

void ZtshHwComm::SetModbusProtoRecovery(const bool recovery)
{
	if (recovery) {
		modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK /*| MODBUS_ERROR_RECOVERY_PROTOCOL*/);
	} else {
		modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_NONE);
	}
}

void ZtshHwComm::ConfigureInverters(const int ra_addr, const int dec_addr)
{
	inv_ra_addr = ra_addr;
	inv_dec_addr = dec_addr;
}

bool ZtshHwComm::AdamEnableInverterPower()
{
	AdamCmd(ADAM_CHANNEL_POWER_RELAY, true);
}

bool ZtshHwComm::AdamDisableInverterPower()
{
	AdamCmd(ADAM_CHANNEL_POWER_RELAY, false);
}

bool ZtshHwComm::AdamRelayEnableDecPlus()
{
	AdamCmd(ADAM_CHANNEL_DELTA_PLUS_RELAY, true);
}

bool ZtshHwComm::AdamRelayDisableDecPlus()
{
	AdamCmd(ADAM_CHANNEL_DELTA_PLUS_RELAY, false);
}

bool ZtshHwComm::AdamRelayEnableDecMinus()
{
	AdamCmd(ADAM_CHANNEL_DELTA_MINUS_RELAY, true);
}

bool ZtshHwComm::AdamRelayDisableDecMinus()
{
	AdamCmd(ADAM_CHANNEL_DELTA_MINUS_RELAY, false);
}

bool ZtshHwComm::SetHourAxisSpeed(const int speed)
{
	modbus_set_slave(ctx, inv_ra_addr);

	if (modbus_write_register(ctx, RA_INVERTER_HZ_REG, speed) == -1) {
		last_error = modbus_strerror(errno);
		return false;
	}

	return true;
}

bool ZtshHwComm::SetDecAxisSpeed(const uint16_t direction, const uint16_t speed)
{
	return DecAxisCmd(direction, speed);
}

bool ZtshHwComm::StopDecAxis()
{
	return DecAxisCmd(0, 0);
}

bool ZtshHwComm::AdamCmd(const uint8_t channel, bool enable)
{
	uint8_t adam_data_buf_len = 8;
	uint8_t adam_data_buf[adam_data_buf_len];

	adam_data_buf[0] = ADAM_ASCII_COMMAND_START;
	adam_data_buf[1] = adam_addr[0];
	adam_data_buf[2] = adam_addr[1];
	adam_data_buf[3] = ADAM_ASCII_COMMAND_OUT_CTRL;
	adam_data_buf[4] = (uint8_t) channel + ASCII_NUM_START;
	adam_data_buf[5] = '0';
	adam_data_buf[6] = (uint8_t) enable + ASCII_NUM_START;
	adam_data_buf[7] = ADAM_ASCII_COMMAND_END;

	write(modbus_get_socket(ctx), adam_data_buf, adam_data_buf_len);

	usleep(250000);

	modbus_flush(ctx);

	if (adam_dump) {
		printf("ADAM SENT MSG: %C %C %C %C %C %C %C <CR>\n",
				adam_data_buf[0], adam_data_buf[1], adam_data_buf[2], adam_data_buf[3], 
				adam_data_buf[4], adam_data_buf[5], adam_data_buf[6]);
	}
}

bool ZtshHwComm::DecAxisCmd(const uint16_t direction, const uint16_t speed)
{
	modbus_set_slave(ctx, inv_dec_addr);

	dex_inverter_data[0] = direction;
	dex_inverter_data[1] = speed;

	if (modbus_write_registers(ctx, DEC_INVERTER_ROTATION_REG, DEC_INVERTER_DATA_LEN, dex_inverter_data) == -1) {
		last_error = modbus_strerror(errno);
		return false;
	}

	return true;
}

std::string ZtshHwComm::GetErrorText()
{
	return last_error;
}

