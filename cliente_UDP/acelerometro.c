#include "acelerometro.h"

static uint16_t catBytes(uint8_t highB, uint8_t lowB){
	return ((uint16_t)highB << 8) | (uint16_t)lowB;
}

//int main(void){
//
//	colorsensor acelerometro = MMA8451_Init();
//    printf("Acelerometer Initialized\n");
//
//  	while(1){							//RPI must wait an interval of 800us to 3ms
//      MMA8451_Read_All(&acelerometro);
//	  }
//
//	close(fd);
//	return 0;
//}



colorsensor MMA8451_Init(){
	colorsensor handle;
	handle.address = I2C_ADDR_MMA8451;
	handle.fd = -1;
	char i2cFile[11];
	int device = 1;

	printf("Initalising ACELEROMETER at address %#02x\n", handle.address);

	sprintf(i2cFile, "/dev/i2c-%d", device);

	handle.fd= open(i2cFile, O_RDWR);
	if(fd < 0) {
			printf("Error: file descriptor opening\n");
      }
	if(ioctl(fd, I2C_SLAVE, I2C_ADDR_MMA8451) < 0){
			printf("Error: ioctl couldnt wake slave\n");
	}

     write_byte(&handle, 0x0E, 0x01);

     write_byte(&handle, 0x2A, 0x01);

     if(read_byte(&handle, WHOIAM) == 0x1A)
         	printf("ACELEROMETER MMA8451Q detected\n");
         else{
         	printf("ACELEROMETER MMA8451Q NOT DETECTED. Connect MMA8451Q...\n");
             exit (1);
         }

     return handle;

}


void MMA8451_Read_All(colorsensor *handle){
// To return the data read
        char x [20];
        char y [20];
        char z [20];

        double axlex=0;
        double axley=0;
        double axlez=0;

        //datos que se envian al servidor rawdata
		buf[0] = read_byte(handle, 0x01);
		buf[1] = read_byte(handle, 0x02);
		buf[2] = read_byte(handle, 0x03);
		buf[3] = read_byte(handle, 0x04);
		buf[4] = read_byte(handle, 0x05);
		buf[5] = read_byte(handle, 0x06);

    uint16_t Pos_X = catBytes(buf[1],buf[2]);
      if(buf[1]>0x7F){
         Pos_X= ~Pos_X+1;
         Pos_X = (Pos_X >> 2);
         axlex=Pos_X;
         axlex=axlex/2048;
        sprintf(x,"X-Axis:-%.2f\n ", axlex);

      }else {
         Pos_X = (Pos_X >> 2);
         axlex=Pos_X;
         axlex=axlex/2048;
         sprintf(x,"X-Axis:%.2f\n ", axlex);
         }
   
		uint16_t Pos_Y = catBytes(buf[3],buf[4]);
       if(buf[3]>0x7F){
        Pos_Y= ~Pos_Y+1;
       Pos_Y = (Pos_Y >> 2);
        axley=Pos_Y;
        axley=axley/2048;
        sprintf(y,"Y-Axis:-%.2f\n ", axley);

       }else {

          Pos_Y = (Pos_Y >> 2);
          axley=Pos_Y;
          axley=axley/2048;
          sprintf(y,"Y-Axis:%.2f\n ", axley);
        }

  	uint16_t Pos_Z = catBytes(buf[5],buf[6]);
       if(buf[5]>0x7F){
         Pos_Z= ~Pos_Z+1;
         Pos_Z = (Pos_Z >> 2);
         axlez=Pos_Z;
         axlez=axlez/2048;
         sprintf(z,"Z-axis:-%.2f\n", axlez);
       
        } else{
       
          Pos_Z = (Pos_Z >> 2);
          axlez=Pos_Z;
          axlez=axlez/2048;
          sprintf(z,"Z-Axis:%.2f\n", axlez);
          }
    

		printf("\n POSITION MEASURED\n ");
		printf(x);
		printf(y);
		printf(z);
		sleep(3);
}
