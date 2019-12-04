#ifndef HEADER_H
#define HEADER_H

#include "comandos_uart.h"
#include <stdint.h>

#define DEBUG //Definir si se debugea el programa.

//Configuracion:
#define BCL_RECIBIR_HOJA_TIMEOUT 2000 //[mS] transcurridos desde que no se recibio ningun dato. \
                                      //al recibir una hoja, para que se destrabe el sistema

#define BCL_SIZE_C_x_RENGLON 28
#define BCL_SIZE_RENGLONES 24

#define BCL_SIZE_BITARRAY_X (BCL_SIZE_C_x_RENGLON * 2) // 56
#define BCL_SIZE_BITARRAY_Y (BCL_SIZE_RENGLONES * 3)   // 72

#define BCL_SIZE_BYTES_x_RENGLON (BCL_SIZE_BITARRAY_X / 8)                    // 7
#define BCL_SIZE_BYTES_TOTAL (BCL_SIZE_BYTES_x_RENGLON * BCL_SIZE_BITARRAY_Y) //504

#define BCL_BUFFER_SIZE (BCL_SIZE_BYTES_TOTAL + 1) // 505. agregado byte de checksum.

//Estados de la comunicacion, para diferenciar que se recibe ya que al recibir una hoja
//se pueden dar todas las combinaciones de bytes posibles.
#define BCL_ESTADO_STANDBY 0         //En estado normal, recibe handshakes, envia y recibe boludeces.
#define BCL_ESTADO_RECIBIENDO_HOJA 1 //En este estado se recibe y guardan los bytes recibidos a la matriz contenidoHoja

//Variables del sistema
uint8_t BCLV_HOJA_ACTUAL = 0;            //Se actualiza, desde la pc, con la hoja actual que se esta imprimiendo
uint8_t BCLV_HOJA_NUMERO = 0;            //Se actualiza, desde la pc, con el numero total de hojas a imprimir
uint8_t BCLV_IMPRIMIENDO = 0;            //Se actualiza, desde la pc, si la impresora entra en modo de impresion.
                                         //1 = Se está imprimiendo, 0 = Se termino la impresion/No se está imprimiendo.
uint8_t BCL_ESTADO = BCL_ESTADO_STANDBY; //Estado del sistema.
uint8_t BCL_SER_REC_BUFF[2];             //Buffer inmediato para recepcion de comandos.
uint8_t BCL_SER_REC_BUFF_INDEX = 0;      //Indice para manejo del buffer inmediato
bool BCL_STANDBY_CMD_RECIBIDO = 0;       //Indicador de que se recibio un comando nuevo en el orden esperado.
uint32_t BCL_LASTMILLIS_TMOUT;           //Marca de tiempo para operaciones de TIMEOUT.

bool bitArray[BCL_SIZE_BITARRAY_X][BCL_SIZE_BITARRAY_Y];
uint8_t serialRXBuffer[BCL_BUFFER_SIZE]; //Buffer para la recepcion de datos x uart.

#endif