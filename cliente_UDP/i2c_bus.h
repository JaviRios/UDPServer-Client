/*
 * i2c_bus.h
 *
 *  Created on: Apr 16, 2021
 *      Author: jrios
 */

#ifndef I2C_BUS_H_
#define I2C_BUS_H_

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/i2c.h>
#include <string.h>
#include <sys/stat.h>

#include "tcs34725_ColorSensor.h"

//DECLARATION
char read_byte(colorsensor *handle, int reg);
void write_byte(colorsensor *handle, int reg, char data);


#endif /* I2C_BUS_H_ */
