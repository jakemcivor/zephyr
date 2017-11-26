/*
 * Copyright (c) 2017
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sensor.h>
#include <stdio.h>

void main(void)
{
	struct sensor_value pres,temp;
	struct device *dev = device_get_binding("LPS22HB");

	if (dev == NULL) {
		printf("Could not get lps22hb device\n");
		return;
	}

	while (1) {
		if (sensor_sample_fetch(dev) < 0) {
			printf("Sensor sample update error\n");
			return;
		}
		
		if (sensor_channel_get(dev, SENSOR_CHAN_PRESS, &pres) < 0) {
			printf("Cannot read LPS22HB pressure channel\n");
			return;
		}

		if (sensor_channel_get(dev, SENSOR_CHAN_TEMP, &temp) < 0) {
			printf("Cannot read LPS22HB temperature channel\n");
			return;
		}

		/* Print pressure data*/
		printf("Pressure:%.3f kPa\n", sensor_value_to_double(&pres));
		
		/* Print pressure data*/
		printf("Temperature:%.1f C\n", sensor_value_to_double(&temp));

		k_sleep(5000);
	}
}
