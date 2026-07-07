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
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


//MODEL UYDU TEKNOFEST 2026 FİZİK MOTORU İLE SİMÜLASYON KODU 2

// --- PAKET SENKRONİZASYON VE BİTMASK MAKROLARI ---
#define PAKET_HEADER 0xAA55U

// Durum Bayrakları Maskeleri (Bit 0 - 3)
#define BAYRAK_APAM  (1U << 0) // Bit 0
#define BAYRAK_SEP   (1U << 1) // Bit 1: Ayrılma
#define BAYRAK_SGM   (1U << 2) // Bit 2: SİGMA Kanatları
#define BAYRAK_BUZZ  (1U << 3) // Bit 3: Buzzer
//bayrakları kaldırmak için durumbayraklari kısmı ile veyalama işlemi yapacağız.
//bayrağı indirmek için ise bayrağın değilini alıp veleyeceğiz.

// Hata Kodu Maskeleri (Bit 4 - 7)
#define HATA_NACK    (0x0FU << 4) // Paket bozuk/eksik hata kodu

typedef struct __attribute__((packed)){
    uint16_t header;           // 2 Bayt: Sabit Senkronizasyon Başlığı (0xAA55)
    uint32_t index;            // 4 Bayt: Paket Sırası
    uint16_t mi1;              // 2 Bayt: Motor İtkisi 1 (Pluto 50A BLHeli)
    uint16_t mi2;              // 2 Bayt: Motor İtkisi 2
    uint16_t mi3;              // 2 Bayt: Motor İtkisi 3
    uint16_t mi4;              // 2 Bayt: Motor İtkisi 4
    uint8_t  durum_bayraklari; // 1 Bayt: Evrensel Bitmask (APAM, SEP, SGM, BUZZ ve Hatalar)
    uint8_t  checksum;         // 1 Bayt: XOR Checksum Doğrulama Baydı
} Iletim_Paketi_t;             // TOPLAM: 16 Bayt (Tam 32-bit hizalı)

typedef union {
    Iletim_Paketi_t paket;
    uint8_t buffer[sizeof(Iletim_Paketi_t)];
} Iletim_Veri_t;

typedef struct __attribute__((packed)) {
    uint16_t header;      // 2 Bayt: Sabit Senkronizasyon Başlığı (0xAA55)
    uint32_t index;       // 4 Bayt: Paket sırası (0 - 4.2 Milyar)

    // IMU Verileri (MPU9250 - 16-bit)
    int16_t acc_x, acc_y, acc_z;    // 6 Bayt (İvme: mg cinsinden. 1g = 1000)
    int16_t gyro_x, gyro_y, gyro_z; // 6 Bayt (Jiroskop: Derece/sn x 10)
    int16_t mag_x, mag_y, mag_z;    // 6 Bayt (Manyetometre: mili-Gauss ham veri)

    // Çevre Sensörleri (BMP280)
    uint32_t basinc;      // 4 Bayt: Pascal cinsinden ham veri (Örn: 101325 Pa)
    int16_t sicaklik;     // 2 Bayt: Santigrat x 100 (Örn: 25.43 C -> 2543)

    // GPS Verileri (BN-880 / Ublox M8N)
    int32_t gps_lat;      // 4 Bayt: Latitude x 10^7
    int32_t gps_lon;      // 4 Bayt: Longitude x 10^7
    int32_t gps_alt;      // 4 Bayt: Yükseklik (Milimetre)
    int16_t gps_vel;      // 2 Bayt: Dikey Hız (cm/s)

    // Güç Sistemi (INA219)
    uint16_t bat_v;       // 2 Bayt: Gerilim (Milivolt)
	int32_t  bat_a;       // 4 Bayt: Akım (Miliamper)
	uint8_t  rezerve;     // 1 Bayt: 52 bayta 4-byte align tamamlayan dolgu
	uint8_t  checksum;    // 1 Bayt: XOR Checksum (EN SONDA)

    } Alim_Paketi_t;          // TOPLAM: 52 Bayt (13 Word)

typedef union {
    Alim_Paketi_t paket;
    uint8_t buffer[sizeof(Alim_Paketi_t)];
} Alim_Veri_t;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define TIME_STEP 0.0025f
// Sabitler için tanımlama (Makro kullanmak performansı artırır)
#define PERIOD_400HZ (1.0f / 400.0f) // 0.0025f
#define PERIOD_100HZ (1.0f / 100.0f) // 0.01f
#define PERIOD_10HZ  (1.0f / 10.0f)  // 0.1f
#define PERIOD_1HZ   (1.0f / 1.0f)   // 1.0f

// UART Zaman Aşımı Koruması (Deadlock Watchdog) Süresi
#define UART_TIMEOUT_MS 5U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

uint32_t previndex=0;
float zaman=0;
float dt=0.0025f;

Alim_Veri_t alinacak;                   // RX Tamponumuz
Iletim_Veri_t ayarlanacak;
Iletim_Veri_t iletilecek;

volatile uint8_t paket_hazir_bayragi = 0; // 1: Kusursuz paket geldi, işlenmeyi bekliyor
volatile uint8_t hata_bayragi = 0;        // 1: Paket eksik/bozuk geldi, NACK gönder

// Çözüm 1: Timeout (Bekçi Köpeği) Değişkenleri
uint32_t paket_baslangic_zamani = 0;
uint8_t  alim_devam_ediyor = 0;

//Godotta yazılan fizik motorundan veri gelmedikçe sistem zamanı ilerlemeyecek.
	  //Stm32den de fizik motoruna veri gitmedikçe fizik motorunda zaman ilerlemeyecek
	  //(Ping pong Senkron Yapısı)

///////////////////////////////////UÇU? YAZILIMI DE?İ?KENLERİ//////////////////////////////

uint8_t yukari=1;

//////////////////////////////////////////////////////////////////////////////////////////


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

// XOR Checksum Hesaplayıcı (Son bayt hariç tüm baytları XOR'lar)
uint8_t Checksum_Hesapla(const uint8_t *veri, uint16_t uzunluk)
{
    uint8_t chk = 0;
    for (uint16_t i = 0; i < uzunluk; i++) {
        chk ^= veri[i];
    }
    return chk;
}


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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Çözüm 1: USB'nin paketi bölmesine toleranslı Standart DMA başlatılıyor
  HAL_UART_Receive_DMA(&huart2, alinacak.buffer, sizeof(Alim_Paketi_t));

  memset(&ayarlanacak, 0, sizeof(Iletim_Veri_t));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

        // ===============================================================
        // YAZILIM BEKÇİ KÖPE?İ (TIMEOUT WATCHDOG - DEADLOCK KORUMASI)
        // ===============================================================
        uint16_t kalan_bayt = __HAL_DMA_GET_COUNTER(huart2.hdmarx);

        // Eğer 52 bayttan az kalmışsa (yani paket akışı başlamış ama henüz bitmemişse)
        if (kalan_bayt < sizeof(Alim_Paketi_t) && kalan_bayt > 0)
        {
            if (alim_devam_ediyor == 0) {
                paket_baslangic_zamani = HAL_GetTick(); // İlk baytın geldiği milisaniyeyi kaydet
                alim_devam_ediyor = 1;
            }
            // Eğer verinin gelmeye başlamasının üzerinden belirlediğimiz süre (5ms) geçtiyse: DEADLOCK!
            else if ((HAL_GetTick() - paket_baslangic_zamani) > UART_TIMEOUT_MS)
            {
                HAL_UART_AbortReceive(&huart2); // Kilitlenmeyi kopar
                memset(alinacak.buffer, 0, sizeof(Alim_Paketi_t));
                alim_devam_ediyor = 0;
                hata_bayragi = 1; // NACK gönderilsin ve Godot uyarılın

                // Hattı yeni ve tam bir paket için sıfırdan kur
                HAL_UART_Receive_DMA(&huart2, alinacak.buffer, sizeof(Alim_Paketi_t));
            }
        }
        else if (kalan_bayt == sizeof(Alim_Paketi_t)) {
            alim_devam_ediyor = 0; // Hat boştaysa veya paket henüz başlamadıysa bayrağı sıfır tut
        }

        // ===============================================================
        // DURUM 1: KUSURSUZ VE TAM (52 BAYTLIK) PAKET ALINDI
        // ===============================================================



    	if (paket_hazir_bayragi == 1)
		  {
			  paket_hazir_bayragi = 0;
			  alim_devam_ediyor = 0; // İşlem başarıyla bittiği için zaman aşımı takibini durdur

			  // Simülasyon Başa Sarma (Restart) Kalkanı
			  if (alinacak.paket.index < previndex) {
				  previndex = alinacak.paket.index;
			  }

			  zaman = (float)(alinacak.paket.index) * TIME_STEP;
			  dt = (float)(alinacak.paket.index - previndex) * TIME_STEP;

			  // ==========================================
			  // GÖREV ZAMANLAYICI (SCHEDULER)
			  // ==========================================
			  static uint32_t task_400Hz_ticks = 0;
			  static uint32_t task_100Hz_ticks = 0;
			  static uint32_t task_10Hz_ticks  = 0;
			  static uint32_t task_1Hz_ticks   = 0;

			  uint32_t delta_ticks = alinacak.paket.index - previndex;

			  memset(&iletilecek, 0, sizeof(Iletim_Veri_t));

			  task_400Hz_ticks += delta_ticks;
			  task_100Hz_ticks += delta_ticks;
			  task_10Hz_ticks  += delta_ticks;
			  task_1Hz_ticks   += delta_ticks;

			  if (task_400Hz_ticks >= 1) {
				  task_400Hz_ticks = 0;
				  // 400 Hz Algoritmaları...
			  }
			  if (task_100Hz_ticks >= 4) {
				  task_100Hz_ticks %= 4;
				  // 100 Hz Algoritmaları...
			  }
			  if (task_10Hz_ticks >= 40) {
				  task_10Hz_ticks %= 40;
				  // 10 Hz Algoritmaları...
			  }
			  if (task_1Hz_ticks >= 400) {
				  task_1Hz_ticks %= 400;
				  // 1 Hz Algoritmaları...
				  if(yukari==1){
					  ayarlanacak.paket.mi1 = ayarlanacak.paket.mi2 = ayarlanacak.paket.mi3 = ayarlanacak.paket.mi4 = 20000;
				  }
				  else if (yukari==0){
					  ayarlanacak.paket.mi1 = ayarlanacak.paket.mi2 = ayarlanacak.paket.mi3 = ayarlanacak.paket.mi4 = 0;
				  }
				  yukari ^= (1U);

			  }

			  // ==========================================
			  // GODOT'A YANIT PAKETİ HAZIRLA VE GÖNDER (ACK)
			  // ==========================================
			  ayarlanacak.paket.header = PAKET_HEADER;
			  ayarlanacak.paket.index  = alinacak.paket.index;
			  // Checksum Hesapla (Son bayt olan checksum alanını hesaba katmadan)
			  // --- Bitmask İle Durum Bayraklarının Ayarlanması ---
						  // Örnek: SEP (Ayrılma) ve BUZZ (Buzzer) aktif edelim
						  //ayarlanacak.paket.durum_bayraklari |= BAYRAK_SEP;
						  //ayarlanacak.paket.durum_bayraklari |= BAYRAK_BUZZ;
						  // Örnek: SGM (Sigma) pasif edelim
						  //ayarlanacak.paket.durum_bayraklari &= ~BAYRAK_SGM;
			  ayarlanacak.paket.checksum = Checksum_Hesapla(ayarlanacak.buffer, sizeof(Iletim_Paketi_t) - 1);
			  iletilecek = ayarlanacak;

			  HAL_UART_Transmit(&huart2, iletilecek.buffer, sizeof(Iletim_Paketi_t), 50);
			  previndex = alinacak.paket.index;
		  }

		  // ===============================================================
		  // DURUM 2: EKSİK / BOZUK / CHECKSUM HATALI PAKET ALARMI (NACK)
		  // ===============================================================
		  if (hata_bayragi == 1)
		  {
			  hata_bayragi = 0;

			  Iletim_Veri_t alarm_paketi;
			  memset(&alarm_paketi, 0, sizeof(Iletim_Veri_t));

			  alarm_paketi.paket.header = PAKET_HEADER;
			  alarm_paketi.paket.index  = previndex; // En son başarılı indeks

			  // Hata olduğunu Godot'a bildirmek için HATA_NACK maskesini yükle
			  alarm_paketi.paket.durum_bayraklari |= HATA_NACK;

			  alarm_paketi.paket.checksum = Checksum_Hesapla(alarm_paketi.buffer, sizeof(Iletim_Paketi_t) - 1);

			  HAL_UART_Transmit(&huart2, alarm_paketi.buffer, sizeof(Iletim_Paketi_t), 50);
		  }
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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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
  huart2.Init.BaudRate = 921600;
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
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Çözüm 1: Sadece tam 52 bayt dolduğunda tetiklenen Standart DMA Tamamlama Kesmesi
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        alim_devam_ediyor = 0; // Tam paket geldi, timeout takibini sıfırla

        // 1. Başlık (Header) Kontrolü (0xAA55 mi?)
        if (alinacak.paket.header == PAKET_HEADER)
        {
            // 2. Checksum Doğrulama Kontrolü
            uint8_t hesaplanan_chk = Checksum_Hesapla(alinacak.buffer, offsetof(Alim_Paketi_t, checksum));

            if (hesaplanan_chk == alinacak.paket.checksum)
            {
                paket_hazir_bayragi = 1; // Her şey kusursuz, ana döngüyü tetikle
            }
            else
            {
                hata_bayragi = 1; // Checksum hatalı
            }
        }
        else
        {
            hata_bayragi = 1; // Header hatalı
        }

        // Bir sonraki 52 baytlık paketi yakalamak için DMA'yı yeniden kur
        HAL_UART_Receive_DMA(&huart2, alinacak.buffer, sizeof(Alim_Paketi_t));
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
  * where the assert_param error has occurred.
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
