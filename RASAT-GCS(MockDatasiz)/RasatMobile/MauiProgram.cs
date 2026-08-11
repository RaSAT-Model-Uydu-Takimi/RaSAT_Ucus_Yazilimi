using Microsoft.Extensions.Logging;
using SkiaSharp.Views.Maui.Controls;
using SkiaSharp.Views.Maui.Controls.Hosting;
using RASATMobile.ViewModels;

namespace RASATMobile;

public static class MauiProgram
{
	public static MauiApp CreateMauiApp()
	{
		var builder = MauiApp.CreateBuilder();
		builder
            .UseMauiApp<App>()
            .UseSkiaSharp() // Mapsui için gerekli
            .ConfigureFonts(fonts =>
			{
				fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
				fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
			});


#if DEBUG
		builder.Logging.AddDebug();
#endif
        
        // Services
        builder.Services.AddSingleton<Services.FirebaseService>();
        
        // Views & ViewModels
        builder.Services.AddTransient<MainViewModel>();
        builder.Services.AddTransient<MainPage>();

		return builder.Build();
	}
}
