/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TM.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
volatile uint8_t tx_complete = 1;

// --- Senkronizasyon Değişkenleri ---
volatile uint32_t ms_sayaci          = 0;
volatile uint8_t senkron_mu          = 0;
volatile uint8_t kacan_paket_sayisi  = 0;
volatile uint8_t kor_ucus_bayragi    = 0;

volatile uint8_t mod_degisti_bayragi = 0;
volatile uint8_t veri_geldi_bayragi  = 0;
volatile uint16_t rx_uzunluk         = 0;

volatile int32_t aktif_mod           = YER_MOD_RX;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // RF Modülü için M0 ve M1 pinlerini 0 yaparak transparan moda al
  HAL_GPIO_WritePin(GPIOA, M0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, M1_Pin, GPIO_PIN_RESET);

  while (HAL_GPIO_ReadPin(GPIOA, AUX_Pin) == GPIO_PIN_RESET) { }
  HAL_Delay(5);

  Telemetri_Dinlemeye_Basla(); // RF'den dinle (USART1)
  PC_Dinlemeye_Basla();        // PC'den dinle (USART2)
  HAL_TIM_Base_Start_IT(&htim2); // 1 ms'lik timer'ı başlat
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* --- GÖREV 1: ZAMAN DİLİMİ KONTROLÜ (Sadece Senkron İse) --- */
      if (senkron_mu && mod_degisti_bayragi)
      {
          mod_degisti_bayragi = 0;

          if (aktif_mod == YER_MOD_TX)
          {
              // [500ms - 1000ms Aralığı]: Yer İstasyonunun konuşma sırası
              while (HAL_GPIO_ReadPin(GPIOA, AUX_Pin) == GPIO_PIN_RESET) {}

              if (yeni_komut_geldi)
              {
                  Komut_Gonder();
                  yeni_komut_geldi = 0;
              }
          }
          else if (aktif_mod == YER_MOD_RX)
          {
              // [Kör Uçuş Kurtarma veya 1050ms Timeout Sonrası RX'e dönüş]
              while (HAL_GPIO_ReadPin(GPIOA, AUX_Pin) == GPIO_PIN_RESET) {}
              Telemetri_Dinlemeye_Basla();
          }
      }

      /* --- GÖREV 2: UYDUDAN TELEMETRİ GELDİ --- */
      if (veri_geldi_bayragi)
      {
          veri_geldi_bayragi = 0;
          Telemetri_Isle(); // Veriyi struct'a al, CRC kontrolü yap ve PC'ye gönder
      }

      /* --- GÖREV 3: PC'DEN ARAYÜZ KOMUTU GELDİ --- */
      if (pc_veri_geldi_bayragi)
      {
          pc_veri_geldi_bayragi = 0;
          PC_Gelen_Veriyi_Isle(); // PC'den gelen veriyi al ve Uydu'ya gönder
      }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, M0_Pin|M1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : M0_Pin M1_Pin */
  GPIO_InitStruct.Pin = M0_Pin|M1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : AUX_Pin */
  GPIO_InitStruct.Pin = AUX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AUX_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// UART Kesmesi: Uydudan paket geldiğinde (KALP ATIŞI) veya PC'den komut geldiğinde
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1) // RF Modülünden (USART1)
    {
        rx_uzunluk = Size;
        veri_geldi_bayragi = 1;

        // --- KALP ATIŞI: UYDUYA KİLİTLEN ---
        ms_sayaci = 0;             // Zamanlayıcıyı sıfırla (0. ms)
        aktif_mod = YER_MOD_RX;    // Şu an hala RX dilimindeyiz
        senkron_mu = 1;            // Senkronizasyon sağlandı
        kacan_paket_sayisi = 0;    // Hata sayacını sıfırla
        kor_ucus_bayragi = 0;

        // Bir sonraki paket için dinlemeyi taze kur
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, RF_MAX_PAKET_BOYUTU);
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART2) // PC'den (ST-Link VCP - USART2)
    {
        pc_rx_uzunluk = Size;
        pc_veri_geldi_bayragi = 1;
        
        // PC'den yeni veri beklemek için tekrar dinlemeyi kur
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, pc_rx_buffer, RF_MAX_PAKET_BOYUTU);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}

// Timer Kesmesi: Her 1 ms'de bir çalışır
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        ms_sayaci++;

        // Yalnızca uyduya senkronize isek zaman dilimlerini işlet
        if (senkron_mu)
        {
            // 500. Milisaniye: Uydunun TX süresi bitti, sıra bizde (Yer İstasyonu TX)
            if (ms_sayaci == 500 && aktif_mod == YER_MOD_RX)
            {
                aktif_mod = YER_MOD_TX;
                mod_degisti_bayragi = 1;
            }

            // 1050. Milisaniye: (Guard Time) - Hala yeni paket gelmediyse bir şeyler ters gitmiştir
            if (ms_sayaci >= 1050)
            {
                ms_sayaci = 0;           // Zamanı başa sar
                aktif_mod = YER_MOD_RX;  // Zorla tekrar dinlemeye geç
                mod_degisti_bayragi = 1;

                kacan_paket_sayisi++;
                kor_ucus_bayragi = 1;    // Şu an ezbere (kör) dinliyoruz

                // Üst üste 3 paket kaçtıysa senkron tamamen koptu kabul et
                if (kacan_paket_sayisi > 3)
                {
                    senkron_mu = 0;
                    // senkron_mu 0 olunca Timer döngüsü durur, sistem sürekli UART dinlemesinde uydudan gelecek ilk paketi bekler.
                }
            }
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) { // RF Modülü TX bittiğinde
        tx_complete = 1;
        // Komut gönderimi biter bitmez hemen dinlemeye dönebilirsiniz
        Telemetri_Dinlemeye_Basla();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) { // RF Hata
        HAL_UART_Abort(huart);
        // Hata durumunda RX'i tekrar başlat ki senkron kopmasın
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, RF_MAX_PAKET_BOYUTU);
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART2) { // PC Hata
        HAL_UART_Abort(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, pc_rx_buffer, RF_MAX_PAKET_BOYUTU);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
