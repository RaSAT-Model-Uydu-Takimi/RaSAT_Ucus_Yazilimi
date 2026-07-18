Proje Modüller Genel Tanımlar



Ana Döngü Modülü:



Rx Tx Bağlantı Modülü :Her input output bağlantılarının kodda hangi 

pine bağlanacağını yaz 



Struct tanımlama Modülü : Gelen veri dosya formatlarını göre açıkla



Filtre Modülü. Ham Sensor - > Filtrelenmiş Sensor



PID Modülü:  işlenmiş veri + Sensor Karar Değişkenleri - > Motor Outputları



Sızdıran Kova Modülü: İşlenmiş ->  phaseState



Faz Modülleri: phaseState -> SensorKararDeğişkenleri 

















Ana Döngü Modülü 





Durum: Başlanılmadı



Düzenleme Erişimi: Kaynak kodları ve tüm testler abdullah’a ait



Öncelik Seviyesi: 1



Tanım: 

1.f407(uçuş kartımız) uygun bir proje dizini oluşturulacak.  

2.Kod çalışmaya başlandığında yapılacaklar teorik olarak belirlenmişti yorum satırı olarak yazılacak modüller tanımlandıkça yorum satırlarına gerçek modül fonksiyonları eklenecek

3.Timerlar(2 adet) tanımlanacak.İçinde çalışacak her modül yorum olarak eklenecek daha sonra kaynak kodları geldikçe asılları ile değişilecek



Fonksiyonlar:

/\*

Burada çalışacak modüllerin isimleri olacak her modül fonksiyon olarak tanımlandıktan sonra buraya eklenecek ve kendi küçük fonksiyonları yardımıyla işlevlerin gerçekleyecek

\*/

Testler: 

//Kafama göre yazıyorum. Bu testi sen kendi kartında yapacaksın ama kod hemen hemen aynı olmasına dikkat et 

1.ana dizinde a=1000 tanımlansın timerlar on kere çalışsın değişken değişsin gibi biliyorum çok kötü sen daha iyi testler yaz her modül ve o modül ve o modülün testi için 



Filtre Modülü:

Durum Başlanılmadı

Erişim:

Tanım:  Senso





