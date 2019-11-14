#include "header.h"

void setup()
{
    Serial.begin(115200);
#ifdef DEBUG
    Serial3.begin(115200);
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
    pinMode(13, OUTPUT);
    Serial3.println("INICIADO");
#endif
    BrailleComLib_Init();
}

void loop()
{
    BrailleComLib_Loop();
}

#ifdef DEBUG
void serialEvent3()
{
    String inputString = Serial3.readStringUntil('\n');
    switch (inputString.toInt())
    {
    case 1:
        BCL_SendEvent(BCLE_EVENTO_IMPRESION_OK, 0);
        break;
    case 2:
        BCL_SendEvent(BCLE_EVENTO_IMPRESION_FAIL, 0);
        break;
    case 3:
        BCL_SendEvent(BCLE_EVENTO_SHUTDOWN, 0);
        break;
    case 4:
        BCL_SendEvent(BCLE_EVENTO_LINEA_TERMINADA, BCLV_HOJA_ACTUAL);
        break;
    }
}
#endif

void BCL_SendEvent(uint8_t EVENTO, uint8_t DATO) //Para la mayoria de los eventos el dato es opcional, poner 0..
{
    uint8_t SerialSendBuffer[3] = {BCLE_EVENTO_PREFIX, EVENTO, DATO};
    Serial.write(SerialSendBuffer, 3);
}

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
#ifdef DEBUG
            Serial3.print("CMD: " + String(cmdByte, HEX) + " - ");
#endif
            switch (cmdByte)
            {
            case BCLS_HANDSHAKE:
                Serial.write(BCLR_CMD_VALIDO);
#ifdef DEBUG
                Serial3.println("HANDSHAKE");
#endif
                break;

            case BCLS_PREPARAR_IMPRESION:
                Serial.write(BCLR_CMD_VALIDO);
                BCLV_IMPRIMIENDO = 1;
                BCL_ESTADO = BCL_ESTADO_RECIBIENDO_HOJA;
#ifdef DEBUG
                Serial3.println("PREP");
#endif
                break;
            case BCLS_HOJA_NUMERO:
                Serial.write(BCLR_CMD_VALIDO);
                BCLV_HOJA_NUMERO = valByte;
#ifdef DEBUG
                Serial3.println("H_TOT: " + String(valByte));
#endif
                break;
            case BCLS_HOJA_ACTUAL:
                Serial.write(BCLR_CMD_VALIDO);
                BCLV_HOJA_ACTUAL = valByte;
#ifdef DEBUG
                Serial3.println("H_ACT: " + String(valByte));
#endif
                break;
            default: //Comando no valido
                Serial.write(BCLR_CMD_INVALIDO);
#ifdef DEBUG
                Serial3.println("INVALIDO");
#endif
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
#ifdef DEBUG
    digitalWrite(13, HIGH);
    Serial3.println("Entrado en modo recibirHoja");
#endif

    uint8_t indiceX = 0;
    uint8_t indiceY = 0;
    uint16_t indiceBuffer = 0;
    byte checksum = 0;

    uint32_t lastMillis = millis();

    Serial.setTimeout(2500);

    size_t bytesRecibidos = Serial.readBytes(serialRXBuffer, BCL_BUFFER_SIZE);

    bool recepcion_ok = false;

    if (bytesRecibidos == BCL_BUFFER_SIZE) //504 bytes de hoja + 1 byte de checksum
    {
        checksum = calcularChecksum(serialRXBuffer);
        byte checksum_pc = serialRXBuffer[BCL_SIZE_BYTES_TOTAL];

        if (checksum == checksum_pc)
            recepcion_ok = true;

        if (recepcion_ok)
        {
            Serial.write(BCLE_RECEPCION_OK);
            //Transponer los datos recibidos al BitArray
            cargarBitArray();
#ifdef DEBUG
            Serial3.println("RECEPCION OK CSUM: " + String(checksum));
#endif
        }
        else
        {
            Serial.write(BCLE_RECEPCION_ERROR); //enviar que hubo un error
#ifdef DEBUG
            Serial3.println("RECEPCION FAIL CSUM PC: " + String(checksum_pc) + " - ESP: " + String(checksum));
#endif
        }        
    }
    else
    {
        Serial.write(BCLE_RECEPCION_ERROR); //enviar que hubo un error
#ifdef DEBUG
        Serial3.println("TIMEOUT, BYTES RECIBIDOS: " + String(bytesRecibidos));
#endif
    }
#ifdef DEBUG
    Serial3.println("Salida del modo recibirHoja");
    digitalWrite(13, LOW);
#endif
}

void BrailleComLib_Init()
{
    cargarBitArray();
}

void cargarBitArray()
{
    uint16_t index_x = 0;
    uint16_t index_y = 0;

    for (uint16_t index = 0; index < BCL_SIZE_BYTES_TOTAL; index++)
    {
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 0);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 1);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 2);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 3);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 4);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 5);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 6);
        bitArray[index_x++][index_y] = bitRead(serialRXBuffer[index], 7);

        if (index_x >= BCL_SIZE_BITARRAY_X - 1)
        {
            index_x = 0;
            index_y++;
        }
    }
#ifdef DEBUG
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
#endif
}

uint8_t calcularChecksum(uint8_t *datos)
{
    uint8_t checksum = 0;
    uint32_t checksum_long = 0;

    for (int i = 0; i < BCL_SIZE_BYTES_TOTAL; i++)
        checksum_long += datos[i];

    checksum = checksum_long % 256;

    return checksum;
}