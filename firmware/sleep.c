/* 
 * Copyright 2021 Gregory Davill <greg.davill@gmail.com> 
 */

#include <generated/csr.h>
#include "sleep.h"

void msleep(int ms)
{
    timer0_en_write(0);
    timer0_reload_write(0);
    timer0_load_write(CONFIG_CLOCK_FREQUENCY/1000*ms);
    timer0_en_write(1);
    timer0_update_value_write(1);
    while(timer0_value_read()) timer0_update_value_write(1);
}