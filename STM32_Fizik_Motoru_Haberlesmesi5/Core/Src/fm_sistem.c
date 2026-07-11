/*
 * fm_sistem.c
 *
 * Created on: Jul 9, 2026
 * Author: abduh
 */

#include "fm_sistem.h"
#include "ucus_kontrol.h"

// KÜTÜPHANE İÇİ GİZLİ DEĞİŞKENLER (Encapsulation)
// Karşı taraftan Alarm paketi geldiğinde tekrar gönderebilmek için son başarılı paketi hafızada tutuyoruz
static Iletim_Veri_t son_gonderilen_paket;

// Sabit alarm paketi: İndex ve diğer her şey sabit.
// Sadece durum bayrağı 0xF0.
const Iletim_Veri_t ALARM_PAKETI_SABIT = {
    .paket = {
	.header = PAKET_HEADER,
	.index = 0,
	.mi1 = 0, .mi2 = 0, .mi3 = 0, .mi4 = 0,
	.durum_bayraklari = 0xF0,
	.checksum = 0x0F // 0xAA ^ 0x55 ^ 0xF0 işleminin sonucu
    }
};





// --- YARDIMCI FONKSİYONLAR ---
// Sadece bu kütüphane içinde kullanılacak gizli XOR Checksum fonksiyonu
static uint8_t Checksum_Hesapla_Sistem(const uint8_t *veri, uint16_t uzunluk) {
    uint8_t chk = 0;
    for (uint16_t i = 0; i < uzunluk; i++) {
        chk ^= veri[i];
    }
    return chk;
}






// ---------------------------------------------------------
// İŞLEM FONKSİYONLARI
// ---------------------------------------------------------

// 1. En Son Paketi Bir Daha Oluştur
void En_Son_Paketi_Bir_Daha_Olustur(Iletim_Veri_t *iletilecek_paket) {
    // Gizli (static) yedek tamponumuzda tuttuğumuz bir önceki başarılı paketi
    // tekrar iletim tamponuna doğrudan kopyalıyoruz.
    memcpy(iletilecek_paket->buffer, son_gonderilen_paket.buffer, sizeof(Iletim_Paketi_t));
}







// 2. Sistem Davranışını Hesapla (Ana Uçuş Yazılımı Modülü)
void Sistem_Davranisini_Hesapla(Alim_Veri_t *gelen_veri, Iletim_Veri_t *iletilecek_paket) {

	// ASIL İŞİ UÇUŞ YAZILIMINA DEVRET!
	// Sadece paketin içindeki verilerin adreslerini uçuş yazılımına gönderiyoruz.
	Ucus_Yazilimini_Calistir(&gelen_veri->paket, &iletilecek_paket->paket);

	// Paket başlığını ve index'ini (protokol gerekliliklerini) fm_sistem ayarlar
	    iletilecek_paket->paket.header = PAKET_HEADER;
	    iletilecek_paket->paket.index = gelen_veri->paket.index;


}







// 3. İletim Paketini Oluştur (Son Hazırlık ve Mühürleme)
void Iletim_Paketini_Olustur(Iletim_Veri_t *iletilecek_paket) {

    // 1. Checksum hesapla (Son bayt hariç tüm paketin XOR'u)
    iletilecek_paket->paket.checksum = Checksum_Hesapla_Sistem(iletilecek_paket->buffer, sizeof(Iletim_Paketi_t) - 1);

    // 2. İleride karşı taraftan Alarm Paketi gelirse diye, mühürlenmiş bu kusursuz paketi
    // static yedeğimize (son_gonderilen_paket) kopyalayıp güvenceye alıyoruz.
    memcpy(son_gonderilen_paket.buffer, iletilecek_paket->buffer, sizeof(Iletim_Paketi_t));
}
