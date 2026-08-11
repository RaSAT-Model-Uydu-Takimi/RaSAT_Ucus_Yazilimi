using System;
using System.Collections.Generic;
using System.Text;

namespace _10OcakRASAT.Managers
{
    public static class Crc8
    {
        // Standart CRC-8 (Polinom: 0x07, Initial: 0x00)
        public static byte Compute(byte[] data, int offset, int length)
        {
            byte crc = 0x00;
            for (int i = offset; i < offset + length; i++)
            {
                crc ^= data[i];
                for (int j = 0; j < 8; j++)
                {
                    if ((crc & 0x80) != 0)
                    {
                        crc = (byte)((crc << 1) ^ 0x07);
                    }
                    else
                    {
                        crc <<= 1;
                    }
                }
            }
            return crc;
        }
    }

    public class StmTelemetryParser
    {
        public const byte HEADER_BYTE = 0xAB;
        public const int PACKET_SIZE = 79;

        public delegate void DataParsedEventHandler(TData data);
        public event DataParsedEventHandler? OnDataParsed;
        public event Action? OnCrcError;

        // Parça parça gelen verileri birleştirmek için tampon (buffer)
        private List<byte> _receiveBuffer = new List<byte>();

        public void ParseBuffer(byte[] buffer, int offset, int length)
        {
            // Yeni gelen verileri ana tampona ekle
            for (int i = 0; i < length; i++)
            {
                _receiveBuffer.Add(buffer[offset + i]);
            }

            // Tamponda en az 1 paketlik veri olduğu sürece dön
            while (_receiveBuffer.Count >= PACKET_SIZE)
            {
                // Header'ı (0xAB) ara
                int headerIndex = _receiveBuffer.IndexOf(HEADER_BYTE);

                if (headerIndex == -1)
                {
                    // Tamponda hiç header yoksa, tüm tamponu çöp olarak temizle
                    _receiveBuffer.Clear();
                    break;
                }

                // Eğer header en başta değilse, header'a kadar olan kısmı çöpe at
                if (headerIndex > 0)
                {
                    _receiveBuffer.RemoveRange(0, headerIndex);
                }

                // Header baştayken, elimizde tam bir paket var mı kontrol et
                if (_receiveBuffer.Count >= PACKET_SIZE)
                {
                    // Tam bir paket var, çıkaralım
                    byte[] packet = new byte[PACKET_SIZE];
                    _receiveBuffer.CopyTo(0, packet, 0, PACKET_SIZE);

                    // CRC Kontrolü
                    byte calculatedCrc = Crc8.Compute(packet, 0, PACKET_SIZE - 1);
                    byte receivedCrc = packet[PACKET_SIZE - 1];

                    if (calculatedCrc == receivedCrc)
                    {
                        // Başarılı!
                        TData tData = ParsePacket(packet);
                        OnDataParsed?.Invoke(tData);
                        
                        // İşlenen paketi tampondan sil
                        _receiveBuffer.RemoveRange(0, PACKET_SIZE);
                    }
                    else
                    {
                        // CRC Hatalı! Gürültü olabilir, sahte bir 0xAB olabilir.
                        // Sadece o sahte header'ı silip devam edelim ki asıl header'ı bulabilelim.
                        OnCrcError?.Invoke();
                        _receiveBuffer.RemoveAt(0); 
                    }
                }
            }
        }

        private TData ParsePacket(byte[] packet)
        {
            TData data = new TData();

            int index = 1; // Header'ı atla
            
            data.PaketNumarasi = (int)BitConverter.ToUInt32(packet, index);
            index += 4;
            
            data.UyduStatusu = packet[index];
            index += 1;
            
            data.HataKodu = Encoding.ASCII.GetString(packet, index, 4);
            index += 4;
            
            string timeStr = Encoding.ASCII.GetString(packet, index, 20);
            int nullIndex = timeStr.IndexOf('\0');
            if (nullIndex >= 0) timeStr = timeStr.Substring(0, nullIndex);
            data.GondermeSaati = timeStr;
            index += 20;
            
            data.Basinc = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.Yukseklik = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.InisHizi = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.Sicaklik = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.PilGerilimi = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.GpsLatitude = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.GpsLongitude = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.GpsAltitude = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.Pitch = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.Roll = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.Yaw = BitConverter.ToSingle(packet, index);
            index += 4;
            
            data.TakimNo = BitConverter.ToUInt32(packet, index);
            index += 4;
            
            return data;
        }
    }
}
