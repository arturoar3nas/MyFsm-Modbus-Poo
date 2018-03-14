#include "Meter.h"

//Hacer Factory

Meter::Meter()
{
	
	
}

Meter::Meter(char const* serial_device, int serial_speed, uint8_t debul_level, dw93_device* dw93) {
	
	JVL::Debug::Printf(JVL::Debug::RELEASE_LEVEL, true, "[Meter] Esperando por conexion en %s\n", IF_SERIAL);
	ctx = modbus_new_rtu(serial_device, serial_speed, 'N', 8, 1);
	
	dw = new dw93_device();
	dw = dw93;
	nSerialSpeed = serial_speed;
	cpSerialDevice = serial_device;
	_debug_level = debul_level;
	rx_errors = 0;
}

Meter::~Meter() {
	stop();	
}

// this function does something
uint8_t Meter::start() {
	
	int rc = 0;
	if (ctx == NULL) {
		JVL::Debug::Printf(JVL::Debug::RELEASE_LEVEL, true, "[Meter] Problema iniciando Libmodbus. Error %s!\n", modbus_strerror(errno));
	} else {		
		modbus_set_debug(ctx, TRUE);
		modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
			
		modbus_set_slave(ctx, 1);

		modbus_get_response_timeout(ctx, &response_timeout);
		JVL::Debug::Printf(JVL::Debug::RELEASE_LEVEL, true, "[Meter] Timeout %d,%d[s]!\n", response_timeout.tv_sec, response_timeout.tv_usec / 1000);

		response_timeout.tv_sec = 2;
		modbus_set_response_timeout(ctx, &response_timeout);
		
		rc = modbus_connect(ctx);
		if (rc == -1) {
			JVL::Debug::Printf(JVL::Debug::RELEASE_LEVEL, true, "[Meter] Problema conexion modbus. Error %s!\n", modbus_strerror(errno));
		}
		JVL::Debug::Printf(JVL::Debug::RELEASE_LEVEL, true, "[IfEMG] Conexion Serial iniciada!\n");
	}
		
	fsmState = IDLE;
	idleTime = 10;
	m_stop = false;
	pthread_create(&pthread, 
		NULL, 
		&Meter::run, 
		this);
	
	return rc;
}

//Never end... never comeback... 
void * Meter::run(void * par) {
	uint16_t msg[64];
	Meter* m = (Meter*)par;
	bool ret = false;
	int nTimeMachine = 1;
		
	JVL::Debug::Printf(JVL::Debug::BETA_LEVEL, true, "[Meter] Iniciando hebra run\n");	
	
	while (!(m->m_stop))
	{				
		sleep(nTimeMachine);
		m->rc = (m->*m->FSM[m->fsmState].fptr)();
		m->lookup_transition(m->fsmState, m->rc);
	}
	
	return 0;
}

// this function stop the modbus and free the ctx variable
void Meter::stop() {
	modbus_close(ctx);
	modbus_free(ctx); 
	m_stop = true; 
}

// this function return the object dw93_device
uint8_t Meter::get_measurement(dw93_device* dw93) {
	dw93 = dw;
	return 0;
}	

// this function read the meter using modbus 
Meter::ReturnCodes  Meter::read_meter() {
	
	Modbus_Configurar_Timeout(20);
	
	ReadValue(METER_ADDRESS, &(dw->Status->current_a_phase), dw->I_A_PHASE, "I A", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->current_b_phase), dw->I_B_PHASE, "I B", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->current_c_phase), dw->I_C_PHASE, "I C", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->pf_a_phase), dw->PF_A_PHASE, "PF A", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->pf_c_phase), dw->PF_B_PHASE, "PF B", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->pf_c_phase), dw->PF_C_PHASE, "PF C", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->total_pf), dw->TOTAL_PF, "T PF", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->total_power), dw->TOTAL_POWER, "T W", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->total_q), dw->TOTAL_Q, "T Q", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->var_a_phase), dw->VAR_A_PHASE, "VAR A", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->var_b_phase), dw->VAR_B_PHASE, "VAR B", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->var_c_phase), dw->VAR_C_PHASE, "VAR C", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->volt_a_phase), dw->V_A_PHASE, "V A", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->var_b_phase), dw->V_B_PHASE, "V B", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->volt_c_phase), dw->V_C_PHASE, "V C", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wh_a_phase), dw->WH_A_PHASE, "WH A", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wh_b_phase), dw->WH_B_PHASE, "WH B", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wh_c_phase), dw->WH_C_PHASE, "WH C", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wh_total), dw->WH_TOTAL, "W T", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wr_a_phase), dw->KW_A_PHASE, "KW A", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wr_b_phase), dw->KW_B_PHASE, "KW B", 3);
	ReadValue(METER_ADDRESS, &(dw->Status->wr_c_phase), dw->KW_C_PHASE, "KW C", 3);

	
	return RETURN_OK;
}

void Meter::Modbus_Configurar_Timeout(int timeout)
{
	response_timeout.tv_sec = timeout;
	modbus_set_response_timeout(ctx, &response_timeout);
	modbus_flush(ctx);
}

bool Meter::ReadValue(uint8_t device_address, uint16_t* value, uint16_t register_address, const char* register_name, int retries)
{
	uint16_t msg[8];
	int rsp_tmp = 0;

	if (file_exist(ReadValue_debug_filename))
		modbus_set_debug(ctx, TRUE);
	else
		modbus_set_debug(ctx, FALSE); 
		
	msleep(100);

	Modbus_Configurar_Direccion(device_address);
	
	while (retries--) {
		rsp_tmp = modbus_read_registers(ctx, register_address, 1, msg);
		if (rsp_tmp == 1)
		{			
			*value = msg[0];
			JVL::Debug::Printf(_debug_level, true, "[Meter]  Device %d -> Register 0x%.3X (%s) read with value %d\n", device_address, register_address, register_name, (uint16_t)(*value));
			return true;
		}		
		else 
		{
			rx_errors++;
			JVL::Debug::Printf(JVL::Debug::ERROR_LEVEL, "[Meter](ReadValue)  Device %d -> Register 0x%.3X (%s) ERROR DE LECTURA, errores:  %d, error: %d\n", device_address, register_address, register_name, rx_errors, rsp_tmp);
		}
	}
		
	JVL::Debug::Printf(_debug_level, true, "[Meter]  Device %d -> PROBLEM to read register 0x%.3X (%s) after %d retries \n", device_address, register_address, register_name, rsp_tmp);
	return RETURN_OK;
}

int Meter::Modbus_Configurar_Direccion(int address)
{
	slave_address = address;
	return modbus_set_slave(ctx, slave_address);
	modbus_flush(ctx);

}

// state idle
Meter::ReturnCodes Meter::idle()
{
	for (;idleTime > 0;idleTime--) {
		//JVL::Debug::Printf(JVL::Debug::RELEASE_LEVEL, true, "[Meter] Idle! zzZZzzZZZzZZ%s!\n",idleTime);
		sleep(1);
	}
	
	return RETURN_OK;
}

// this function does something
Meter::ReturnCodes Meter::reset_meter()
{
	dw->Status = { 0 }; //set 0 
	
	Modbus_Configurar_Timeout(20);	
	
	ConfigValue(METER_ADDRESS, (dw->Status->current_a_phase), dw->I_A_PHASE, "I A", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->current_b_phase), dw->I_B_PHASE, "I B", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->current_c_phase), dw->I_C_PHASE, "I C", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->pf_a_phase), dw->PF_A_PHASE, "PF A", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->pf_c_phase), dw->PF_B_PHASE, "PF B", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->pf_c_phase), dw->PF_C_PHASE, "PF C", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->total_pf), dw->TOTAL_PF, "T PF", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->total_power), dw->TOTAL_POWER, "T W", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->total_q), dw->TOTAL_Q, "T Q", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->var_a_phase), dw->VAR_A_PHASE, "VAR A", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->var_b_phase), dw->VAR_B_PHASE, "VAR B", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->var_c_phase), dw->VAR_C_PHASE, "VAR C", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->volt_a_phase), dw->V_A_PHASE, "V A", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->var_b_phase), dw->V_B_PHASE, "V B", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->volt_c_phase), dw->V_C_PHASE, "V C", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wh_a_phase), dw->WH_A_PHASE, "WH A", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wh_b_phase), dw->WH_B_PHASE, "WH B", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wh_c_phase), dw->WH_C_PHASE, "WH C", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wh_total), dw->WH_TOTAL, "W T", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wr_a_phase), dw->KW_A_PHASE, "KW A", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wr_b_phase), dw->KW_B_PHASE, "KW B", true, 3);
	ConfigValue(METER_ADDRESS, (dw->Status->wr_c_phase), dw->KW_C_PHASE, "KW C", true, 3);
}

bool Meter::ConfigValue(uint8_t device_address, uint16_t value, uint16_t register_address, const char* register_name, bool force, int retries)
{
	uint16_t msg[8];
	int rsp_tmp = 0;
		
	if (file_exist(ConfigValue_debug_filename))
		modbus_set_debug(ctx, TRUE);
	else
		modbus_set_debug(ctx, FALSE);
		
	Modbus_Configurar_Direccion(device_address);

	JVL::Debug::Printf(_debug_level, true, "[Meter]  Device %d -> New value %d to register 0x%.3X (%s). \n", device_address, value, register_address, register_name);
		
	while (retries--)
	{			
		if (force) {				
			if (modbus_write_register(ctx, register_address, value) == 1) {
				JVL::Debug::Printf(_debug_level, false, "-> Send OK  \n");
				rsp_tmp = modbus_read_registers(ctx, register_address, 1, msg);
				if (rsp_tmp == 1) {
					JVL::Debug::Printf(_debug_level, true, "[Meter]  Device %d -> Register 0x%.3X (%s) read with value %d\n", device_address, register_address, register_name, msg);
					return true;
				}
				else 
					JVL::Debug::Printf(_debug_level, true, "[Meter]  Device %d -> Register 0x%.3X (%s) read with error! \n", device_address, register_address, register_name);
				return true;
			}
			else
				JVL::Debug::Printf(_debug_level, true, "-> Problem sending new value  \n");
		}
		else
		{
			if (rsp_tmp = modbus_read_registers(ctx, register_address, 1, msg) >= 0) {
				msleep(50);
				if (value != msg[0]) {			
					JVL::Debug::Printf(_debug_level, false, "-> Previous value %d. Sending new value. ", msg[0]);
					if (modbus_write_register(ctx, register_address, value) == 1) {
						JVL::Debug::Printf(_debug_level, false, "-> Send OK  \n");
						return true;
					}					
					else
						JVL::Debug::Printf(_debug_level, true, "-> Problem sending new value  \n");
				}
			}
			else if (rsp_tmp == -1)
			{
				rx_errors++;
				JVL::Debug::Printf(JVL::Debug::ERROR_LEVEL, "[IfEMG]  Device %d -> Register 0x%.3X (%s) ERROR DE LECTURA, errores:  %d\n", device_address, register_address, register_name, rx_errors);
			}
			else
			{					
				rx_errors++;
				JVL::Debug::Printf(JVL::Debug::ERROR_LEVEL, true, "-> Can't read register\n");
			}
			msleep(50);  //sleep(1);
		}
		sleep(50);
	}
	return false;
}

// this function does something
Meter::ReturnCodes Meter::setting_meter()
{
	Modbus_Configurar_Timeout(20);
	
	//Communication address position 0x01 by default
	//?
	//?
	//?
	
	return RETURN_OK;
}

Meter::StateMachineType const * Meter::table_begin(void)
{
	return &FSM[0];
}

Meter::StateMachineType const * Meter::table_end(void)
{
	return &FSM[SIZE_FSM];
}
//! Buscamos el estado de transicion
void Meter::lookup_transition(StateType fsmstate, ReturnCodes returnCodes)
{
	StateMachineType const * pEntry = table_begin();
	StateMachineType const * pEnd = table_end();
	bool bStateFound = false;
	while ((!bStateFound) && (pEntry != pEnd)) {
		if (pEntry->CurrentState == fsmstate) {
			if (pEntry->ReturnState == returnCodes) {
				fsmState = pEntry->NextState;
				bStateFound = true;
				break;
			}
		}
		++pEntry;
	}
}