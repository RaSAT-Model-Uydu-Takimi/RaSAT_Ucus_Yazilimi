# Firebase Credentials Kurulum Rehberi

Bu proje Firebase kullanmaktadır. Projeyi başka bir bilgisayarda çalıştırmak için Firebase kimlik bilgilerinizi yapılandırmanız gerekir.

## Kurulum Adımları

### 1. Firebase Credentials Dosyasını Oluşturma

1. `RasatDesktop/firebase-credentials.json.template` dosyasını kopyalayın
2. Kopyayı `firebase-credentials.json` olarak yeniden adlandırın
3. Dosyayı açın ve placeholder değerleri gerçek Firebase bilgilerinizle değiştirin

**Komut satırından:**
```bash
cd RasatDesktop
copy firebase-credentials.json.template firebase-credentials.json
```

### 2. Firebase Credentials Dosyasını Yerleştirme

Uygulama, `firebase-credentials.json` dosyasını **aşağıdaki konumlardan birinde** arar (öncelik sırasına göre):

#### Seçenek 1: .exe Dosyasının Yanında (ÖNERİLEN)
```
RasatDesktop.exe
firebase-credentials.json  ← Buraya koyun
```
**Avantaj:** En basit yöntem, exe dosyasını taşıdığınızda credentials da birlikte gider.

#### Seçenek 2: RasatDesktop Alt Klasörü
```
RasatDesktop.exe
RasatDesktop/
  └── firebase-credentials.json  ← Buraya koyun
```
**Avantaj:** Daha düzenli klasör yapısı.

#### Seçenek 3: Documents Klasörü
```
C:\Users\[KullanıcıAdı]\Documents\RasatGCS\firebase-credentials.json
```
**Avantaj:** Merkezi konum, birden fazla uygulama kopyası aynı dosyayı kullanabilir.

#### Seçenek 4: AppData Klasörü
```
C:\Users\[KullanıcıAdı]\AppData\Roaming\RasatGCS\firebase-credentials.json
```
**Avantaj:** Windows standart uygulama veri konumu.

**Not:** Uygulama yukarıdaki konumları sırayla kontrol eder ve **ilk bulduğu dosyayı** kullanır.

### 3. Firebase Bilgilerinizi Alma

Firebase kimlik bilgilerinizi almak için:

1. [Firebase Console](https://console.firebase.google.com/) adresine gidin
2. Projenizi seçin
3. **Project Settings** (Proje Ayarları) > **Service Accounts** sekmesine gidin
4. **Generate New Private Key** butonuna tıklayın
5. İndirilen JSON dosyasının içeriğini `firebase-credentials.json` dosyasına yapıştırın

### 4. Güvenlik Uyarısı

⚠️ **ÖNEMLİ:** `firebase-credentials.json` dosyası hassas bilgiler içerir!

- Bu dosyayı **asla** GitHub'a yüklemeyin
- Bu dosyayı **asla** başkalarıyla paylaşmayın
- Bu dosya `.gitignore` içinde listelenmiştir ve Git tarafından göz ardı edilecektir

### 5. Doğrulama

Dosyanızın doğru formatta olduğunu kontrol edin:

```json
{
  "type": "service_account",
  "project_id": "your-actual-project-id",
  "private_key_id": "your-actual-key-id",
  "private_key": "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n",
  "client_email": "your-service-account@your-project.iam.gserviceaccount.com",
  "client_id": "your-client-id",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/...",
  "universe_domain": "googleapis.com"
}
```

## Sorun Giderme

### "Firebase credentials not found" hatası alıyorsanız:
- `firebase-credentials.json` dosyasının `RasatDesktop/` klasöründe olduğundan emin olun
- Dosya adının tam olarak `firebase-credentials.json` olduğunu kontrol edin (`.template` uzantısı olmamalı)

### "Invalid credentials" hatası alıyorsanız:
- JSON formatının doğru olduğundan emin olun
- Firebase Console'dan yeni bir key indirip tekrar deneyin
- Dosyada ekstra boşluk veya karakter olmadığından emin olun

## Yardım

Daha fazla bilgi için [Firebase Admin SDK Setup](https://firebase.google.com/docs/admin/setup) dokümantasyonuna bakabilirsiniz.
