# 1 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
# 1 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"


void setup()
{
    Serial.begin(115200);
    Serial3.begin(115200);
    BrailleComLib_Init();
    pinMode(0, 0x1);
    digitalWrite(0, 0x1);
    pinMode(13, 0x1);
    Serial3.println("INICIADO");
}

void loop()
{
    BrailleComLib_Loop();
}

//Configuracion:



//Variables del sistema
uint8_t BCLV_HOJA_ACTUAL = 0; //Se actualiza, desde la pc, con la hoja actual que se esta imprimiendo
uint8_t BCLV_HOJA_NUMERO = 0; //Se actualiza, desde la pc, con el numero total de hojas a imprimir
uint8_t BCLV_IMPRIMIENDO = 0; //Se actualiza, desde la pc, si la impresora entra en modo de impresion.
                              //1 = Se está imprimiendo, 0 = Se termino la impresion/No se está imprimiendo.

byte contenidoHoja[7][72]; // 7 Bytes, 72 renglones -> 56x72 puntos -> 28 x 24 caracteres braille

//Estados de la comunicacion, para diferenciar que se recibe ya que al recibir una hoja
//se pueden dar todas las combinaciones de bytes posibles.


uint8_t BCL_ESTADO = 0 /*En estado normal, recibe handshakes, envia y recibe boludeces.*/;

//Comandos a recibir.





//Respuestas a enviar.



//Respuestas a enviar dadas por eventos.





void BrailleComLib_Init()
{
}

uint8_t BCL_SER_REC_BUFF[2];
uint8_t BCL_SER_REC_BUFF_INDEX = 0;
bool BCL_STANDBY_CMD_RECIBIDO = 0;
uint32_t BCL_LASTMILLIS_TMOUT;

void BrailleComLib_Loop()
{

    if (BCL_ESTADO == 0 /*En estado normal, recibe handshakes, envia y recibe boludeces.*/)
    {
        uint8_t cmdByte, valByte;
        BCL_STANDBY_CMD_RECIBIDO = 0;

        if (Serial.available() > 1)
        {
            cmdByte = Serial.read();
            valByte = Serial.read();

            if (cmdByte >= 0xF0 && cmdByte <= 0xF3)
            {
                BCL_STANDBY_CMD_RECIBIDO = 1;
            }
            else
            {
                Serial.write(0xF5 /*Se envia si el comando recibido en STANDBY es invalido...*/);
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
            Serial3.print("CMD: " + String(cmdByte, 16) + " - ");

            switch (cmdByte)
            {
            case 0xF0:
                Serial.write(0xF4 /*Se envia si el comando recibido en STANDBY es VALIDO...*/);
                Serial3.println("HANDSHAKE");
                break;

            case 0xF1 /*Se debe mandar al final de toda configuracion, justo antes de recibir datos de hoja.*/:
                Serial.write(0xF4 /*Se envia si el comando recibido en STANDBY es VALIDO...*/);
                BCLV_IMPRIMIENDO = 1;
                BCL_ESTADO = 1 /*En este estado se recibe y guardan los bytes recibidos a la matriz contenidoHoja*/;
                Serial3.println("PREP");
                break;
            case 0xF2:
                Serial.write(0xF4 /*Se envia si el comando recibido en STANDBY es VALIDO...*/);
                BCLV_HOJA_NUMERO = valByte;
                Serial3.println("H_TOT: " + String(valByte));
                break;
            case 0xF3:
                Serial.write(0xF4 /*Se envia si el comando recibido en STANDBY es VALIDO...*/);
                BCLV_HOJA_ACTUAL = valByte;
                Serial3.println("H_ACT: " + String(valByte));
                break;
            default: //Comando no valido
                Serial.write(0xF5 /*Se envia si el comando recibido en STANDBY es invalido...*/);
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
        BCL_ESTADO = 0 /*En estado normal, recibe handshakes, envia y recibe boludeces.*/;
    }

    //Actualizar la variable que indica si se esta imprimiendo.
    if (BCLV_IMPRIMIENDO && BCLV_HOJA_ACTUAL == BCLV_HOJA_NUMERO)
        BCLV_IMPRIMIENDO = 0;
}

void recibirHoja()
{
    //poner en 0 para salir del loop
    digitalWrite(13, 0x1);
    uint8_t serialRXBuffer[505]; //zarpadaso bufer logi
    Serial3.println("Entrado en modo recibirHoja");

    uint8_t indiceX = 0;
    uint8_t indiceY = 0;
    uint16_t indiceBuffer = 0;
    byte checksum = 0;

    uint32_t lastMillis = millis();

    Serial.setTimeout(2500);

    size_t bytesRecibidos = Serial.readBytes(serialRXBuffer, 505);

    bool recepcion_ok = false;

    if (bytesRecibidos == 505) //504 bytes de hoja + 1 byte de checksum
    {
        for (indiceX = 0; indiceX < 7; indiceX++)
            for (indiceY = 0; indiceY < 72; indiceY++)
                contenidoHoja[indiceX][indiceY] = serialRXBuffer[indiceBuffer++];

        checksum = calcularChecksum(serialRXBuffer);
        byte checksum_pc = serialRXBuffer[504];

        if (checksum == checksum_pc)
            recepcion_ok = true;
    }

    if (recepcion_ok)
    {
        Serial.write(0xF8 /*Se envia si se recibio la hoja correctamente.*/);
        Serial3.println("RECEPCION OK CSUM: " + String(checksum));
    }
    else
    {
        Serial.write(0xF9 /*Se envia si hubo un error al recibir una hoja.*/); //enviar que hubo un error
        Serial3.println("TIMEOUT, BYTES RECIBIDOS: " + String(bytesRecibidos));
    }

    Serial3.println("Salida del modo recibirHoja");
    digitalWrite(13, 0x0);
}

uint8_t calcularChecksum(uint8_t *datos)
{
    uint8_t checksum = 0;
    uint32_t checksum_long = 0;

    for (int i = 0; i < 504; i++)
        checksum_long += datos[i];

    checksum = checksum_long % 256;

    return checksum;
}
