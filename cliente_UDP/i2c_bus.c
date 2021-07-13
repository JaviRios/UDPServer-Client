/*
 * i2c_bus.c
 *
 *  Created on: Apr 16, 2021
 *      Author: jrios
 */
#include "i2c_bus.h"


char read_byte(colorsensor *handle, int reg){

 // esta estructura de mensaje se utiliza para leer y escribir

   unsigned char write_bytes, read_bytes;
   struct i2c_rdwr_ioctl_data packets;
   struct i2c_msg messages[2]; //Set size depending of number of messages in one transaction
   int e_ioctl = -1;

   write_bytes = reg; // register to read
  //Send the addres in a message
   messages[0].addr  = handle->address;
   messages[0].flags = 0;
   messages[0].len   = sizeof(write_bytes);
   messages[0].buf   = &write_bytes;

  //Incoming data will be written here
   messages[1].addr  = handle->address;
   messages[1].flags = I2C_M_RD;
   messages[1].len   = sizeof(read_bytes);
   messages[1].buf   = &read_bytes;

  //Build packet
   packets.msgs = messages;
   packets.nmsgs = 2;

   //Send to the bus
   e_ioctl = ioctl(handle->fd,I2C_RDWR,&packets);

   if (e_ioctl < 0 )
      printf("Error: unable to send messages. IN READ_BYTE\n");

   return read_bytes; // Stores byte from the colorsensor

   }

void write_byte(colorsensor *handle, int reg, char data){

   unsigned char write_bytes [2];
   struct i2c_rdwr_ioctl_data packets;
   struct i2c_msg messages[1];
   int e_ioctl = -1;

   write_bytes[0]=reg;
   write_bytes[1]=data;


   messages[0].addr  = handle->address;
   messages[0].flags = 0;
   messages[0].len   = sizeof(write_bytes);
   messages[0].buf   = write_bytes;

    packets.msgs  = messages;
    packets.nmsgs = 1;


    e_ioctl = ioctl(handle->fd,I2C_RDWR,&packets);
    if (e_ioctl == -1 )
      printf("Error: unable to send messages. IN WRITE_BYTE\n");

 }

