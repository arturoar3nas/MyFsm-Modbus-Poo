#ifndef METER_H
#define METER_H

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <thread>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <modbus.h>
#include <atomic>
#include <assert.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "target.h"
extern "C" { 
#include "utils.h"
}
#include "Gpio.hpp"
#include "Debug.hpp"
#include "Meter_device.h"
#include "Aob292e.h"
#include "dw93_device.h"

class Meter
{
public:
	Meter();
	Meter(char const* serial_device, int serial_speed, uint8_t debul_level, dw93_device* Ao);
	Meter(char const* serial_device, int serial_speed, uint8_t debul_level, Meter_device* meter_device);
	~Meter();
	
public:
	uint8_t start();
	void stop();
	uint8_t get_measurement(Meter_device* meter_d);
	void lookup_transition(StateType fsmstate, ReturnCodes returnCodes);
	
private:	
	static void * run(void * par);
	
	dw93_device* ao;
	Meter_device* mymeter;
	pthread_t pthread;	

	char const* cpSerialDevice;	
	NeetMeter::Gpio gpio;
	modbus_t *ctx;	
	int nSerialSpeed;
	int estadoComunicacion;
	timeval response_timeout;
	
	struct timespec now_time;
	struct timespec inicioEstado;
	uint8_t _debug_level;

	
	/*
		FSM
	*/
	typedef struct {
		StateType CurrentState;
		ReturnCodes ReturnState;
		StateType NextState;
		ReturnCodes(Meter_device::*fptr)(void);
	}StateMachineType;	

	StateMachineType const *table_begin(void);
	StateMachineType const *table_end(void);
	
public:
	ReturnCodes rc;
	std::atomic<bool> m_stop;
	StateType fsmState;
	
	const StateMachineType FSM[4] =
	{
	//  Current		Transition   Next
	//  State ID	Letter       State ID
		{ READ_METER, RETURN_OK, IDLE, &Meter_device::read_meter },		
		{ IDLE, RETURN_OK, READ_METER, &Meter_device::idle },
		{ RESET_METER, RETURN_OK, IDLE, &Meter_device::reset_meter },
		{ SETTING_METER, RETURN_OK, IDLE, &Meter_device::setting_meter }
	};
	const int SIZE_FSM = sizeof(FSM) / sizeof(FSM[0]);
};

#endif