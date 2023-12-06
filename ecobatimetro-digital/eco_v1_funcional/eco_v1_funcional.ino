//---------------- MACROS Genericas ---------------//
#define clear_bit(p, b) ((p) &= ~(1 << (b)))
#define set_bit(p, b) ((p) |= (1 << (b)))
#define toggle_bit(p, b) ((p) ^= (1 << (b)))

//---------------- MACROS TIMERS ----------------//
// -- timer 1 --
#define T1A 1                                    // bit habilitacao de interrupcao por T1A de TIMSK1
#define T1_clear TCCR1A = TCCR1B = 0;            // limpa registradores do T1
#define T1_prescaller_256 set_bit(TCCR1B, CS12)  // prescaller 256 (CS12)
#define T1_CTC_OCR1A set_bit(TCCR1B, WGM12);     // CTC - overflow em OCR1A
#define T1_int(x) set_bit(TIMSK1, x);            // habilita interrupção (OVF/T1A/T1B)
#define T1_nint(x) clear_bit(TIMSK1, x);          // desabilita interrupção (OVF/T1A/T1B)
#define T1A_v TIMER1_COMPA_vect                  // bit habilitar interr T1A em TIMSK1
// -- timer 2 macros -
#define T2_clear TCCR2A = TCCR2B = 0;
#define T2_CTC_OCR2A set_bit(TCCR2A, WGM21);      // CTC - overflow em OCR2A
#define T2A 1                                    // bit habilitacao de interrupcao por T2A de TIMSK2 no R (OCFA)
#define T2_prescaller_O set_bit(TCCR2B, CS20)    // prescaller 0 (CA21)
#define T2_int(x) set_bit(TIMSK2, x);            // habilita interrupção (OVF/T2A/T2B)
#define T2_nint(x) clear_bit(TIMSK2, x);          // desabilita interrupção (OVF/T1A/T1B)
#define T2A_v TIMER2_COMPA_vect                  // bit habilitar interr T2A em TIMSK2

#define SPEED 1500


#include <math.h>
#include <inindThread.h>

#define pinANALOG A1                  //Configura o pino de leitura

#define FILTER_ORDER1 50
//---------------- FIM MACROS -------------------//


///////////////////////Funções do Filtro/////////////////////////////////////
typedef struct {
  double *history;
  const double *taps;
  unsigned int last_index;
  unsigned int filterOrder;
} DigitalFilter;
   
void DigitalFilter_init(DigitalFilter* f,const int filterOrder,const double *const filterTaps) {
  f->history = (double *) malloc(filterOrder*sizeof(double));
  for(int i = 0; i < filterOrder; ++i) f->history[i] = 0;  
  f->taps = filterTaps;
  f->filterOrder = filterOrder;
  f->last_index = 0;
}
  
void DigitalFilter_put(DigitalFilter* f,const double input) {
  f->history[f->last_index++] = input;
  if(f->last_index == f->filterOrder) f->last_index = 0;
}
  
double DigitalFilter_get(DigitalFilter* f) {
  double acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < f->filterOrder; ++i) {
    index = index != 0 ? index-1 : f->filterOrder-1;
    acc += f->history[index] * f->taps[i];
  };
  return acc;
}
//////////////////////////////Funções das Trheads///////////////////////////////////

DigitalFilter filter1;
const double filter_taps1[FILTER_ORDER1] = {
-0.0009950539745549227,
 -0.00014126209414331922,
 0.001344693038448063,
 -0.0003678021233808433,
 -0.001999297262386677,
 0.0014993524267699842,
 0.0026722207541640653,
 -0.003639817616884292,
 -0.0027337125401820225,
 0.006913882647863537,
 0.0012882301388646104,
 -0.011010131141181968,
 0.0026744572409477764,
 0.015072905395655709,
 -0.010122219790444538,
 -0.01765227128784015,
 0.021944483506804703,
 0.016591866876676978,
 -0.039320670782237756,
 -0.008364204989404109,
 0.06554331293324701,
 -0.015726679347238266,
 -0.11675521331826265,
 0.10591063312253107,
 0.48737229818616795,
 0.48737229818616795,
 0.10591063312253107,
 -0.11675521331826266,
 -0.015726679347238266,
 0.06554331293324704,
 -0.008364204989404113,
 -0.03932067078223776,
 0.01659186687667698,
 0.021944483506804703,
 -0.01765227128784016,
 -0.010122219790444538,
 0.015072905395655718,
 0.002674457240947777,
 -0.011010131141181975,
 0.001288230138864611,
 0.006913882647863544,
 -0.0027337125401820247,
 -0.003639817616884292,
 0.0026722207541640666,
 0.0014993524267699842,
 -0.0019992972623866802,
 -0.0003678021233808433,
 0.0013446930384480647,
 -0.00014126209414331922,
 -0.0009950539745549227
};

void analogReadFunc() {                     // Faz a leitura do sinal Analogico 
  const int analog_Value = analogRead(pinANALOG)-512;
  DigitalFilter_put(&filter1,analog_Value);
  
  Serial.println(DigitalFilter_get(&filter1));
  //Serial.println(analog_Value);
}
// ---- Helpers --- //

unsigned int times = 0;
bool toprint = false;
bool enable = false;
double time = 0;
double dist = 0;

unsigned int TIM16_ReadTCNT1(void)
{
  cli();  // Desativa interrupções
  unsigned int i = TCNT1;
  sei();  // Reativa interrupções
  return i;
}

void printDistance(void)
{
  Serial.print(dist, 6);  // Print com 6 casas decimais
  Serial.println();
}

void reset()
{
  T1_nint(T1A);
  T2_nint(T2A);
  times = 0;
  T1_int(T1A);  // Reativa a interrupção T1A
}

// --- Helpers --- //

void interruptFunction() {
  if (!enable) {
    enable = true;
  }
}


void setup()
{
  Serial.begin(9600);
  // configura o timer 1 para contar ate o tempo de retorno maximo
  T1_clear;                 // limpa registradores de configuracao do TIMER1
  T1_CTC_OCR1A;             // modo: CTC - overflow em OCR1A
  T1_prescaller_256;        // prescaller TIMER1 256
  OCR1A = 4124;             // (tempo_s*clock)/prescaller-1 = (66ms*16Mhz)/(256)-1 = 4124

  // configura o timer 2
  T2_clear;
  T2_prescaller_O;
  T2_CTC_OCR2A;
  OCR2A = 199; // 12.5 microsegundos (período/2 de 40 kHz)

  EICRA = 1 << ISC01 | 1 << ISC11;  // Seta o modo de interrupcao pra quando tiver uma descida.
  EIMSK = 1 << INT0 | 1 << INT1;    // habilitacao para a chamada da interrupcao em INT0 e INT1.
  //Configura o filtro digital//
  pinMode(pinANALOG, INPUT);
  DigitalFilter_init(&filter1,FILTER_ORDER1,filter_taps1);  
  threadSetup(analogReadFunc,10,NULL);//parametros:funcão,intervalo,funcão,intervalo,...,NULL 
}

void loop()
{
  // Constantemente verifica se está na hora de printar o resultado
  if (toprint)
  {
    printDistance();
    toprint = false;
  }
}

ISR(INT1_vect)
{ // pino D3
  if (!enable)
  {
    enable = true; // Há uma execução em andamento
    reset();
    T2_int(T2A); // Chama a interrupção do TIMER 2
  }
}

ISR(T2A_v)
{
  // Interrupção OCR2A match faz o toggle da porta B5.
  // Para gerar a onda de 40 kHz
  // Toggle feito em 12.5 microsegundos
  times++; // Soma cada inversão feita
  if (times <= 111)
  { // 111 -> Número de inversões máximas (40kHz) (t_init/(T/2)).
    toggle_bit(PORTB, 5);
    return;
  }
  else
  {
    toggle_bit(PORTB, 5);
    T2_nint(T2A); // Desativa a interrupção do TIMER 2.
    T1_int(T1A); // Ativa a interrupção do TIMER 1.
    TCNT1 = 84; // Correção de precisão: 84 = (1.344 ms * 16 MHz/256)
  }
}

ISR(T1A_v)
{
  // Interrupção OCR1A match, ocorre no "overflow" do TIMER 1
  if (dist > 50.0)
  {
    dist = 50.0; // Se a distância for maior que 50 m, força para ser 50 m.
  }
  reset();
  toprint = true;
  enable = false;
}

ISR(INT0_vect)
{ // pino D2
  // Calcula a distância
  unsigned int i = TIM16_ReadTCNT1(); // Pega tempo final do TIMER1
  time = (double(i + 1) * 256.0) / 16000000.0 / 2.0; // Desfaz o prescaler

  // Ajuste para evitar a impressão de distâncias maiores que a máxima
  if (time * double(SPEED) > 50.0)
  {
    dist = 50.0;
  }
  else
  {
    dist = time * double(SPEED);
  }

  reset(); // Reseta os timers
  toprint = true;
  enable = false;
}