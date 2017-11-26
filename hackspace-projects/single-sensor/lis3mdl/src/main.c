/*
 * Copyright (c) nobody yet - cobbled together by Bob A.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sensor.h>
#include <stdio.h>
#include <lis3mdl.h>

#define DECIMATION_FACTOR	4

static void trigger_handler(struct device *dev, struct sensor_trigger *trigger)
{
	/* tbd
}

void main(void)
{
	struct sensor_value accel[3];
	struct sensor_value magn[3];
	
	struct device *dev = device_get_binding("LIS3MDL");

	if (dev == NULL) {
		printf("Could not get fxos8700 device\n");
		return;
	}

	struct sensor_trigger trig = {
		.type = SENSOR_TRIG_DATA_READY,
		.chan = SENSOR_CHAN_ACCEL_XYZ,
	};
	
	if (sensor_trigger_set(dev, &trig, trigger_handler)) {
		printf("Could not set trigger\n");
		return;
	}

	while (1) {
		
		sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
		sensor_channel_get(dev, SENSOR_CHAN_MAGN_XYZ, magn);

		if (sensor_channel_get(dev, , &  ) < 0) {
			printf("Cannot read  channel\n");
			return;
		}



		/* Print accel x,y,z and mag x,y,z data */
		printf("AX=%10.6f AY=%10.6f AZ=%10.6f "
		       "MX=%10.6f MY=%10.6f MZ=%10.6f "
		       "T=%10.6f\n",
		       sensor_value_to_double(&accel[0]),
		       sensor_value_to_double(&accel[1]),
		       sensor_value_to_double(&accel[2]),
		       sensor_value_to_double(&magn[0]),
		       sensor_value_to_double(&magn[1]),
		       sensor_value_to_double(&magn[2]),
	
		k_sleep(2000);
	}
}
