#include "stm32c0xx_hal.h"
#include <math.h>

// Define LED pin and port
#define LED_PIN_0   GPIO_PIN_5
#define LED_PIN_1   GPIO_PIN_6
#define LED_PIN_2   GPIO_PIN_7
#define LED_PIN_3   GPIO_PIN_9

#define LED_PORT    GPIOA

#define N_BITS      4

static void MX_GPIO_Init(void);
int ToBinaryArray(int, int*);
int CreateBinaryMatrix(int M[][N_BITS]);
int WriteBit(int*, int, int);


int main(void)
{
  HAL_Init();

  MX_GPIO_Init();

  int num_of_code_words = pow(2, N_BITS);
  int code_words[num_of_code_words][N_BITS];
  CreateBinaryMatrix(code_words);

  int index = 0;
  int led_pins[N_BITS] = {LED_PIN_3, LED_PIN_2, LED_PIN_1, LED_PIN_0};

  while (1)
  {
    for(int i = 0; i < N_BITS; i++){
      WriteBit(led_pins, code_words[index][i], i);
    }
    HAL_Delay(500);
    index = (index + 1) % num_of_code_words;
  }

  return 0;
}

int WriteBit(int led_pins[], int state, int index){
  switch(state){
    case 0:
      HAL_GPIO_WritePin(LED_PORT, led_pins[index], GPIO_PIN_RESET);
      return 1;
    case 1:
      HAL_GPIO_WritePin(LED_PORT, led_pins[index], GPIO_PIN_SET);
      return 1;
    default:
      return 0;
  }
}

int ToBinaryArray(int number, int code_word[]){
    if(code_word == NULL || number < 0){
        return 0;
    }
    int index = 0;
    while(number){
        code_word[N_BITS - 1 - index] = number % 2;
        number /= 2;
        index++;
    }
    return 1;
}

int CreateBinaryMatrix(int M[][N_BITS]){

  int length = pow(2, N_BITS);
  for(int i = 0; i < length; i++){
    for(int j = 0; j < N_BITS; j++){
      M[i][j] = 0;
    }
    ToBinaryArray(i, M[i]);
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct_0 = {0};
  GPIO_InitTypeDef GPIO_InitStruct_1 = {0};
  GPIO_InitTypeDef GPIO_InitStruct_2 = {0};
  GPIO_InitTypeDef GPIO_InitStruct_3 = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct_0.Pin = LED_PIN_0;
  GPIO_InitStruct_1.Pin = LED_PIN_1;
  GPIO_InitStruct_2.Pin = LED_PIN_2;
  GPIO_InitStruct_3.Pin = LED_PIN_3;

  GPIO_InitStruct_0.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_1.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_2.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_3.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct_0.Pull = GPIO_NOPULL;
  GPIO_InitStruct_1.Pull = GPIO_NOPULL;
  GPIO_InitStruct_2.Pull = GPIO_NOPULL;
  GPIO_InitStruct_3.Pull = GPIO_NOPULL;

  GPIO_InitStruct_0.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct_1.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct_2.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct_3.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct_0);
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct_1);
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct_2);
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct_3);
}
