#include <Arduino.h>
#line 1 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
#line 1 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
#include <stdint.h>

#line 3 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
void setup();
#line 14 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
void loop();
#line 53 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
void BrailleComLib_Init();
#line 62 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
void BrailleComLib_Loop();
#line 144 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
void recibirHoja();
#line 188 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
uint8_t checksum_get(const byte *data, size_t dataLength);
#line 195 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
byte CRC8(const byte *data, size_t dataLength);
#line 3 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
void setup()
{
    Serial.begin(115200);
    Serial3.begin(115200);
    BrailleComLib_Init();
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
    pinMode(13, OUTPUT);
    Serial3.println("INICIADO");
}

void loop()
{
    BrailleComLib_Loop();
}

//Configuracion:
#define BCL_RECIBIR_HOJA_TIMEOUT 2000 //[mS] transcurridos desde que no se recibio ningun dato. \
                                      //al recibir una hoja, para que se destrabe el sistema.

//Variables del sistema
uint8_t BCLV_HOJA_ACTUAL = 0; //Se actualiza, desde la pc, con la hoja actual que se esta imprimiendo
uint8_t BCLV_HOJA_NUMERO = 0; //Se actualiza, desde la pc, con el numero total de hojas a imprimir
uint8_t BCLV_IMPRIMIENDO = 0; //Se actualiza, desde la pc, si la impresora entra en modo de impresion.
                              //1 = Se está imprimiendo, 0 = Se termino la impresion/No se está imprimiendo.

byte contenidoHoja[7][72]; // 7 Bytes, 72 renglones -> 56x72 puntos -> 28 x 24 caracteres braille

//Estados de la comunicacion, para diferenciar que se recibe ya que al recibir una hoja
//se pueden dar todas las combinaciones de bytes posibles.
#define BCL_ESTADO_STANDBY 0         //En estado normal, recibe handshakes, envia y recibe boludeces.
#define BCL_ESTADO_RECIBIENDO_HOJA 1 //En este estado se recibe y guardan los bytes recibidos a la matriz contenidoHoja
uint8_t BCL_ESTADO = BCL_ESTADO_STANDBY;

//Comandos a recibir.
#define BCLS_HANDSHAKE 0xF0
#define BCLS_PREPARAR_IMPRESION 0xF1 //Se debe mandar al final de toda configuracion, justo antes de recibir datos de hoja.
#define BCLS_HOJA_NUMERO 0xF2
#define BCLS_HOJA_ACTUAL 0xF3

//Respuestas a enviar.
#define BCLR_CMD_VALIDO 0xF4   //Se envia si el comando recibido en STANDBY es VALIDO...
#define BCLR_CMD_INVALIDO 0xF5 //Se envia si el comando recibido en STANDBY es invalido...

//Respuestas a enviar dadas por eventos.
#define BCLE_IMPRESION_OK 0xF6    //Se envia si se termino de imprimir una hoja, y se espera mas datos.
#define BCLE_IMPRESION_ERROR 0xF7 //Se envia si no se pudo imprimir la hoja...
#define BCLE_RECEPCION_OK 0xF8    //Se envia si se recibio la hoja correctamente.
#define BCLE_RECEPCION_ERROR 0xF9 //Se envia si hubo un error al recibir una hoja.

void BrailleComLib_Init()
{
}

uint8_t BCL_SER_REC_BUFF[2];
uint8_t BCL_SER_REC_BUFF_INDEX = 0;
bool BCL_STANDBY_CMD_RECIBIDO = 0;
uint32_t BCL_LASTMILLIS_TMOUT;

void BrailleComLib_Loop()
{

    if (BCL_ESTADO == BCL_ESTADO_STANDBY)
    {
        uint8_t cmdByte, valByte;
        BCL_STANDBY_CMD_RECIBIDO = 0;

        if (Serial.available() > 1)
        {
            cmdByte = Serial.read();
            valByte = Serial.read();

            if (cmdByte >= BCLS_HANDSHAKE && cmdByte <= BCLS_HOJA_ACTUAL)
            {
                BCL_STANDBY_CMD_RECIBIDO = 1;
            }
            else
            {
                Serial.write(BCLR_CMD_INVALIDO);
            }
            BCL_LASTMILLIS_TMOUT = millis();
        }
        else if (Serial.available() == 1)
        {
            if (millis() >= BCL_LASTMILLIS_TMOUT + 500)
            {
                uint8_t dump = Serial.read();
            }
        }
        else
        {
            BCL_LASTMILLIS_TMOUT = millis();
        }

        if (BCL_STANDBY_CMD_RECIBIDO)
        {
            Serial3.print("CMD: " + String(cmdByte, HEX) + " - ");

            switch (cmdByte)
            {
            case BCLS_HANDSHAKE:
                Serial.write(BCLR_CMD_VALIDO);
                Serial3.println("HANDSHAKE");
                break;

            case BCLS_PREPARAR_IMPRESION:
                Serial.write(BCLR_CMD_VALIDO);
                BCLV_IMPRIMIENDO = 1;
                BCL_ESTADO = BCL_ESTADO_RECIBIENDO_HOJA;
                Serial3.println("PREP");
                break;
            case BCLS_HOJA_NUMERO:
                Serial.write(BCLR_CMD_VALIDO);
                BCLV_HOJA_NUMERO = valByte;
                Serial3.println("H_TOT: " + String(valByte));
                break;
            case BCLS_HOJA_ACTUAL:
                Serial.write(BCLR_CMD_VALIDO);
                BCLV_HOJA_ACTUAL = valByte;
                Serial3.println("H_ACT: " + String(valByte));
                break;
            default: //Comando no valido
                Serial.write(BCLR_CMD_INVALIDO);
                Serial3.println("INVALIDO");
                break;
            }
        }
    }
    else
    {
        //ESTADO_RECIBIENDO_HOJA
        //Esto traba el codigo hasta que termine de recibir la hoja, o transcurra un timeout...
        recibirHoja();
        BCL_ESTADO = BCL_ESTADO_STANDBY;
    }

    //Actualizar la variable que indica si se esta imprimiendo.
    if (BCLV_IMPRIMIENDO && BCLV_HOJA_ACTUAL == BCLV_HOJA_NUMERO)
        BCLV_IMPRIMIENDO = 0;
}

void recibirHoja()
{
    //poner en 0 para salir del loop
    digitalWrite(13, HIGH);
    uint8_t serialRXBuffer[504]; //zarpadaso bufer logi
    Serial3.println("Entrado en modo recibirHoja");

    uint8_t indiceX = 0;
    uint8_t indiceY = 0;
    uint16_t indiceBuffer = 0;

    uint32_t lastMillis = millis();

    Serial.setTimeout(2500);

    size_t bytesRecibidos = Serial.readBytes(serialRXBuffer, 504);

    bool recepcion_ok = false;

    if (bytesRecibidos == (size_t)504)
    {
        for (indiceX = 0; indiceX < 7; indiceX++)
            for (indiceY = 0; indiceY < 72; indiceY++)
                contenidoHoja[indiceX][indiceY] = serialRXBuffer[indiceBuffer];
        recepcion_ok = true;
    }

    if (recepcion_ok)
    {
        Serial.write(BCLE_RECEPCION_OK);
        uint8_t csum = checksum_get(serialRXBuffer, 504);
        Serial.write(csum);
        Serial3.println("RECEPCION OK CRC: " + String(csum));
    }
    else
    {
        Serial.write(BCLE_RECEPCION_ERROR); //enviar que hubo un error
        Serial3.println("TIMEOUT, BYTES RECIBIDOS: " + String(bytesRecibidos));
    }

    Serial3.println("Salida del modo recibirHoja");
    digitalWrite(13, LOW);
}

uint8_t checksum_get(const byte *data, size_t dataLength)
{
    return(CRC8(data, dataLength));
}



byte CRC8(const byte *data, size_t dataLength)
{
    byte crc = 0x00;
    while (dataLength--)
    {
        byte extract = *data++;
        for (byte tempI = 8; tempI; tempI--)
        {
            byte sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if (sum)
            {
                crc ^= 0x8C;
            }
            extract >>= 1;
        }
    }
    return crc;
}
