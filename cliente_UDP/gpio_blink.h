/*
 * gpio_blink.h
 *
 *  Created on: 16 abr. 2021
 *      Author: jaime
 */

#ifndef GPIO_BLINK_H_
#define GPIO_BLINK_H_


#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//CONFIGURACIÓN COMO PIN DE SALIDA O ENTRADA
#define IN  0
#define OUT 1

//VALOR LÓGICO ALTO O BAJO
#define LOW  0
#define HIGH 1


#define PIN  24
#define BLUE_LED 5  //GPIO 5 pin29
#define GREEN_LED 6 //GPIO 6 pin31
#define RED_LED 4 //GPIO 4 pin4


int GPIOExport(int pin);

int GPIOUnexport(int pin);

/**
 * Configura como salida o entrada un PIN
 * @param pin
 * @param dir
 * @return
 */
int GPIODirection(int pin, int dir);

/**
 * Realiza una lectura en el pin
 * @param pin
 * @return
 */
int GPIORead(int pin);

/**
 * Realiza una escritura digital en el pin que se pasa por parámetro * @param pin
 * @param value
 * @return
 */
int GPIOWrite(int pin, int value);



#endif /* GPIO_BLINK_H_ */
