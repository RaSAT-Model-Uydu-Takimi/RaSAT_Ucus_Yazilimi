using System;

namespace _10OcakRASAT.Managers
{
    public class ArasManager
    {
        // Hata türleri (0, 1, 2, 3 gibi indexlerle eşleşecek)
        public enum ErrorType
        {
            InisHiziHatasi = 0,
            GpsHatasi = 1,
            AyrilmaHatasi = 2,
            AcilParasut = 3,
            BilinmeyenHata = -1
        }

        public class AlarmEventArgs : EventArgs
        {
            public ErrorType Type { get; set; }
            public string Message { get; set; } = string.Empty;
        }

        // Alarm tetiklendiğinde fırlatılacak event
        public event EventHandler<AlarmEventArgs>? OnCriticalErrorDetected;
        // Alarm durumu geçtiğinde fırlatılacak event
        public event EventHandler? OnAlarmCleared;

        private bool _isAlarmActive = false;

        public void CheckData(TData data)
        {
            if (string.IsNullOrEmpty(data.HataKodu) || data.HataKodu == "0000" || data.HataKodu == "00000")
            {
                if (_isAlarmActive)
                {
                    _isAlarmActive = false;
                    OnAlarmCleared?.Invoke(this, EventArgs.Empty);
                }
                return;
            }

            // Hata varsa
            _isAlarmActive = true;
            string errorMsg = $"HATA KODU: <{data.HataKodu}>\n";
            ErrorType primaryType = ErrorType.BilinmeyenHata;

            if (data.HataKodu.Length >= 4)
            {
                if (data.HataKodu[0] == '1')
                {
                    errorMsg += "(Görev yükü iniş hızının 8-10 m/s dışında olması durumu)\n";
                    primaryType = ErrorType.InisHiziHatasi;
                }
                if (data.HataKodu[1] == '1')
                {
                    errorMsg += "(Görev yükü konum verisinin iletilememesi durumu)\n";
                    if (primaryType == ErrorType.BilinmeyenHata) primaryType = ErrorType.GpsHatasi;
                }
                if (data.HataKodu[2] == '1')
                {
                    errorMsg += "(Görev yükünün taşıyıcıdan ayrılmasının gerçekleşmemesi durumu)\n";
                    if (primaryType == ErrorType.BilinmeyenHata) primaryType = ErrorType.AyrilmaHatasi;
                }
                if (data.HataKodu[3] == '1')
                {
                    errorMsg += "(Acil Paraşüt Açma Mekanizmasının aktifleştirildiği durum)\n";
                    if (primaryType == ErrorType.BilinmeyenHata) primaryType = ErrorType.AcilParasut;
                }
            }

            OnCriticalErrorDetected?.Invoke(this, new AlarmEventArgs
            {
                Type = primaryType,
                Message = errorMsg
            });
        }
    }
}
