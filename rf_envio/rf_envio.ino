#include <SPI.h> // Inclusão de biblioteca
#include <nRF24L01.h> // Inclusão de biblioteca
#include <RF24.h> // Inclusão de biblioteca

#define botao1 5
#define botao2 6

RF24 radio(9, 10); // Cria uma instância utilizando os pinos (CE, CSN)
const byte address[6] = "00002"; //CRIA UM ENDEREÇO PARA ENVIO DOS
//DADOS (O TRANSMISSOR E O RECEPTOR DEVEM SER CONFIGURADOS COM O MESMO ENDEREÇO)

const int senhaNormal = 1; // Variável que indica para o receiver que é senha normal
const int senhaPref = 2; // Variável que indica para o receiver que é senha preferencial

int tempo = 500;

int cont_senha = 1;  // Contador de senhas
int cont_senha_pref = 501;  // Contador de senhas preferenciais

void setup()
{
  Serial.begin(9600);
  // Serial.println("setup");
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);

  // Configuração da impressora
  begin();
  justification(1);
  size(1);
  bold(1);

  // Configuração do RF24L01
  radio.begin(); // Inicializa a comunicação sem fio
  radio.openWritingPipe(address); // Define o endereço para envio de dados ao receptor
  radio.setPALevel(RF24_PA_HIGH); // Define o nível do amplificador de potência
  radio.stopListening(); // Define o módulo como transmissor (não recebe dados)
}

void loop()
{
  // Envia senhas normais
  if (digitalRead(botao1) == HIGH && digitalRead(botao2) == LOW)
  {
    Serial.println("Fila Normal");
    Serial.println("Senha: ");
    feed(1);
    size(51);

    if (cont_senha < 10) 
    {
      Serial.println("0" + String(cont_senha));
    }

    else
    {
      Serial.println(cont_senha);
    }

    cont_senha++;

    // Reseta o contador se a senha normal chegar a 500
    if (cont_senha == 500) 
    {
      cont_senha = 1;
    }

    // senhaNormal = 1; // Variável que recebe a mensagem a ser transmitida
    radio.write(&senhaNormal, sizeof(int)); // Envia a mensagem ao receptor

    // digitalWrite(13, true); // Pisca o led built-in para mostrar que enviou dados
    // digitalWrite(13, false);

    size(1);
    feed(2);
    cut();
    delay(tempo);
  }

  // Envia senhas preferenciais
  if (digitalRead(botao2) == HIGH && digitalRead(botao1) == LOW)
  {
    Serial.println("Fila Preferencial");
    Serial.println("Senha: ");
    feed(1);
    size(51);

    Serial.println("P " + String(cont_senha_pref));
    cont_senha_pref++;

    if (cont_senha_pref == 1000)
    {
      cont_senha_pref = 501;
    }

    radio.write(&senhaPref, sizeof(int)); // Envia a mensagem ao receptor

    // digitalWrite(13, true); // Pisca o led built-in para mostrar que enviou dados
    // digitalWrite(13, false);

    size(1);
    feed(2);
    cut();
    delay(tempo);
  }
}


//////////////////////////////////////////////////////////////////////////
//////////////////  Funções para utilizar a impressora  //////////////////
//////////////////////////////////////////////////////////////////////////

// As funções foram feitas com base no manual da impressora Sweda SI-300S
// http://sistemas.sweda.com.br/downloads/523_manual_de_comandos_si3aas_e_si3aal.pdf

void begin() // Inicializa
{
  Serial.write(27);
  Serial.write(64);
}

void justification(int just) // Escolhe a posição em que o texto será impresso
{                            // 0 = Esquerda | 1 = Meio | 2 = Direita
  Serial.write(27);
  Serial.write(97);
  Serial.write(just);
}

void feed(int lines) // Pula o número de linhas correspondente à variável "lines"
{
  Serial.write(27);
  Serial.write(100);
  Serial.write(lines);
}

void cut() // Pula 4 linhas e corta o papel
{
  feed(4);
  Serial.write(27);
  Serial.write(105);
}

void color(bool col) // Muda a cor do texto
{                    // 0 = Preto | 1 = Branco
  Serial.write(29);
  Serial.write(66);
  Serial.write(col);
}

void size(int siz) // Muda o tamanho do caracter, possui 8 tamanhos de altura e 8 de largura
// Largura: 0, 16, 32, 48, 64, 80, 96, 112
// Altura: 0, 1, 2, 3, 4, 5, 6, 7
// Soma-se o número das medidas desejadas e substitui a variável "siz"
{                  
  Serial.write(29);
  Serial.write(33);
  Serial.write(siz);
}

void bold(bool bol) // Coloca o texto em negrito
{                   // 0 = Desligado | 1 = Ligado
  Serial.write(27);
  Serial.write(69);
  Serial.write(bol);
}