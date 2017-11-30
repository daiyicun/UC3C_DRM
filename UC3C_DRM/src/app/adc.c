/*
 * adc.c
 *
 * Created: 2017/11/26 15:10:41
 *  Author: root
 */ 
//#include "compiler.h"
//#include "preprocessor.h"
#include "gpio.h"
#include "adcifa.h"
#include "sysclk.h"
#include "adc.h"
#include "delay.h"

/* GPIO pin/adc-function map. */
const gpio_map_t ADCIFA_GPIO_MAP = {
{AVR32_ADCREF0_PIN, AVR32_ADCREF0_FUNCTION},
{AVR32_ADCREFP_PIN, AVR32_ADCREFP_FUNCTION},
{AVR32_ADCREFN_PIN, AVR32_ADCREFN_FUNCTION},
{EXAMPLE_ADC_AnalogIn_PIN, EXAMPLE_ADC_AnalogIn_FUNCTION},
{EXAMPLE_ADC_THERM_PIN, EXAMPLE_ADC_THERM_FUNCTION}
};

/* ADC Configuration */
adcifa_opt_t adc_config_t = {
	.frequency                = 1000000,
	//.reference_source         = ADCIFA_ADCREF0,
	.reference_source         = ADCIFA_REF06VDD,	
	.sample_and_hold_disable  = false,
	.single_sequencer_mode    = false,
	.free_running_mode_enable = false,
	.sleep_mode_enable        = false,
	.mux_settle_more_time	  = false
};

adcifa_sequencer_opt_t adcifa_sequence_opt = {
	.convnb               = EXAMPLE_ADCIFA_NUMBER_OF_SEQUENCE,
	.resolution           = ADCIFA_SRES_12B,
	.trigger_selection    = ADCIFA_TRGSEL_SOFT,
	.start_of_conversion  = ADCIFA_SOCB_ALLSEQ,
	.sh_mode              = ADCIFA_SH_MODE_OVERSAMP,
	.half_word_adjustment = ADCIFA_HWLA_NOADJ,
	.software_acknowledge = ADCIFA_SA_NO_EOS_SOFTACK
};

/** \ADC initial */
void adc_init( void )
{
	/* Conversions in the Sequencer Configuration */
	adcifa_sequencer_conversion_opt_t adcifa_sequence_conversion_opt[2];
	adcifa_sequence_conversion_opt[0].channel_p = EXAMPLE_ADC_AnalogIn_INP;
	adcifa_sequence_conversion_opt[0].channel_n = EXAMPLE_ADC_AnalogIn_INN;
	adcifa_sequence_conversion_opt[0].gain      = ADCIFA_SHG_1;
	adcifa_sequence_conversion_opt[1].channel_p = EXAMPLE_ADC_THERM_INP;
	adcifa_sequence_conversion_opt[1].channel_n = EXAMPLE_ADC_THERM_INN;
	adcifa_sequence_conversion_opt[1].gain      = ADCIFA_SHG_1;
	
	/* Assign and enable GPIO pins to the ADC function. */
	gpio_enable_module(ADCIFA_GPIO_MAP, sizeof(ADCIFA_GPIO_MAP)/sizeof(ADCIFA_GPIO_MAP[0]));
	/* Get ADCIFA Factory Configuration */
	adcifa_get_calibration_data(&AVR32_ADCIFA, &adc_config_t);
	/* Calibrate offset first*/
	adcifa_calibrate_offset(&AVR32_ADCIFA, &adc_config_t, sysclk_get_cpu_hz());
	/* Configure ADCIFA core */
	adcifa_configure(&AVR32_ADCIFA, &adc_config_t, sysclk_get_cpu_hz());
	/* Configure ADCIFA sequencer 0 */
	adcifa_configure_sequencer(&AVR32_ADCIFA, 0, &adcifa_sequence_opt,adcifa_sequence_conversion_opt);
}

int16_t adc_get_one_data(void)
{
	int16_t adc_values[2];
	bool adc_valid[2];
	const int16_t low=-30,high=2060;
	//Get Values from sequencer 0
	adcifa_start_sequencer(&AVR32_ADCIFA, 0);
	while (true) {
		if (adcifa_get_values_from_sequencer(&AVR32_ADCIFA, 0, &adcifa_sequence_opt, adc_values) == ADCIFA_STATUS_COMPLETED)
		{
			adc_values[0] = 0.0f-(adc_values[0]);
			adc_valid[0] = (adc_values[0] > low && adc_values[0] < high);
			adc_values[1] = 0.0f-(adc_values[1]);
			adc_valid[1] = (adc_values[1] > low && adc_values[1] < high);
			
			if(adc_valid[0] && adc_valid[1]) break;
		}
	}
	//Clear end-of-sequence for sequencer 0, ready for next conversion
	ADCIFA_clear_eos_sequencer_0();
	return adc_values[0];
}

#define SAMPLE_NUM 4
int16_t adc_get_value()
{
	int16_t adc_sample[SAMPLE_NUM],i,sum=0;
	float average=0,varation=0,tt;
	
	do
	{
		sum=0;varation=0;
		for(i=0;i<SAMPLE_NUM;i++)
		{
			adc_sample[i] = adc_get_one_data();
			sum += adc_sample[i];
			delay_ms(5);
		}
		average = (float)sum/SAMPLE_NUM;
		for(i=0;i<SAMPLE_NUM;i++)
		{
			tt = (average-adc_sample[i])/(average);
			tt = Abs(tt);
			if(tt>varation) varation = tt;
		}
	} while (varation > 0.05);
	return sum/SAMPLE_NUM;
}