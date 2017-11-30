/*
 * uart.c
 *
 * Created: 2017/11/26 15:25:39
 *  Author: root
 */ 
#include "uart.h"
#include "gpio.h"
#include "usart.h"
#include "conf_usart_serial.h"
#include "conf_board.h"

#define EXAMPLE_TARGET_PBACLK_FREQ_HZ BOARD_OSC0_HZ

__attribute__((__interrupt__)) static void usart_int_handler(void)
{
	int receive_int;
	char c;
	usart_read_char(EXAMPLE_USART, &receive_int);
	c = (char)receive_int;	
}

void uart_init(void)
{
	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION},			
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = EXAMPLE_USART_BAUDRATE,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};
	
	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));
	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS,EXAMPLE_TARGET_PBACLK_FREQ_HZ);
	// Disable all interrupts.
	Disable_global_interrupt();
	// Initialize interrupt vectors.
	INTC_init_interrupts();

	/*
	 * Register the USART interrupt handler to the interrupt controller.
	 * usart_int_handler is the interrupt handler to register.
	 * EXAMPLE_USART_IRQ is the IRQ of the interrupt handler to register.
	 * AVR32_INTC_INT0 is the interrupt priority level to assign to the
	 * group of this IRQ.
	 */
	INTC_register_interrupt(&usart_int_handler, EXAMPLE_USART_IRQ,AVR32_INTC_INT0);
	// Enable USART Rx interrupt.
	EXAMPLE_USART->ier = AVR32_USART_IER_RXRDY_MASK;
	// Enable all interrupts.
	Enable_global_interrupt();
}
