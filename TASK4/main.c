#include "task4.h"
#include <math.h>

int DebouncePB();
int showMode(uint8_t, uint16_t);

int main(){
  HAL_Init();
  SystemClock();
  Pot_Init();
  Digital_Init();
  ADC_Init();

  uint32_t timer = 0;
  uint8_t mappingMode = 0;
  int output;
  int input;

  while(1){
    mappingMode = (mappingMode + DebouncePB()) % 4;
    input = ReadPollADC();
    output = showMode(mappingMode, input);
    if(HAL_GetTick() - timer > 500){
      printf("\nPedal: %d%%\t", input);
      if(mappingMode == 0){
        printf("|\tMode: Linear\t|\t");
      }
      else if(mappingMode == 1){
        printf("|\tMode: Eco\t|\t");
      }else if(mappingMode == 2){
        printf("|\tMode: Smooth\t|\t");
      }
      else if(mappingMode == 3){
        printf("|\tMode: Sport\t|\t");
      }
      printf("Ouput: %d%%\n", output);
      timer = HAL_GetTick();
    }
  }
}

int DebouncePB(){static uint8_t lastState = GPIO_PIN_SET;
  static uint8_t stableState = GPIO_PIN_SET;
  static uint32_t lastChangeTime = 0;

  uint8_t current = HAL_GPIO_ReadPin(PORT, PB);

  if (current != lastState)
  {
    lastChangeTime = HAL_GetTick();
    lastState = current;
  }

  if ((HAL_GetTick() - lastChangeTime) > 20)
  {
    if (stableState != current)
    {
      stableState = current;
      if (stableState == GPIO_PIN_RESET)
        return 1;
    }
  }
  return 0;
}

int showMode(uint8_t mode, uint16_t input){
  float helper = (float)input;

  //matrica kodnih rjeci za prikaz modova
  //0 - linearno
  //1 - eco
  //2 - smooth
  //3 - sport
  uint8_t binaryMatrix[4][2] = {
    {GPIO_PIN_RESET, GPIO_PIN_RESET},
    {GPIO_PIN_RESET, GPIO_PIN_SET},
    {GPIO_PIN_SET, GPIO_PIN_RESET},
    {GPIO_PIN_SET, GPIO_PIN_SET}
  };

  //postavljanje led dioda na vrijednosti bitova u rednom broju moda mapiranja
  HAL_GPIO_WritePin(PORT, LED_1, binaryMatrix[mode][0]);
  HAL_GPIO_WritePin(PORT, LED_2, binaryMatrix[mode][1]);

  if(mode == 0){
    //linearno mapiranje
    helper = helper;
  }
  else if(mode == 1){
    //nelinearno mapiranje - eco
    helper = pow(100, -0.8) * pow(helper, 1.8);
  }
  else if(mode == 2){
    //nelinearno mapiranje - smooth
    helper = 100 * pow(helper, 2) / (pow(helper, 2) + pow(100 - helper, 2));
  }
  else if(mode == 3){
    //nelinearno mapiranje - sport
    helper = pow(100, 0.666) * pow(helper, 0.333);
  }
  return (int)helper;
}


