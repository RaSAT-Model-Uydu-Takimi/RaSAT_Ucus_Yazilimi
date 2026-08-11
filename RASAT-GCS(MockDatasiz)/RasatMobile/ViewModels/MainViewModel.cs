using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using RASATMobile.Models;
using RASATMobile.Services;

namespace RASATMobile.ViewModels
{
    public class MainViewModel : INotifyPropertyChanged
    {
        private readonly FirebaseService _firebaseService;
        private TData _currentData = new TData();
        private string _connectionStatus = "Bağlanıyor...";
        private double _mapMode = 0; // 0=FullView, 1=Follow, 2=Inspect
        private IDisposable? _subscription;
        private System.Timers.Timer? _pollingTimer;

        // Tüm telemetri geçmişi
        public ObservableCollection<TData> AllTelemetryData { get; } = new();

        public event PropertyChangedEventHandler? PropertyChanged;

        public MainViewModel()
        {
            _firebaseService = new FirebaseService();
            
            // Tüm geçmiş verileri çek ve realtime dinlemeyi başlat
            _ = InitializeAsync();
        }

        private async Task InitializeAsync()
        {
            // 1. Tüm geçmiş verileri çek
            await FetchAllHistoryAsync();
            
            // 2. Son veriyi çek
            await FetchLatestDataAsync();
            
            // 3. Realtime subscription başlat
            StartRealtimeSubscription();
            
            // 4. Yedek polling başlat
            StartPolling();
        }

        #region Properties

        public TData CurrentData
        {
            get => _currentData;
            set
            {
                _currentData = value;
                OnPropertyChanged();
            }
        }

        public string ConnectionStatus
        {
            get => _connectionStatus;
            set
            {
                _connectionStatus = value;
                OnPropertyChanged();
            }
        }

        /// <summary>
        /// Harita modu: 0=Full View, 1=Follow, 2=Inspect
        /// </summary>
        public double MapMode
        {
            get => _mapMode;
            set
            {
                _mapMode = Math.Round(value); // Snap to nearest integer
                OnPropertyChanged();
                OnPropertyChanged(nameof(MapModeText));
                OnPropertyChanged(nameof(FullViewColor));
                OnPropertyChanged(nameof(FollowColor));
                OnPropertyChanged(nameof(InspectColor));
            }
        }

        public string MapModeText
        {
            get
            {
                return (int)_mapMode switch
                {
                    0 => "🔍 Full View - Tüm rota görünümü",
                    1 => "📍 Follow - Son 10 koordinat takibi",
                    2 => "🖱️ Inspect - Serbest gezinme",
                    _ => ""
                };
            }
        }

        public string TelemetryCountText => $"📊 Toplam {AllTelemetryData.Count} veri noktası";

        // Mod etiket renkleri
        public Color FullViewColor => (int)_mapMode == 0 ? Colors.Cyan : Colors.Gray;
        public Color FollowColor => (int)_mapMode == 1 ? Colors.Cyan : Colors.Gray;
        public Color InspectColor => (int)_mapMode == 2 ? Colors.Cyan : Colors.Gray;

        #endregion

        private bool _isBusy;
        public bool IsBusy
        {
            get => _isBusy;
            set
            {
                _isBusy = value;
                OnPropertyChanged();
            }
        }

        #region Data Fetching Methods

        private async Task FetchAllHistoryAsync()
        {
            if (Connectivity.Current.NetworkAccess != NetworkAccess.Internet)
            {
                ConnectionStatus = "⚠️ İnternet Gezginine Erişilemiyor";
                return;
            }

            try
            {
                IsBusy = true;
                ConnectionStatus = "📥 Geçmiş veriler yükleniyor...";
                
                var historyData = await _firebaseService.GetAllTelemetryHistoryAsync();
                
                MainThread.BeginInvokeOnMainThread(() =>
                {
                    AllTelemetryData.Clear();
                    foreach (var data in historyData)
                    {
                        AllTelemetryData.Add(data);
                    }
                    OnPropertyChanged(nameof(TelemetryCountText));
                    ConnectionStatus = $"✅ {historyData.Count} veri yüklendi";
                });
            }
            catch (Exception ex)
            {
                MainThread.BeginInvokeOnMainThread(() =>
                {
                    ConnectionStatus = $"❌ Geçmiş yükleme hatası: {ex.Message}";
                });
            }
            finally
            {
                IsBusy = false;
            }
        }

        private async Task FetchLatestDataAsync()
        {
            if (Connectivity.Current.NetworkAccess != NetworkAccess.Internet)
            {
                ConnectionStatus = "⚠️ İnternet Yok";
                return;
            }

            try
            {
                var data = await _firebaseService.GetLatestTelemetryAsync();
                if (data != null)
                {
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        CurrentData = data;
                        ConnectionStatus = $"🟢 Bağlı - Paket: {data.PaketNumarasi}";
                    });
                }
            }
            catch (Exception ex)
            {
                MainThread.BeginInvokeOnMainThread(() =>
                {
                    ConnectionStatus = $"🔴 Bağlantı hatası: {ex.Message}";
                });
            }
        }

        private void StartRealtimeSubscription()
        {
            try
            {
                _subscription = _firebaseService.SubscribeToTelemetry(
                    data =>
                    {
                        MainThread.BeginInvokeOnMainThread(() =>
                        {
                            CurrentData = data;
                            ConnectionStatus = $"🟢 Realtime - Paket: {data.PaketNumarasi}";
                            
                            // Yeni veriyi geçmişe ekle
                            AllTelemetryData.Add(data);
                            OnPropertyChanged(nameof(TelemetryCountText));
                        });
                    },
                    error =>
                    {
                        MainThread.BeginInvokeOnMainThread(() =>
                        {
                            ConnectionStatus = "🟡 Subscription hatası, polling aktif";
                        });
                    }
                );
            }
            catch (Exception)
            {
                ConnectionStatus = "🟡 Subscription başlatılamadı, polling aktif";
            }
        }

        private void StartPolling()
        {
            _pollingTimer = new System.Timers.Timer(2000); // 2 saniye
            _pollingTimer.Elapsed += async (s, e) =>
            {
                try
                {
                    var data = await _firebaseService.GetLatestTelemetryAsync();
                    if (data != null)
                    {
                        MainThread.BeginInvokeOnMainThread(() =>
                        {
                            // Sadece yeni veri geldiyse güncelle
                            if (data.PaketNumarasi != CurrentData.PaketNumarasi)
                            {
                                CurrentData = data;
                                ConnectionStatus = $"🟢 Polling - Paket: {data.PaketNumarasi}";
                                
                                // Yeni veriyi geçmişe ekle
                                AllTelemetryData.Add(data);
                                OnPropertyChanged(nameof(TelemetryCountText));
                            }
                        });
                    }
                }
                catch (Exception)
                {
                    // Polling hatası - sessizce devam et
                }
            };
            _pollingTimer.AutoReset = true;
            _pollingTimer.Start();
        }

        #endregion

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }
    }
}
