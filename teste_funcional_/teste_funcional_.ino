#define botao1 2
#define botao2 3
#define botao3 4
#define botao4 5

int cont_senha = 1;  // Contador de senhas
int cont_senha_pref = -1;  // Contador de senhas preferenciais

const unsigned int num_senha = 99;  // Vetor que armazena a ordem das senhas
int lista_senhas[num_senha];
int ordem_senha = 0;

int senha_atual = 0;
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
  if (digitalRead(botao1) == HIGH) {
    Serial.println("Fila Normal");
    Serial.println("Senha:");
    if (cont_senha < 10) {
      Serial.println("0" + String(cont_senha));
    }
    else {
      Serial.println(cont_senha);
    }

    lista_senhas[ordem_senha] = cont_senha;
    cont_senha++;
    ordem_senha++;

    if (cont_senha == 100) {
      cont_senha = 0;
    }
    if (ordem_senha == 100) {
      ordem_senha = 0;
    }
    delay(500);
  }

  if (digitalRead(botao2) == HIGH) {
    cont_senha_pref *= -1;  //Torna a senha preferencial em inteiro positivo
    Serial.println("Fila Preferencial");
    Serial.println("Senha:");
    if (cont_senha_pref < 10) {
      Serial.println(String("P 0") + cont_senha_pref);
    }
    else {
      Serial.println(String("P ") + cont_senha_pref);
    }
    cont_senha_pref *= -1;
    lista_senhas[ordem_senha] = cont_senha_pref;
    cont_senha_pref--;
    ordem_senha++;
    reordenar(lista_senhas, num_senha);

    delay(500);
  }



  if (digitalRead(botao3) == HIGH) {
    if (lista_senhas[senha_atual] == 0) {
      Serial.println("Não há novas senhas");
    }
    else {

      Serial.println("Chamando Senha");
      if (lista_senhas[senha_atual] < 0) {
        lista_senhas[senha_atual] *= -1;
        if (lista_senhas[senha_atual] < 10) {
          Serial.println(String("Senha Preferencial: P 0") + lista_senhas[senha_atual]);
        }
        else {
          Serial.println(String("Senha Preferencial: P ") + lista_senhas[senha_atual]);
        }
        lista_senhas[senha_atual] *= -1;
      }

      if (lista_senhas[senha_atual] > 0) {
        if (lista_senhas[senha_atual] < 10) {
          Serial.println(String("Senha Normal: 0") + lista_senhas[senha_atual]);
        }
        else {
          Serial.println(String("Senha Normal: ") + lista_senhas[senha_atual]);
        }

      }

      ultima_senha1 = lista_senhas[senha_atual];
      senha_atual++;
    }
    delay(500);
  }

  if (digitalRead(botao4) == HIGH)  {
    Serial.println("Fila: ");
    for (byte y = 0; y < num_senha; y++) {
      if (lista_senhas[y] == 0) {
        break;
      }
      else {
        Serial.print(lista_senhas[y]);
        Serial.print(", ");
      }

      if (y >= num_senha) {
        y = 0;
      }
    }

    Serial.println();
    Serial.println("Rechamando a senha");
    if (ultima_senha1 < 0) {
      ultima_senha1 *= -1;
      if (ultima_senha1 < 10) {
        Serial.println(("Senha Preferencial: P 0") + String(ultima_senha1));
      }
      else {
        Serial.println("Senha Preferencial: P " + String(ultima_senha1));
      }
      ultima_senha1 *= -1;
    }
    if (ultima_senha1 > 0) {
      Serial.println(String("Senha Normal: ") + ultima_senha1);
    }
    delay(500);
  }
}


//  if (digitalRead(botao4) == HIGH) {
//    Serial.println("Rechamando a senha");
//    if (ultima_senha1 < 0) {
//      ultima_senha1 *= -1;
//      if (ultima_senha1 < 10) {
//        Serial.println(String("Senha Preferencial: P 0") + ultima_senha1);
//      }
//      else {
//        Serial.println("Senha Preferencial: P " + ultima_senha1);
//      }
//      ultima_senha1 *= -1;
//    }
//    if (ultima_senha1 > 0) {
//      Serial.println(String("Senha Normal: ") + ultima_senha1);
//    }
//    delay(500);
//  }
//}


void andar_para_direita(int fila[], int num_senha, int fora_da_ordem, int atual)
{
  char temporario = fila[atual];
  for (int i = atual; i > fora_da_ordem; i--) {
    fila[i] = fila[i - 1];
  }
  fila[fora_da_ordem] = temporario;
}

void reordenar(int fila[], int num_senha)
{
  int fora_do_lugar = -1;

  for (int index = senha_atual; index < num_senha; index++) {
    if (fora_do_lugar >= 0) {
      if ((fila[index] < 0) && (fila[fora_do_lugar] >= 0))  {
        andar_para_direita(fila, num_senha, fora_do_lugar, index);

        if (index - fora_do_lugar >= 2) {
          fora_do_lugar += 2;
        }
        else  {
          fora_do_lugar = -1;
        }
      }
    }

    if (fora_do_lugar == -1) {

      if (((fila[index] >= 0) && (!(index & 0x01)))
          || ((fila[index] < 0) && (index & 0x01))) {
        fora_do_lugar = index;
      }
    }
  }
}
