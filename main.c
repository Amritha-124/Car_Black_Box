/*
 * File:   main.c
 * Author: Amritha G
 *
 * Created on February 5, 2024, 11:32 AM
 */


#include <xc.h>
#include"main.h"
#include "car_black_box.h"
#include "keypad.h"
#include "adc.h"
#include "clcd.h"
#include "timer.h"
#include "ds1307.h"
#include "i2c.h"
#include "uart.h"
#include "ext_eeprom.h"
void init_config(void)
{
    init_clcd();
    
    init_matrix_keypad();
    init_adc();
    init_timer0();
    init_i2c();
    init_ds1307();
    init_uart();
    PEIE=1;
    GIE=1;
   
}
extern char time[9];
void main(void) {
    init_config();
    //run only one time to intialize log count and password to avoid reading gv,after that comment it
   /* write_external_eeprom(0x32,0);
    write_external_eeprom(0x33,'0');
    write_external_eeprom(0x34,'0');
    write_external_eeprom(0x35,'0');
    write_external_eeprom(0x36,'0');*/
    unsigned char key;
    while(1)
    {
        //reading switch
        key=read_switches(STATE_CHANGE);
        //calling display dashboard
        display_dashboard(key);
        
        
        
    }
    return;
}
