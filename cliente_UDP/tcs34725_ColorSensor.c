/*
 * tcs34725_ColorSensor.c
 *
 *  Created on: Apr 16, 2021
 *      Author: jrios
 */
#include "tcs34725_ColorSensor.h"
#include "i2c_bus.h"

int valorAzul = 0;
int valorVerde = 0;
int valorRojo = 0;
int valorIR = 0;

int getValorAzul(void){
	return valorAzul;
}

int getValorRojo(void){
	return valorRojo;
}

int getValorVerde(void){
	return valorVerde;
}




colorsensor config (){
	colorsensor handle;

    handle.fd = -1; //Start as uninitialized
    handle.address = ADDR;
	char i2cFile[11];
    int device = 1;

    printf("Initalising TCS34725 sensor at address %#02x\n", handle.address);

    sprintf(i2cFile, "/dev/i2c-%d", device);

    handle.fd = open(i2cFile, O_RDWR); //Obtain file descriptor for RW
    if (handle.fd < 0 ){
      printf("Error: file descriptor opening\n");
    }
    int v_ioctl = ioctl(handle.fd, I2C_SLAVE, ADDR); //Configure the file for I2C communications with slave
    if(v_ioctl < 0)
      printf("Error: ioctl couldn´t wake slave\n");
    // Select enable register(0x80)
    // Power ON, RGBC enable, wait time disable(0x03)
    write_byte(&handle, ENA_REG, 0x03);
    // Select ALS time register(0x81)
	// Atime = 700 ms(0x00)
    write_byte( &handle, ATIME_REG, 0x00);
    // Select Wait Time register(0x83)
    // WTIME : 2.4ms(0xFF)
    write_byte( &handle, WTIME_REG, 0xFF);
    // Select control register(0x8F)
    // AGAIN = 1x(0x00)
    write_byte( &handle, CONTROL_REG, 0x02);
    //ID REGISTERs
    if(read_byte(&handle, ID_REG) == 0x44)
    	printf("Colorsensor TCS34725 detected\n");
    else{
    	printf("Colorsensor TCS34725 NOT DETECTED. Connect TCS34725...\n");
        exit (1);
    }

    return handle;
}

void reading_data(colorsensor *handle){
	// Read 8 bytes of data from register(0x94)
	// cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb

		data[0] = read_byte(handle, CDATAL);
		data[1] = read_byte(handle, CDATAH);
		data[2] = read_byte(handle, RDATAL);
		data[3] = read_byte(handle, RDATAH);
		data[4] = read_byte(handle, GDATAL);
		data[5] = read_byte(handle, GDATAH);
		data[6] = read_byte(handle, BDATAL);
		data[7] = read_byte(handle, BDATAH);

			// Convert the data
			valorIR=  (data[1] * 256 + data[0]);
			valorRojo= (data[3] * 256 + data[2]);
			valorVerde= (data[5] * 256 + data[4]);
			valorAzul= (data[7] * 256 + data[6]);

			float luminance = (0.299) * (valorRojo) + (0.587) * (valorVerde) + (0.114) * (valorAzul);//Formula de audiovisuales
			if(luminance < 0)
			{
				luminance = 0;
			}

			// Output data to screen
			printf("Color ROJO  : %d lux \n", valorRojo);
			printf("Green color luminance : %d lux \n", valorVerde);
			printf("Blue color luminance : %d lux \n", valorAzul);
			printf("IR  luminance : %d lux \n", valorIR);
			printf("Ambient Light Luminance : %.2f lux \n", luminance);
}

