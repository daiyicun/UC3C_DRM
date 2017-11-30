/*
 * adc.h
 *
 * Created: 2017/11/26 15:12:58
 *  Author: root
 */ 
#ifndef ADC_H_
#define ADC_H_

#define EXAMPLE_ADCIFA_NUMBER_OF_SEQUENCE     2
#define EXAMPLE_ADC_AnalogIn_INP			  AVR32_ADCIFA_INP_GNDANA//for IN5
#define EXAMPLE_ADC_AnalogIn_INN			  AVR32_ADCIFA_INN_ADCIN9
#define EXAMPLE_ADC_AnalogIn_PIN			  AVR32_ADCIN9_PIN
#define EXAMPLE_ADC_AnalogIn_FUNCTION		  AVR32_ADCIN9_FUNCTION

#define EXAMPLE_ADC_THERM_INP                   AVR32_ADCIFA_INP_GNDANA//for IN6
#define EXAMPLE_ADC_THERM_INN                   AVR32_ADCIFA_INN_ADCIN8
#define EXAMPLE_ADC_THERM_PIN                   AVR32_ADCIN8_PIN
#define EXAMPLE_ADC_THERM_FUNCTION              AVR32_ADCIN8_FUNCTION

void adc_init(void);
int16_t adc_get_value(void);
int16_t adc_get_one_data(void);
extern adcifa_sequencer_opt_t adcifa_sequence_opt;
#endif /* ADC_H_ */