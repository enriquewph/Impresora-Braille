#ifndef _LCD_H
#define _LCD_H

/*
 *	Archivo HEADER de la libreria para manejo de LCD
 */

#include	<xc.h>

void lcd_write( unsigned char );		/* Escribe 1 byte en el LCD en modo de 4 bits */

void lcd_putch( char c );

char lcd_print( const char * s);         /* Escribe una cadena de texto en el LCD */

void lcd_clear(void);					/* Borra y retorna el cursor del LCD */

void lcd_clear_line(const char line);	/* Borra la linea indicada */

void lcd_goto(unsigned char pos);		/* Va a la posicion especificada */
	
void lcd_init(void);					/* Inicia el LCD. Llamar antes de cualquier otra funcion */

void lcd_setCursor(unsigned int columna, unsigned int fila); /* Va a la posicion especificada */

//#define RW_A_GND                      /* Descomentar si el pin RW del display esta conectado a masa */

#define LCD_EN		(PORTAbits.RA5)
#define LCD_EN_TRIS	(TRISAbits.TRISA5)
#define LCD_RS		(PORTEbits.RE1)
#define LCD_RS_TRIS	(TRISEbits.TRISE1)

#ifndef RW_A_GND
#define	LCD_RW		(PORTEbits.RE0)
#define	LCD_RW_TRIS	(TRISEbits.TRISE0)
#endif

#define LCD_D4      (PORTAbits.RA4)
#define LCD_D4_TRIS (TRISAbits.TRISA4)
#define LCD_D5      (PORTAbits.RA3)
#define LCD_D5_TRIS (TRISAbits.TRISA3)
#define LCD_D6      (PORTAbits.RA2)
#define LCD_D6_TRIS (TRISAbits.TRISA2)
#define LCD_D7      (PORTAbits.RA1)
#define LCD_D7_TRIS (TRISAbits.TRISA1)

#endif
