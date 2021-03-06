#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"


void user_init_normal(void)//this will get executed one time after power up
{
	random_generator_init();  //must
	init_ble();	
	init_sensor();
	init_lcd();	
	show_atc_mac();
}

_attribute_ram_code_ void user_init_deepRetn(void)//after sleep this will get executed
{
	blc_ll_initBasicMCU();
	rf_set_power_level_index (RF_POWER_P3p01dBm);
	blc_ll_recoverDeepRetention();
}

RAM u32 last_delay = 0xFFFF0000, last_delay_min = 0xFFFF0000;
RAM u8 last_smiley;
u16 temp = 0;
u16 humi = 0;
u8 battery_level;

void main_loop (void)
{	
	if((clock_time()-last_delay) > 5000*CLOCK_SYS_CLOCK_1MS){		
		battery_level = get_battery_level();
		read_sensor(&temp,&humi);		
		
		show_big_number(temp,1);
		show_temp_symbol(1);
		show_small_number(humi,1);
		update_lcd();	
		if(ble_get_connected()){
			ble_send_temp(temp);
			ble_send_humi(humi);
			ble_send_battery(battery_level);
		}
		
		if((clock_time()-last_delay_min) > 60000*CLOCK_SYS_CLOCK_1MS){
			set_adv_data(temp, humi, battery_level, get_battery_mv());
			last_delay_min = clock_time();
		}
		show_battery_symbol((battery_level<35)?1:0);
		
		if(last_smiley)
			last_smiley=0;
		else
			last_smiley=1;
		show_smiley(last_smiley);
		
		update_lcd();
		last_delay = clock_time();
	}
	blt_sdk_main_loop();
	blt_pm_proc();	
}
