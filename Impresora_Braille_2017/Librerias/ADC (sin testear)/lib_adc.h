#ifndef LIB_ADC_HEADER
#define	LIB_ADC_HEADER

/*
 *	Archivo HEADER de la libreria para manejo del conversor analogico-digital (ADC)
 */

#include	<xc.h>


#define VCFGREG ADCON1bits      // Configurar segun datasheet
#define ADFMREG ADCON1bits      //
#define ADCSREG ADCON0bits      //
#define ADONREG ADCON0bits      //
#define CHSREG  ADCON0bits      //
#define GOREG   ADCON0bits      //


void analogInit(unsigned int ans);      /* Inicializa la configuracion del ADC */

int analogRead(unsigned int pin);       /* Retorna un valor entre 0 a 1023 con respecto al voltaje en la entrada analogica "pin" */


#endif	/* LIB_ADC_HEADER */