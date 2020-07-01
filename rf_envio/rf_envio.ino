#include <VirtualWire.h>

#define botao1 6
#define botao2 7

int tempo = 500;

void setup()
{
  Serial.begin(9600);
  Serial.println("setup");

  // Configuração do emissor rf
  // vw_set_rx_pin(12);
  vw_set_ptt_inverted(true);
  vw_setup(2000);
}

void loop()
{
  if (digitalRead(botao1) == HIGH)
  {
    const char *msg1 = "normal";

    digitalWrite(13, true); // Pisca para mostrar que enviou dados
    vw_send((uint8_t *)msg1, strlen(msg1));
    vw_wait_tx();
    digitalWrite(13, false);
    Serial.println("Enviado");
    delay(tempo);
  }

  if (digitalRead(botao2) == HIGH)
  {
    const char *msg2 = "preferencial";

    digitalWrite(13, true); // Pisca para mostrar que enviou dados
    vw_send((uint8_t *)msg2, strlen(msg2));
    vw_wait_tx();
    digitalWrite(13, false);
    Serial.println("Enviado");
    delay(tempo);
  }
}
