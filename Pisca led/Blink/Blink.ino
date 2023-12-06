#include <Thread.h>

// Define os pinos do LED e a Thread
const int ledPin = 13;  // Pino do LED (pode variar dependendo do seu Arduino)
Thread ledThread = Thread();

// Função que faz o LED piscar
void ledBlink() {
  while (true) {
    digitalWrite(ledPin, HIGH);   // Liga o LED
    delay(250);                    // Mantém o LED ligado por 250 ms
    digitalWrite(ledPin, LOW);    // Desliga o LED
    delay(250);                    // Mantém o LED desligado por 250 ms
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  
  // Inicia a Thread que faz o LED piscar
  ledThread.onRun(ledBlink);
  ledThread.setInterval(500); // Define o intervalo de execução para 500 ms
  ledThread.enabled = true;   // Habilita a Thread
}

void loop() {
  // O loop principal do programa não faz nada neste exemplo
}
