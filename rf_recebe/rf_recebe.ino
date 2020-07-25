#include <VirtualWire.h>

/////////////////////////////////////////////////
//  Variáveis para a biblioteca VirtualWire  ////
/////////////////////////////////////////////////

char codigo;
String mensagem = "";

/////////////////////////////////////////////////
///////  Variáveis para gerenciar a fila  ///////
/////////////////////////////////////////////////
#define NOTE_B5 988
#define NOTE_C4 262
#define buzzerPin 4

#define botao1 3
#define botao2 2
// #define botao3 4
#define botao4 5
#define botao5 6
#define botao6 7

unsigned long previousMillis = 0; // Marca o tempo atual
const long interval = 500;        // Marca o intervalo que deve passar
int estado_som;                   // Marca o estado da função som()
bool toca_som = false;            // Ativa a função som

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
// int senha_atual_mista = 0; // Variável que indica a senha (da lista mista) que foi chamada

int identificador = 0; // Variável que indica qual balança foi usada (1, 2 ou 3)

int tempo = 500; // Delay padrão usado nos botões e no alarme sonoro

void setup()
{
  Serial.begin(9600);
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Configuração do receptor rf
  vw_set_ptt_inverted(true);
  vw_setup(2000);
  vw_rx_start();
}

void loop()
{
  som();
  condicoes(); // Condições para armazenar na lista de filas.
  decoder();   // Converte os dados recebidos via radio frequência para string

  ///////////// Mensagens recebidas pelo emissor rf ////////

  // Adiciona uma senha à fila normal
  if (mensagem == "norm")
  {
    lista_mista[ordem_mista] = cont_senha;
    lista_senhas[ordem_senha] = cont_senha;
    cont_senha++;
    ordem_senha++;
    ordem_mista++;
    mensagem = "";
  }

  // Adiciona uma senha à fila preferencial
  else if (mensagem == "pref")
  {
    lista_senhas_pref[ordem_senha_pref] = cont_senha_pref;
    lista_mista[ordem_mista] = cont_senha_pref;
    cont_senha_pref++;
    ordem_senha_pref++;
    ordem_mista++;
    mensagem = "";
  }
  //////////////////////////////////////////////////////////

  if (digitalRead(botao1) == HIGH)
  {
    // identificador = 1;
    Serial.println("teste");
    // chama_senha();
    delay(tempo);
  }

  if (digitalRead(botao2) == HIGH)
  {
    // identificador = 1;
    Serial.println("teste2");
    // rechama_senha(0);
    delay(tempo);
  }

  // if (digitalRead(botao3) == HIGH)
  // {
  //   identificador = 2;
  //   chama_senha();
  //   delay(tempo);
  // }

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

// Decodifica a mensagem recebida pelo módulo rf
void decoder()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen))
  {
    digitalWrite(13, true); // Pisca para indicar que recebeu uma mensagem
    //    Serial.print("Recebeu: ");
    for (int i = 0; i < buflen; i++)
    {
      //      Serial.print(buf[i], HEX);
      //      Serial.print(" ");
      codigo = char(buf[i]);
      mensagem.concat(codigo);
    }
    digitalWrite(13, false);
  }
}

// Em ambas as funções senha_normal() e senha_pref(), após enviar a senha pelo Serial, a senha é armazenada pelo guarda_senha(), removida da lista de ordem de chegada
// apagada da lista de senhas preferenciais

// Chama a senha preferencial
void senha_pref()
{
  senha_pref_armazenada = lista_senhas_pref[senha_atual_pref];
  Serial.println("P " + String(senha_pref_armazenada) + "-" + String(identificador));

  guarda_senha(lista_senhas_pref[senha_atual_pref]);
  remove_da_lista_mista(lista_senhas_pref[senha_atual_pref]);
  lista_senhas_pref[senha_atual_pref] = 0;
  // senha_atual_mista = i; 
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
    // senha_atual_mista = i;
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
      // ainda_tem_pref = false;
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

  // if (senha_atual_mista == num_mista)
  // {
  //   senha_atual_mista = 0;
  // }
}

void som()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (estado_som == 0 && toca_som == true)
    {
      tone(buzzerPin, NOTE_B5);
      estado_som = 1;
    }

    else if (estado_som == 1)
    {
      tone(buzzerPin, NOTE_C4);
      estado_som = 2;
    }

    else if (estado_som == 2)
    {
      noTone(buzzerPin);
      estado_som = 0;
      toca_som = false;
    }
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