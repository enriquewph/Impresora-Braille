#ifndef COMANDOS_UART_H
#define COMANDOS_UART_H

//Comandos a recibir.
#define BCLS_HANDSHAKE 0xF0
#define BCLS_PREPARAR_IMPRESION 0xF1 //Se debe mandar al final de toda configuracion, justo antes de recibir datos de hoja.
#define BCLS_HOJA_NUMERO 0xF2
#define BCLS_HOJA_ACTUAL 0xF3

//Respuestas a enviar.
#define BCLR_CMD_VALIDO 0xF4      //Se envia si el comando recibido en STANDBY es VALIDO...
#define BCLR_CMD_INVALIDO 0xF5    //Se envia si el comando recibido en STANDBY es invalido...
#define BCLE_RECEPCION_OK 0xF6    //Se envia si se recibio la hoja correctamente.
#define BCLE_RECEPCION_ERROR 0xF7 //Se envia si hubo un error al recibir una hoja.

//Respuestas a enviar dadas por eventos.
#define BCLE_EVENTO_PREFIX 0xF8 //Para indicar un evento de los de abajo:

#define BCLE_EVENTO_IMPRESION_OK 0xA1    //Para indicar que la hoja se termino de imprimir.
#define BCLE_EVENTO_IMPRESION_FAIL 0xA2  //Para indicar que hubo un error al imprimir la hoja.
#define BCLE_EVENTO_SHUTDOWN 0xA3        //Para indicar que el usuario presiono el boton de apagado.
#define BCLE_EVENTO_LINEA_TERMINADA 0xA4 //Va acompañada de el numero de linea justo a continuacion.

#endif