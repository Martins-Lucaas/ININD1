#include <math.h>
#include <inindThread.h>

#define pinANALOG A5                        //Configura o pino de leitura Analógica
#define pinPWM 3                            //Configura o pino de Saida do PWM
#define pinLED 13                           //Configura o pino de Saida do LED
#define pinBotao 1

unsigned int amplitude = 0;
bool statusVar = false;

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
  pinMode(pinBotao, INPUT);
}

void loop(){
  if(digitalRead(pinBotao)==false){
    delay(500);
    statusVar = !statusVar;
    if(statusVar == true){
      threadSetup(ledFunc,500,analogReadFunc,1,pwmFunc,1,NULL);//parametros:funcão,intervalo,funcão,intervalo,...,NULL
    } else {
      analogWrite(pinPWM, 1);
      digitalWrite(pinLED,false);
      threadSetup();//parametros:funcão,intervalo,funcão,intervalo,...,NULL
    }
  }
}
