/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <kernel.h>
#include <i2c.h>
#include <sensor.h>
#include <gpio.h>
#include <misc/util.h>
#include "vl53l0x.h"

#ifdef CONFIG_VL53L0X_TRIGGER_OWN_THREAD
static K_THREAD_STACK_DEFINE(vl53l0x_thread_stack, CONFIG_VL53L0X_THREAD_STACK_SIZE);
static struct k_thread vl53l0x_thread;
#endif

int vl53l0x_trigger_set(struct device *dev,
		       const struct sensor_trigger *trig,
		       sensor_trigger_handler_t handler)
{
	struct vl53l0x_data *data = dev->driver_data;

	switch (trig->type) {
	case SENSOR_TRIG_DATA_READY:
		if (i2c_reg_update_byte(data->i2c_master,
					data->i2c_slave_addr,
					VL53L0X_REG_IRQ_MSK,
					VL53L0X_CONV_DONE_IRQ,
					VL53L0X_CONV_DONE_IRQ) < 0) {
			return -EIO;
		}
		data->handler_drdy = handler;
		data->trigger_drdy = *trig;
		break;

	case SENSOR_TRIG_NEAR_FAR:
		if (i2c_reg_update_byte(data->i2c_master,
					data->i2c_slave_addr,
					VL53L0X_REG_IRQ_MSK,
					VL53L0X_NEAR_FAR_IRQ,
					VL53L0X_NEAR_FAR_IRQ) < 0) {
			return -EIO;
		}
		data->handler_near_far = handler;
		data->trigger_near_far = *trig;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

#ifdef CONFIG_VL53L0X_TRIGGER_OWN_THREAD

static void vl53l0x_gpio_cb(struct device *port,
			   struct gpio_callback *cb, u32_t pins)
{
	struct vl53l0x_data *data =
		CONTAINER_OF(cb, struct vl53l0x_data, gpio_cb);

	ARG_UNUSED(pins);

	k_sem_give(&data->sem);
}

static void vl53l0x_thread_main(int arg1, int unused)
{
	struct device *dev = INT_TO_POINTER(arg1);
	struct vl53l0x_data *data = dev->driver_data;
	u8_t reg_val;

	ARG_UNUSED(unused);

	while (1) {
		k_sem_take(&data->sem, K_FOREVER);

		if (i2c_reg_read_byte(data->i2c_master, data->i2c_slave_addr,
					VL53L0X_REG_IRQ_SRC, &reg_val) < 0) {
			SYS_LOG_DBG("vl53l0x: error reading IRQ source register");
			continue;
		}

		if ((reg_val & VL53L0X_CONV_DONE_IRQ) && data->handler_drdy) {
			data->handler_drdy(dev, &data->trigger_drdy);
		}

		if ((reg_val & VL53L0X_NEAR_FAR_IRQ) && data->handler_near_far) {
			data->handler_near_far(dev, &data->trigger_near_far);
		}
	}
}

#else /* CONFIG_VL53L0X_TRIGGER_GLOBAL_THREAD */

static void vl53l0x_gpio_cb(struct device *port,
			   struct gpio_callback *cb, u32_t pins)
{
	struct vl53l0x_data *data =
		CONTAINER_OF(cb, struct vl53l0x_data, gpio_cb);

	ARG_UNUSED(pins);

	k_work_submit(&data->work);
}

static void vl53l0x_gpio_thread_cb(void *arg)
{
	struct device *dev = arg;
	struct vl53l0x_data *data = dev->driver_data;
	u8_t reg_val;

	if (i2c_reg_read_byte(data->i2c_master, data->i2c_slave_addr,
			      VL53L0X_REG_IRQ_SRC, &reg_val) < 0) {
		SYS_LOG_DBG("vl53l0x: error reading IRQ source register");
		return;
	}

	if ((reg_val & VL53L0X_CONV_DONE_IRQ) && data->handler_drdy) {
		data->handler_drdy(dev, &data->trigger_drdy);
	}

	if ((reg_val & VL53L0X_NEAR_FAR_IRQ) && data->handler_near_far) {
		data->handler_near_far(dev, &data->trigger_near_far);
	}
}
#endif /* CONFIG_VL53L0X_TRIGGER_GLOBAL_THREAD */

#ifdef CONFIG_VL53L0X_TRIGGER_GLOBAL_THREAD
static void vl53l0x_work_cb(struct k_work *work)
{
	struct vl53l0x_data *data =
		CONTAINER_OF(work, struct vl53l0x_data, work);

	vl53l0x_gpio_thread_cb(data->dev);
}
#endif

int vl53l0x_setup_interrupt(struct device *dev)
{
	struct vl53l0x_data *data = dev->driver_data;
	struct device *gpio;

#ifdef CONFIG_VL53L0X_TRIGGER_OWN_THREAD
	k_sem_init(&data->sem, 0, UINT_MAX);
#else
	data->work.handler = vl53l0x_work_cb;
	data->dev = dev;
#endif

	gpio = device_get_binding(CONFIG_VL53L0X_GPIO_CONTROLLER);
	if (!gpio) {
		SYS_LOG_DBG("vl53l0x: gpio controller %s not found",
			    CONFIG_VL53L0X_GPIO_CONTROLLER);
		return -EINVAL;
	}

	gpio_pin_configure(gpio, CONFIG_VL53L0X_GPIO_PIN,
			   GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE |
			   GPIO_INT_ACTIVE_LOW | GPIO_INT_DEBOUNCE);

	gpio_init_callback(&data->gpio_cb,
			   vl53l0x_gpio_cb,
			   BIT(CONFIG_VL53L0X_GPIO_PIN));

	gpio_add_callback(gpio, &data->gpio_cb);
	gpio_pin_enable_callback(gpio, CONFIG_VL53L0X_GPIO_PIN);

#ifdef CONFIG_VL53L0X_TRIGGER_OWN_THREAD
	k_thread_create(&vl53l0x_thread, vl53l0x_thread_stack,
			CONFIG_VL53L0X_THREAD_STACK_SIZE,
			vl53l0x_thread_main, POINTER_TO_INT(dev), 0, NULL,
			K_PRIO_COOP(CONFIG_VL53L0X_THREAD_PRIORITY), 0, 0);
#endif

	return 0;
}
