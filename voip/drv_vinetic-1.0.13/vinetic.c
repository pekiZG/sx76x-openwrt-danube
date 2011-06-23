/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
//-----------------------------------------------------------------------
//Description:	
// generic register access tools under Linux for Amazon
//-----------------------------------------------------------------------
//Author:	Qi-Ming.Wu@infineon.com
//Created:	16-May-2005
//-----------------------------------------------------------------------
/* History
 * Last changed on:
 * Last changed by: 
 */


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "src/vinetic_io.h"

int fd;
int c=0;
int input_flag=0;
int digit_optind=0;
int cmd;
int dev_num=0;
int chan_num=-1;
int mode=1;
int pin=0;
int gpio_value=0;
void (*func)()=NULL;


void display_version()
{
   printf("amazon vinetic gpio test version1.0\nby Wu Qi Ming\nQi-Ming.Wu@infineon.com\n");

   return;
}


void vinetic_help()
{
    printf("Usage:vinetic [options] [parameter] ...\n");
    printf("options:\n");
    printf("-h --help            Display help information\n");       
    printf("-v --version         Display version information\n");
    printf("-n --device=DEV_NUM  Select device number\n");
    printf("-c --channel=CH_NUM  Select channel number\n");
    printf("-p --pin=PIN_NUM     Select GPIO pin\n");
    printf("-d --dump            Dump pin value \n");
    printf("-w --write=VALUE     Set pin value\n");   
    printf("-m --mode=MODE       Set mode\n");
    printf("MODE: 0=input, 1=output      \n");
    return;
}

void dump()
{
  int fd,ret;
  char dev_name[20]={0};
  VINETIC_IO_GPIO_CONTROL ioCtl={0};
  sprintf(dev_name,"/dev/vin%d%d",dev_num+1,chan_num+1);
  fd = open(dev_name, O_RDWR);
  if (fd <= 0)
      {
         printf("cannot open duslic device!\n");
	 exit(-1);
      }
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_RESERVE,&ioCtl);
  if(ret<0)
    {
       printf("cannot reserve gpio resource!\n");
       exit(-1);
    }    
  ret=ioctl(fd,FIO_VINETIC_GPIO_GET,&ioCtl);
    if(ret<0)
    {
       printf("cannot get gpio value!\n");
       exit(-1);
    }
  printf("VALUE = %d\n",(ioCtl.nGpio>>pin)&1);
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_RELEASE,&ioCtl);
  if(ret<0)
    {
       printf("cannot release gpio resource!\n");
       exit(-1);
    }    

  close(fd);    
  return;
}

void gpio_write()
{
  int fd,ret;
  char dev_name[20]={0};
  VINETIC_IO_GPIO_CONTROL ioCtl={0};
  sprintf(dev_name,"/dev/vin%d%d",dev_num+1,chan_num+1);
  fd = open(dev_name, O_RDWR);
  if (fd <= 0)
      {
         printf("cannot open duslic device!\n");
	 exit(-1);
      }
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_RESERVE,&ioCtl);
  if(ret<0)
    {
       printf("cannot reserve gpio resource!\n");
       exit(-1);
    }    
  ioCtl.nGpio=gpio_value<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_SET,&ioCtl);
    if(ret<0)
    {
       printf("cannot get gpio value!\n");
       exit(-1);
    }
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_RELEASE,&ioCtl);
  if(ret<0)
    {
       printf("cannot release gpio resource!\n");
       exit(-1);
    }    
  close(fd);    
   return;
}

void gpio_config()
{
  int fd,ret;
  char dev_name[20]={0};
  VINETIC_IO_GPIO_CONTROL ioCtl={0};
  sprintf(dev_name,"/dev/vin%d%d",dev_num+1,chan_num+1);
  fd = open(dev_name, O_RDWR);
  if (fd <= 0)
      {
         printf("cannot open duslic device!\n");
	 exit(-1);
      }
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_RESERVE,&ioCtl);
  if(ret<0)
    {
       printf("cannot reserve gpio resource!\n");
       exit(-1);
    }    
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=mode<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_CONFIG,&ioCtl);
    if(ret<0)
    {
       printf("cannot get gpio value!\n");
       exit(-1);
    }
    
  ioCtl.nGpio=1<<pin;
  ioCtl.nMask=1<<pin; 
  ret=ioctl(fd,FIO_VINETIC_GPIO_RELEASE,&ioCtl);
  if(ret<0)
    {
       printf("cannot release gpio resource!\n");
       exit(-1);
    }    
  close(fd);    
   return;
}


int main (int argc, char** argv) {
      while(1) {
       int option_index=0;
       static struct option long_options[] ={
           {"help",0,0,0},
	   {"version",0,0,0},
	   {"device",1,0,0},
	   {"channel",1,0,0},
	   {"pin",1,0,0},
	   {"dump",1,0,0},
	   {"write",1,0,0},
	   {"mode",1,0,0}
       };
       c = getopt_long(argc,argv, "hvn:c:p:dw:m:",
                      long_options, &option_index);
       
       if(c==-1)
         {
	  if(input_flag==0)
	  {
	   printf("vinetic:please specify parameters\n");
	   func=&vinetic_help;
          }
	  if(func)
	   (*func)();
	  else
	   {
ERROR:      vinetic_help();
	   } 
	  break;
	 }
       input_flag=1;
       switch (c) {
            case 0:
	       if(option_index==0)
	         goto HELP;
	       if(option_index==1)
		 goto VERSION;
	       if(option_index==2)
		 goto DEVICE;
	       if(option_index==3)
		 goto CHANNEL;
	       if(option_index==4)
		 goto PIN;
	       if(option_index==5)
		 goto DUMP;
	       if(option_index==6)
		 goto WRITE;
	       if(option_index==7)
		 goto MODE;	            	 	 	 	 	 			 		
HELP:	    case 'h':
	       func=&vinetic_help;
	       break;    
VERSION:    case 'v':
	       func=&display_version;
	       break;
DEVICE:     case 'n':
               if(!optarg) goto ERROR;
	       dev_num=strtoul(optarg,NULL,0);
	       break;
CHANNEL:    case 'c':
	       if(!optarg) goto ERROR;
	       chan_num=strtoul(optarg,NULL,0);
	       break;
PIN:        case 'p':
	       if(!optarg) goto ERROR;
	       pin=strtoul(optarg,NULL,0);
	       break;
DUMP:       case 'd':
	       func=&dump;
	       break;
WRITE:      case 'w':
               if(!optarg) goto ERROR;
	       gpio_value=strtoul(optarg,NULL,0);
	       func=&gpio_write;
	       break;
MODE:       case 'm':
               if(!optarg) goto ERROR;
	       mode=strtoul(optarg,NULL,0);
	       func=&gpio_config;
	       break;            
	    }     
     }
}

