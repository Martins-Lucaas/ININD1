#include <math.h>
#include <inindThread.h>
#define MAXNUMTHREAD 3
#define pinANALOG A5                        //Configura o pino de leitura Analógica
#define pinPWM 3                            //Configura o pino de Saida do PWM
#define pinLED 13                           //Configura o pino de Saida do LED

unsigned int amplitude = 0;

void ledFunc()                             // Faz a leitura do sinal Analogico
{ 
  digitalWrite(pinLED,!digitalRead(pinLED));
}

void analogReadFunc()                      // Faz a leitura do sinal Analogico
{ 
  amplitude = (unsigned int) (255.0*analogRead(pinANALOG)/1023.0);
  Serial.println(amplitude);
}

void pwmFunc()                            //Faz a leitura e escrita da serial
{  
  analogWrite(pinPWM, amplitude);
}
  
void setup()                              // Codigo de configuração
{
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
  pinMode(pinPWM, OUTPUT);
  pinMode(pinLED, OUTPUT);
  trheadSetup(ledFunc,500,analogReadFunc,1,pwmFunc,1,NULL);//parametros:funcão,intervalo,funcão,intervalo...NULL
}

void loop(){}
