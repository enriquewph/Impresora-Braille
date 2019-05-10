/*
 *  Autor: Enrique Walter Philippeaux
 *  Fecha: 07/07/2017
 *	Editado de la version del autor: Juan Cruz Becerra, del:	 20/05/2003
 */

/*
 *	Libreria para el manejo de un display LCD
 *	Utiliza las rutinas de delay.c y funciona con XC8
 *	Este codigo conecta al PIC con el controlador estandar 
 *	HD44780 de Hitachi. Usa el modo de 4 bits, con el hardware 
 *	conectado segun como defina las salidas LCD_X.
 *	
 *	Para utilizar la libreria, realize la configuracion correspondiente en el archivo
 *  "drv_lcd.h" ( al final de este ) y llame luego de todas las rutinas de configuración (TRISA, TRISB, ...)
 *  a la funcion lcd_init();
 */

#include	<stdio.h>
#include	"drv_lcd.h"

#ifndef LCD_RS 
	#error "No define pin de R/S del LCD"
#endif
#ifndef RW_A_GND 
    #ifndef LCD_RW
        #error "No define pin de RW del LCD, o no declaro su conexion a masa."
    #endif
#endif
#ifndef LCD_EN
	#error "No define pin de E del LCD"
#endif

#ifndef	LCD_D4
#error	"No se define el LCD_D4 de datos del LCD"
#endif

#ifndef	LCD_D5
#error	"No se define el LCD_D5 de datos del LCD"
#endif

#ifndef	LCD_D6
#error	"No se define el LCD_D6 de datos del LCD"
#endif

#ifndef	LCD_D7
#error	"No se define el LCD_D7 de datos del LCD"
#endif

/* Organiza el byte a escribir segun como definimos las conexiones */
void Lcd_Port(char a)
{
	if(a & 1)
    {LCD_D4 = 1;}
	else
    {LCD_D4 = 0;}

	if(a & 2)
    {LCD_D5 = 1;}
	else
    {LCD_D5 = 0;}

	if(a & 4)
    {LCD_D6 = 1;}
	else
    {LCD_D6 = 0;}

	if(a & 8)
    {LCD_D7 = 1;}
	else
    {LCD_D7 = 0;}
}

/* Escribe un byte en el LCD en modo de 4-bits */
void lcd_write(unsigned char c)
{
	Lcd_Port(c >> 4);
	
	asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

	LCD_EN = 1;

	asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

	LCD_EN = 0;

	Lcd_Port(c & 0x0F);

	asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

	LCD_EN = 1;

	asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

	LCD_EN = 0;
	__delay_us( 200 );
}

/* Borra el LCD y setea el cursor a la dir. 0 */
void lcd_clear(void)
{
	LCD_RS = 0;
	lcd_write( 0x1 );
	__delay_ms( 5 );
}

/* Borra la linea que se desea */
/**
 * @Summary
 * Borrado de linea.
 * 
 * @Description
 * Borra la linea seleccionada del LCD. 
 * 
 * @param line
 * Numero de linea que se desea borrar.\n
 * 1 para la primer linea.\n
 * 2 para la segunda linea.
 * 
 * @example \n
 * lcd_puts("Hola mundo !!!");\n\n
 * o bien ... \n
 * char text[] = "Hola mundo !!!";\n
 * lcd_puts(texto);
 * 
 * @author
 * jcbecerra
 * 
 * @File
 * drv_lcd.c
 * 
 */
void lcd_clear_line( const char line )
{
	lcd_goto( (line-1) * 64 );
	for( unsigned char l = 0; l < 32; l++ )
    {
        lcd_putch( ' ' );
    }
}

/**********************************************************************************************/
/**
 * @Summary
 * Cadena de caracteres.
 * 
 * @Description
 * Esta funcion escribe una cadena de caracteres o "STRING" en el LCD. 
 * 
 * @param s
 * Puntero al texto a escribir. Puede enviarse un texto directamente o el puntero al texto.
 * 
 * @example \n
 * lcd_print("Hola mundo !!!");\n\n
 * o bien ... \n
 * char text[] = "Hola mundo !!!";\n
 * lcd_print(texto);
 * 
 * @author
 * jcbecerra
 * 
 * @File
 * drv_lcd.c
 * 
 */
char lcd_print( const char * s )
{
	unsigned char cs = 0;
	while( *s )
	{
		lcd_putch( *s++ );
		cs++;
	}
	return cs;
}

/**********************************************************************************************/
/**
 * @Summary
 * Escribir caracter.
 * 
 * @Description
 * Esta funcion escribe un caracter en el LCD. 
 * 
 * @param c
 * Caracter a escribir en el LCD.
 * 
 * @example
 * lcd_putch('c');\n
 * 
 * @author
 * jcbecerra, ewphilippeaux
 * 
 * @File
 * drv_lcd.c
 * 
 */
void lcd_putch( char c )
{
	LCD_RS = 1;
	lcd_write( c );
}

/**********************************************************************************************/
/**
 * @Summary
 * Posiciona el cursor.
 * 
 * @Description
 * Esta funcion permite posicionar el cursor ya sea en un display 16x2 o 20x4.\n
 * 
 * @param columna
 * Columna indicada.
 * 
 * @param fila
 * Fila indicada.
 * 
 * @author
 * ewphilippeaux
 * 
 * @File
 * drv_lcd.c
 * 
 */
void lcd_setCursor(unsigned int columna, unsigned int fila)
{
    LCD_RS = 0;
    if (fila == 0)
    {
        lcd_write(0x80 + columna);
    }
    else if (fila == 1)
    {
        lcd_write(0xC0 + columna);
    }
    else if (fila == 2)
    {
        lcd_write(0x94 + columna);
    }
    else if (fila == 3)
    {
        lcd_write(0xD4 + columna);
    }
}

/**********************************************************************************************/
/**
 * @Summary
 * Inicializa el Display.
 * 
 * @Description
 * Inicializa el modulo DISPLAY LCD con el controlador HD44780 o similar para ser usado con un 
 * bus de 4 bits.\n
 * Se debe definir en el archivo drv_lcd.h previamente lo siguiente: \n
 * #define LCD_EN		(PORTEbits.RE2)		--->	Pin Enable del LCD				\n
 * #define LCD_EN_TRIS	(TRISEbits.TRISE2)	--->									\n
 * #define LCD_RS		(PORTEbits.RE0)		--->	Pin Register Select del LCD		\n
 * #define LCD_RS_TRIS	(TRISEbits.TRISE0)	--->									\n
 * #define LCD_RW		(PORTEbits.RE1)		--->	Pin Read/Write del LCD			\n
 * #define LCD_RW_TRIS	(TRISEbits.TRISE1)	--->									\n
 * #define LCD_D4      (PORTDbits.RD0)		--->	Pin D4 del LCD          		\n
 * #define LCD_D4_TRIS (TRISDbits.TRISD0)	--->									\n
 * #define LCD_D5      (PORTDbits.RD1)		--->	Pin D5 del LCD          		\n
 * #define LCD_D5_TRIS (TRISDbits.TRISD1)	--->									\n
 * #define LCD_D6      (PORTDbits.RD2)		--->	Pin D6 del LCD          		\n
 * #define LCD_D6_TRIS (TRISDbits.TRISD2)	--->									\n
 * #define LCD_D7      (PORTDbits.RD3)		--->	Pin D7 del LCD          		\n
 * #define LCD_D7_TRIS (TRISDbits.TRISD3)	--->									\n
 * 
 * @example
 * lcd_init();\n
 * 
 * @author
 * jcbecerra, ewphilippeaux
 * 
 * @File
 * drv_lcd.c
 * 
 */
void lcd_init(void)
{	
	LCD_EN_TRIS = 0;
	LCD_RS_TRIS = 0;
#ifndef RW_A_GND
	LCD_RW_TRIS = 0;
#endif
    LCD_D4_TRIS = 0;
    LCD_D5_TRIS = 0;
    LCD_D6_TRIS = 0;
    LCD_D7_TRIS = 0;
	LCD_RS = 0;
#ifndef RW_A_GND
	LCD_RW = 0;
#endif
	LCD_EN = 0;
    LCD_D4 = 0;
    LCD_D5 = 0;
    LCD_D6 = 0;
    LCD_D7 = 0;

	__delay_ms( 100 );

	Lcd_Port(0b00000010);
	
	LCD_EN = 1;
	__delay_us( 2 );

	LCD_EN = 0;
	__delay_us( 2 );

	Lcd_Port(0b00001100);
	
	LCD_EN = 1;
	__delay_us( 2 );
	LCD_EN = 0;

	__delay_us( 50 );
	lcd_write( 0xC );		// Display ON, cursor off, blink off

	__delay_us( 50 );
	lcd_write( 0x1 );		// Display clear

	__delay_ms( 2 );
	lcd_write( 0x6 );		// entry mode
}
