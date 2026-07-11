/*
 * fm_haberlesme.c
 *
 * Created on: Jul 9, 2026
 * Author: abduh
 */

#include "fm_haberlesme.h"

// KÜTÜPHANE YAPISINA ÖZEL (Header'dan buraya taşıdık, artık güvendeyiz)
static UART_HandleTypeDef *secilen_uart;
static TIM_HandleTypeDef  *secilen_tim;
static Alim_Veri_t        alinacak_ham; //DMA çift tampon sistemindeki bufferlardan biri

// --- YARDIMCI FONKSİYONLAR ---
// Sadece bu kütüphane içinde kullanılacak gizli XOR Checksum fonksiyonu
static uint8_t Checksum_Hesapla(const uint8_t *veri, uint16_t uzunluk) {
    uint8_t chk = 0;
    for (uint16_t i = 0; i < uzunluk; i++) {
        chk ^= veri[i];
    }
    return chk;
}





// ---------------------------------------------------------
// 1. Donanım Tanıtma (Sistemin başında 1 kez çağrılır)
// ---------------------------------------------------------
void Donanim_Sec(UART_HandleTypeDef *huart, TIM_HandleTypeDef *htim) {
    secilen_uart = huart;
    secilen_tim = htim;
}






// ---------------------------------------------------------
// İŞLEM FONKSİYONLARI
// ---------------------------------------------------------

uint8_t Alim_Paketini_Al(Alim_Veri_t *islenecek_guvenli) {
	// 1. Önceki kalıntıları temizle ve DMA ile yeni alımı başlat
	memset(alinacak_ham.buffer, 0, sizeof(Alim_Paketi_t));
	HAL_UART_Receive_DMA(secilen_uart, alinacak_ham.buffer, sizeof(Alim_Paketi_t));

	// 2. İLK BAYT BEKLEME DÖNGÜSÜ (Ping-pong dondurması burada gerçekleşir!)
	// DMA'nın sayacı (Counter) başlangıçta 52'dir.
	// İlk bayt geldiğinde bu sayı 51'e düşer. Sayı 52 olduğu sürece sonsuza dek burada bekle.
	while (__HAL_DMA_GET_COUNTER(secilen_uart->hdmarx) == sizeof(Alim_Paketi_t)) {
		// Fizik motorundan ilk bayt gelene kadar STM32 burada donar. Timer falan çalışmaz.
	}

	// 3. İLK BAYT GELDİ! Artık zaman aşımını başlatabiliriz.
	// Kalan baytlar 5ms içinde gelmezse paket eksik gelmiş (Bozuk Paket) demektir.
	__HAL_TIM_SET_COUNTER(secilen_tim, 0);
	HAL_TIM_Base_Start(secilen_tim);

	// 4. Kalan paketin tamamlanmasını bekleme döngüsü
	while (secilen_uart->RxState != HAL_UART_STATE_READY) {

		// Zaman aşımı olduysa (5ms dolduysa ve hala 52 bayt tamamlanmadıysa)
		if (__HAL_TIM_GET_COUNTER(secilen_tim) >= (secilen_tim->Init.Period - 1)) {

			HAL_UART_AbortReceive(secilen_uart); // Arızalı/Eksik alımı iptal et
			HAL_TIM_Base_Stop(secilen_tim);      // Timer'ı durdur
			return PAKET_EKSIK; 							 // 2 demek eksik paket geldi demek
		}
	}

	// 5. Zaman aşımı olmadan paket eksiksiz (52 bayt) geldiyse
	HAL_TIM_Base_Stop(secilen_tim);
	memcpy(islenecek_guvenli->buffer, alinacak_ham.buffer, sizeof(Alim_Paketi_t));

	return PAKET_TAM; // 1 demek tam paket geldi demek
}






uint8_t Bozuk_Paket_Mi(Alim_Veri_t *paket) {
    // 1. Header Kontrolü (Paketin başında 0xAA55 yoksa bozuktur)
    if (paket->paket.header != PAKET_HEADER) {
        return 1; // Hata: Bozuk Paket
    }

    // 2. Checksum (CHK) Kontrolü
    // Checksum baytı (listenin en sonundaki bayt) hariç tüm paketin XOR'unu alıyoruz
    uint8_t hesaplanan_chk = Checksum_Hesapla(paket->buffer, sizeof(Alim_Paketi_t) - 1);

    if (hesaplanan_chk != paket->paket.checksum) {
        return 2; // Hata: Veri iletim sırasında bozulmuş
    }

    return 0; // Paket Sağlam
}






uint8_t Alarm_Paket_Mi(Alim_Veri_t *paket) {
    // STM32'de çalışırken Gelen Paketin "rezerve" byte'ı F0 mı kontrolü
    if (paket->paket.rezerve == 0xF0) {
        return 1; // Uyarı: Alarm Paketi Geldi!
    }

    return 0; // Normal Paket
}







void Gonder(Iletim_Veri_t *paket) {
    // secilen_uart pointer'ını kullanarak doğrudan fizik motoruna gönderim yapıyoruz.
    // Timeout değerini 100ms olarak belirledik ki sistem kilitlenmesin.
    HAL_UART_Transmit(secilen_uart, paket->buffer, sizeof(Iletim_Paketi_t), 100);
}
