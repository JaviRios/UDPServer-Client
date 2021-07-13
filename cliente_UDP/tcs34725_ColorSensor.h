/*
 * tcs34725_ColorSensor.h
 *
 *  Created on: Apr 16, 2021
 *      Author: jrios
 */

#ifndef TCS34725_COLORSENSOR_H_
#define TCS34725_COLORSENSOR_H_

#define ADDR  0x29 //7-bit Slave address
#define ENA_REG 0x80
#define ATIME_REG 0x81
#define WTIME_REG 0x83
#define CONTROL_REG 0X8F
#define ID_REG 0X92
#define CDATAL 0x94
#define CDATAH 0x95
#define RDATAL 0x96
#define RDATAH 0x97
#define GDATAL 0x98
#define GDATAH 0x99
#define BDATAL 0x9A
#define BDATAH 0x9B

char data[8];

/**
 *Devuelve si el color mayoritario es el rojo
 * @return 1
 */
int getValorRojo(void);
/**
 * Devuelve si el color mayoritario es el Azul
 * @return
 */
int getValorAzul(void);
/**
 * Devuelve si el color mayoritario es el verde
 * @return 1
 */
int getValorVerde(void);

typedef struct tcs34725_{
	int fd;
	unsigned char address;
	char data[8];
}
colorsensor;

/**
 * Realiza la configuración para realizar medidas sobre el sensor TCS34725
 * @return colorsensor
 */
colorsensor config ();
/**
 *
 * @param handle
 */
void reading_data(colorsensor *handle);


#endif /* TCS34725_COLORSENSOR_H_ */
