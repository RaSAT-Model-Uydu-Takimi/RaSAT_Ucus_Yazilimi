using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace _10OcakRASAT.Managers
{
    public class SerialConnectionManager : IDisposable
    {
        private SerialPort? _serialPort;
        private CancellationTokenSource? _cancellationTokenSource;

        public delegate void DataReceivedHandler(byte[] data, int length);
        public event DataReceivedHandler? OnDataReceived;
        public event Action<string>? OnError;

        public bool IsOpen => _serialPort != null && _serialPort.IsOpen;

        public string[] GetAvailablePorts()
        {
            return SerialPort.GetPortNames();
        }

        public bool Connect(string portName, int baudRate = 115200)
        {
            try
            {
                if (IsOpen) Disconnect();

                _serialPort = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One);
                _serialPort.Open();

                _cancellationTokenSource = new CancellationTokenSource();
                Task.Run(() => ReadLoop(_cancellationTokenSource.Token));

                return true;
            }
            catch (Exception ex)
            {
                OnError?.Invoke($"Bağlantı hatası: {ex.Message}");
                return false;
            }
        }

        public void Disconnect()
        {
            if (_cancellationTokenSource != null)
            {
                _cancellationTokenSource.Cancel();
                _cancellationTokenSource = null;
            }

            if (_serialPort != null)
            {
                if (_serialPort.IsOpen)
                {
                    try
                    {
                        _serialPort.Close();
                    }
                    catch { }
                }
                _serialPort.Dispose();
                _serialPort = null;
            }
        }

        public void Write(byte[] buffer)
        {
            if (IsOpen && _serialPort != null)
            {
                try
                {
                    _serialPort.Write(buffer, 0, buffer.Length);
                }
                catch (Exception ex)
                {
                    OnError?.Invoke($"Yazma hatası: {ex.Message}");
                }
            }
        }

        private void ReadLoop(CancellationToken token)
        {
            byte[] buffer = new byte[4096];
            while (!token.IsCancellationRequested && IsOpen && _serialPort != null)
            {
                try
                {
                    if (_serialPort.BytesToRead > 0)
                    {
                        int bytesRead = _serialPort.Read(buffer, 0, buffer.Length);
                        if (bytesRead > 0)
                        {
                            // DEBUG İÇİN GEÇİCİ LOG: Gelen tüm ham baytları dosyaya yazalım
                            try {
                                string logPath = @"m:\Yedek Projeler\Projeler\RASAT-GCS(MockDatasiz)\Rasat_Com_Log.txt";
                                string hexDump = BitConverter.ToString(buffer, 0, bytesRead) + "-";
                                System.IO.File.AppendAllText(logPath, hexDump);
                            } catch {}

                            OnDataReceived?.Invoke(buffer, bytesRead);
                        }
                    }
                    else
                    {
                        Thread.Sleep(10); // İşlemciyi yormamak için kısa bir bekleme
                    }
                }
                catch (Exception ex)
                {
                    if (!token.IsCancellationRequested)
                    {
                        OnError?.Invoke($"Okuma hatası: {ex.Message}");
                        Disconnect();
                    }
                }
            }
        }

        public void Dispose()
        {
            Disconnect();
        }
    }
}
