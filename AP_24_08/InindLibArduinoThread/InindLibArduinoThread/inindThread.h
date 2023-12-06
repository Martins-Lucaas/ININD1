//https://blog.eletrogate.com/interrupcao-o-que-e-como-utilizar-arduin/
//https://www.squids.com.br/arduino/index.php/projetos-arduino/projetos-squids/basico/263-projeto-79-interrupcao-por-timer-no-arduino-alarme-sensor-de-toque

//O timer0 é utilizado pelo Arduino para funções como delay(), millis() e micros(). Então não se deve utilizar esse timer para evitar comprometer essas funções.
//O timer1 no Arduino UNO esse é o timer utilizado pela biblioteca de controle de servos. Caso você não esteja utilizando essa biblioteca, esse timer está livre para ser utilizado para outros propósitos. No Arduino Mega esse timer só será utilizado para controlar os servos se você estiver usando mais de 12 servos. (timer utilizado no projeto)
//O timer2 é utilizado pela função tone(). Então se você não precisar da função tone() esse timer está livre para outras aplicações.
#include <stdarg.h>

using CallbackFunc = void(*)(); //definição de tipo para nossa callback
volatile CallbackFunc threadFunc[5]; 
volatile int numThread,threadInterval[5],threadTime[5];

void threadSetup(){
  numThread=0;
}
void threadSetup(CallbackFunc callback,int threadIntervalFunc,...){
  numThread = 1;
  va_list args;
  va_start (args, threadIntervalFunc);
  
  while(va_arg(args, CallbackFunc)){ 
    va_arg(args, int);
    numThread++;
  }

  //Dynamically allocate memory using malloc()
  //threadTime     = (int *) malloc(numThread * sizeof(int));
  //threadInterval = (int *) malloc(numThread * sizeof(int));
  //threadFunc     = (CallbackFunc *) malloc(numThread * sizeof(CallbackFunc));
  
  threadTime[0]     = 0;
  threadFunc[0]     = callback;
  threadInterval[0] = threadIntervalFunc;

  va_start (args, threadIntervalFunc);
  for(int i=1;i<numThread;i++){
    threadTime[i]     = 0;
    threadFunc[i]     = va_arg(args, CallbackFunc);
    threadInterval[i] = va_arg(args,int);
  }
  va_end(args);
  cli(); // desliga as interrupções para não haver error durante a configuração 
  TCCR1A = 0b01000000;//modo comparação
  TCCR1B = 0b00001011;//configuração do prescaler em 64 e OCR1A register is used to manipulate the counter resolution
  TIMSK1 = 0b00000001;//habilita a interrupção por overflow do timer 
  OCR1A  = 249; //compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
  sei();//habilita interrupções globais
}

ISR(TIMER1_OVF_vect){
  for(int i=0;i < numThread;i++){
    if(threadTime[i] >= threadInterval[i]) {
      threadFunc[i]();
      threadTime[i] = 0;
    }
    else threadTime[i]++;
  } 
}

