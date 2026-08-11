using System.Windows.Forms.Integration;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using HelixToolkit.Wpf;
using _10OcakRASAT.Managers;
using Serilog;

namespace _10OcakRASAT.Panels
{
    ///  
    /// HelixToolkit ile 3D silindir model görüntüleyici
    /// Pitch, Roll, Yaw değerlerine göre döner
    
    public class Model3DPanel : IPanelContent
    {
        private readonly TelemetryManager _telemetryManager;

        private System.Windows.Forms.Panel? _container;
        private ElementHost? _elementHost;
        private HelixViewport3D? _viewport;
        private ModelVisual3D? _cylinderModel;
        private RotateTransform3D? _rotateTransform;
        private QuaternionRotation3D? _rotation;

        public string PanelName => "🛰️ 3D Model";
        public string Description => "Uydu 3D silindir modeli (Pitch/Roll/Yaw)";

        public Model3DPanel(TelemetryManager telemetryManager)
        {
            _telemetryManager = telemetryManager;
        }

        public Control CreateControl()
        {
            _container = new System.Windows.Forms.Panel
            {
                Dock = DockStyle.Fill,
                BackColor = System.Drawing.Color.FromArgb(28, 28, 28)
            };

            // ElementHost ile WPF kontrolü gömme
            _elementHost = new ElementHost
            {
                Dock = DockStyle.Fill,
                BackColor = System.Drawing.Color.Transparent
            };

            // HelixViewport3D oluştur
            _viewport = new HelixViewport3D
            {
                Background = new SolidColorBrush(System.Windows.Media.Color.FromRgb(28, 28, 28)),
                ShowCoordinateSystem = true,
                ShowViewCube = true,
                ZoomExtentsWhenLoaded = true
            };

            // Işıklandırma
            SetupLighting();

            // Silindir modeli oluştur
            CreateCylinderModel();

            _elementHost.Child = _viewport;
            _container.Controls.Add(_elementHost);
            
            LoadLastState();

            // Telemetry event'e abone ol
            _telemetryManager.OnDataReceived += OnNewData;

            return _container;
        }

        private void LoadLastState()
        {
            var lastData = _telemetryManager.DataHistory.LastOrDefault();
            if (lastData != null)
            {
                Log.Information("Model3DPanel: Geçmişteki son duruma dönülüyor.");
                UpdateRotation(lastData);
            }
        }

        private void SetupLighting()
        {
            if (_viewport == null) return;

            // Ambient ışık
            var ambientLight = new AmbientLight(System.Windows.Media.Color.FromRgb(80, 80, 80));
            _viewport.Children.Add(new ModelVisual3D { Content = ambientLight });

            // Directional ışık
            var directionalLight = new DirectionalLight(
                System.Windows.Media.Colors.White,
                new Vector3D(-1, -1, -1));
            _viewport.Children.Add(new ModelVisual3D { Content = directionalLight });

            // İkinci directional ışık (karşı taraf)
            var directionalLight2 = new DirectionalLight(
                System.Windows.Media.Color.FromRgb(150, 150, 150),
                new Vector3D(1, 1, 1));
            _viewport.Children.Add(new ModelVisual3D { Content = directionalLight2 });
        }

        private void SetupCamera()
        {
            if (_viewport == null) return;
            
            // Kullanıcı talebi üzerine NWU (North-West-Up) Sağ El Kuralı'na geçildi!
            // Eksen Göstergesini (Widget) açık tutuyoruz, NWU sağ el kuralıdır ve widget ile kusursuz eşleşir.
            _viewport.ShowCoordinateSystem = true;

            // Kamera -X'den (Arkadan) +X'e (İleriye) doğru bakıyor.
            // NWU: Z ekseni YUKARI (+Z Up) kabul edilir.
            // Bu yüzden Ekranın "Yukarı" (UpDirection) vektörü standart +Z olmalıdır.
            _viewport.Camera = new PerspectiveCamera
            {
                Position = new Point3D(-7, 0, 0),        // Arkadan
                LookDirection = new Vector3D(7, 0, 0),   // İleriye (+X'e) doğru bak
                UpDirection = new Vector3D(0, 0, 1),     // Ekranın üstü +Z
                FieldOfView = 45
            };
        }

        private void CreateCylinderModel()
        {
            if (_viewport == null) return;

            var transformGroup = new Model3DGroup();
            var droneBuilder = new MeshBuilder();

            // 1. Ana Gövde (Yükseklik = 3, Çap = 1 -> Yarıçap = 0.5)
            // NWU sisteminde +Z YUKARIDIR. Gövdenin altı -1.5, tepesi +1.5 konumundadır.
            droneBuilder.AddCylinder(
                new Point3D(0, 0, -1.5),   // Alt merkez
                new Point3D(0, 0, 1.5),    // Üst merkez
                0.5,                       // Yarıçap
                36);                       // Segment

            // 2. Kollar (4 Çapraz Kol)
            // Kollar silindirin üst kısmına yerleştirilecek. Üst merkez +1.5. Kolları +1.0 seviyesine koyalım.
            droneBuilder.AddBox(new Point3D(0, 0, 1.0), 2.4, 0.1, 0.1); // X ekseni yatay çubuğu
            droneBuilder.AddBox(new Point3D(0, 0, 1.0), 0.1, 2.4, 0.1); // Y ekseni yatay çubuğu
            
            MeshGeometry3D baseMesh = droneBuilder.ToMesh();

            // Gövde materyali (Alüminyum Gri)
            var droneMaterial = new MaterialGroup();
            droneMaterial.Children.Add(new DiffuseMaterial(new SolidColorBrush(System.Windows.Media.Color.FromRgb(150, 150, 150))));
            droneMaterial.Children.Add(new SpecularMaterial(new SolidColorBrush(System.Windows.Media.Colors.White), 80));

            var droneModel = new GeometryModel3D(baseMesh, droneMaterial);
            droneModel.BackMaterial = droneMaterial;
            
            // Kolların tam çapraz (X) durması için Z ekseni etrafında 45 derece döndürelim
            var rotate45 = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(0, 0, 1), 45));
            droneModel.Transform = rotate45;
            
            transformGroup.Children.Add(droneModel);

            // Dinamik Rotasyon Transformu
            _rotation = new QuaternionRotation3D();
            _rotateTransform = new RotateTransform3D(_rotation);

            var modelVisual = new ModelVisual3D { Content = transformGroup };
            modelVisual.Transform = _rotateTransform;

            _cylinderModel = modelVisual;
            _viewport.Children.Add(_cylinderModel);

            // Referans eksenler
            AddReferenceAxes();
        }

        private void AddReferenceAxes()
        {
            if (_viewport == null) return;

            // TAM STANDART NWU SİSTEMİ:
            // X (Kırmızı) = North (İleri)
            // Y (Yeşil) = West (Sol)
            // Z (Mavi) = Up (Yukarı)

            // Kırmızı Eksen (+X) = İLERİ (Forward)
            var xAxis = new LinesVisual3D { Color = System.Windows.Media.Colors.Red, Thickness = 3 };
            xAxis.Points.Add(new Point3D(0, 0, 0));
            xAxis.Points.Add(new Point3D(4, 0, 0)); 
            _viewport.Children.Add(xAxis);

            // Yeşil Eksen (+Y) = SOL (Left)
            var yAxis = new LinesVisual3D { Color = System.Windows.Media.Colors.Green, Thickness = 3 };
            yAxis.Points.Add(new Point3D(0, 0, 0));
            yAxis.Points.Add(new Point3D(0, 4, 0)); 
            _viewport.Children.Add(yAxis);

            // Mavi Eksen (+Z) = YUKARI (Up)
            var zAxis = new LinesVisual3D { Color = System.Windows.Media.Colors.Blue, Thickness = 3 };
            zAxis.Points.Add(new Point3D(0, 0, 0));
            zAxis.Points.Add(new Point3D(0, 0, 4)); 
            _viewport.Children.Add(zAxis);
        }

        private void OnNewData(TData data)
        {
            if (_container?.InvokeRequired == true)
            {
                _container.Invoke(new Action(() => UpdateRotation(data)));
            }
            else
            {
                UpdateRotation(data);
            }
        }

        private void UpdateRotation(TData data)
        {
            if (_rotation == null) return;

            // NWU Sistemi Telemetri Eşleşmesi:
            // Roll  (Yatma)     -> İleri eksen (X) etrafında döner
            // Pitch (Yunuslama) -> Sol eksen (Y) etrafında döner
            // Yaw   (Sapma)     -> Yukarı eksen (Z) etrafında döner
            
            var qRoll  = new System.Windows.Media.Media3D.Quaternion(new Vector3D(1, 0, 0), data.Roll);
            var qPitch = new System.Windows.Media.Media3D.Quaternion(new Vector3D(0, 1, 0), data.Pitch);
            var qYaw   = new System.Windows.Media.Media3D.Quaternion(new Vector3D(0, 0, 1), data.Yaw);

            // Havacılık dönüş sırası (Yaw -> Pitch -> Roll)
            _rotation.Quaternion = qYaw * qPitch * qRoll;
        }

        public void Dispose()
        {
            Log.Information("Model3DPanel kapatılıyor...");
            _telemetryManager.OnDataReceived -= OnNewData;
            _elementHost?.Dispose();
            _container?.Dispose();
        }
    }
}
