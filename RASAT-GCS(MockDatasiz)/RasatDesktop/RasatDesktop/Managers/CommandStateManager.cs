namespace _10OcakRASAT.Managers
{
    /// <summary>
    /// Global komut durumu yöneticisi (Singleton Pattern)
    /// Son gönderilen LED komutunu saklar ve tüm uygulama genelinde erişilebilir kılar
    /// </summary>
    public class CommandStateManager
    {
        private static CommandStateManager? _instance;
        private static readonly object _lock = new object();
        
        private string _lastCommand = "0R0G0B";
        private string _lastCommandMessage = "Başlangıç Durumu: 0R0G0B";

        /// <summary>
        /// Singleton instance
        /// </summary>
        public static CommandStateManager Instance
        {
            get
            {
                if (_instance == null)
                {
                    lock (_lock)
                    {
                        if (_instance == null)
                        {
                            _instance = new CommandStateManager();
                        }
                    }
                }
                return _instance;
            }
        }

        /// <summary>
        /// Komut değiştiğinde tetiklenir
        /// </summary>
        public event Action<string, string>? OnCommandChanged;

        private CommandStateManager()
        {
            // Private constructor - Singleton pattern
        }

        /// <summary>
        /// Son gönderilen komutu günceller
        /// </summary>
        /// <param name="command">Komut (örn: 1R0G1B)</param>
        /// <param name="message">Kullanıcıya gösterilecek mesaj</param>
        public void UpdateCommand(string command, string message)
        {
            lock (_lock)
            {
                _lastCommand = command;
                _lastCommandMessage = message;
            }

            // Event'i tetikle (thread-safe)
            OnCommandChanged?.Invoke(command, message);
        }

        /// <summary>
        /// Son gönderilen komutu döndürür
        /// </summary>
        public string GetLastCommand()
        {
            lock (_lock)
            {
                return _lastCommand;
            }
        }

        /// <summary>
        /// Son komut mesajını döndürür
        /// </summary>
        public string GetLastCommandMessage()
        {
            lock (_lock)
            {
                return _lastCommandMessage;
            }
        }

        /// <summary>
        /// Komutu sıfırlar (test için)
        /// </summary>
        public void Reset()
        {
            UpdateCommand("0R0G0B", "Sıfırlandı: 0R0G0B");
        }
    }
}
