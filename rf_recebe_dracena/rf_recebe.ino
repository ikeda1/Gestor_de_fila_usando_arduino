#include <SPI.h> // Inclusão de biblioteca
#include <nRF24L01.h> // Inclusão de biblioteca
#include <RF24.h> // Inclusão de biblioteca

/////////////////////////////////////////////////
//////  Variáveis para a biblioteca RF24  ///////
/////////////////////////////////////////////////

RF24 radio(9, 10); // Cria uma instância utilizando os pinos (CE, CSN)
const byte address[6] = "00046"; // Cria um endereço para envio de dados
//int num_recebido;
int num_pref;

/////////////////////////////////////////////////
///////  Variáveis para gerenciar a fila  ///////
/////////////////////////////////////////////////

#define botao1 2
#define botao2 3
#define botao3 4
#define botao4 5
#define botao5 6
#define botao6 7

int cont = 0; // Contador da função confere_ordem, marca o número de senhas preferenciais seguidas

const unsigned int num_senha = 50; // Número de senhas dentro da lista_senhas
int lista_senhas[num_senha];       // Vetor que armazena senhas existentes
int lista_senhas_pref[num_senha];  // Vetor que armazena senhas PREFERENCIAIS
int guarda_senhas[3];              // Vetor que armazena as últimas senhas chamadas em cada balança. A ordem indica o numero da balança ("posição = num_balança" | 0 = 1 | 1 = 2 | 2 = 3)

int senha_pref_armazenada;
int senha_normal_armazenada;

int ordem_senha = 0;      // Variável que indica as posições em que as senhas serão armazenadas no vetor "lista_senhas"
int ordem_senha_pref = 0; // Variável que indica as posições em que as senhas PREFERENCIAIS serão armazenadas no vetor "lista_senhas_pref"


bool alterna_pref = false; // Variável que registra a alternancia entre senhas normais e preferenciais

int soma_preferencial; // Armazena a soma das senhas preferenciais
int soma_normal;       // Armazena a soma das senhas normais

int senha_atual = 0;       // Variável que indica a senha que está sendo chamada
int senha_atual_pref = 0;  // Variável que indica a senha preferencial que está sendo chamada

int identificador = 0; // Variável que indica qual balança foi usada (1, 2 ou 3)

unsigned long previousMillis_botao1 = 0;
unsigned long previousMillis_botao2 = 0;
unsigned long previousMillis_botao3 = 0;
unsigned long previousMillis_botao4 = 0;
unsigned long previousMillis_botao5 = 0;
unsigned long previousMillis_botao6 = 0;

int lastBtnState1 == HIGH;
int lastBtnState2 == HIGH;
int lastBtnState3 == HIGH;
int lastBtnState4 == HIGH;
int lastBtnState5 == HIGH;
int lastBtnState6 == HIGH;

int intervalo = 1250; // Delay padrão usado nos botões

void setup()
{
  Serial.begin(9600);
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(botao3, INPUT);
  pinMode(botao4, INPUT);
  pinMode(botao5, INPUT);
  pinMode(botao6, INPUT);

//  Serial.println("setup");

  // Configuração do receptor rf
  radio.begin(); // Inicializa a comunicação sem fio
  radio.openReadingPipe(0, address); // Define o endereço para recebimento de dados
  radio.setPALevel(RF24_PA_LOW); // Define o nível do amplificador de potência
  radio.setDataRate(RF24_250KBPS);
  radio.startListening(); // Define o módulo como receptor (Não envia dados)
}

void loop()
{
  int reading_1 = digitalRead(botao1);
  int reading_2 = digitalRead(botao2);
  int reading_3 = digitalRead(botao3);
  int reading_4 = digitalRead(botao4);
  // int reading_5 = digitalRead(botao5);
  // int reading_6 = digitalRead(botao6);
  // digitalWrite(led, LOW);
  condicoes(); // Condições para armazenar na lista de filas.
  recebe_senha();   // Converte os dados recebidos via nRF24L01 para string
  
  if (reading_1 == LOW && lastBtnState1 == HIGH)
  {
    unsigned long currentMillis_botao1 = millis();
    if ((unsigned long)(currentMillis_botao1 - previousMillis_botao1) >= intervalo)
    {
      previousMillis_botao1 = millis();
      identificador = 1;
      chama_senha();
//      digitalWrite(led, HIGH);
    }
  }

  if (reading_2 == LOW && lastBtnState2 == HIGH)
  {
    unsigned long currentMillis_botao2 = millis();
    if ((unsigned long)(currentMillis_botao2 - previousMillis_botao2) >= intervalo)
    {
      previousMillis_botao2 = millis();
      identificador = 1;
      rechama_senha(0);
//      digitalWrite(led, HIGH);
    }
  }

  if (reading_3 == LOW && lastBtnState3 == HIGH)
  {
    unsigned long currentMillis_botao3 = millis();
    if ((unsigned long)(currentMillis_botao3 - previousMillis_botao3) >= intervalo)
    {
      previousMillis_botao3 = millis();
      identificador = 2;
      chama_senha();    
//      digitalWrite(led, HIGH);  
    }
  }

  if (reading_4 == LOW && lastBtnState4 == HIGH)
  {
    unsigned long currentMillis_botao4 = millis();
    if ((unsigned long)(currentMillis_botao4 - previousMillis_botao4) >= intervalo)
    {
      previousMillis_botao4 = millis();
      identificador = 2;
      rechama_senha(1);
//      digitalWrite(led, HIGH);
    }
  }

 if (reading_5 == LOW && lastBtnState5 == HIGH)
 {
   unsigned long currentMillis_botao5 = millis();
   if ((unsigned long)(currentMillis_botao5 - previousMillis_botao5) >= intervalo)
   {
      previousMillis_botao5 = millis();
      identificador = 3;
      chama_senha();
    //  digitalWrite(led, HIGH);
   }
 }

  if (reading_6 == LOW && lastBtnState6 == HIGH)
 {
   unsigned long currentMillis_botao6 = millis();
   if ((unsigned long)(currentMillis_botao6 - previousMillis_botao6) >= intervalo)
   {
     previousMillis_botao6 = millis();
     identificador = 3;
     rechama_senha(2);
    //  digitalWrite(led, HIGH);
   }
 }

  lastBtnState1 = reading_1;
  lastBtnState2 = reading_2;
  lastBtnState3 = reading_3;
  lastBtnState4 = reading_4;
  lastBtnState5 = reading_5;
  lastBtnState6 = reading_6;

// ---------------------------------------------------------------------------------------------------------------------- //
  // ---------  Botao feito para ajudar no debug, mostra o valor armazenado nas três listas e a variável cont --------- //
  // ---------  Utiliza o botao4 como input                                                                   --------- //

  // if (digitalRead(botao4) == HIGH)
  // {
  //   Serial.println();
  //   Serial.println("Senhas Normais");
  //   for (byte i = 0; i < num_senha; i++)
  //   {
  //     Serial.print(lista_senhas[i]);
  //     Serial.print(" | ");
  //   }

  //   Serial.println();
  //   Serial.println("Senhas Preferenciais");
  //   for (byte i = 0; i < num_senha; i++)
  //   {
  //     Serial.print(lista_senhas_pref[i]);
  //     Serial.print(" | ");
  //   }

  //   Serial.println();

  //   delay(500);
  // }
  
// ---------------------------------------------------------------------------------------------------------------------- //
}

// Recebe a mensagem pelo módulo RF24L01
void recebe_senha()
{
  if (radio.available()) { // Se a comunicação estiver habilitada, faz...
    int num_recebido; // Armazena os dados recebidos
    radio.read(&num_recebido, sizeof(num_recebido)); // Lê os dados recebidos
//    Serial.println(num_recebido);
    digitalWrite(13, true);
    digitalWrite(13, false);
    // Adiciona uma senha à fila normal
    if (num_recebido >= 1 && num_recebido <= 500) 
    {
      lista_senhas[ordem_senha] = num_recebido;
      ordem_senha++;
    }

    // Adiciona uma senha à fila preferencial
    else if (num_recebido >= 501 && num_recebido <= 1000)
    {
      lista_senhas_pref[ordem_senha_pref] = num_recebido;
      ordem_senha_pref++;
    }
  }
}

// Em ambas as funções senha_normal() e senha_pref(), após enviar a senha pelo Serial, a senha é armazenada pelo guarda_senha(), removida da lista de ordem de chegada
// e de senhas preferenciais

// Chama a senha preferencial
void senha_pref()
{
  senha_pref_armazenada = lista_senhas_pref[senha_atual_pref];
  Serial.println("P " + String(senha_pref_armazenada) + "-" + String(identificador));

  guarda_senha(lista_senhas_pref[senha_atual_pref]);
  lista_senhas_pref[senha_atual_pref] = 0;
  senha_atual_pref++;
}

// Chama a senha normal
void senha_normal()
{

  if (soma_normal > 0)
  {
    senha_normal_armazenada = lista_senhas[senha_atual];
    if (lista_senhas[senha_atual] < 10)
    {
      Serial.println("0" + String(senha_normal_armazenada) + "-" + String(identificador));
    }

    else
    {
      Serial.println(String(senha_normal_armazenada) + "-" + String(identificador));
    }

    guarda_senha(lista_senhas[senha_atual]);
    lista_senhas[senha_atual] = 0;
    senha_atual++;
  }
}


void soma()
{
  for (byte x = 0; x < num_senha; x++) // Soma as senhas para verificar se existem mais senhas
  {
    soma_normal += lista_senhas[x];
  }
}

// Soma as senhas preferenciais para verificar se existem mais senhas preferenciais
void soma_pref()
{
  for (byte x = 0; x < num_senha; x++)
  {
    soma_preferencial += lista_senhas_pref[x];
  }
}

void chama_senha()
{
  soma_normal = 0;
  soma_preferencial = 0;
  soma();
  soma_pref();
  // Serial.println("== Chamando Senha ==");
  if (soma_normal != 0 || soma_preferencial != 0)
  {
    if (soma_preferencial != 0) // 1° caso - Somente senhas preferenciais
    {
      senha_pref();
      // Serial.println("1° caso - senha_pref()");
    }

    else if (soma_normal != 0 && soma_preferencial == 0) // 2° caso - Somente senhas normais
    {
      senha_normal();
      // Serial.println("2° caso - senha_normal()");
    }
  }
  // else
  // {
  //   Serial.println("Não há mais senhas");
  // }
}

void condicoes()
{
  // Se a lista ficar cheia, reseta para a posição inicial
  if (ordem_senha == num_senha)
  {
    ordem_senha = 0;
  }

  if (ordem_senha_pref == num_senha)
  {
    ordem_senha_pref = 0;
  }

  // Indica em qual posição a senha que será chamada se encontra em cada lista
  if (senha_atual == num_senha)
  {
    senha_atual = 0;
  }

  if (senha_atual_pref == num_senha)
  {
    senha_atual_pref = 0;
  }
}

void guarda_senha(int senha)
{
  switch (identificador)
  {
  case 1:
    guarda_senhas[0] = senha;
    break;

  case 2:
    guarda_senhas[1] = senha;
    break;

  case 3:
    guarda_senhas[2] = senha;
    break;

  default:
    break;
  }
}

void rechama_senha(int i)
{
  int x = guarda_senhas[i];
  
  if (x < 10 && x > 0)
  {
    Serial.println("0" + String(x) + "-" + String(identificador));
  }

  else if (x > 9 && x < 501)
  {
    Serial.println(String(x) + "-" + String(identificador));
  }

  else if (x > 500 && x < 1000)
  {
    Serial.println("P " + String(x) + "-" + String(identificador));
  }
}
