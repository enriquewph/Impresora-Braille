/*
 *  Autor: Enrique Walter Philippeaux
 *  
 *	Fecha: 11/07/2017
 */

/*
 *	Libreria para el manejo del conversor Analogico Digital (ADC)
 *  Para utilizar la libreria, incluya el archivo "lib_adc.h" en su programa y luego
 *  realize la configuracion correspondiente en el archivo "lib_adc.h" y llame luego 
 *  de todas las rutinas de configuración (TRISA, TRISB, ...) a la funcion 
 *  analogInit(numero que representa cuales entradas seran configuradas como analogicas.);
 */


#include <xc.h>
#include "lib_adc.h"
#ifndef	VCFGREG
#error	"No se definen los registros del ADC."
#endif
#ifndef	ADFMREG
#error	"No se definen los registros del ADC."
#endif
#ifndef	ADCSREG
#error	"No se definen los registros del ADC."
#endif
#ifndef	ADONREG
#error	"No se definen los registros del ADC."
#endif
#ifndef	CHSREG
#error	"No se definen los registros del ADC."
#endif
#ifndef	GOREG
#error	"No se definen los registros del ADC."
#endif

/**
 * @Summary
 * Inicia el ADC
 * 
 * @Description
 * Inicia el conversor Analogico Digital y cuales pines se utilizaran como entradas analogicas 
 * 
 * @param ans
 * Numero que representa cuales pines se utilizaran como entradas analogicas.
 * 
 * @example \n
 * analogInit( 0b0000000001 );\n
 * 
 * @author
 * ewphilippeaux
 * 
 * @File
 * lib_adc.c
 * 
 */

void analogInit(unsigned int ans)   //Ej: analogInit( 0b0000000001 );  : Se habilita la entrada analogica del pin "AN 0"
{
    ANSEL = ans & 0xFF;
    ANSELH = ans >> 8;
    VCFGREG.VCFG1 = 0;              // Selecciona la referencia negativa del ADC: 0 para Masa | 1 para Vref- (segun datasheet, AN 2 en PIC16F887)
    VCFGREG.VCFG0 = 0;              // Selecciona la referencia postivia del ADC: 0 para Vcc  | 1 para Vref+ (segun datasheet, AN 3 en PIC16F887)
    ADFMREG.ADFM = 1;
    ADCSREG.ADCS = 0;               // Setea el prescaler de Fosc para el conversor ADC
    ADONREG.ADON = 1;               // Se enciende el ADC
}

/**
 * @Summary
 * Retorna la lectura analogica
 * 
 * @Description
 * Inicia el ADC, y retorna un valor entre 0 a 1023 con respecto al voltaje en la entrada analogica "pin"
 * 
 * @param pin
 * Entrada analogica a leer
 * 
 * @example \n
 * analogRead(0);\n
 * 
 * @author
 * ewphilippeaux
 * 
 * @File
 * lib_adc.c
 * 
 */
int analogRead(unsigned int pin)
{
    CHSREG.CHS = pin;
    GOREG.GO = 1;                   // Inicia la conversión
    while(GOREG.GO){};              // Aguarda hasta que la conversión termine
    return (ADRESH<<8)+ADRESL;      // Retorna un valor entre 0 a 1023 con respecto al voltaje en la entrada analogica "pin"
}