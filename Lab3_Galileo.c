#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


int main( int argc, char **argv )
{
        int i;
        int r;
        int fd;
        unsigned char command[2];
        unsigned char value[4]={0};
		int readTemp = 0;
		int picTaken = 0;

        useconds_t delay = 2000;

        char *dev = "/dev/i2c-0";
        int addr = 0x48;

        printf("TMP102 Test\n");

        fd = open(dev, O_RDWR );
        if(fd < 0)
        {
                perror("Opening i2c device node\n");
                return 1;
        }

        r = ioctl(fd, I2C_SLAVE, addr);
        if(r < 0)
        {
                perror("Selecting i2c device\n");
        }

			//reads temperature
        while(1)
        {
					
		while (readTemp < 27)	
			{				
                for(i = 0; i < 4; i++)
                {
                       command[0] = 0x40 | ((i + 1) & 0x03); // output enable | read input i
                       command[1]++;
                       r = write(fd, &command, 2);
                        usleep(delay);
                        // the read is always one step behind the selected input
                        r = read(fd, &value[i], 1);
                        if(r != 1)
                        {
                                perror("reading i2c device\n");
                        }
                        usleep(delay);
                }
                
				printf("Temp = %d degC\n", value[3]);
				readTemp = value[3];
			}
			
			if (readTemp >= 27)
			{
				printf("THRESHOLD REACHED: REQUESTING IMAGE\n");
			
				char buffer[50];
				sprintf(buffer, "./run");
				system(buffer);
				picTaken++;
				usleep(delay);
			}
        }

        close(fd);
        return(0);
}
