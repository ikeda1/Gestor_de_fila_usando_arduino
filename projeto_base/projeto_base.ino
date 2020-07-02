#define botao1 2
#define botao2 3
#define botao3 4
#define botao4 5

int cont_senha = 1;        // Contador de senhas, mostra o número da senha que será impressa
int cont_senha_pref = 201; // Contador de senhas preferenciais, mostra o número da senha preferencial que será impressa

const unsigned int num_senha = 30; // Número de senhas dentro do vetor
int lista_senhas[num_senha];       // Vetor que armazena senhas existentes
int lista_senhas_pref[num_senha];  // Vetor que armazena senhas PREFERENCIAIS

int ordem_senha = 0;      // Variável que indica as posições em que as senhas serão armazenadas no vetor "lista_senhas"
int ordem_senha_pref = 0; // Variável que indica as posições em que as senhas PREFERENCIAIS serão armazenadas no vetor "lista_senhas_pref"

bool tem_preferencial = false; // Marca se tem ou não preferencial
int soma_preferencial;
int soma_normal;

int senha_atual = 0;      // Variável que indica a senha que está sendo chamada
int senha_atual_pref = 0; // Variável que indica a senha preferencial que está sendo chamada

// Cada variável pertence a um soquete, armazenando a última senha chamada em cada balança
int ultima_senha1 = 0;
int ultima_senha2 = 0;
int ultima_senha3 = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(botao3, INPUT);
  pinMode(botao4, INPUT);
}

void loop()
{

  if (cont_senha == 201)
  {
    cont_senha = 1;
  }
  if (cont_senha_pref == 401)
  {
    cont_senha_pref = 201;
  }

  // Se a lista ficar cheia, reseta para a posição inicial para sobreescrever senhas antigas
  if (ordem_senha == num_senha)
  {
    ordem_senha = 0;
  }

  if (senha_atual == num_senha)
  {
    senha_atual = 0;
  }

  if (digitalRead(botao1) == HIGH)
  {
    Serial.println();
    Serial.println("Fila Normal");
    Serial.println("Senha:");

    if (cont_senha < 10)
    {
      Serial.println("0" + String(cont_senha));
    }

    else
    {
      Serial.println(cont_senha);
    }

    lista_senhas[ordem_senha] = cont_senha;
    cont_senha++;
    ordem_senha++;
    delay(500);
  }

  if (digitalRead(botao2) == HIGH)
  {
    Serial.println();
    Serial.println("Fila Preferencial");
    Serial.println("Senha:");
    Serial.println(String("P ") + cont_senha_pref);
    lista_senhas_pref[ordem_senha_pref] = cont_senha_pref;
    cont_senha_pref++;
    ordem_senha_pref++;
    tem_preferencial = true;
    delay(500);
  }

  if (digitalRead(botao3) == HIGH)
  {
    Serial.println();
    chama_senha();
    delay(500);
  }

  if (digitalRead(botao4) == HIGH)
  {
    Serial.println();
    Serial.println("Senhas Normais");
    for (byte i = 0; i < num_senha; i++)
    {
      Serial.print(lista_senhas[i]);
      Serial.print(" | ");
    }

    Serial.println();
    Serial.println("Senhas Preferenciais");

    for (byte i = 0; i < num_senha; i++)
    {
      Serial.print(lista_senhas_pref[i]);
      Serial.print(" | ");
    }

    delay(500);
  }
}

//  if (digitalRead(botao4) == HIGH) {
//    Serial.println("Rechamando a senha");
//    if (ultima_senha1 > 200) {
//        Serial.println("Senha Preferencial: P " + ultima_senha1);
//    }
//    if (ultima_senha1 < 200 && ultima_senha1 != 0) {
//      if (ultima_senha1 < 10) {
//        Serial.println("Senha Normal: 0" + String(ultima_senha1));
//      }
//      else {
//        Serial.println("Senha Normal: " + ultima_senha);
//      }
//    }
//    delay(500);
//  }
// }

void tem_pref()
{
  if (tem_preferencial == true)
  {
    Serial.println("Senha Preferencial: P " + String(lista_senhas_pref[senha_atual_pref]));
    ultima_senha1 = lista_senhas_pref[senha_atual_pref];
    lista_senhas_pref[senha_atual_pref] = 0;
    senha_atual_pref++;
  }
}

void senha_normal()
{

  if (soma_normal > 0)
  {
    if (lista_senhas[senha_atual] == 0) 
    {
      senha_atual++;
    }
    if (lista_senhas[senha_atual] < 10)
    {
      Serial.println("Senha Normal: 0" + String(lista_senhas[senha_atual]));
    }

    else
    {
      Serial.println("Senha Normal: " + String(lista_senhas[senha_atual]));
    }

    ultima_senha1 = lista_senhas[senha_atual];
    lista_senhas[senha_atual] = 0;
    senha_atual++;
  }
}

void soma()
{
  for (byte x = 0; x < num_senha; x++) // Soma as senhas para verificar se ainda existem mais
  {
    soma_normal += lista_senhas[x];
  }
}

void soma_pref()
{
  for (byte x = 0; x < num_senha; x++) // Soma as senhas preferenciais para verificar se existe mais senhas
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
  Serial.println("== Chamando Senha ==");
  if (soma_normal != 0 || soma_preferencial != 0)
  {
    if (tem_preferencial == true)
    {
      tem_pref();
      tem_preferencial = false;
    }

    else
    {
      senha_normal();
      if (soma_preferencial != 0)
      {
        tem_preferencial = true;
      }
    }
  }

  else
  {
    Serial.println("Não há mais senhas");
  }
}
