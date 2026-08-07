#include "M2.1_CalibrationUI.h"

// STM32F407 Discovery LED Pin Tanimlamalari
#define LED_GREEN_PIN    GPIO_PIN_12
#define LED_ORANGE_PIN   GPIO_PIN_13
#define LED_RED_PIN      GPIO_PIN_14
#define LED_BLUE_PIN     GPIO_PIN_15
#define LED_PORT         GPIOD

void CalibrationUI_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. GPIOD Clock Aktif Et
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // 2. LED Pinlerini Cikis (Output) Olarak Ayarla
    GPIO_InitStruct.Pin = LED_GREEN_PIN | LED_ORANGE_PIN | LED_RED_PIN | LED_BLUE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Push Pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // Baslangicta hepsini sondur
    CalibrationUI_SetState(UI_STATE_IDLE);
}

void CalibrationUI_SetState(Calibration_UI_State_e state) {
    // Once hepsini sondur
    HAL_GPIO_WritePin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_ORANGE_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);

    switch (state) {
        case UI_STATE_IDLE:
            // Tumu sonuk kalir
            break;
            
        case UI_STATE_TRANSITION:
            // Hazirlik / Gecis -> Turuncu
            HAL_GPIO_WritePin(LED_PORT, LED_ORANGE_PIN, GPIO_PIN_SET);
            break;
            
        case UI_STATE_MEASURING:
            // Olcum -> Kirmizi
            HAL_GPIO_WritePin(LED_PORT, LED_RED_PIN, GPIO_PIN_SET);
            break;
            
        case UI_STATE_FINISHED:
            // Bitti -> Yesil
            HAL_GPIO_WritePin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
            break;
    }
}

void CalibrationUI_PrintInstruction(uint8_t current_step) {
    // printf fonksiyonunun SWV veya UART uzerinden calistigi varsayilir.
    switch (current_step) {
        case 0:
            printf("\r\n[ADIM 0/6] Lutfen uydunuzu TERS Z (Tepesi Asagi) pozisyonuna alin ve 10sn sabit bekleyin...\r\n");
            break;
        case 1:
            printf("\r\n[ADIM 1/6] Lutfen uydunuzu DUZ Z (Masaustu normal) pozisyonuna alin ve 10sn sabit bekleyin...\r\n");
            break;
        case 2:
            printf("\r\n[ADIM 2/6] Lutfen uydunuzu TERS Y pozisyonuna alin ve 10sn sabit bekleyin...\r\n");
            break;
        case 3:
            printf("\r\n[ADIM 3/6] Lutfen uydunuzu DUZ Y pozisyonuna alin ve 10sn sabit bekleyin...\r\n");
            break;
        case 4:
            printf("\r\n[ADIM 4/6] Lutfen uydunuzu TERS X pozisyonuna alin ve 10sn sabit bekleyin...\r\n");
            break;
        case 5:
            printf("\r\n[ADIM 5/6] Lutfen uydunuzu DUZ X pozisyonuna alin ve 10sn sabit bekleyin...\r\n");
            break;
        case 6:
            printf("\r\n[BASARILI] 6 Noktali Kalibrasyon Tamamlandi! Yesil LED yaniyor.\r\n");
            break;
        default:
            break;
    }
}

void CalibrationUI_PrintResults(SensorCalib_t *calib) {
    if (!calib) return;
    
    // Degerleri terminale firlat
    printf("\r\n\r\n/* ======================================================= */\r\n");
    printf("/* M0.1_FilterConfig.h DOSYASINA KOPYALAYIN                */\r\n");
    printf("/* ======================================================= */\r\n");
    
    printf("#define CALIB_ACC_X_BIAS            %.5ff\r\n", calib->acc_x_bias);
    printf("#define CALIB_ACC_X_SCALE           %.5ff\r\n", calib->acc_x_scale);
    printf("#define CALIB_ACC_X_NOISE           %.5ff\r\n\r\n", calib->acc_variance[0]);

    printf("#define CALIB_ACC_Y_BIAS            %.5ff\r\n", calib->acc_y_bias);
    printf("#define CALIB_ACC_Y_SCALE           %.5ff\r\n", calib->acc_y_scale);
    printf("#define CALIB_ACC_Y_NOISE           %.5ff\r\n\r\n", calib->acc_variance[1]);

    printf("#define CALIB_ACC_Z_BIAS            %.5ff\r\n", calib->acc_z_bias);
    printf("#define CALIB_ACC_Z_SCALE           %.5ff\r\n", calib->acc_z_scale);
    printf("#define CALIB_ACC_Z_NOISE           %.5ff\r\n\r\n", calib->acc_variance[2]);

    printf("#define CALIB_GYRO_X_BIAS           %.5ff\r\n", calib->gyro_x_bias);
    printf("#define CALIB_GYRO_Y_BIAS           %.5ff\r\n", calib->gyro_y_bias);
    printf("#define CALIB_GYRO_Z_BIAS           %.5ff\r\n", calib->gyro_z_bias);
    printf("#define CALIB_GYRO_X_NOISE          %.5ff\r\n", calib->gyro_variance[0]);
    printf("#define CALIB_GYRO_Y_NOISE          %.5ff\r\n", calib->gyro_variance[1]);
    printf("#define CALIB_GYRO_Z_NOISE          %.5ff\r\n", calib->gyro_variance[2]);
    printf("/* ======================================================= */\r\n\r\n");
}
