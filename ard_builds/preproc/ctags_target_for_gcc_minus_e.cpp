# 1 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
# 2 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino" 2

void setup()
{
    Serial.begin(115200);

    Serial3.begin(115200);
    pinMode(0, 0x1);
    digitalWrite(0, 0x1);
    pinMode(13, 0x1);
    Serial3.println("INICIADO");

    BrailleComLib_Init();
}

void loop()
{
    BrailleComLib_Loop();
}


void serialEvent3()
{
    String inputString = Serial3.readStringUntil('\n');
    switch (inputString.toInt())
    {
    case 1:
        BCL_SendEvent(0xA1 /*Para indicar que la hoja se termino de imprimir.*/, 0);
        break;
    case 2:
        BCL_SendEvent(0xA2 /*Para indicar que hubo un error al imprimir la hoja.*/, 0);
        break;
    case 3:
        BCL_SendEvent(0xA3 /*Para indicar que el usuario presiono el boton de apagado.*/, 0);
        break;
    case 4:
        BCL_SendEvent(0xA4 /*Va acompañada de el numero de linea justo a continuacion.*/, BCLV_HOJA_ACTUAL);
        break;
    }
}


void BCL_SendEvent(uint8_t EVENTO, uint8_t DATO) //Para la mayoria de los eventos el dato es opcional, poner 0..
{
    uint8_t SerialSendBuffer[3] = {0xF8 /*Para indicar un evento de los de abajo:*/, EVENTO, DATO};
    Serial.write(SerialSendBuffer, 3);
}

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
    Serial3.println("Entrado en modo recibirHoja");


    uint8_t indiceX = 0;
    uint8_t indiceY = 0;
    uint16_t indiceBuffer = 0;
    byte checksum = 0;

    uint32_t lastMillis = millis();

    Serial.setTimeout(2500);

    size_t bytesRecibidos = Serial.readBytes(serialRXBuffer, ((((28 * 2) /* 56*/ / 8) /* 7*/ * (24 * 3) /* 72*/) /*504*/ + 1) /* 505. agregado byte de checksum.*/);

    bool recepcion_ok = false;

    if (bytesRecibidos == ((((28 * 2) /* 56*/ / 8) /* 7*/ * (24 * 3) /* 72*/) /*504*/ + 1) /* 505. agregado byte de checksum.*/) //504 bytes de hoja + 1 byte de checksum
    {
        checksum = calcularChecksum(serialRXBuffer);
        byte checksum_pc = serialRXBuffer[(((28 * 2) /* 56*/ / 8) /* 7*/ * (24 * 3) /* 72*/) /*504*/];

        if (checksum == checksum_pc)
            recepcion_ok = true;

        if (recepcion_ok)
        {
            Serial.write(0xF6 /*Se envia si se recibio la hoja correctamente.*/);
            //Transponer los datos recibidos al BitArray
            cargarBitArray();

            Serial3.println("RECEPCION OK CSUM: " + String(checksum));

        }
        else
        {
            Serial.write(0xF7 /*Se envia si hubo un error al recibir una hoja.*/); //enviar que hubo un error

            Serial3.println("RECEPCION FAIL CSUM PC: " + String(checksum_pc) + " - ESP: " + String(checksum));

        }
    }
    else
    {
        Serial.write(0xF7 /*Se envia si hubo un error al recibir una hoja.*/); //enviar que hubo un error

        Serial3.println("TIMEOUT, BYTES RECIBIDOS: " + String(bytesRecibidos));

    }

    Serial3.println("Salida del modo recibirHoja");
    digitalWrite(13, 0x0);

}

void BrailleComLib_Init()
{
    cargarBitArray();
}

void cargarBitArray()
{
    uint16_t index_x = 0;
    uint16_t index_y = 0;

    for (uint16_t index = 0; index < (((28 * 2) /* 56*/ / 8) /* 7*/ * (24 * 3) /* 72*/) /*504*/; index++)
    {
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (0)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (1)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (2)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (3)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (4)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (5)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (6)) & 0x01);
        bitArray[index_x++][index_y] = (((serialRXBuffer[index]) >> (7)) & 0x01);

        if (index_x >= (28 * 2) /* 56*/ - 1)
        {
            index_x = 0;
            index_y++;
        }
    }

    /*Serial3.print("bool bitArray[56][72] = {");

    for (uint16_t y = 0; y < BCL_SIZE_BITARRAY_Y; y++)

    {

        if (y > 0)

            Serial3.print("                         ");

        for (uint16_t x = 0; x < BCL_SIZE_BITARRAY_X; x++)

        {

            Serial3.print(bitArray[x][y]);



            if (y < 71 || x < 55)

                Serial3.print(", ");

        }

        if (y < 71)

            Serial3.print('\n');

    }

    Serial3.print("};");*/
# 245 "c:\\Users\\quiqu\\Documents\\GitHub\\Impresora-Braille\\BrailleComLib_arduino\\BrailleComLib_arduino.ino"
}

uint8_t calcularChecksum(uint8_t *datos)
{
    uint8_t checksum = 0;
    uint32_t checksum_long = 0;

    for (int i = 0; i < (((28 * 2) /* 56*/ / 8) /* 7*/ * (24 * 3) /* 72*/) /*504*/; i++)
        checksum_long += datos[i];

    checksum = checksum_long % 256;

    return checksum;
}
