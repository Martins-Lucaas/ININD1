const int buttonPin = 2; // Pino do botão
const int ledPin = 13;   // Pino do LED

int ledState = LOW;         // Estado atual do LED
int lastButtonState = LOW;  // Estado anterior do botão
unsigned long lastDebounceTime = 0;  // Último momento em que o botão foi lido
unsigned long debounceDelay = 50;    // Tempo de debounce (evita leituras falsas)

unsigned long previousMillis = 0;  // Último momento em que o LED mudou de estado
unsigned long interval = 1000;     // Intervalo padrão para piscar o LED (1 segundo)

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Configura o pino do botão como entrada com pull-up interno
  pinMode(ledPin, OUTPUT);          // Configura o pino do LED como saída
  Serial.begin(9600);              // Inicia a comunicação serial a 9600 bps
}

void loop() {
  int reading = digitalRead(buttonPin); // Lê o estado atual do botão

  // Verifica se o estado do botão mudou
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Registra o momento atual
  }

  // Verifica se o botão está estável por tempo suficiente
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Se o estado do botão mudou, atualiza o estado do LED
    if (reading != lastButtonState) {
      lastButtonState = reading;

      if (lastButtonState == LOW) { // Verifica se o botão foi pressionado (baixo)
        ledState = !ledState; // Alterna o estado do LED (liga se estava desligado e vice-versa)
        Serial.println(ledState == HIGH ? "LED ligado." : "LED desligado.");
      }
    }
  }

  // Atualiza o estado do LED
  digitalWrite(ledPin, ledState);
  
  // Verifica se há dados disponíveis na serial
  if (Serial.available() > 0) {
    interval = Serial.parseInt(); // Lê o intervalo de tempo da serial
    Serial.print("Intervalo alterado para: ");
    Serial.print(interval);
    Serial.println(" ms");
  }

  // Piscar o LED com base no intervalo especificado
  unsigned long currentMillis = millis();
  if (ledState == HIGH && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = LOW;
    digitalWrite(ledPin, ledState);
  }
}
