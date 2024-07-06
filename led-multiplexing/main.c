/*
Press the green play button to run the simulation --->

Description:
Charlieplexing LED Circuit (top) 
  - N pins control N*(N-1) LEDs
Multiplexing LED Circuit (bottom)
  - N+M pins control N*M LEDs

Author: Caite S.
*/

#include <stdio.h>
#include "pico/stdlib.h"

#define LED_A 18
#define LED_B 19
#define LED_C 20
#define GND_A 21
#define GND_B 22
#define GND_C 26
#define LED_D 13
#define LED_E 12
#define LED_F 11
#define BTN_SWITCH 15
#define BTN_NEXT 14
#define LEN_multiplex 3
#define LEN_charlieplex 3

void multiplex_put(int LED_num, bool on);
void charlieplex_put(int LED_num, bool on);
void multiplexAllOff();
void charlieplexAllOff();
void multiplex_loop();
void charlieplex_loop();


int main() {
  stdio_init_all();
  gpio_init(LED_A);
  gpio_init(LED_B);
  gpio_init(LED_C);
  gpio_init(GND_A);
  gpio_init(GND_B);
  gpio_init(GND_C);
  gpio_init(LED_D);
  gpio_init(LED_E);
  gpio_init(LED_F);
  gpio_init(BTN_SWITCH);
  gpio_init(BTN_NEXT);
  gpio_set_dir(LED_A, GPIO_OUT);
  gpio_set_dir(LED_B, GPIO_OUT);
  gpio_set_dir(LED_C, GPIO_OUT);
  gpio_set_dir(GND_A, GPIO_OUT);
  gpio_set_dir(GND_B, GPIO_OUT);
  gpio_set_dir(GND_C, GPIO_OUT);
  gpio_set_dir(BTN_SWITCH, GPIO_IN);
  gpio_set_dir(BTN_NEXT, GPIO_IN);

  multiplexAllOff();
  charlieplexAllOff();

  while(true) {
     multiplex_loop();
     charlieplex_loop();
  }
}



void multiplex_put(int LED_num, bool on) {
  const int cols[] = {LED_A, LED_B, LED_C};
  const int rows[] = {GND_A, GND_B, GND_C};
  int col = LED_num % LEN_multiplex;
  int row = LED_num / LEN_multiplex;
  
  if (on) {
    gpio_put(cols[col], 1);
    gpio_put(rows[row], 0);
  }
  else {
    gpio_put(cols[col], 0);
    gpio_put(rows[row], 1);
  }
  return;
} 

void charlieplex_put(int LED_num, bool on) {
  // 0, 1, 2 means LOW, HIGH, Z
  int arr[LEN_charlieplex*(LEN_charlieplex-1)][3] = { 
    {0, 1, 2}, 
    {1, 0, 2}, 
    {2, 0, 1}, 
    {2, 1, 0}, 
    {0, 2, 1}, 
    {1, 2, 0},
  };
  int inputs[LEN_charlieplex] = {LED_D, LED_E, LED_F};

  if (on) {
    for (int i=0; i<LEN_charlieplex; i++) {
      if (arr[LED_num][i] == 2) {
        gpio_set_dir(inputs[i], GPIO_IN);  // Z
      } 
      else {
        gpio_put(inputs[i], arr[LED_num][i]);
      }
    }
  }
  else {
    for (int i=0; i<LEN_charlieplex; i++) {
      if (arr[LED_num][i] == 2) {
        gpio_set_dir(inputs[i], GPIO_OUT);
        gpio_put(inputs[i], 0);
      } 
      else {
        gpio_put(inputs[i], 0);
      }
    }
  }
  return;
} 

void multiplexAllOff() {
  gpio_put(LED_A, 0);
  gpio_put(LED_B, 0);
  gpio_put(LED_C, 0);
  gpio_put(GND_A, 1);
  gpio_put(GND_B, 1);
  gpio_put(GND_C, 1);
}

void charlieplexAllOff() {
  gpio_set_dir(LED_D, GPIO_OUT);
  gpio_set_dir(LED_E, GPIO_OUT);
  gpio_set_dir(LED_F, GPIO_OUT);
  gpio_put(LED_D, 0);
  gpio_put(LED_E, 0);
  gpio_put(LED_F, 0);
}

void multiplex_wave() {
  for (int count=0; count<3; count++) {
    for (int i=0; i<LEN_multiplex*LEN_multiplex; i++) {
      multiplex_put(i, 1);
      sleep_ms(100);
      multiplex_put(i, 0);
    }
  }
  multiplexAllOff();
  sleep_ms(200);  
} 

void multiplex_all_on() {
  for (int count=0; count<30000; count++) {
    for (int i=0; i<LEN_multiplex*LEN_multiplex; i++) {
      multiplex_put(i, 1);
      multiplex_put(i, 0);
    }
  }
  multiplexAllOff();
  sleep_ms(200);  
}

void multiplex_blink_one() {
  for (int count=0; count<3; count++) {
    multiplex_put(4, 1);
    sleep_ms(300);
    multiplex_put(4, 0);
    sleep_ms(300);
  }  
}

void multiplex_loop() {
  multiplex_wave();
  multiplex_all_on();
  multiplex_blink_one();
}

void charlieplex_wave() {
    for (int count=0; count<3; count++) {
      for (int i=0; i<LEN_charlieplex*(LEN_charlieplex-1); i++) {
        charlieplex_put(i, 1);
        sleep_ms(150);
        charlieplex_put(i, 0);
      }
    }
    charlieplexAllOff();
    sleep_ms(200);
}

void charlieplex_all_on() {
  for (int count=0; count<30000; count++) {
    for (int i=0; i<LEN_charlieplex*(LEN_charlieplex-1); i++) {
      charlieplex_put(i, 1);
      charlieplex_put(i, 0);
    }
  }  
  charlieplexAllOff();
  sleep_ms(200);
}

void charlieplex_blink_one() {
  for (int count=0; count<3; count++) {
    charlieplex_put(2, 1);
    sleep_ms(300);
    charlieplex_put(2, 0);
    sleep_ms(300);
  }
}

void charlieplex_loop() {
  charlieplex_wave();
  charlieplex_all_on();
  charlieplex_blink_one();
}
