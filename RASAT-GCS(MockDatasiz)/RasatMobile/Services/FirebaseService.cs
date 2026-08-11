using Firebase.Database;
using Firebase.Database.Query;
using RASATMobile.Models;

namespace RASATMobile.Services
{
    public class FirebaseService
    {
        private readonly FirebaseClient _firebaseClient;
        
        // Doğru bölge URL'si (Europe-West1)
        private const string DatabaseUrl = "https://groundstationdatabase-default-rtdb.europe-west1.firebasedatabase.app/";

        public FirebaseService()
        {
            _firebaseClient = new FirebaseClient(DatabaseUrl);
        }

        /// <summary>
        /// Firebase'den son telemetri verisini çeker (tek seferlik)
        /// </summary>
        public async Task<TData?> GetLatestTelemetryAsync()
        {
            try
            {
                var data = await _firebaseClient
                    .Child("telemetry")
                    .Child("latest")
                    .OnceSingleAsync<TData>();
                
                return data;
            }
            catch (Exception)
            {
                return null;
            }
        }

        /// <summary>
        /// Firebase'den tüm telemetri geçmişini çeker
        /// </summary>
        public async Task<List<TData>> GetAllTelemetryHistoryAsync()
        {
            var result = new List<TData>();
            
            try
            {
                // telemetry/history altındaki tüm verileri çek
                var historyData = await _firebaseClient
                    .Child("telemetry")
                    .Child("history")
                    .OnceAsync<TData>();

                if (historyData != null)
                {
                    foreach (var item in historyData)
                    {
                        if (item.Object != null)
                        {
                            result.Add(item.Object);
                        }
                    }
                }
                
                // Timestamp'e göre sırala
                result = result.OrderBy(x => x.TimeStamp).ToList();
            }
            catch (Exception)
            {
                // Hata durumunda boş liste döndür
            }
            
            return result;
        }

        /// <summary>
        /// Realtime subscription - Firebase değişikliklerini dinler
        /// </summary>
        public IDisposable SubscribeToTelemetry(Action<TData> onDataReceived, Action<Exception> onError)
        {
            return _firebaseClient
                .Child("telemetry")
                .Child("latest")
                .AsObservable<TData>()
                .Subscribe(
                    change =>
                    {
                        if (change.Object != null)
                        {
                            onDataReceived(change.Object);
                        }
                    },
                    error =>
                    {
                        onError(error);
                    }
                );
        }
    }
}
