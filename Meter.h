#ifndef METER_H
#define METER_H

#include <thread>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <modbus.h>
#include <atomic>
#include <assert.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "dw93_device.h"
#include "Gpio.hpp"
#include "Debug.hpp"

#define METER_ADDRESS	0x01


class Meter
{
public:
	Meter();
	Meter(char const* serial_device, int serial_speed, uint8_t debul_level, dw93_device* dw93);
	~Meter();
	
private:
	typedef enum { READ_METER, IDLE, RESET_METER, SETTING_METER } StateType;
	typedef enum { RETURN_OK, RETURN_FAIL, RETURN_REPEAT }ReturnCodes;
	
public:
	uint8_t start();
	void stop();
	uint8_t get_measurement(dw93_device* dw93);
	void lookup_transition(StateType fsmstate, ReturnCodes returnCodes);
	
private:	
	ReturnCodes read_meter(void);
    ReturnCodes idle(void);
	ReturnCodes reset_meter(void);
	ReturnCodes setting_meter(void);
	static void * run(void * par);
	int Modbus_Configurar_Direccion(int address);
	bool ReadValue(uint8_t device_address, uint16_t* value, uint16_t register_address, const char* register_name, int retries);
	void Modbus_Configurar_Timeout(int timeout);
	bool ConfigValue(uint8_t device_address, uint16_t value, uint16_t register_address, const char* register_name, bool force, int retries);
	
	dw93_device* dw;
	pthread_t pthread;	
	char const *ConfigValue_debug_filename = "/tmp/CONFIG_DEBUG";
	char const *ReadValue_debug_filename = "/tmp/READ_DEBUG";
	char const *SendValue_debug_filename = "/tmp/SEND_DEBUG";	
	char const* cpSerialDevice;	
	JVL::Gpio gpio;
	modbus_t *ctx;	
	int nSerialSpeed;
	int estadoComunicacion;
	int rx_errors;
	int slave_address;
	timeval response_timeout;
	struct timespec now_time;
	struct timespec inicioEstado;
	uint8_t _debug_level;
	uint8_t idleTime;
	
	/*
		FSM
	*/
	typedef struct {
		StateType CurrentState;
		ReturnCodes ReturnState;
		StateType NextState;
		ReturnCodes(Meter::*fptr)(void);
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
		{ READ_METER, RETURN_OK, IDLE, &Meter::read_meter },		
		{ IDLE, RETURN_OK, READ_METER, &Meter::idle },
		{ RESET_METER, RETURN_OK, IDLE, &Meter::reset_meter },
		{ SETTING_METER, RETURN_OK, IDLE, &Meter::setting_meter }
	};
	const int SIZE_FSM = sizeof(FSM) / sizeof(FSM[0]);
};

#endif