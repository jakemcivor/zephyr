/*
 * Copyright (c) 2017
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VL53L0X_PROXIMITY_H
#define __VL53L0X_PROXIMITY_H

/* Defines -------------------------------------------------------------------*/
#define PROXIMITY_I2C_ADDRESS         ((uint16_t)0x0052)
#define VL53L0X_ID                    ((uint16_t)0xEEAA)
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC

void VL53L0X_PROXIMITY_MspInit(void);
uint16_t VL53L0X_PROXIMITY_GetDistance(void);
void VL53L0X_PROXIMITY_Init(void);

#endif /* __VL53L0X_PROXIMITY_H */



#ifndef __SENSOR_VL53L0X_H__
#define __SENSOR_VL53L0X_H__

#include <device.h>
#include <zephyr/types.h>

#define SYS_LOG_DOMAIN "VL53L0X"
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_SENSOR_LEVEL
#include <logging/sys_log.h>

#define VL53L0X_I2C_ADDR		0x52
#define Vl52L0X_AUTOINCREMENT_ADDR	BIT(7)
	

//#define VL53L0X_REG_IRQ_SRC		0x00
//#define VL53L0X_REG_STAT		0x01
//#define VL53L0X_REG_IRQ_MSK		0x03

//#define VL53L0X_REG_PROX_CTRL0		0x06
//#define VL53L0X_REG_PROX_CTRL1		0x07

/* These are used both in the IRQ_SRC register, to identify which
 * interrupt occur, and in the IRQ_MSK register, to enable specific
 * interrupts.
 */
//#define VL53L0X_CONV_DONE_IRQ		(1 << 3)
//#define VL53L0X_NEAR_FAR_IRQ		((1 << 5) | (1 << 6))

struct vl53l0x_data {
	struct device *i2c_master;
	u16_t i2c_slave_addr;
	u8_t prox_stat;

	struct gpio_callback gpio_cb;

#ifdef CONFIG_VL53L0X_TRIGGER_OWN_THREAD
	struct k_sem sem;
#endif

#ifdef CONFIG_VL53L0X_TRIGGER_GLOBAL_THREAD
	struct k_work work;
	struct device *dev;
#endif

#ifdef CONFIG_VL53L0X_TRIGGER
	struct sensor_trigger trigger_drdy;
	struct sensor_trigger trigger_near_far;

	sensor_trigger_handler_t handler_drdy;
	sensor_trigger_handler_t handler_near_far;
#endif
};

#ifdef CONFIG_VL53L0X_TRIGGER
int vl53l0x_setup_interrupt(struct device *dev);
int vl53l0x_trigger_set(struct device *dev,
		       const struct sensor_trigger *trig,
		       sensor_trigger_handler_t handler);
#else
static inline int vl53l0x_setup_interrupt(struct device *dev)
{
	return 0;
}
#endif


#endif /* __SENSOR_VL53L0X_H__ */
