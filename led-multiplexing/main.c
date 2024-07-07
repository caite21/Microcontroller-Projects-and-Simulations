/*
Press the green play button to run the simulation --->
Try pressing the buttons!

Features:
Charlieplexing LED Circuit (top) 
  - N pins control N*(N-1) LEDs
Multiplexing LED Circuit (bottom)
  - N+M pins control N*M LEDs
Push Buttons using interrupts
  - Left controls which circuit
  - Right controls which LED pattern

Author: Caite S.
*/

#include <stdio.h>
#include "pico/stdlib.h"

#define DEBOUNCE_DELAY_MS 150
#define BTN_CIRCUIT 14
#define BTN_PATTERN 15
#define LEN_multiplex 3
#define LEN_charlieplex 3

const int column_PINS[LEN_multiplex] = {18, 19, 20};
const int row_PINS[LEN_multiplex] = {21, 22, 26};
const int charlieplex_PINS[LEN_charlieplex] = {13, 12, 11};

volatile bool isMultiplexActive = true;
volatile int patternState = 1;
volatile uint64_t circuit_btn_time_pressed = 0;
volatile uint64_t pattern_btn_time_pressed = 0;

void multiplex_put(int LED_num, bool on);
void charlieplex_put(int LED_num, bool on);
void handle_button_press(uint /*gpio*/, uint32_t /*event_mask*/);
void switch_circuit();
void next_pattern();
void multiplex_blink_one();
void multiplex_wave();
void multiplex_all_on();
void charlieplex_blink_one();
void charlieplex_wave();
void charlieplex_all_on();



int main() {
  stdio_init_all();
  // init pins in LED circuits
  for (int i=0; i<LEN_multiplex; i++) {
    gpio_init(column_PINS[i]);
    gpio_set_dir(column_PINS[i], GPIO_OUT);
    gpio_put(column_PINS[i], 0);
  }
  for (int i=0; i<LEN_multiplex; i++) {
    gpio_init(row_PINS[i]);
    gpio_set_dir(row_PINS[i], GPIO_OUT);
    gpio_put(row_PINS[i], 1);
  }
  for (int i=0; i<LEN_charlieplex; i++) {
    gpio_init(charlieplex_PINS[i]);
    gpio_set_dir(charlieplex_PINS[i], GPIO_OUT);
    gpio_put(charlieplex_PINS[i], 0);
  }

  // push buttons
  gpio_init(BTN_CIRCUIT);
  gpio_init(BTN_PATTERN);
  gpio_set_dir(BTN_CIRCUIT, GPIO_IN);
  gpio_set_dir(BTN_PATTERN, GPIO_IN);
  gpio_pull_up(BTN_CIRCUIT);
  gpio_pull_up(BTN_PATTERN);

  // interrupt on button press
  gpio_set_irq_enabled_with_callback(BTN_CIRCUIT, GPIO_IRQ_EDGE_FALL, true, &handle_button_press);
  gpio_set_irq_enabled_with_callback(BTN_PATTERN, GPIO_IRQ_EDGE_FALL, true, &handle_button_press);
  
  void (*multiplex_patterns[3])() = {multiplex_blink_one, multiplex_wave, multiplex_all_on};
  void (*charlieplex_patterns[3])() = {charlieplex_blink_one, charlieplex_wave, charlieplex_all_on};

  while (true) {
    if (isMultiplexActive) {
      (*multiplex_patterns[patternState - 1])();
    } else {
      (*charlieplex_patterns[patternState - 1])();
    }
  }
}



void handle_button_press(uint /*gpio*/, uint32_t /*event_mask*/) {
  uint64_t current_time = time_us_64();

  if (gpio_get(BTN_PATTERN) == 0) {
    // debounce
    if (current_time - pattern_btn_time_pressed > (DEBOUNCE_DELAY_MS * 1000)) {
      pattern_btn_time_pressed = current_time;  
      next_pattern();
    }
  }
  if (gpio_get(BTN_CIRCUIT) == 0) {
    // debounce
    if (current_time - circuit_btn_time_pressed > (DEBOUNCE_DELAY_MS * 1000)) {
      circuit_btn_time_pressed = current_time; 
      switch_circuit();
    }
  }
}

void switch_circuit() {
  isMultiplexActive = !isMultiplexActive;
}

void next_pattern() {
  if (patternState >= 3) {
    patternState = 1;
  } else {
    patternState++;
  }
}

void multiplex_put(int LED_num, bool on) {
  int col = LED_num % LEN_multiplex;
  int row = LED_num / LEN_multiplex;
  const int column_PINS[LEN_multiplex] = {18, 19, 20};
  const int row_PINS[LEN_multiplex] = {21, 22, 26};
  
  if (on) {
    gpio_put(column_PINS[col], 1);
    gpio_put(row_PINS[row], 0);
  }
  else {
    gpio_put(column_PINS[col], 0);
    gpio_put(row_PINS[row], 1);
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

  if (on) {
    for (int i=0; i<LEN_charlieplex; i++) {
      if (arr[LED_num][i] == 2) {
        gpio_set_dir(charlieplex_PINS[i], GPIO_IN);  // Z
      } 
      else {
        gpio_put(charlieplex_PINS[i], arr[LED_num][i]);
      }
    }
  }
  else {
    for (int i=0; i<LEN_charlieplex; i++) {
      if (arr[LED_num][i] == 2) {
        gpio_set_dir(charlieplex_PINS[i], GPIO_OUT);
        gpio_put(charlieplex_PINS[i], 0);
      } 
      else {
        gpio_put(charlieplex_PINS[i], 0);
      }
    }
  }
  return;
} 

void multiplex_wave() {
  for (int i=0; i<LEN_multiplex*LEN_multiplex; i++) {
    multiplex_put(i, 1);
    sleep_ms(100);
    multiplex_put(i, 0);
  } 
} 

void multiplex_all_on() {
  for (int i=0; i<LEN_multiplex*LEN_multiplex; i++) {
    multiplex_put(i, 1);
    multiplex_put(i, 0);
  }  
}

void multiplex_blink_one() {
  multiplex_put(4, 1);
  sleep_ms(200);
  multiplex_put(4, 0);
  sleep_ms(200); 
}

void charlieplex_wave() {
  for (int i=0; i<LEN_charlieplex*(LEN_charlieplex-1); i++) {
    charlieplex_put(i, 1);
    sleep_ms(100);
    charlieplex_put(i, 0);
  }
}

void charlieplex_all_on() {
  for (int i=0; i<LEN_charlieplex*(LEN_charlieplex-1); i++) {
    charlieplex_put(i, 1);
    charlieplex_put(i, 0);
  }
}

void charlieplex_blink_one() {
  charlieplex_put(2, 1);
  sleep_ms(200);
  charlieplex_put(2, 0);
  sleep_ms(200);
}
