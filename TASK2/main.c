#include "stm32c0xx_hal.h"

#define LED_PIN GPIO_PIN_1
#define POT_PIN GPIO_PIN_2

#define PORT GPIOA


typedef struct{
  int id;
  int value;
  int status;
}Message;

static void InitializeLED();
static void InitializePot();
void InitializeADC();
int ReadPot();
void Transmitter(int*, Message*, int*);
void Receiver(int*, Message);

ADC_HandleTypeDef adc;


int main(){

  HAL_Init();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  InitializeLED();
  InitializePot();
  InitializeADC();

  int startTimeR = HAL_GetTick();
  int startTimeT = HAL_GetTick();
  Message buffer[100] = {0};
  int startRead = 0;
  int prevRead = 0;

  while(1){
    
    Transmitter(&startTimeT, buffer, &startRead);

    Receiver(&startTimeR, buffer[prevRead]);
    prevRead = startRead;
    
    
    /*Testiranje mirenja vrimena
    if(HAL_GetTick() - start_time >= 1000 && state == 0){
      HAL_GPIO_WritePin(PORT, LED_PIN, GPIO_PIN_SET);
      state = 1;
      start_time = HAL_GetTick();
    }
    if(HAL_GetTick() - start_time >= 1000 && state == 1){
      HAL_GPIO_WritePin(PORT, LED_PIN, GPIO_PIN_RESET);
      state = 0;
      start_time = HAL_GetTick();
    }
    */
  }
  return 0;
}

void Transmitter(int *t, Message buff[], int *start){
  if(HAL_GetTick() - *t >= 500){
    Message msg = {0};
    msg.id = 1;
    msg.value = ReadPot();
    if(HAL_GPIO_ReadPin(PORT, LED_PIN) == GPIO_PIN_SET){
      msg.status = 1;
    }
    else{
      msg.status = 0;
    }
    buff[*start] = msg;
    *t = HAL_GetTick();
    if(*start == 99){
      *start = 0;
    }
    else{
      (*start)++;
    }
  }
  return ;
}

void Receiver(int *t, Message msg){
  if(msg.id == 1){
    if(msg.value > 2000){
      printf("High ADC value from Node 1!\n");
      HAL_GPIO_WritePin(PORT, LED_PIN, GPIO_PIN_SET);
    }
    else{
      HAL_GPIO_WritePin(PORT, LED_PIN, GPIO_PIN_RESET);
      printf("ID: %d\n", msg.id);
      printf("ADC value: %d\n", msg.value);
      printf("LED status: %d\n\n", msg.status);
    }
    if(HAL_GetTick() - *t >= 1000){
      printf("ACK sent\n");
      *t = HAL_GetTick();
    }
    msg.id = -1;
  }
  return ;
}

static void InitializeLED(){
  GPIO_InitTypeDef led = {0};

  led.Pin = LED_PIN;
  led.Mode = GPIO_MODE_OUTPUT_PP;
  led.Pull = GPIO_NOPULL;
  led.Speed = GPIO_SPEED_FREQ_LOW;  

  
  HAL_GPIO_Init(PORT, &led);
}

static void InitializePot(){
  GPIO_InitTypeDef pot = {0};

  pot.Pin = POT_PIN;
  pot.Mode = GPIO_MODE_ANALOG;
  pot.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PORT, &pot);
}

void InitializeADC()
{
    ADC_ChannelConfTypeDef sConfig = {0};

    adc.Instance = ADC1;
    adc.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
    adc.Init.Resolution            = ADC_RESOLUTION_12B;
    adc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adc.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    adc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    adc.Init.LowPowerAutoWait      = DISABLE;
    adc.Init.ContinuousConvMode    = DISABLE;
    adc.Init.NbrOfConversion       = 1;
    adc.Init.DiscontinuousConvMode = DISABLE;
    adc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    HAL_ADC_Init(&adc);

    sConfig.Channel      = ADC_CHANNEL_2;
    HAL_ADC_ConfigChannel(&adc, &sConfig);
}

int ReadPot()
{
    HAL_ADC_Start(&adc);
    HAL_ADC_PollForConversion(&adc, 10);
    int value = HAL_ADC_GetValue(&adc);
    HAL_ADC_Stop(&adc);
    return value;
}




