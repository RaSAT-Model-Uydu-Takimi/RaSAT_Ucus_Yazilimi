using System;

public class GeneratorTData
{
    // Simulation değişkenleri
    private int _packetCounter;
    private double _currentAltitude;
    private double _elapsedTime; // Geçen süre (saniye)
    
    // GPS başlangıç noktası (Camimiz)
    private double _baseLat = 41.0283;
    private double _baseLon = 28.8890;
    
    // Hedef iniş noktası
    private double _targetLat = 41.0250;
    private double _targetLon = 28.8920;
    
    // Ay-Yıldız patern değişkenleri
    private double _scale = 0.003; // GPS derece cinsinden ölçek (yaklaşık 300m)
    private int _patternStep = 0;  // Hangi adımda olduğumuz
    private double _patternAngle = 0; // Çizim açısı
    private bool _patternComplete = false;
    
    // Mevcut GPS konumu
    private double _currentLat;
    private double _currentLon;

    // Oryantasyon değerleri (yumuşak geçiş için)
    private double _currentPitch = 0;
    private double _currentRoll = 0;
    private double _currentYaw = 0;

    // Hedef oryantasyon (sinüsoidal hareket için)
    private double _pitchPhase = 0;
    private double _rollPhase = Math.PI / 3;
    private double _yawPhase = Math.PI / 6;

    public GeneratorTData()
    {
        _packetCounter = 1;
        _currentAltitude = 2000.0; // 2000 metreden başla
        _elapsedTime = 0;
        _currentLat = _baseLat;
        _currentLon = _baseLon;
    }

    public TData GenerateBasicTData()
    {
        TData newData = new TData();

        _elapsedTime += 1.0; // Her çağrıda 1 saniye geçiyor

        newData.PaketNumarasi = GeneratePacketNumber();
        newData.UyduStatusu = GenerateStatus();
        newData.HataKodu = GenerateErrorCode();
        newData.GondermeSaati = GenerateTime();
        newData.Yukseklik = GenerateAltitude();
        newData.Basinc = GeneratePressure(newData.Yukseklik);
        newData.InisHizi = GenerateDescentSpeed();
        newData.Sicaklik = GenerateTemperature();
        newData.PilGerilimi = GenerateBatteryVoltage();

        // GPS - Ay-Yıldız paterni sonra hedef noktaya
        var (lat, lon) = GenerateGpsCoordinates();
        newData.GpsLatitude = lat;
        newData.GpsLongitude = lon;
        newData.GpsAltitude = newData.Yukseklik + 2; // GPS yükseklik küçük hata payı

        // Oryantasyon - gerçekçi salınım hareketi
        var (pitch, roll, yaw) = GenerateOrientation();
        newData.Pitch = pitch;
        newData.Roll = roll;
        newData.Yaw = yaw;

        // Komut echo - global state'ten al
        newData.RHRHRH = _10OcakRASAT.Managers.CommandStateManager.Instance.GetLastCommand();
        newData.TakimNo = 12345;

        return newData;
    }

    private int GeneratePacketNumber()
    {
        return _packetCounter++;
    }

    private int GenerateStatus()
    {
        if (_currentAltitude <= 0) return 4; // İniş tamamlandı
        if (_currentAltitude < 100) return 3; // Son iniş aşaması
        return 2; // İniş aşaması
    }

    private string GenerateErrorCode()
    {
        return "00000";
    }

    private string GenerateTime()
    {
        return DateTime.Now.ToString("dd/MM/yyyy HH:mm:ss");
    }

    private double GenerateAltitude()
    {
        // Paraşüt iniş simülasyonu - sabit hızda azalma
        // ~12-13 m/s iniş hızı
        double descentRate = 12.5;
        _currentAltitude -= descentRate;

        if (_currentAltitude < 0) _currentAltitude = 0;

        return Math.Round(_currentAltitude, 2);
    }

    private double GeneratePressure(double altitude)
    {
        // Barometrik formül
        double pressure = 101325 * Math.Pow((1 - (0.0065 * altitude) / 288.15), 5.255);
        return Math.Round(pressure, 2);
    }

    private double GenerateDescentSpeed()
    {
        // Sabit iniş hızı (paraşütle)
        if (_currentAltitude <= 0) return 0;
        return Math.Round(12.5 + Math.Sin(_elapsedTime * 0.1) * 0.5, 2);
    }

    private double GenerateTemperature()
    {
        // Yükseklikle düşen sıcaklık (-6.5°C per km)
        double baseTemp = 25;
        double tempDrop = (_currentAltitude / 1000.0) * 6.5;
        return Math.Round(baseTemp - tempDrop, 1);
    }

    private double GenerateBatteryVoltage()
    {
        // Yavaşça düşen pil gerilimi
        double voltage = 7.6 - (_elapsedTime * 0.01);
        if (voltage < 0.0) voltage = 0.0;
        return Math.Round(voltage, 2);
    }

    private (double lat, double lon) GenerateGpsCoordinates()
    {
        if (!_patternComplete)
        {
            // Ay-Yıldız çizimi
            var (lat, lon) = DrawCrescentAndStar();
            _currentLat = lat;
            _currentLon = lon;
        }
        else
        {
            // Hedef noktaya doğru git
            NavigateToTarget();
        }

        return (Math.Round(_currentLat, 6), Math.Round(_currentLon, 6));
    }

    private (double lat, double lon) DrawCrescentAndStar()
    {
        double lat = _baseLat;
        double lon = _baseLon;
        
        // Toplam adım sayısı: Hilal (40) + Yıldız (10) = 50
        int crescentSteps = 40;
        int starSteps = 10;
        int totalSteps = crescentSteps + starSteps;

        if (_patternStep < crescentSteps)
        {
            // === HİLAL (AY) ÇİZİMİ ===
            // Dış daire
            double outerRadius = _scale;
            // İç daire (hilal oluşturmak için kaydırılmış)
            double innerRadius = _scale * 0.75;
            double innerOffset = _scale * 0.3; // Sağa kaydırma
            
            double angle = Math.PI * 0.3 + (_patternStep / (double)crescentSteps) * Math.PI * 1.4; // 1.4 PI = hilal arc
            
            // Dış daire noktası
            lat = _baseLat + Math.Sin(angle) * outerRadius;
            lon = _baseLon + Math.Cos(angle) * outerRadius;
        }
        else if (_patternStep < totalSteps)
        {
            // === YILDIZ ÇİZİMİ (5 köşeli) ===
            int starStep = _patternStep - crescentSteps;
            
            // Yıldız merkezi (hilalin sağında)
            double starCenterLat = _baseLat;
            double starCenterLon = _baseLon + _scale * 0.6;
            
            // 5 köşeli yıldız
            double starRadius = _scale * 0.25;
            int pointIndex = starStep % 5;
            
            // Her köşe: dış nokta → iç nokta geçişi
            // Yıldız çizimi için: 0-2-4-1-3-0 sırası (kapalı yıldız)
            int[] starOrder = { 0, 2, 4, 1, 3, 0, 2, 4, 1, 3 };
            int currentPoint = starOrder[starStep];
            
            double starAngle = -Math.PI / 2 + currentPoint * (2 * Math.PI / 5); // Tepeden başla
            lat = starCenterLat + Math.Sin(starAngle) * starRadius;
            lon = starCenterLon + Math.Cos(starAngle) * starRadius;
        }
        else
        {
            // Patern tamamlandı
            _patternComplete = true;
        }

        _patternStep++;
        return (lat, lon);
    }

    private void NavigateToTarget()
    {
        // Hedefe doğru yumuşak hareket
        double smoothing = 0.05; // Her adımda %5 yaklaş
        
        double latDiff = _targetLat - _currentLat;
        double lonDiff = _targetLon - _currentLon;
        
        // Hafif gürültü ekle (rüzgar etkisi)
        double noise = (new Random().NextDouble() - 0.5) * 0.00005;
        
        _currentLat += latDiff * smoothing + noise;
        _currentLon += lonDiff * smoothing + noise;
        
        // Hedefe çok yaklaştıysa sabitle
        if (Math.Abs(latDiff) < 0.0001 && Math.Abs(lonDiff) < 0.0001)
        {
            _currentLat = _targetLat;
            _currentLon = _targetLon;
        }
    }

    private (double pitch, double roll, double yaw) GenerateOrientation()
    {
        // Sinüsoidal salınım - gerçekçi uydu hareketi
        double time = _elapsedTime;

        // Hedef değerler 
        double targetPitch = Math.Sin(time * 0.3 + _pitchPhase) * 15; // ±15 derece
        double targetRoll = Math.Sin(time * 0.25 + _rollPhase) * 10;  // ±10 derece
        double targetYaw = (time * 5) % 360; // Yavaş dönüş (5 derece/saniye)

        // Yumuşak geçiş için
        double smoothing = 0.3;
        _currentPitch += (targetPitch - _currentPitch) * smoothing;
        _currentRoll += (targetRoll - _currentRoll) * smoothing;
        _currentYaw += (targetYaw - _currentYaw) * smoothing;

        // Yaw 360 derece döngüsü
        if (_currentYaw < 0) _currentYaw += 360;
        if (_currentYaw >= 360) _currentYaw -= 360;

        return (
            Math.Round(_currentPitch, 2),
            Math.Round(_currentRoll, 2),
            Math.Round(_currentYaw, 2)
        );
    }

    /// Simülasyonu sıfırlar
    
    public void Reset()
    {
        _packetCounter = 1;
        _currentAltitude = 2000.0;
        _elapsedTime = 0;
        _patternStep = 0;
        _patternComplete = false;
        _currentLat = _baseLat;
        _currentLon = _baseLon;
        _currentPitch = 0;
        _currentRoll = 0;
        _currentYaw = 0;
    }
    
    /// Scale değerini değiştirir (varsayılan: 0.003 ≈ 300m)
    public void SetScale(double scale)
    {
        _scale = scale;
    }
    
    /// Hedef iniş noktasını ayarlar
    public void SetTargetLocation(double lat, double lon)
    {
        _targetLat = lat;
        _targetLon = lon;
    }
}