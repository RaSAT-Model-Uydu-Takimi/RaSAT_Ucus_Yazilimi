using System;
using System.IO;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Google.Apis.Auth.OAuth2;
using Newtonsoft.Json;

namespace _10OcakRASAT.Managers
{
    public class FirebaseManager
    {
        private static FirebaseManager? _instance;
        private static readonly object _lock = new object();
        private readonly HttpClient _httpClient;
        private GoogleCredential? _credential;
        private string? _accessToken;
        private DateTime _tokenExpiry;

        // Kullanıcının verdiği veritabanı adresi
        private const string DatabaseUrl = "https://groundstationdatabase-default-rtdb.europe-west1.firebasedatabase.app/";

        // Singleton Pattern
        public static FirebaseManager Instance
        {
            get
            {
                lock (_lock)
                {
                    if (_instance == null)
                    {
                        _instance = new FirebaseManager();
                    }
                    return _instance;
                }
            }
        }

        private FirebaseManager()
        {
            _httpClient = new HttpClient();
            InitializeFirebase();
        }

        private void InitializeFirebase()
        {
            try
            {
                string jsonPath = FindFirebaseCredentialsFile();

                if (!string.IsNullOrEmpty(jsonPath) && File.Exists(jsonPath))
                {
                    using (var stream = new FileStream(jsonPath, FileMode.Open, FileAccess.Read))
                    {
                        _credential = GoogleCredential.FromStream(stream)
                            .CreateScoped("https://www.googleapis.com/auth/userinfo.email", "https://www.googleapis.com/auth/firebase.database");
                    }
                    System.Diagnostics.Debug.WriteLine($"Firebase credentials loaded from: {jsonPath}");
                }
                else
                {
                    System.Diagnostics.Debug.WriteLine("Firebase credentials file not found. Application will run without Firebase integration.");
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Firebase Init Error: {ex.Message}");
            }
        }

        /// <summary>
        /// Firebase credentials dosyasını birden fazla konumda arar
        /// </summary>
        private string FindFirebaseCredentialsFile()
        {
            const string fileName = "firebase-credentials.json";
            
            // Arama yapılacak konumlar (öncelik sırasına göre)
            var searchPaths = new[]
            {
                // 1. .exe dosyasının bulunduğu klasör
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, fileName),
                
                // 2. .exe dosyasının yanında RasatDesktop klasörü
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "RasatDesktop", fileName),
                
                // 3. Proje klasörü (development için)
                Path.Combine(Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, @"..\..\..")), fileName),
                
                // 4. Proje RasatDesktop klasörü (development için)
                Path.Combine(Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, @"..\..\..")), "RasatDesktop", fileName),
                
                // 5. Kullanıcının Documents klasörü
                Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "RasatGCS", fileName),
                
                // 6. Kullanıcının AppData klasörü
                Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "RasatGCS", fileName)
            };

            // İlk bulunan dosyayı döndür
            foreach (var path in searchPaths)
            {
                if (File.Exists(path))
                {
                    return path;
                }
            }

            return string.Empty;
        }

        private async Task<string> GetAccessTokenAsync()
        {
            if (_credential == null) return "";

            // Token geçerli mi kontrol et (süresi dolmaya yakınsa yenile)
            if (_accessToken != null && DateTime.UtcNow < _tokenExpiry.AddMinutes(-1))
            {
                return _accessToken;
            }

            try
            {
                var token = await _credential.UnderlyingCredential.GetAccessTokenForRequestAsync();
                _accessToken = token;
                // Token genelde 1 saatliktir, biz güvenli taraf seçip 50 dk varsayalım veya expiry kontrolü yapalım
                // (Basitlik için şimdilik expiry'i manuel set ediyoruz, normalde token response'dan alınır)
                _tokenExpiry = DateTime.UtcNow.AddMinutes(50); 
                return _accessToken;
            }
            catch
            {
                return "";
            }
        }

        public async Task SendTelemetryData(TData data)
        {
            // Credential yoksa gönderme (JSON dosyası eklenmemiş demektir)
            if (_credential == null) return;

            try
            {
                var token = await GetAccessTokenAsync();
                if (string.IsNullOrEmpty(token)) return;

                var timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
                
                var firebaseData = new
                {
                    data.PaketNumarasi,
                    data.UyduStatusu,
                    data.HataKodu,
                    data.GondermeSaati,
                    data.Basinc,
                    data.Yukseklik,
                    data.InisHizi,
                    data.Sicaklik,
                    data.PilGerilimi,
                    data.GpsLatitude,
                    data.GpsLongitude,
                    data.GpsAltitude,
                    data.Pitch,
                    data.Roll,
                    data.Yaw,
                    data.RHRHRH,
                    data.TakimNo,
                    TimeStamp = timestamp
                };

                var json = JsonConvert.SerializeObject(firebaseData);
                var content = new StringContent(json, Encoding.UTF8, "application/json");

                // REST API ile PUT isteği (Latest veriyi günceller)
                // PUT: Var olan veriyi ezer (Tam istediğimiz şey)
                // Auth: access_token querystring parametresi ile
                string url = $"{DatabaseUrl}telemetry/latest.json?access_token={token}";

                await _httpClient.PutAsync(url, content);
            }
            catch (Exception)
            {
                // Hata durumunda akışı bozma
            }
        }

        /// <summary>
        /// Firebase'deki telemetri verisini siler
        /// </summary>
        public async Task<bool> DeleteTelemetryDataAsync()
        {
            if (_credential == null) return false;

            try
            {
                var token = await GetAccessTokenAsync();
                if (string.IsNullOrEmpty(token)) return false;

                // Telemetry/latest düğümünü sil
                string url = $"{DatabaseUrl}telemetry/latest.json?access_token={token}";
                
                var response = await _httpClient.DeleteAsync(url);
                return response.IsSuccessStatusCode;
            }
            catch (Exception)
            {
                return false;
            }
        }
    }
}
