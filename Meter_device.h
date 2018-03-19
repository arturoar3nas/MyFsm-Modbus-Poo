#ifndef METER_DEVICE_H
#define METER_DEVICE_H

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <modbus.h>
#include "utils.h"

class Meter_device {
public:
	Meter_device() {
		rx_errors = 0;
		idleTime = 10;
	}
	virtual ~Meter_device() {}
public: 
	virtual ReturnCodes read_meter(void)=0;
	virtual ReturnCodes idle(void)=0;
	virtual ReturnCodes reset_meter(void)=0;
	virtual ReturnCodes setting_meter(void)=0;
	virtual Meter_device* Clone() = 0;
	void set_ctx(modbus_t *ctx_) {
		ctx = ctx_;
	}
protected:
	char const *ConfigValue_debug_filename = "/tmp/ CONFIG_DEBUG";
	char const *ReadValue_debug_filename = "/tmp/READ_DEBUG";
	char const *SendValue_debug_filename = "/tmp/SEND_DEBUG";
	
	int slave_address;
	modbus_t *ctx;	//copiar ctx del cliente
	timeval response_timeout;
	int rx_errors;
	uint8_t idleTime;
	
	int Modbus_Configurar_Direccion(int address) {
		slave_address = address;
		return modbus_set_slave(ctx, slave_address);
		modbus_flush(ctx);

	}
	void Modbus_Configurar_Timeout(int timeout) {
		response_timeout.tv_sec = timeout;
		modbus_set_response_timeout(ctx, &response_timeout);
		modbus_flush(ctx);
	}
	bool ReadValue(uint8_t device_address, uint16_t* value, uint16_t register_address, const char* register_name, int retries) {
		uint16_t msg[8];
		int rsp_tmp = 0;

		if (file_exist(ReadValue_debug_filename)) {
			//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  File Exist\n");
			modbus_set_debug(ctx, TRUE);
		}
		else
			//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  NO! File Exist\n");
		
		msleep(100);

		Modbus_Configurar_Direccion(device_address);
	
		while (retries--) {
			rsp_tmp = modbus_read_registers(ctx, register_address, 1, msg);
			if (rsp_tmp == 1) {			
				*value = msg[0];
				//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  Device %d -> Register 0x%.3X (%s) read with value %d\n", device_address, register_address, register_name, (uint16_t)(*value));
				return true;
			}		
			else 
			{
				rx_errors++;
				//NeetMeter::Debug::Printf(//NeetMeter::Debug::ERROR_LEVEL, "[Meter](ReadValue)  Device %d -> Register 0x%.3X (%s) ERROR DE LECTURA, errores:  %d, error: %d, modbus error: %s\n", device_address, register_address, register_name, rx_errors, rsp_tmp, modbus_strerror(errno));
			}
		}
		
		//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  Device %d -> PROBLEM to read register 0x%.3X (%s) after %d retries \n", device_address, register_address, register_name, rsp_tmp);
		return RETURN_OK;
	}
	bool ConfigValue(uint8_t device_address, uint16_t value, uint16_t register_address, const char* register_name, bool force, int retries) {
		uint16_t msg[8];
		int rsp_tmp = 0;
		
		if (file_exist(ConfigValue_debug_filename))
			modbus_set_debug(ctx, TRUE);
		
		Modbus_Configurar_Direccion(device_address);

		//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  Device %d -> New value %d to register 0x%.3X (%s). \n", device_address, value, register_address, register_name);
		
		while (retries--)
		{			
			if (force) {				
				if (modbus_write_register(ctx, register_address, value) == 1)
				{
					//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, false, "-> Send OK  \n");
					rsp_tmp = modbus_read_registers(ctx, register_address, 1, msg);
					if (rsp_tmp == 1)
					{
						//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  Device %d -> Register 0x%.3X (%s) read with value %d\n", device_address, register_address, register_name, msg);
						return true;
					}
					else 
						//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter]  Device %d -> Register 0x%.3X (%s) read with error! \n", device_address, register_address, register_name);
						return true;
				}
				else
				{
					//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "-> Problem sending new value  \n");
				}
					
			}
			else
			{
				if (rsp_tmp = modbus_read_registers(ctx, register_address, 1, msg) >= 0) {
					msleep(50);
					if (value != msg[0]) {			
						//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, false, "-> Previous value %d. Sending new value. ", msg[0]);
						if (modbus_write_register(ctx, register_address, value) == 1)
						{
							//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, false, "-> Send OK  \n");
							return true;
						}					
						else
						{
							//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "-> Problem sending new value  \n");
						}
							
					}
				}
				else if (rsp_tmp == -1)
				{
					rx_errors++;
					//NeetMeter::Debug::Printf(//NeetMeter::Debug::ERROR_LEVEL, "[IfEMG]  Device %d -> Register 0x%.3X (%s) ERROR DE LECTURA, errores:  %d\n", device_address, register_address, register_name, rx_errors);
				}
				else
				{					
					rx_errors++;
					//NeetMeter::Debug::Printf(//NeetMeter::Debug::ERROR_LEVEL, true, "-> Can't read register\n");
				}
				msleep(50);  //sleep(1);
			}
			sleep(50);
		}
		return false;
	}

};

#endif // !METER_DEVICE_H
