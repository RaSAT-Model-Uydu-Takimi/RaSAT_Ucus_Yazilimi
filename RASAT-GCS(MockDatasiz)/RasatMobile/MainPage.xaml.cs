using Mapsui;
using Mapsui.Extensions;
using Mapsui.Layers;
using Mapsui.Projections;
using Mapsui.Styles;
using Mapsui.Tiling;
using Mapsui.UI.Maui;
using Mapsui.Nts;
using NetTopologySuite.Geometries;
using RASATMobile.Models;
using RASATMobile.ViewModels;
using System.Collections.Concurrent;
using System.Collections.Specialized;
using MColor = Mapsui.Styles.Color;
using MBrush = Mapsui.Styles.Brush;
using MPen = Mapsui.Styles.Pen;
using Point = NetTopologySuite.Geometries.Point;

namespace RASATMobile
{
    public partial class MainPage : ContentPage
    {
        private readonly MainViewModel _viewModel;
        private MemoryLayer? _routeLayer;
        private MemoryLayer? _pinLayer;
        private MemoryLayer? _deviceLocationLayer;
        private bool _isMapInitialized = false;
        private System.Timers.Timer? _locationTimer;
        private MPoint? _deviceLocation;

        // Optimization: Local caches to update incrementally
        private readonly List<Coordinate> _routeCoordinates = new();
        private readonly List<MPoint> _pinPoints = new();

        public MainPage(MainViewModel viewModel)
        {
            InitializeComponent();
            _viewModel = viewModel;
            BindingContext = _viewModel;

            TelemetryMap.Loaded += TelemetryMap_Loaded;
            
            _viewModel.PropertyChanged += ViewModel_PropertyChanged;
            _viewModel.AllTelemetryData.CollectionChanged += AllTelemetryData_CollectionChanged;
        }

        private void TelemetryMap_Loaded(object? sender, EventArgs e)
        {
            if (_isMapInitialized) return;
            InitializeMap();
            _isMapInitialized = true;
        }

        private void InitializeMap()
        {
            var map = new Mapsui.Map();
            map.Layers.Add(OpenStreetMap.CreateTileLayer());

            _routeLayer = CreateRouteLayer();
            _pinLayer = CreatePinLayer();
            _deviceLocationLayer = CreateDeviceLocationLayer();

            map.Layers.Add(_routeLayer);
            map.Layers.Add(_pinLayer);
            map.Layers.Add(_deviceLocationLayer);

            TelemetryMap.Map = map;
            
            // Mapsui varsayılan UI kontrollerini gizle
            TelemetryMap.Map.Widgets.Clear();

            // Load initial data
            ProcessFullTelemetryList();
            RefreshLayers();
            
            // Cihaz konumu takibini başlat
            StartLocationTracking();
        }

        private MemoryLayer CreateRouteLayer()
        {
            return new MemoryLayer
            {
                Name = "RouteLayer",
                Style = new VectorStyle
                {
                    Line = new MPen { Color = MColor.Cyan, Width = 4 }
                }
            };
        }

        private MemoryLayer CreatePinLayer()
        {
            return new MemoryLayer
            {
                Name = "PinLayer",
                Style = null
            };
        }

        private MemoryLayer CreateDeviceLocationLayer()
        {
            return new MemoryLayer
            {
                Name = "DeviceLocationLayer",
                Style = null
            };
        }

        #region Device Location Tracking

        private async void StartLocationTracking()
        {
            // İlk konumu al
            await UpdateDeviceLocationAsync();
            
            // 5 saniyede bir konumu güncelle
            _locationTimer = new System.Timers.Timer(5000);
            _locationTimer.Elapsed += async (s, e) => await UpdateDeviceLocationAsync();
            _locationTimer.AutoReset = true;
            _locationTimer.Start();
        }

        private async Task UpdateDeviceLocationAsync()
        {
            try
            {
                var status = await Permissions.CheckStatusAsync<Permissions.LocationWhenInUse>();
                if (status != PermissionStatus.Granted)
                {
                    status = await MainThread.InvokeOnMainThreadAsync(async () =>
                        await Permissions.RequestAsync<Permissions.LocationWhenInUse>());
                    
                    if (status != PermissionStatus.Granted)
                        return;
                }

                var location = await Geolocation.GetLocationAsync(new GeolocationRequest
                {
                    DesiredAccuracy = GeolocationAccuracy.Best,
                    Timeout = TimeSpan.FromSeconds(10)
                });

                if (location != null)
                {
                    var mercator = SphericalMercator.FromLonLat(location.Longitude, location.Latitude);
                    _deviceLocation = new MPoint(mercator.x, mercator.y);
                    
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        UpdateDeviceLocationOnMap();
                    });
                }
            }
            catch (Exception)
            {
                // Konum alınamadı - sessizce devam et
            }
        }

        private void UpdateDeviceLocationOnMap()
        {
            if (_deviceLocation == null || _deviceLocationLayer == null) return;

            var deviceFeature = new PointFeature(_deviceLocation)
            {
                Styles = new[]
                {
                    new SymbolStyle
                    {
                        Fill = new MBrush(new MColor(0, 120, 255, 255)), // Mavi
                        Outline = new MPen(MColor.White, 3),
                        SymbolScale = 1.0f
                    }
                }
            };

            _deviceLocationLayer.Features = new[] { deviceFeature };
            TelemetryMap.Refresh();
        }

        #endregion

        private void ViewModel_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(MainViewModel.MapMode))
            {
                UpdateCamera((int)_viewModel.MapMode);
            }
        }

        private void AllTelemetryData_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                if (TelemetryMap.Map == null) return;

                bool dataChanged = false;

                if (e.Action == NotifyCollectionChangedAction.Add && e.NewItems != null)
                {
                    // Efektif Ekleme: Sadece yeni gelenleri işle
                    foreach (TData newItem in e.NewItems)
                    {
                        if (TryGetCoordinates(newItem, out var coord, out var mPoint))
                        {
                            _routeCoordinates.Add(coord);
                            _pinPoints.Add(mPoint);
                            dataChanged = true;
                        }
                    }
                }
                else if (e.Action == NotifyCollectionChangedAction.Reset)
                {
                    // Reset durumunda her şeyi temizle
                    _routeCoordinates.Clear();
                    _pinPoints.Clear();
                    ProcessFullTelemetryList();
                    dataChanged = true;
                }
                
                if (dataChanged)
                {
                    RefreshLayers();
                    
                    // Follow modundaysa kamerayı ortala
                    if (_viewModel.MapMode == 1) // Follow
                    {
                        UpdateCamera(1);
                    }
                }
            });
        }

        private void ProcessFullTelemetryList()
        {
            // Initial load or full refresh
            foreach (var d in _viewModel.AllTelemetryData)
            {
                if (TryGetCoordinates(d, out var coord, out var mPoint))
                {
                    _routeCoordinates.Add(coord);
                    _pinPoints.Add(mPoint);
                }
            }
        }

        private bool TryGetCoordinates(TData d, out Coordinate coord, out MPoint mPoint)
        {
            if (d.GpsLatitude != 0 && d.GpsLongitude != 0)
            {
                var mercator = SphericalMercator.FromLonLat(d.GpsLongitude, d.GpsLatitude);
                coord = new Coordinate(mercator.x, mercator.y);
                mPoint = new MPoint(mercator.x, mercator.y);
                return true;
            }
            coord = new Coordinate();
            mPoint = new MPoint();
            return false;
        }

        private void RefreshLayers()
        {
            if (_routeCoordinates.Count < 2) return;

            // Update Route
            var lineString = new LineString(_routeCoordinates.ToArray());
            var lineFeature = new GeometryFeature { Geometry = lineString };
            if (_routeLayer != null)
                _routeLayer.Features = new[] { lineFeature };

            // Update Pins (Start/End)
            var features = new List<IFeature>();
            if (_pinPoints.Any())
            {
                features.Add(CreatePinFeature(_pinPoints.First(), MColor.Green)); // Start
                if (_pinPoints.Count > 1)
                {
                    features.Add(CreatePinFeature(_pinPoints.Last(), MColor.Red));   // End
                }
            }
            if (_pinLayer != null)
                _pinLayer.Features = features;

            TelemetryMap.Refresh(); // Ekrana çiz
        }

        private PointFeature CreatePinFeature(MPoint point, MColor color)
        {
            return new PointFeature(point)
            {
                Styles = new[]
                {
                    new SymbolStyle
                    {
                        Fill = new MBrush(color),
                        Outline = new MPen(MColor.White, 2),
                        SymbolScale = 0.8f
                    }
                }
            };
        }

        private void UpdateCamera(int mode)
        {
            var navigator = TelemetryMap.Map?.Navigator;
            if (navigator == null || !_pinPoints.Any()) return;

            switch (mode)
            {
                case 0: // Full View
                    if (_pinPoints.Count == 0) return;
                    
                    var minX = _pinPoints.Min(p => p.X);
                    var maxX = _pinPoints.Max(p => p.X);
                    var minY = _pinPoints.Min(p => p.Y);
                    var maxY = _pinPoints.Max(p => p.Y);

                    var width = maxX - minX;
                    var height = maxY - minY;
                    
                    if (width < 1 && height < 1) // Tek nokta veya çok yakın
                    {
                        navigator.CenterOn(_pinPoints[0]);
                        navigator.ZoomTo(10);
                    }
                    else
                    {
                        var paddingX = width * 0.1;
                        var paddingY = height * 0.1;
                        var bounds = new MRect(minX - paddingX, minY - paddingY, maxX + paddingX, maxY + paddingY);
                        navigator.ZoomToBox(bounds);
                    }
                    break;

                case 1: // Follow
                    if (!_pinPoints.Any()) return;
                    var lastPoint = _pinPoints.Last();
                    navigator.CenterOn(lastPoint);
                    // Keep existing zoom level unless it is too far out? 
                    // Let's just center to keep it simple and unintrusive as user zooms himself usually.
                    // Or ensure minimum zoom.
                    break;

                case 2: // Inspect
                    // Do nothing, let user roam
                    break;
            }
        }
    }
}
