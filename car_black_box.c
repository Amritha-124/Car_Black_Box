/*
 * File:   car_black_box.c
 * Author: HP
 *
 * Created on February 5, 2024, 12:07 PM
 */


#include <xc.h>

#include "car_black_box.h"
#include "clcd.h"
#include "adc.h"
#include "keypad.h"
#include <string.h>
#include "ds1307.h"
#include "ext_eeprom.h"
#include "uart.h"

unsigned char i=0;
char *event[8]={"ON","GR","GN","G1","G2","G3","G4","_C"};
unsigned char *menu[5]={"VIEW LOG       ","DOWNLOAD MENU  ","CLEAR LOG      ","SET TIME       ","CHANGE PASSWORD"};
unsigned char flag=1,flag_5sec=1,flag_30sec=1,flag_2min=1,pass_flag=0;
unsigned char login_flag=0,block_time=120,block_flag=0,log_flag=0,lng_sw,log_count=0,download_flag=0;
extern unsigned long int min2_count;
extern unsigned int block_count;
 char time[9]="",str_log[6]="",log[16]="";
 unsigned int delay1=0,delay2=0,delay_1=0;
unsigned char pass_index=0,star_arr1[6]="    ";
//display dashboard
void display_dashboard(unsigned char key) 
{
   
    unsigned short  adc_val;
    if(key==MK_SW11 || login_flag)
        {
             login_flag=1;
            
             
             
            if(password_authentication(key)=='0')
            {
                //CLEAR_DISP_SCREEN;
                display_menu(key);
            }
            /* if(key==MK_SW1 || key ==MK_SW2 || key == MK_SW3)
             {
                 adc_val = (int)((read_adc(CHANNEL4)/(float)10.23));
                 get_time();
                 str_log[0]=((time[0]-48)*10)+(time[1]-48);
                 str_log[1]=((time[3]-48)*10)+(time[4]-48);
                str_log[2]= ((time[6]-48)*10)+(time[7]-48);
                str_log[4]=adc_val;
                str_log[5]='\0';
             
             if(key==MK_SW1)
            {
                i=7;
                str_log[3]=i;
            if(log_count++<10)
               store_log(str_log); 

            }
            else if(key==MK_SW2)
            {
                if(i==7)
                    i=2;
                else if(i<6)
                  i++;
                str_log[3]=i;
            if(log_count++<10)
            {
                store_log(str_log);

            }

            }
            else if(key==MK_SW3)
            {
                if(i==7)
                    i=2;
                else if(i>1)
                    i--;
                str_log[3]=i;
                if(log_count++<10)
                store_log(str_log);

            }
          }*/
    
        }
    else
    {
        //reading adc value
    adc_val = (int)((read_adc(CHANNEL4)/(float)10.23));
    
    clcd_print("  TIME   EV  SP ",LINE1(0));
    get_time();
    clcd_print(time,LINE2(0));
    clcd_putch(' ',LINE2(8));
    clcd_putch(' ',LINE2(11));
    clcd_putch(' ',LINE2(12));
    clcd_putch(' ',LINE2(15));
    //loading value to storing log to eeprom
    str_log[0]=((time[0]-48)*10)+(time[1]-48);
    str_log[1]=((time[3]-48)*10)+(time[4]-48);
    str_log[2]= ((time[6]-48)*10)+(time[7]-48);
    str_log[4]=adc_val;
    str_log[5]='\0';
    //reading log count
    log_count=read_external_eeprom(0x32);
    //key to collision
    if(key==MK_SW1)
    {
        i=7;
        str_log[3]=i;
    if(log_count++<10)
       store_log(str_log); 
       
    }
    //key to increment log
    else if(key==MK_SW2)
    {
        if(i==7)
            i=2;
        else if(i<6)
          i++;
        str_log[3]=i;
    if(log_count++<10)
    {
        store_log(str_log);
        
    }
        
    }
    //key to scroll up log
    else if(key==MK_SW3)
    {
        if(i==7)
            i=2;
        else if(i>1)
            i--;
        str_log[3]=i;
        if(log_count++<10)
        store_log(str_log);
        
    }
    
    //displaying in clcd
    clcd_print(event[i],LINE2(9));
    clcd_putch(((adc_val/10)+'0'),LINE2(13));
    clcd_putch(((adc_val%10)+'0'),LINE2(14));
    
    }
}
//password authentication
unsigned char pass[5]="";
unsigned char password_authentication(unsigned char sw)
{
    static short int chance=3;
   
   //reading password from eeprom to compare
    if(pass_index==0)
    {
        pass[0]=read_external_eeprom(0x33); 
        pass[1]=read_external_eeprom(0x34);
        pass[2]=read_external_eeprom(0x35);
        pass[3]=read_external_eeprom(0x36);
        pass[4]='\0';
    }
    
        
   
    static char arr[5]="";
     
    
         if(pass_index==4)
           {
             
             
            //if password matched enter menu
         if(strcmp(pass,arr)==0)
        {
            
             return '0';
        }
         //not matched
        else
        {
              pass_index=0;
              //give 3 attempt
           if(chance>1)
           {
               chance--;
               strcpy(arr,"    ");
               strcpy(star_arr1,"    ");
              
               CLEAR_DISP_SCREEN;
               clcd_print("ATTEMPT FAILURE",LINE1(0));
               clcd_putch(chance+48,LINE2(0));
               clcd_print(" ATTEMPT LEFT",LINE2(1));
               TMR0=6;
               min2_count=0;
               TMR0IE=1;
               while(flag_30sec);
               TMR0IE=0;
               min2_count=0;
               flag_5sec=1;
               flag_30sec=1;
               CLEAR_DISP_SCREEN;
           }
              //if 3 attempt over block for 2 min
           else
           {
               clcd_print("you are blocked!",LINE1(0));
               clcd_print("wait for ",LINE2(0));
               
               
               
               arr[0]=NULL;
               star_arr1[0]=NULL;
               chance=3;
               TMR0=6;
               min2_count=0;
               block_count=0;
               block_time=120;
               TMR0IE=1;
               while(flag_2min)
               {
                   if(block_flag)
                   {
                   clcd_putch((block_time/100)+48,LINE2(10));
                   clcd_putch(((block_time/10)%10)+48,LINE2(11));
                   clcd_putch((block_time%10)+48,LINE2(12));
                    clcd_putch('s',LINE2(13));
                    block_flag=0;
                   }
               }
               
               TMR0IE=0;
               min2_count=0;
               flag_5sec=1;
               flag_30sec=1;
               flag_2min=1;
                
               //return '1';
           }
           
        }
         
            
        }
            else
            {
             if(pass_index==0)
             {
             TMR0IE=1;
             }
             //enter password 
              clcd_print("ENTER PASSWORD: ", LINE1(0));
              clcd_print("            ", LINE2(4));
              if(flag_5sec==0)
              {
                  TMR0IE=0;
                  TMR0=6;
                  flag_5sec=1;
                  min2_count=0;
                  login_flag=0;
                  strcpy(arr,"    ");
                  strcpy(star_arr1,"    ");
                  pass_index=0;
                  CLEAR_DISP_SCREEN;
                  
              }
              for(unsigned long int wait=50000;wait--;);
              //clcd_print("HELLO WORLD  ", LINE1(0));
              //clcd_print("            ", LINE2(4));
             //key for 0
                if(sw==MK_SW5)
            {
                arr[pass_index]='0';
                star_arr1[pass_index]='*';
                arr[pass_index+1]='\0';
                star_arr1[pass_index+1]='\0';
                pass_index++;
                
                
                min2_count=0;
            }
             // key for 1
           if(sw==MK_SW6)
           {
                arr[pass_index]='1';
                star_arr1[pass_index]='*';
                arr[pass_index+1]='\0';
                star_arr1[pass_index+1]='\0';
                pass_index++;
               
                min2_count=0;
           }
              //cursor blinking
                /* if(flag)
                {
                clcd_putch('_',LINE2(pass_index));
                }
                else
                clcd_putch(' ',LINE2(pass_index)); */
                
                clcd_print(star_arr1,LINE2(0));
                
                
                
            }
   

}
//display menu

void display_menu(unsigned char sw)
{
    //reading switch
    lng_sw=read_switches(LEVEL_CHANGE);
    
    static int log=0;
    static unsigned char scroll=0;
    static unsigned int menu_delay_6=0,menu_delay_5=0;
    //long press to enter the selected menu
    if(log_flag ==1 || (lng_sw==MK_SW5 && menu_delay_5++>2000))
    {
        
       log_flag=1;
       menu_delay_5=0;
       if(scroll==0)
       {
           if(log==0)
       {
       if(log_count>0)
        view_log(sw);
       }
           else if(log==2)
           {
           clear_log(sw);
           }
           else if(log==1)
           {
                log=0;
               download_log(sw);
              
               log_flag=0;
               
           }
           else if(log==3)
           {
               set_time(sw);
               
           }
       }
       else
       {
           if(log+1==2)
           {
           clear_log(sw);
           }
           else if(log+1==1)
           {
               download_log(sw);
               //log=0;
               log_flag=0;
               
           }
            else if(log+1==4)
           {
              // log=0;
               change_password(sw);
           }
            else if(log+1==3)
            {
                set_time(sw);
            }
       }
       
    }
    
    else
    {
   clcd_print(menu[log],LINE1(1));
   clcd_print(menu[log+1],LINE2(1));
   if(scroll==0)
   {
     clcd_putch('*',LINE1(0));
     clcd_putch(' ',LINE2(0));
   }
   else
   {
      clcd_putch('*',LINE2(0)); 
      clcd_putch(' ',LINE1(0));
   }
    
   //long press sw6 to go back dashboard
    if ((lng_sw==MK_SW6 && menu_delay_6++>2000))
    {
        menu_delay_6=0;
        login_flag=0;
    }
   //sw 6 to scroll down
   if(lng_sw==ALL_RELEASED &&menu_delay_6>0 )
   {
       menu_delay_6=0;
     if(scroll==0)
     {
         
         scroll=1;
     }
     else if(log<3)
         log++;
         
   }
   //sw5 to scroll up
   if(lng_sw==ALL_RELEASED && menu_delay_5>0 )
   {
       menu_delay_5=0;
       if(scroll)
       {
          scroll=0;
       }
       else if(log>0)
           log--;
       
   }
   
    }
   
   
    }

//storing the log
void store_log(unsigned char* str_log)
{
    char i=0;
    static unsigned char add=0,index_viewlog=1;
    if(index_viewlog++ > 10)
    {
        index_viewlog=0;
        add=0;
        
    }
    while(str_log[i]!='\0')
    {
        //storing log
       write_external_eeprom(add++, str_log[i]); 
       i++;
    }
    //storing log count
     write_external_eeprom(0x32, log_count);
}

//view log
void view_log(unsigned char sw)
{
    
   
    unsigned char add1=0;
    static unsigned char ind=0,log_delay=0; 
    clcd_print("  TIME   EV  SP ",LINE1(0));
    clcd_print(" ",LINE2(15));
    lng_sw=read_switches(LEVEL_CHANGE);
    //sw6 long press to go back
    if(lng_sw==MK_SW6 && log_delay++>20)
    {
        log_flag=0;
        ind=0;
        log_delay=0;
    }
    //sw5 to scroll up
    if(sw==MK_SW5)
    {
        if(ind>0)
        ind--;
    }
    //sw6 to scroll down
     if(lng_sw==ALL_RELEASED && log_delay>0)
    {
        if(ind<log_count-1)
        ind++;
        log_delay=0;
        
    }
    //updating eeprom address
    add1=(5*ind);
    //reading values from clcd by character and displaying
    unsigned char temp=read_external_eeprom(add1++);
    clcd_putch((temp/10)+48,LINE2(0));
    clcd_putch((temp%10)+48,LINE2(1));
    clcd_putch(':',LINE2(2));
    temp=read_external_eeprom(add1++);
    clcd_putch((temp/10)+48,LINE2(3));
    clcd_putch((temp%10)+48,LINE2(4));
    clcd_putch(':',LINE2(5));
    temp=read_external_eeprom(add1++);
    clcd_putch((temp/10)+48,LINE2(6));
    clcd_putch((temp%10)+48,LINE2(7));
    clcd_putch(' ',LINE2(8));
    temp=read_external_eeprom(add1++);
    clcd_print(event[temp],LINE2(9));
    clcd_print("  ",LINE2(11));
    temp=read_external_eeprom(add1++);
    clcd_putch(((temp/10)+'0'),LINE2(13));
    clcd_putch(((temp%10)+'0'),LINE2(14));
    
    for(unsigned long int wait=50000;wait--;);
    
    
}

//clear log
void clear_log(unsigned char sw)
{
    //clearing log,updating log_count as 0 
     write_external_eeprom(0x32, 0);
     clcd_print("    Clear Log    ",LINE1(0));
     clcd_print("   Successfull  ",LINE2(0));
     flag_5sec=1;
     min2_count=0;
     TMR0=6;
     TMR0IF=0;
     TMR0IE=1;
     while(flag_5sec);
     TMR0IE=0;
     TMR0=6;
     flag_5sec=1;
     min2_count=0;
     log_flag=0;
}

//download log
void download_log(unsigned char sw)
{
    unsigned char add1=0;
    //if log empty
    if(log_count<=0)
    {
        puts("log empty \n\r");
        clcd_print("    LOG EMPTY   ",LINE1(0));
        clcd_print("                ",LINE2(0));
        flag_5sec=1;
        min2_count=0;
        TMR0=6;
        TMR0IF=0;
        TMR0IE=1;
        while(flag_5sec);
        TMR0IE=0;
        TMR0=6;
        flag_5sec=1;
        min2_count=0;
    }
    else
    {
        //if there is log 
    puts("  TIME   EV  SP \n\r");
    for(char i=0;i<log_count;i++)
    {
    unsigned char temp=read_external_eeprom(add1++);
    log[0]=(temp/10)+48;
    log[1]=(temp%10)+48;
    log[2]=':';
    temp=read_external_eeprom(add1++);
    log[3]=(temp/10)+48;
    log[4]=(temp%10)+48;
    log[5]=':';
    temp=read_external_eeprom(add1++);
    log[6]=(temp/10)+48;
    log[7]=(temp%10)+48;
    log[8]=' ';
    temp=read_external_eeprom(add1++);
    log[9]=event[temp][0];
    log[10]=event[temp][1];
    log[11]=' ';
    log[12]=' ';
    temp=read_external_eeprom(add1++);
    log[13]=((temp/10)+'0');
    log[14]=(temp%10)+'0';
    log[15]='\0';
    puts(log);
    puts("\n\r");
    
    }
    puts("DOWNLOAD COMPLETED\n\r");
    clcd_print("    DOWNLOAD    ",LINE1(0));
    clcd_print("   SUCCESSFULL  ",LINE2(0));
    flag_5sec=1;
    min2_count=0;
    TMR0=6;
    TMR0IF=0;
    TMR0IE=1;
    while(flag_5sec);
    TMR0IE=0;
    TMR0=6;
    flag_5sec=1;
    min2_count=0;
    }
    
   
}
void change_password(unsigned char sw)
{
    
    static unsigned char pass_arr[5]="",star_arr[6]="    ",index=0,confirm_arr[5]="    ",re_star_arr[6]="    ";
    //enter new password
    if(index<4)
    {
    clcd_print("  new password  ",LINE1(0));
    clcd_print("            ", LINE2(4));
    
        if(sw==MK_SW5)
        {
            pass_arr[index]='0';
            star_arr[index]='*';
            pass_arr[index+1]='\0';
            star_arr[index+1]='\0';
            index++;
        }
        if(sw==MK_SW6)
        {
            pass_arr[index]='1';
            star_arr[index]='*';
            pass_arr[index+1]='\0';
            star_arr[index+1]='\0';
            index++;
        }
    clcd_print(star_arr,LINE2(0));
    }
     
    //confirm password
    else if(index>=4&&index<8)
    {
       
    clcd_print("confirm password",LINE1(0));
    clcd_print("           ", LINE2(4));
     //index=0;
   
        if(sw==MK_SW5)
        {
            confirm_arr[index-4]='0';
            re_star_arr[index-4]='*';
            confirm_arr[(index+1)-4]='\0';
            re_star_arr[(index+1)-4]='\0';
            index++;
        }
        if(sw==MK_SW6)
        {
            confirm_arr[index-4]='1';
            re_star_arr[index-4]='*';
            confirm_arr[(index+1)-4]='\0';
            re_star_arr[(index+1)-4]='\0';
            index++;
        }
    
    clcd_print(re_star_arr,LINE2(0));
    }
    else if(index==8)
    {
        //if password matches
    if(strcmp(pass_arr,confirm_arr)==0)
    {
        lng_sw=read_switches(LEVEL_CHANGE);
        clcd_print("password entered",LINE1(0));
        clcd_print("    correctly   ",LINE2(0));
        //longpress switch 5 to save and go to dashboard
        if((lng_sw==MK_SW5 && delay_1++>2000 ))
        {
            unsigned char add=0x33;
           for(unsigned char i=0;i<4;i++)
           {
              write_external_eeprom(add++, pass_arr[i]);  
           }
            clcd_print(" password saved ",LINE1(0));
            clcd_print("  successfully  ",LINE2(0));
            flag_5sec=1;
            min2_count=0;
            TMR0=6;
            TMR0IF=0;
            TMR0IE=1;
            while(flag_5sec);
            TMR0IE=0;
            TMR0=6;
            flag_5sec=1;
            min2_count=0;
            delay_1=0;
            index=0;
            pass_index=0;
            strcpy(star_arr1,"    ");
             strcpy(re_star_arr,"    ");
             strcpy(star_arr,"    ");
             pass_flag=0;
            log_flag=0;
            login_flag=0;
            
            
            CLEAR_DISP_SCREEN;
            
        }
        //long press sw6 to discard and go back to menu
        else if(lng_sw==MK_SW6 && delay2++>2000)
        {
            log_flag=0;
            delay2=0;
            index=0;
             strcpy(star_arr1,"    ");
             strcpy(re_star_arr,"    ");
             strcpy(star_arr,"    ");
        }
    }
    //doesnt match return to menu
    else
    {
        clcd_print(" doesn't match  ",LINE1(0));
        clcd_print("                ",LINE2(0));
        flag_5sec=1;
        min2_count=0;
        TMR0=6;
        TMR0IF=0;
        TMR0IE=1;
        while(flag_5sec);
        TMR0IE=0;
        TMR0=6;
        flag_5sec=1;
        min2_count=0;
        index=0;
        log_flag=0;
        strcpy(star_arr1,"    ");
             strcpy(re_star_arr,"    ");
             strcpy(star_arr,"    ");
    }
    }
}
void set_time(unsigned char sw)
{
    static unsigned char mode=0;
    //sec_mode=0,min_mode=1,hr_mode=2
    unsigned int sec_time,min_time,hr;
    unsigned static int delay=0,delay1=0;
    unsigned char dummy,once=1;
    //turning off rtc
	write_ds1307(SEC_ADDR,0x80); 
    if(once)
    {
        TMR0=6;
        TMR0IF=0;
        min2_count=0;
        
        TMR0IE=1;
        
    }
    
    
    
    clcd_print("    SET_TIME    ",LINE1(0));
 
    clcd_print("        ",LINE2(8));
    
     lng_sw=read_switches(LEVEL_CHANGE);
        //TMR0IE=0;
     //sec mode
        if(mode==0)
        {
            //sw 5 to increment time
           if(lng_sw==ALL_RELEASED && delay>0)
           { 
               delay=0;
            sec_time=(time[6]-48)*10+(time[7]-48);
            if(sec_time<59)
            sec_time++;
            else if(sec_time==59)
            sec_time=0;
            time[6]=(sec_time/10)+48;
            time[7]=(sec_time%10)+48;
           }
           clcd_putch(time[0],LINE2(0));
       clcd_putch(time[1],LINE2(1));
       clcd_putch(time[2],LINE2(2)); 
       clcd_putch(time[3],LINE2(3));
       clcd_putch(time[4],LINE2(4));
       clcd_putch(time[5],LINE2(5));
       //cursor blinking for selected field
       if(flag)
       {
        
       clcd_putch('_',LINE2(6));
       clcd_putch('_',LINE2(7));
        }
        else
        {
       clcd_putch(time[6],LINE2(6));
       clcd_putch(time[7],LINE2(7));
        }
           
        }
     //min mode
        else if(mode==1)
        {
            //increment time
            if(lng_sw==ALL_RELEASED && delay>0)
           {
                delay=0;
            min_time=(time[3]-48)*10+(time[4]-48);
            if(min_time<59)
            min_time++;
            else if(min_time==59)
            sec_time=0;
            
            time[3]=(min_time/10)+48;
            time[4]=(min_time%10)+48;
            }
            clcd_putch(time[0],LINE2(0));
       clcd_putch(time[1],LINE2(1));
       clcd_putch(time[2],LINE2(2)); 
       clcd_putch(time[7],LINE2(7));
       clcd_putch(time[6],LINE2(6));
       clcd_putch(time[5],LINE2(5));
       if(flag)
       {
       clcd_putch('_',LINE2(3));
       clcd_putch('_',LINE2(4));
        }
        else
        {
       clcd_putch(time[3],LINE2(3));
       clcd_putch(time[4],LINE2(4));
        }
        }
     //hr field
        else if(mode ==2)
        {
            if(lng_sw==ALL_RELEASED && delay>0)
           {
                delay=0;
            hr=(time[0]-48)*10+(time[1]-48);
            if(hr<23)
            hr++;
            else if(hr==23)
             hr=0;
            time[0]=(hr/10)+48;
            time[1]=(hr%10)+48;
            }
            clcd_putch(time[6],LINE2(6));
            clcd_putch(time[7],LINE2(7));
            clcd_putch(time[2],LINE2(2)); 
            clcd_putch(time[3],LINE2(3));
            clcd_putch(time[4],LINE2(4));
            clcd_putch(time[5],LINE2(5));
       if(flag)
       {
       clcd_putch('_',LINE2(0));
       clcd_putch('_',LINE2(1));
        }
        else
        {
       clcd_putch(time[0],LINE2(0));
       clcd_putch(time[1],LINE2(1));
        }
        }
        
    //to select field
     if(lng_sw==ALL_RELEASED && delay1>0)
        {
         delay1=0;
            //TMR0IE=0;
          if(mode<2)
          {
              mode++;
          }
          else if(mode==2)
          {
              mode=0;
          }
          
        }
   // count=0;
    
    
   
    
    //count=0;
   //saving time
    if(lng_sw==MK_SW5 && delay++>200)
    {
        TMR0IE=0;
        TMR0=6;
        min2_count=0;
        
        dummy = (time[3]-48)<<4 | (time[4]-48);
	write_ds1307(MIN_ADDR, dummy ); 
	/* Seting 24 Hr Format */
	dummy = (time[0]-48)<<4 | (time[1]-48);
    write_ds1307(HOUR_ADDR, dummy ); 
    dummy = (time[6]-48)<<4 | (time[7]-48);
        write_ds1307(SEC_ADDR,dummy & 0x7F); 
        delay=0;
        login_flag=0;
        log_flag=0;
    }
     //discard and exit
    else if(lng_sw==MK_SW6 && delay1++>200)
    {
      TMR0IE=0;
      //login_flag=0;
      log_flag=0;
    }
    
}

