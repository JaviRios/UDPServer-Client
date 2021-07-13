/*
 * acelerometro.h
 *
 *  Created on: May 16, 2021
 *      Author: jrios
 */

#ifndef ACELEROMETRO_H_
#define ACELEROMETRO_H_

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>
#include "i2c_bus.h"

#define I2C_DEVICE              "/dev/i2c-1"
#define I2C_ADDR_MMA8451         0x1D
#define READING_REGISTER_DATA    0x00
#define WHOIAM                   0x0D

char buf[6];
int fd;
char portName[20];
char reg[2];


colorsensor MMA8451_Init();
void MMA8451_Read_All(colorsensor *handle);

#endif /* ACELEROMETRO_H_ */
