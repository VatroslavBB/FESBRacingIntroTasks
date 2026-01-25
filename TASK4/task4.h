#include "stm32c0xx_hal.h"
#include <stdio.h>

//Inicijalizacija svih periferija, ADC-a i SysClk za TASK4

#define POT   GPIO_PIN_0
#define LED_2 GPIO_PIN_2
#define LED_3 GPIO_PIN_3
#define LED_1 GPIO_PIN_4
#define PB    GPIO_PIN_13
#define PORT  GPIOB

ADC_HandleTypeDef adc;

void ErrorHandler();
void SystemClock();
static void Pot_Init();
static void ADC_Init();
uint16_t ReadPollADC();
static void Digital_Init();


void ErrorHandler() {
  printf("\nINIT ERROR\n");
  while (1) {
  }
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

static void Pot_Init() {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef potSetup = {0};

  potSetup.Pin = POT;
  potSetup.Pull = GPIO_NOPULL;
  potSetup.Mode = GPIO_MODE_ANALOG;

  HAL_GPIO_Init(GPIOA, &potSetup);
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

  ADC_ChannelConfTypeDef sConfig = {0};
  
  sConfig.Channel       = ADC_CHANNEL_0;
  sConfig.Rank          = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime  = ADC_SAMPLETIME_79CYCLES_5;

  if (HAL_ADC_ConfigChannel(&adc, &sConfig) != HAL_OK)
    ErrorHandler();
}

uint16_t ReadPollADC(){
  HAL_ADC_Start(&adc);
  HAL_ADC_PollForConversion(&adc, 10);
  uint16_t value = HAL_ADC_GetValue(&adc);
  HAL_ADC_Stop(&adc);
  return value * 100 / 4095;
}

static void Digital_Init(){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef periphSetup = {0};

  periphSetup.Pin = LED_1 | LED_2;
  periphSetup.Pull = GPIO_NOPULL;
  periphSetup.Mode = GPIO_MODE_OUTPUT_PP;

  HAL_GPIO_Init(PORT, &periphSetup);

  periphSetup.Pin = PB;
  periphSetup.Pull = GPIO_PULLUP;
  periphSetup.Mode = GPIO_MODE_INPUT;

  HAL_GPIO_Init(PORT, &periphSetup);
}

