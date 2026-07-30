using System;

namespace FilterModule
{
    public struct SensorData
    {
        public float rawValue;        // Fiziksel ölçüm veya kestirim değeri (SI standart biriminde)
        public float calibratedValue;  // Kalibrasyon, ofset ve termal telafi uygulanmış değer
        public float filteredValue;    // Filtrelenmiş Veri
        public float confidence;       // Bu veriye özel anlık güven skoru [0-1]
    }

    public struct DoubleSensorData
    {
        public double rawValue;        // Fiziksel ölçüm veya kestirim değeri (SI standart biriminde)
        public double calibratedValue;  // Kalibrasyon, ofset ve termal telafi uygulanmış değer
        public double filteredValue;    // Filtrelenmiş Veri
        public float confidence;       // Bu veriye özel anlık güven skoru [0-1]
    }

    public struct DerivedData
    {
        public float value;         // Farklı sensorlerden gelen total verinin kestirimi
        public float confidence;    // Bu veriye özel anlık güven skoru [0-1]
    }

    public struct Acc
    {
        public SensorData x;       // X ekseni ivmesi [g]
        public SensorData y;       // Y ekseni ivmesi [g]
        public SensorData z;       // Z ekseni ivmesi [g]
        public uint UpdateTime;    // Son güncelleme zamanı (us)
    }

    public struct Gyro
    {
        public SensorData x;       // X ekseni açısal hızı [dps]
        public SensorData y;       // Y ekseni açısal hızı [dps]
        public SensorData z;       // Z ekseni açısal hızı [dps]
        public uint UpdateTime;    // Son güncelleme zamanı (us)
    }

    public struct Mag
    {
        public SensorData x;        // X ekseni manyetik alan [uT]
        public SensorData y;        // Y ekseni manyetik alan [uT]
        public SensorData z;        // Z ekseni manyetik alan [uT]
        public uint UpdateTime;     // Son güncelleme zamanı (us)
    }

    public struct Gps
    {
        public DoubleSensorData x;       // Enlem [°]
        public DoubleSensorData y;       // Boy
        public SensorData z;             // Yükseklik [m]
        public SensorData course;        // Rota
        public float HDOP;               // HDOP = Horizontal Dilution of Precision - Yatay Hassasiyet Çarpanı
        public byte satelliteCount;      // Görünen uydu sayısı
        public byte fixQuality;          // 0: Fix Yok, 1: GPS Fix, 2: DGPS Fix
        public SensorData speed;         // Yere göre hız [m/s]
        public uint UpdateTime;          // Son güncelleme zamanı (us)
    }

    public struct Baro
    {
        public SensorData press; // Basınç [Pa]
        public SensorData temp;  // Sıcaklık [°C]
        public uint UpdateTime;  // Son güncelleme zamanı (us)
    }

    public struct Batt
    {
        public SensorData battVolt; // Batarya gerilimi [V]
        public SensorData battCurr; // Batarya akımı [A]
        public uint UpdateTime;     // Son güncelleme zamanı (us)
    }

    public struct EstimatedDatas
    {
        public DerivedData pos_x; // X ekseni konum [m]
        public DerivedData pos_y; // Y ekseni konum [m]  
        public DerivedData pos_z; // Z ekseni konum [m] 

        public DerivedData vel_x; // X ekseni hız [m/s]
        public DerivedData vel_y; // Y ekseni hız [m/s]
        public DerivedData vel_z; // Z ekseni hız [m/s]

        public DerivedData a_x; // X ekseni ivme [m/s^2]
        public DerivedData a_y; // Y ekseni ivme [m/s^2]
        public DerivedData a_z; // Z ekseni ivme [m/s^2]
        
        // Dünya ekseni (NED) ivmeleri (M7 modülünden gelir)
        public DerivedData earth_a_x;
        public DerivedData earth_a_y;
        public DerivedData earth_a_z;

        public DerivedData q0; // Kuaterniyon w (real)
        public DerivedData q1; // Kuaterniyon x
        public DerivedData q2; // Kuaterniyon y
        public DerivedData q3; // Kuaterniyon z
        
        public DerivedData pitch; // Pitch açısı [°]
        public DerivedData roll;  // Roll açısı [°]
        public DerivedData yaw;   // Yaw açısı [°]
    }

    public struct DataCenter
    {
        public Acc acc;                   // 3 Eksenli İvmeölçer (Accelerometer) Kanalları
        public Gyro gyro;                 // 3 Eksenli Jiroskop (Gyroscope) Kanalları
        public Mag mag;                   // 3 Eksenli Manyetometre (Magnetometer) Kanalları
        public Baro baro;                 // İrtifa Sensörü Kanalları
        public Gps gps;                   // GPS ve Navigasyon Kanalları
        public Batt batt;                 // Batarya Kanalları

        public EstimatedDatas estimated;   // Türetilmiş Veriler
    }
}
