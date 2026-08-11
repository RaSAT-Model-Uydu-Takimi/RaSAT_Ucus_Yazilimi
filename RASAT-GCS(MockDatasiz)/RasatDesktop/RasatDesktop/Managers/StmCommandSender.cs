using System;
using System.IO.Ports;

namespace _10OcakRASAT.Managers
{
    public class StmCommandSender
    {
        public const byte HEADER_BYTE = 0xAA;

        // Komut Tipleri (Yeni Liste)
        public const byte CMD_HEARTBEAT = 0x00;
        public const byte CMD_AYRILMA = 0x01;
        public const byte CMD_PARASUT = 0x02;
        public const byte CMD_MESSAGE = 0x03;
        public const byte CMD_IOT = 0x04;

        private byte _nextCommand = CMD_HEARTBEAT;
        private byte[] _nextParams = new byte[3] { 0, 0, 0 };

        private readonly SerialConnectionManager _serialManager;

        public StmCommandSender(SerialConnectionManager serialManager)
        {
            _serialManager = serialManager;
        }

        // Bir butona basıldığında tek seferlik yollanacak komutu ayarlar
        public void SetNextCommand(byte command, byte[] parameters)
        {
            _nextCommand = command;
            if (parameters != null && parameters.Length >= 3)
            {
                _nextParams[0] = parameters[0];
                _nextParams[1] = parameters[1];
                _nextParams[2] = parameters[2];
            }
            else
            {
                _nextParams[0] = 0;
                _nextParams[1] = 0;
                _nextParams[2] = 0;
            }
        }

        // STM32'den her telemetri paketi geldiğinde bu metot çağrılacak (Ping-Pong)
        public void SendCurrentCommand()
        {
            if (!_serialManager.IsOpen) return;

            byte[] packet = new byte[6];
            packet[0] = HEADER_BYTE;
            packet[1] = _nextCommand;
            packet[2] = _nextParams[0]; // R
            packet[3] = _nextParams[1]; // G
            packet[4] = _nextParams[2]; // B
            
            // İlk 5 byte'ın CRC'si (Header(1) + Komut(1) + Param(3))
            packet[5] = Crc8.Compute(packet, 0, 5);

            _serialManager.Write(packet);

            // Komut gönderildikten sonra eski haline (Heartbeat) dön
            if (_nextCommand != CMD_HEARTBEAT && _nextCommand != CMD_IOT)
            {
                _nextCommand = CMD_HEARTBEAT;
                _nextParams[0] = 0;
                _nextParams[1] = 0;
                _nextParams[2] = 0;
            }
            // Not: IOT komutu sürekli güncel LED durumunu göndermeye devam edebilir,
            // Veya sadece bir butona basıldığında gider. Ping-Pong'da sadece gerektiğinde atmayı hedefler.
            else if (_nextCommand == CMD_IOT)
            {
                 _nextCommand = CMD_HEARTBEAT;
                 _nextParams[0] = 0;
                 _nextParams[1] = 0;
                 _nextParams[2] = 0;
            }
        }
    }
}
