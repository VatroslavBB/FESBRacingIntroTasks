#include "stm32c0xx_hal.h"
#include <stdio.h>

#define POT1 GPIO_PIN_0
#define POT2 GPIO_PIN_1
#define PORT GPIOA
#define LED_PORT GPIOB
#define LED GPIO_PIN_1

ADC_HandleTypeDef adc;

static void Pot_Init();
static void ADC_Init();
void ErrorHandler();
void ErrorHandlerImpl();
void SystemClock();
uint16_t ReadPollADC();
void ChangeChannel(int);
double GetDiff(int, int);
static void Led_Init();

int main() {
  HAL_Init();
  SystemClock();
  Pot_Init();
  Led_Init();
  ADC_Init();

  int val1, val2;
  double difference;
  int timerImpl;
  int startedImpl = 0;
  int timer = 0;

  while (1){
    if(HAL_GetTick() - timer >= 1000){
      timer = HAL_GetTick();
    }

    //ocitavanje vrijednosti i konstantno mjenjanje kanala za ADC
    ChangeChannel(0);
    val1 = ReadPollADC();
    ChangeChannel(1);
    val2 = ReadPollADC();

    //razlika u vrijednostima dvaju potenciometara
    difference = GetDiff(val1, val2);

    if(startedImpl == 1 && HAL_GetTick() - timerImpl > 3000){
      ErrorHandlerImpl();
    }

    //detekcija implauzibilnosti
    if(difference > 10){
      if(HAL_GetTick() - timer >= 1000){
        printf("\tWARNING difference greater than 10 percent\n");
      }
      if(startedImpl == 0){
        timerImpl = HAL_GetTick();
      }
      startedImpl = 1;
    }
    else{
      startedImpl = 0;
    }
  }
  return 0;
}

double GetDiff(int val1, int val2){
  return ((double)(abs(val1 - val2)) / 4095) * 100;
}

void ChangeChannel(int ch){
  ADC_ChannelConfTypeDef sConfig = {0};

  if(ch == 0){
    sConfig.Channel = ADC_CHANNEL_0;
  }
  else{
    sConfig.Channel = ADC_CHANNEL_1;
  }
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_79CYCLES_5;

  if (HAL_ADC_ConfigChannel(&adc, &sConfig) != HAL_OK)
    ErrorHandler();

}

uint16_t ReadPollADC(){
  HAL_ADC_Start(&adc);
  HAL_ADC_PollForConversion(&adc, 10);
  uint16_t value = HAL_ADC_GetValue(&adc);
  HAL_ADC_Stop(&adc);
  return value;
}

void SystemClock() {
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSIDiv = RCC_HSI_DIV1;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    if(HAL_RCC_OscConfig(&osc) != HAL_OK){
      ErrorHandler();
    }

    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_1) != HAL_OK){
      ErrorHandler();
    }
}

void ErrorHandler() {
  printf("konfiguracija kriva...\n");
  while (1) {
    //obicni config error loop
  }
}

void ErrorHandlerImpl(){
  printf("\tABORT\n");
  //kada implauzibilnost traje duze od 3000ms pali se led dioda
  HAL_GPIO_WritePin(LED_PORT, LED, GPIO_PIN_SET);
  while(1){
  }
}

static void Pot_Init() {
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef potSetup = {0};

  potSetup.Pin = POT1 | POT2;
  potSetup.Pull = GPIO_NOPULL;
  potSetup.Mode = GPIO_MODE_ANALOG;

  HAL_GPIO_Init(PORT, &potSetup);
}

static void Led_Init(){
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef ledSetup = {0};

  ledSetup.Pin = LED;
  ledSetup.Pull = GPIO_NOPULL;
  ledSetup.Mode = GPIO_MODE_OUTPUT_PP;

  HAL_GPIO_Init(LED_PORT, &ledSetup);
}

static void ADC_Init() {

    __HAL_RCC_ADC_CLK_ENABLE();
    
    adc.Instance = ADC1;
    adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    adc.Init.Resolution = ADC_RESOLUTION_12B;
    adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc.Init.LowPowerAutoWait = DISABLE;
    adc.Init.ContinuousConvMode = DISABLE;
    adc.Init.NbrOfConversion = 1;
    adc.Init.DiscontinuousConvMode = DISABLE;
    adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc.Init.Overrun = ADC_OVR_DATA_PRESERVED;

    if(HAL_ADC_Init(&adc) != HAL_OK){
      ErrorHandler();
    }
}