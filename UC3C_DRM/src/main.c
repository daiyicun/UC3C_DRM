/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include ".\app\adc.h"
#include ".\app\uart.h"
#include ".\app\crc16.h"

#define CMD_LEN 11
#define ADC_SHORT_H 200
#define ADC_SHORT_L 100
#define ADC_WORK_H 1200
#define ADC_WORK_L 900
#define ADC_OPEN 1300
#define STATE_SHORT 3
#define STATE_WORK 1
#define STATE_OPEN 2

#define write_packet(a) usart_serial_write_packet(EXAMPLE_USART,(a),CMD_LEN)

int main (void)
{
	int16_t current_state=0,new_state=0;
	int16_t adc_values;
	uint8_t command_on[CMD_LEN] = {0x01,0x10,0x13,0x8D,0x00,0x01,0x02,0x00,0xCF,0xDA,0x18}; 
	uint8_t command_off[CMD_LEN]= {0x01,0x10,0x13,0x8D,0x00,0x01,0x02,0x00,0xCE,0xDA,0x18};
	/* Insert system clock initialization code here (sysclk_init()). */
	board_init();
	sysclk_init();

	get_crc16(command_on,CMD_LEN-2, &command_on[CMD_LEN-1], &command_on[CMD_LEN-2]);
	get_crc16(command_off,CMD_LEN-2, &command_off[CMD_LEN-1], &command_off[CMD_LEN-2]);

	adc_init();
	uart_init();
	/* Insert application code here, after the board has been initialized. */
	write_packet(command_on);
	current_state = STATE_WORK;//1:work, 2:open, 3:short
	while (true)
	{
		adc_values=adc_get_value();
		if (adc_values  >  ADC_OPEN) {
			new_state = STATE_OPEN;
		}
		else if (adc_values < ADC_SHORT_H && adc_values > ADC_SHORT_L) {
			new_state = STATE_SHORT;
		}
		else if (adc_values < ADC_WORK_H && adc_values > ADC_WORK_L){
			new_state = STATE_WORK;
		}
		else {
			new_state = current_state;
		}
		if(new_state != current_state){
			if(new_state == STATE_WORK){
				write_packet(command_on);
			}
			else {
				write_packet(command_off);
			}
			current_state = new_state;
		}
	}
}
