/*8k
 * File:   isr.c
 * Author: HP
 *
 * Created on February 7, 2024, 1:33 PM
 */


#include <xc.h>
extern unsigned char flag,flag_5sec,flag_30sec,flag_2min,block_time,block_flag,log_flag;
unsigned long int min2_count=0;
unsigned int block_count=0;
void __interrupt() isr(void)
{
	static unsigned long int count=0;
     
	if (TMR0IF)
	{
		TMR0 = TMR0 + 8;
        if(count++==10000)
        {
            count=0;
            flag=!flag;
        }
       
        if(min2_count++==100000)
        {
            flag_5sec=0;
           
            
        }
		if(min2_count==600000)
        {
            flag_30sec=0;
        }
        if(min2_count==1200000)
        {
            flag_2min=0;
            min2_count=0;
               
        }
        if(block_count++==20000)
        {
            if(block_time>0)
            block_time--;
            block_flag=1;
            block_count=0;
        }
		TMR0IF = 0;
	}
}

