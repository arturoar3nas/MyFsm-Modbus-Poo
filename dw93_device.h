#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "target.h"
#include "utils.h"
#include "Meter_device.h"


class Meter_device;

class dw93_device : public Meter_device
{
public:
	dw93_device()
	{
	}
	
public:
	
	struct id_t_struct
	{
		uint8_t zone;
		uint16_t number;
	} __attribute__((packed));
	typedef struct id_t_struct id_t; 
	
	struct time_t_struct
	{
		uint8_t hour;
		uint8_t minutes;
		uint8_t seconds;
		uint8_t day;
		uint8_t month;
		uint8_t year; 
	} __attribute__((packed));
	typedef struct time_t_struct time_t; 

	struct Config_struct
	{			
		uint8_t id;
		bool enabled;
		uint8_t com_address;
		uint8_t alarm_1;
		uint8_t alarm_1_deviation;
		uint8_t alarm_1_mode;
		uint8_t alarm_2;
		uint8_t alarm_2_deviation;
		uint8_t alarm_2_mode;
		uint8_t alarm_3;
		uint8_t alarm_3_deviation;
		uint8_t alarm_3_mode;
		uint8_t current_output;
		uint8_t low_current;
		uint8_t high_current;
		uint8_t volt_pt_a;
		uint8_t ct_current_a;
		uint8_t ct_current_b;
		uint8_t ct_current_c;
		uint8_t point_float;
		uint8_t parameter_display;
		uint8_t phase_wire;			
	}__attribute__((packed)); 
	typedef struct Config_struct Config_t;

	struct status_struct
	{			
		bool sync;					
		uint16_t status;		
		uint16_t ageMesure;				//0 a 65535 (min)		
		uint16_t total_power;
		uint16_t total_pf;
		uint16_t total_q;
		uint16_t volt_a_phase;
		uint16_t current_a_phase;
		uint16_t pf_a_phase;
		uint16_t wr_a_phase;
		uint16_t var_a_phase;
		uint16_t volt_b_phase;
		uint16_t current_b_phase;
		uint16_t pf_b_phase;
		uint16_t wr_b_phase;
		uint16_t var_b_phase;
		uint16_t volt_c_phase;
		uint16_t current_c_phase;
		uint16_t pf_c_phase;
		uint16_t wr_c_phase;
		uint16_t var_c_phase;
		uint16_t wh_total;
		uint16_t wh_a_phase;
		uint16_t wh_b_phase;
		uint16_t wh_c_phase;
		
	}__attribute__((packed)); 
	typedef struct status_struct Status_t;
	
	struct status_msg_t_struct
	{
		uint16_t fw;
		id_t id;
		time_t hora;
		float input_voltage;
		bool medicionValidaABC;      
		bool medicionValidaRST;     
		bool alimentacionExterna;
		bool bateriaOK;
		bool rel1_open;
		bool puertaAbierta;
		uint8_t keep_alive_time;
		uint8_t protocolo;
		Status_t state;		
		uint32_t tiempoEncendido;
		uint8_t calidadEnlace;
		int16_t temperaturaInterna;
		int16_t temperaturaExterna;		
	} __attribute__((packed));
	typedef struct status_msg_t_struct status_msg_test_t;
	

		
	typedef enum {
		TOTAL_POWER         = 0xB9,//ΣKW 2 3 phase total power													Read only
		TOTAL_PF            = 0xBC,//ΣPF 2 3 phase average power factor											Read only
		TOTAL_Q             = 0xBA,//ΣQ 2 3 phase inactive power 												Read only HZ 2 Frequency(1 - 400HZ)
		V_A_PHASE           = 0xC2,//A - V 2 A phase voltage 													Read only
		I_A_PHASE           = 0xC5,//A - I 2 A phase current 													Read only
		PF_A_PHASE          = 0xC8,//A - PF 2 A phase power factor 												Read only
		KW_A_PHASE          = 0xCB,//A - KW 2 A phase reactive power 											Read only
		VAR_A_PHASE         = 0xCE,//A - VAR 2 A phase inactive power 											Read only
		V_B_PHASE           = 0xD1,//B - V 2 B phase voltage 													Read only
		I_B_PHASE           = 0xD4,//B - I 2 B phase current 													Read only
		PF_B_PHASE          = 0xD7,//B - PF 2 B phase power factor 												Read only
		KW_B_PHASE          = 0xDA,//B - KW 2 B phase reactive power 											Read only
		VAR_B_PHASE         = 0xDD,//B - VAR 2 B phase inactive power 											Read only
		V_C_PHASE           = 0xE0,//C - V 2 C phase voltage 													Read only
		I_C_PHASE           = 0xE3,//C - I 2 C phase current 													Read only
		PF_C_PHASE          = 0xE6,//C - PF 2 C phase power factor 												Read only
		KW_C_PHASE          = 0xE9,//C - KW 2 C phase reactive power 											Read only
		VAR_C_PHASE         = 0xEC,//C - VAR 2 C phase inactive power 											Read only
		WH_TOTAL            = 0xF0,//ΣKWH 2 3 phase total KWH 													Read only
		WH_A_PHASE          = 0xF5,//A - KWH 2 A phase KWH 														Read only
		WH_B_PHASE          = 0xFA,//B - KWH 2 B phase KWH 														Read only
		WH_C_PHASE          = 0x5A,//C - KWH 2 C phase KWH 														Read only
		COM_ADD             = 0x03,//Add 1 Communication address 												R / W
		AL1                 = 0x04,//AL1 2 Alarm 1 value 														R / W
		AL1_D               = 0x08,//HY1 2 AL1 deviation alarm setting 											R / W
		AL1_M               = 0x0B,//AM1 1 AL1 Alarm mode 														R / W
		AL2                 = 0x0C,//AL2 2 Alarm 2 value 														R / W
		AL2_D               = 0x10,//HY2 2 AL2 deviation alarm setting 											R / W
		AL2_M               = 0x13,//M2 1 AL2 Alarm mode 														R / W
		AL3                 = 0x14,//AL3 2 Alarm 3 value 														R / W
		AL3_D               = 0x18,//HY3 2 AL3 deviation alarm setting 											R / W
		AL3_M               = 0x1B,//AM3 1 AL3 Alarm mode 														R / W
		I_OUTPUT_SET        = 0x1C,//BSL 2 Current output parameter setting 									R / W
		I_LOW_SET           = 0x20,//BRL 2 Current output low limit setting 									R / W
		I_HIGH_SET          = 0x24,//BRH 2 Current output high limit setting 									R / W
		V_PTA               = 0x28,//PTA 2 3 phase voltage PT ratio setting 									R / W
		CT_I_A              = 0x2C,//CTA 2 A phase current CT ratio setting 									R / W
		CT_I_B              = 0x38,//CTB 2 B phase current CT ratio setting 									R / W
		CT_I_C              = 0x3C,//CTC 2 C phase current CT ratio setting 									R / W
		POINT_FLOAT_DISPLAY = 0x3F,//ADP 1 Decimal point of current display setting 							R / W
		PARAMETER_DISPLAY   = 0x40,//DIS 1 Parameter display setting when power on 								R / W
		PHASE_WIRE          = 0x43 //LIN 1 3 phase wire - connection type 										R / W		
	} MemoryAddress;	
	
	Status_t* Status;
	Config_t* Config;
	virtual dw93_device* Clone() {
		return new dw93_device(*this);
	}
	virtual ReturnCodes read_meter() {
	
		Modbus_Configurar_Timeout(20);
	
		ReadValue(METER_ADDRESS, &(Status->current_a_phase), I_A_PHASE, "I A", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->current_b_phase), m->I_B_PHASE, "I B", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->current_c_phase), m->I_C_PHASE, "I C", 3);
		ReadValue(METER_ADDRESS, &(Status->pf_a_phase), PF_A_PHASE, "PF A", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->pf_c_phase), m->PF_B_PHASE, "PF B", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->pf_c_phase), m->PF_C_PHASE, "PF C", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->total_pf), m->TOTAL_PF, "T PF", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->total_power), m->TOTAL_POWER, "T W", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->total_q), m->TOTAL_Q, "T Q", 3);
		ReadValue(METER_ADDRESS, &(Status->var_a_phase), VAR_A_PHASE, "VAR A", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->var_b_phase), m->VAR_B_PHASE, "VAR B", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->var_c_phase), m->VAR_C_PHASE, "VAR C", 3);
		ReadValue(METER_ADDRESS, &(Status->volt_a_phase), V_A_PHASE, "V A", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->var_b_phase), m->V_B_PHASE, "V B", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->volt_c_phase), m->V_C_PHASE, "V C", 3);
		ReadValue(METER_ADDRESS, &(Status->wh_a_phase), WH_A_PHASE, "WH A", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->wh_b_phase), m->WH_B_PHASE, "WH B", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->wh_c_phase), m->WH_C_PHASE, "WH C", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->wh_total), m->WH_TOTAL, "W T", 3);
		ReadValue(METER_ADDRESS, &(Status->wr_a_phase), KW_A_PHASE, "KW A", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->wr_b_phase), m->KW_B_PHASE, "KW B", 3);
		//ReadValue(METER_ADDRESS, &(m->Status->wr_c_phase), m->KW_C_PHASE, "KW C", 3);	
		return RETURN_OK;
	}
	virtual ReturnCodes reset_meter() {
		Status = { 0 }; //set 0 
	
		Modbus_Configurar_Timeout(20);	
	
		ConfigValue(METER_ADDRESS, (Status->current_a_phase), I_A_PHASE, "I A", true, 3);
		ConfigValue(METER_ADDRESS, (Status->current_b_phase), I_B_PHASE, "I B", true, 3);
		ConfigValue(METER_ADDRESS, (Status->current_c_phase), I_C_PHASE, "I C", true, 3);
		ConfigValue(METER_ADDRESS, (Status->pf_a_phase), PF_A_PHASE, "PF A", true, 3);
		ConfigValue(METER_ADDRESS, (Status->pf_c_phase), PF_B_PHASE, "PF B", true, 3);
		ConfigValue(METER_ADDRESS, (Status->pf_c_phase), PF_C_PHASE, "PF C", true, 3);
		ConfigValue(METER_ADDRESS, (Status->total_pf), TOTAL_PF, "T PF", true, 3);
		ConfigValue(METER_ADDRESS, (Status->total_power), TOTAL_POWER, "T W", true, 3);
		ConfigValue(METER_ADDRESS, (Status->total_q), TOTAL_Q, "T Q", true, 3);
		ConfigValue(METER_ADDRESS, (Status->var_a_phase), VAR_A_PHASE, "VAR A", true, 3);
		ConfigValue(METER_ADDRESS, (Status->var_b_phase), VAR_B_PHASE, "VAR B", true, 3);
		ConfigValue(METER_ADDRESS, (Status->var_c_phase), VAR_C_PHASE, "VAR C", true, 3);
		ConfigValue(METER_ADDRESS, (Status->volt_a_phase), V_A_PHASE, "V A", true, 3);
		ConfigValue(METER_ADDRESS, (Status->var_b_phase), V_B_PHASE, "V B", true, 3);
		ConfigValue(METER_ADDRESS, (Status->volt_c_phase), V_C_PHASE, "V C", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wh_a_phase), WH_A_PHASE, "WH A", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wh_b_phase), WH_B_PHASE, "WH B", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wh_c_phase), WH_C_PHASE, "WH C", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wh_total), WH_TOTAL, "W T", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wr_a_phase), KW_A_PHASE, "KW A", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wr_b_phase), KW_B_PHASE, "KW B", true, 3);
		ConfigValue(METER_ADDRESS, (Status->wr_c_phase), KW_C_PHASE, "KW C", true, 3);
		return RETURN_OK;
	}
	virtual ReturnCodes idle() {
		for (; idleTime > 0; idleTime--) {
			//NeetMeter::Debug::Printf(//NeetMeter::Debug::RELEASE_LEVEL, true, "[Meter] Idle! zzZZzzZZZzZZ %d\n", idleTime);
			sleep(1);
		}
		idleTime = 10;	
		return RETURN_OK;
	}
	virtual ReturnCodes setting_meter() {
		Modbus_Configurar_Timeout(20);
	
		//Communication address position 0x01 by default
		//?
		//?
		//?
	
		return RETURN_OK;
	}


};

