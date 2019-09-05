#include <stdint.h>

uint8_t contenidoHoja[7][72]; // 7 Bytes, 72 renglones -> 56x72 puntos -> 28 x 24 caracteres braille


void setup()
{
    BrailleComLib_Init();
}

void loop()
{
    BrailleComLib_Loop();
}

