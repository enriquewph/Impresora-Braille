#define LED1_PIN 2
#define LED2_PIN 15
#define BOTON1_PIN 0
#define BOTON2_PIN 4

#define MOTORDC_A_PIN 27
#define MOTORDC_B_PIN 26

#define ACS722_PIN 34

#define ENCODER_B 32
#define ENCODER_A 33
#define MOTORSTEP_STEP 25
#define MOTORSTEP_DIR 14

#define COOLER_PIN 12
#define SOLENOIDE_PIN 13



void setup()
{
    // put your setup code here, to run once:
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[12], PIN_FUNC_GPIO);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(COOLER_PIN, OUTPUT);
}

#define DELAY_T 500
void loop()
{
    // put your main code here, to run repeatedly:
    digitalWrite(LED1_PIN, 0);
    digitalWrite(COOLER_PIN, 0);
    delay(DELAY_T);
    digitalWrite(LED1_PIN, 1);
    digitalWrite(COOLER_PIN, 1);
    delay(DELAY_T);
}
