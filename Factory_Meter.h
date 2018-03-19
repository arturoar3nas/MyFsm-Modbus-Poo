#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#include "Meter_device.h"
#include "Aob292e.h"
#include "dw93_device.h"


class Aob292e;
class dw93_device;

typedef enum
{ 
	AOB292E2,
	DW93
}meters;


class Factory_meter {
public:
	Meter_device* createMeter(meters m) {
		switch (m)
		{
		case AOB292E2:
			return new Aob292e;
			break;
		case DW93:
			return new dw93_device;
		default:
			break;
		}
	return NULL;
	}
};
