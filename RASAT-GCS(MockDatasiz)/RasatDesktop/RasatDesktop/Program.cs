using Serilog;

namespace _10OcakRASAT
{
    internal static class Program
    {
        ///  
        ///  The main entry point for the application.
        
        [STAThread]
        static void Main()
        {
            // Serilog Konfigürasyonu
            string logFileName = $"Logs/Run_{DateTime.Now:yyyyMMdd_HHmmss}.log";
            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File(logFileName, rollingInterval: RollingInterval.Infinite)
                .CreateLogger();

            try
            {
                Log.Information("Rasat-GCS Uygulaması Başlatılıyor...");
                
                // To customize application configuration such as set high DPI settings or default font,
                // see https://aka.ms/applicationconfiguration.
                ApplicationConfiguration.Initialize();
                Application.Run(new Form1());
            }
            catch (Exception ex)
            {
                Log.Fatal(ex, "Uygulama beklenmedik bir şekilde çöktü!");
            }
            finally
            {
                Log.Information("Rasat-GCS Uygulaması Kapatılıyor...");
                Log.CloseAndFlush();
            }
        }
    }
}