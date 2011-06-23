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
// daa access tools under Linux for Amazon
//-----------------------------------------------------------------------
//Author:	Qi-Ming.Wu@infineon.com
//Created:	26-Augest-2005
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
#include "drv_cpc5621_interface.h"


int c;
int input_flag=0;
int on=0;
void (*func)()=NULL;
int fddev;

void display_version()
{
   printf("daa test version1.0\nby Wu Qi Ming\nQi-Ming.Wu@infineon.com\n");

   return;
}


void daa_help()
{
    printf("Usage:daa [options] [parameter] ...\n");
    printf("options:\n");
    printf("-h --help            Display help information\n");       
    printf("-v --version         Display version information\n");
    return;
}


void on_hook()
{
    
    ioctl(fddev,FIO_CPC5621_SET_OH,on);
    return;

}

int main (int argc, char** argv) {

    fd_set               rfds, trfds, xfds, txfds;
   unsigned int rwd;
   
    
     int ret;
     fddev=open("/dev/cpc5621",O_RDWR);
     if(fddev==-1)
     {
      printf("cannot open daa device!\n");
      exit(-1);
     }
     
     ret=ioctl(fddev,FIO_CPC5621_INIT);
     if(ret<0)
     {
       printf("cannot initialize cpc5621!\n");
     }


		FD_ZERO(&rfds);
   	rwd = -1;
      
      FD_SET(fddev, &rfds);
      rwd = fddev;
      
       printf("----- before select \n");
      
      while(1) {
	      ret = select(rwd + 1, &rfds, NULL, NULL, NULL);
	       printf("----- after select, ret=%d\n", ret);
	
	      if (FD_ISSET(fddev, &rfds))
		   	printf("----- 1\n");
			else
		   	printf("----- 2 \n");
		}

/*
	 sleep(10);

     printf("FIO_CPC5621_SET_OH 0!\n");

     ret=ioctl(fddev,FIO_CPC5621_SET_OH, 0);
     if(ret<0)
     {
       printf("ERROR, FIO_CPC5621_SET_OH 0!\n");
     }
*/
	while(1) ;


     while(1) {
       int option_index=0;
       static struct option long_options[] ={
           {"help",0,0,0},
           {"version",0,0,0}
	   };
       c = getopt_long(argc,argv, "vho:",
                      long_options, &option_index);
       
       	
       if(c==-1)
         {
	  if(input_flag==0)
	  {
	   printf("daa:please specify parameters\n");
	   func=&daa_help;
          }
	  if(func)
	   (*func)();
	  else
	   {
ERROR:      daa_help();
	   } 
	  break;
	 }
       input_flag=1;


       switch (c) {
            case 0:
	       if(option_index==0)
	        {
		 func=&daa_help;
		 break; 
	        } 
	       if(option_index==1)
	        {
		    func=&display_version;
		    break;		
		}
	       if(option_index==2)
	        {
		    goto OH;
		    		
		}
	    case 'h':
	       func=&daa_help;
	       break;    
	    case 'v':
	       func=&display_version;
	       break;
OH:         case 'o':
               if(!optarg) goto ERROR;
	       if(strcmp(optarg,"on")==0)
               {
                    on=1;
		    
               }
               else if(strcmp(optarg,"off")==0)
               {
                    on=0;
               }
	       func=&on_hook;
                  
               break;
	        }
       }

          close(fddev);
 	    
          return 0;
}

