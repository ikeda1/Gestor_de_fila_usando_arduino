#include <SPI.h> // Inclusão de biblioteca
#include <nRF24L01.h> // Inclusão de biblioteca
#include <RF24.h> // Inclusão de biblioteca

/////////////////////////////////////////////////
//////  Variáveis para a biblioteca RF24  ///////
/////////////////////////////////////////////////

RF24 radio(0, 10); // Cria uma instância utilizando os pinos (CE, CSN)
const byte address[6] = "00002"; // Cria um endereço para envio de dados

/////////////////////////////////////////////////
///////  Variáveis para gerenciar a fila  ///////
/////////////////////////////////////////////////

#define botao1 3
#define botao2 2
#define botao3 4
#define botao4 5
#define botao5 6
#define botao6 7

int cont_senha = 1;        // Contador de senhas, mostra o número da senha que será impressa
int cont_senha_pref = 501; // Contador de senhas preferenciais, mostra o número da senha preferencial que será impressa

int cont = 0; // Contador da função confere_ordem, marca o número de senhas preferenciais seguidas
int i;        // Contador da função remove_da_lista_mista

const unsigned int num_mista = 50; // Número de senhas dentro da lista_mista
const unsigned int num_senha = 50; // Número de senhas dentro da lista_senhas
int lista_senhas[num_senha];       // Vetor que armazena senhas existentes
int lista_senhas_pref[num_senha];  // Vetor que armazena senhas PREFERENCIAIS
int lista_mista[num_mista];        // Vetor que armazena todas as senhas na ordem de chegada
int guarda_senhas[3];              // Vetor que armazena as últimas senhas chamadas em cada balança. A ordem indica o numero da balança ("posição = num_balança" | 0 = 1 | 1 = 2 | 2 = 3)

int senha_pref_armazenada;
int senha_normal_armazenada;

int ordem_senha = 0;      // Variável que indica as posições em que as senhas serão armazenadas no vetor "lista_senhas"
int ordem_senha_pref = 0; // Variável que indica as posições em que as senhas PREFERENCIAIS serão armazenadas no vetor "lista_senhas_pref"
int ordem_mista = 0;      // Variável que indica as posições em que todas as senhas serão armazenadas no vetor "lista_mista"

bool alterna_pref = false; // Variável que registra a alternancia entre senhas normais e preferenciais

int soma_preferencial; // Armazena a soma das senhas preferenciais
int soma_normal;       // Armazena a soma das senhas normais

int senha_atual = 0;       // Variável que indica a senha que está sendo chamada
int senha_atual_pref = 0;  // Variável que indica a senha preferencial que está sendo chamada

int identificador = 0; // Variável que indica qual balança foi usada (1, 2 ou 3)

int tempo = 500; // Delay padrão usado nos botões e no alarme sonoro

void setup()
{
  Serial.begin(9600);
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Configuração do receptor rf
  radio.begin(); // Inicializa a comunicação sem fio
  radio.openReadingPipe(0, address); // Define o endereço para recebimento de dados
  radio.setPALevel(RF24_PA_HIGH); // Define o nível do amplificador de potência
  radio.startListening(); // Define o módulo como receptor (Não envia dados)
}

void loop()
{
  condicoes(); // Condições para armazenar na lista de filas.
  recebe_senha();   // Converte os dados recebidos via radio frequência para string

  if (digitalRead(botao1) == HIGH)
  {
    identificador = 1;
    chama_senha();
    delay(tempo);
  }

  if (digitalRead(botao2) == HIGH)
  {
    identificador = 1;
    rechama_senha(0);
    delay(tempo);
  }

  if (digitalRead(botao3) == HIGH)
  {
    identificador = 2;
    chama_senha();
    delay(tempo);
  }

  if (digitalRead(botao4) == HIGH)
  {
    identificador = 2;
    rechama_senha(1);
    delay(tempo);
  }

  if (digitalRead(botao5) == HIGH)
  {
    identificador = 3;
    chama_senha();
    delay(tempo);
  }

    if (digitalRead(botao6) == HIGH)
  {
    identificador = 3;
    rechama_senha(2);
    delay(tempo);
  }
// ---------------------------------------------------------------------------------------------------------------------- //
  // ---------  Botao feito para ajudar no debug, mostra o valor armazenado nas três listas e a variável cont --------- //

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
  //   Serial.println("Senha Mista");
  //   for (byte i = 0; i < num_mista; i++)
  //   {
  //     Serial.print(lista_mista[i]);
  //     Serial.print(" | ");
  //   }
  //   confere_ordem();
  //   Serial.println("cont = " + String(cont));

  //   delay(500);
  // }
// ---------------------------------------------------------------------------------------------------------------------- //
}

// Recebe a mensagem pelo módulo RF24L01
void recebe_senha()
{
  if (radio.available()) { // Se a comunicação estiver habilitada, faz...
    int num; // Armazena os dados recebidos
    radio.read(&num, sizeof(int)); // Lê os dados recebidos

    // Adiciona uma senha à fila normal
    if (num == 1) 
    {
      lista_mista[ordem_mista] = cont_senha;
      lista_senhas[ordem_senha] = cont_senha;
      cont_senha++;
      ordem_senha++;
      ordem_mista++;
    }

    // Adiciona uma senha à fila preferencial
    else if (num == 2)
    {
      lista_senhas_pref[ordem_senha_pref] = cont_senha_pref;
      lista_mista[ordem_mista] = cont_senha_pref;
      cont_senha_pref++;
      ordem_senha_pref++;
      ordem_mista++;
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
  remove_da_lista_mista(lista_senhas_pref[senha_atual_pref]);
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
    remove_da_lista_mista(lista_senhas[senha_atual]);
    lista_senhas[senha_atual] = 0;
    senha_atual++;
  }
}

// Essa função altera a última senha chamada para 0 na lista_mista[]
void remove_da_lista_mista(int lista)
{
  for (i = 0; i < num_senha; i++)
  {
    if (lista_mista[i] == lista)
    {
      lista_mista[i] = 0;
      break;
    }
  }
}

// Confere se existem senhas preferenciais seguidas na ordem de chamada.
void confere_ordem()
{
  // A variável cont vai armazenar quantas senhas preferenciais em seguida foram encontradas
  cont = 0;
  for (int x = 0; x < num_senha; x++)
  {
    if (lista_mista[x] == 0) // Se o valor encontrado for 0, pula para o próximo loop do for
    {
      continue;
    }

    // Se o encontrar uma senha normal, encerra a busca pois será aplicado o método de senha alternada
    else if (lista_mista[x] < 501)
    {
      break;
    }

    // Se encontrar um preferencial, entra em um loop para verificar as próximas senhas
    // a partir da primeira preferencial encontrada
    if (lista_mista[x] >= 501)
    {
      int y = x;
      cont++;

      while (lista_mista[y] >= 501)
      {
        y++;

        // Se y chegar no limite da lista, ele vai para o começo e continua procurando preferenciais seguidos
        if (y == num_mista)
        {
          y = 0;
        }

        // Se encontrar senha preferencial, cont++
        else if (lista_mista[y] >= 501)
        {
          cont++;
        }

        // Se encontrar senha normal, encerra a busca
        else if (lista_mista[y] < 501)
        {
          break;
        }
      }
      break;
    }
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

// Chamada de senha de forma alternada, sempre começando pela preferencial
void misto()
{
  if (alterna_pref == false)
  {
    senha_pref();
    alterna_pref = true;
  }

  else
  {
    senha_normal();
    alterna_pref = false;
  }
}

// Chamada de senha quando existem ambas senhas, mas as preferenciais estão seguidas
void misto_so_pref()
{
  senha_pref();
  cont--;
}

void chama_senha()
{
  soma_normal = 0;
  soma_preferencial = 0;
  soma();
  soma_pref();
  // Serial.println("== Chamando Senha ==");
  if (soma_normal || soma_preferencial != 0)
  {
    if (soma_normal == 0 && soma_preferencial != 0) // 1° caso - Somente senhas preferenciais
    {
      senha_pref();
      // Serial.println("1° caso - senha_pref()");
    }

    else if (soma_normal != 0 && soma_preferencial == 0) // 2° caso - Somente senhas normais
    {
      senha_normal();
      // Serial.println("2° caso - senha_normal()");
    }

    else if (soma_normal != 0 && soma_preferencial != 0) // 3° caso - Ambas as senhas, com preferenciais seguidas
    {
      confere_ordem();

      if (cont != 0)
      {
        misto_so_pref();
        // Serial.println("3° caso - misto_so_pref()");
      }

      else // 3° caso - Ambas as senhas, sem preferenciais em seguida
      {
        misto();
        // Serial.println("3° caso - misto()");
      }
    }
    toca_som = true;
  }
  else
  {
    // Serial.println("Não há mais senhas");
  }
}

void condicoes()
{
  // Define o número máximo das senhas | normal = 1 ~ 500 | preferencial = 501 ~ 999
  if (cont_senha == 501)
  {
    cont_senha = 1;
  }
  if (cont_senha_pref == 1000)
  {
    cont_senha_pref = 501;
  }

  // Se a lista ficar cheia, reseta para a posição inicial para sobreescrever senhas antigas em suas respectivas listas
  if (ordem_senha == num_senha)
  {
    ordem_senha = 0;
  }

  if (ordem_senha_pref == num_senha)
  {
    ordem_senha_pref = 0;
  }

  if (ordem_mista == num_mista)
  {
    ordem_mista = 0;
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
  toca_som = true;
  
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
