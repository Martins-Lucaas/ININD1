#include <math.h>
#define pinANALOG A5                        //Configura o pino de leitura Analógica
#define pinPWM 3                            //Configura o pino de Saida do PWM
#define pinLED 13                           //Configura o pino de Saida do LED

#define ANALOGREADSTEP 500                  //Em ms
#define PWMSTEP 500                         //Em ms
#define LEDSTEP 500                         //Em ms

unsigned long ledTime=0, analogReadTime = 0, pwmTime = 0;
unsigned int amplitude = 0;

void ledFunc()                              // Faz a leitura do sinal Analogico
{ 
  digitalWrite(pinLED,!digitalRead(pinLED));
}

void analogReadFunc()                       // Faz a leitura do sinal Analogico
{ 
  amplitude = (unsigned int)(255.0*((float)analogRead(pinANALOG))/1023.0);
  Serial.println(amplitude);
}

void pwmFunc()                              //Faz a leitura e escrita da serial
{  
  analogWrite(pinPWM,amplitude);
}

void setup()                                // Codigo de configuração
{
  Serial.begin(19200);
  pinMode(pinANALOG, INPUT);
  pinMode(pinPWM, OUTPUT);
}

void loop()                                 // Codigo loop
{
  const unsigned long timeAtual = millis();
  if((timeAtual >= (analogReadTime + ANALOGREADSTEP) ) && 
     ((analogReadTime + ANALOGREADSTEP) >= ANALOGREADSTEP )){  // Para o caso do contador ter reiniciado
    analogReadTime = timeAtual;
    analogReadFunc();
  }
  if((timeAtual >= (pwmTime + PWMSTEP) ) &&
     ((pwmTime + PWMSTEP) >= PWMSTEP )){ // Para o caso do contador ter reiniciado
    pwmTime = timeAtual;
    pwmFunc();
  }
  if((timeAtual >= (ledTime + LEDSTEP) ) &&
     ((ledTime + LEDSTEP) >= LEDSTEP )){ // Para o caso do contador ter reiniciado
    ledTime = timeAtual;
    ledFunc();
  }
}
