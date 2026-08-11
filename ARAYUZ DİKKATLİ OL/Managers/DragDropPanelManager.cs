using _10OcakRASAT.Panels;
using Serilog;

namespace _10OcakRASAT.Managers
{
    ///  
    /// Sürükle-bırak panel yöneticisi
    /// Tüm sürükle-bırak mantığını bu sınıf yönetir
    
    public class DragDropPanelManager
    {
        private readonly ListBox _panelList;
        private readonly List<GroupBox> _targetContainers;
        private readonly Dictionary<GroupBox, IPanelContent?> _placedPanels;
        
        private PanelFactory.PanelType? _draggedPanelType;

        /// <summary>
        /// Bir panel başarılı bir şekilde eklendiğinde tetiklenir
        /// </summary>
        public event Action<IPanelContent>? OnPanelDropped;

        ///  
        /// Manager'ı oluşturur
        
        /// <param name="panelList">Sol taraftaki panel listesi</param>
        /// <param name="targetContainers">Hedef GroupBox'lar</param>
        public DragDropPanelManager(ListBox panelList, List<GroupBox> targetContainers)
        {
            _panelList = panelList;
            _targetContainers = targetContainers;
            _placedPanels = new Dictionary<GroupBox, IPanelContent?>();

            Initialize();
        }

        ///  
        /// Event bağlantılarını kurar
        
        private void Initialize()
        {
            // Panel listesini doldur
            PopulatePanelList();

            // Panel listesi sürükleme eventleri
            _panelList.MouseDown += PanelList_MouseDown;

            // Hedef container'lar için drop eventleri
            foreach (var container in _targetContainers)
            {
                container.AllowDrop = true;
                container.DragEnter += Container_DragEnter;
                container.DragDrop += Container_DragDrop;
                container.DragLeave += Container_DragLeave;
                _placedPanels[container] = null;
            }
        }

        ///  
        /// Panel listesini doldurur
        
        private void PopulatePanelList()
        {
            _panelList.Items.Clear();
            _panelList.DisplayMember = "Name";

            foreach (var panelInfo in PanelFactory.GetAvailablePanels())
            {
                _panelList.Items.Add(panelInfo);
            }
        }

  
        // Liste üzerinde mouse basıldığında sürükleme başlat
        
        private void PanelList_MouseDown(object? sender, MouseEventArgs e)
        {
            if (_panelList.SelectedItem is PanelTypeInfo panelInfo)
            {
                _draggedPanelType = panelInfo.Type;
                _panelList.DoDragDrop(panelInfo, DragDropEffects.Copy);
            }
        }

        ///  
        /// Container üzerine sürükleme girdiğinde
        
        private void Container_DragEnter(object? sender, DragEventArgs e)
        {
            if (e.Data?.GetDataPresent(typeof(PanelTypeInfo)) == true)
            {
                e.Effect = DragDropEffects.Copy;
                
                // Görsel geri bildirim
                if (sender is GroupBox gb)
                {
                    gb.BackColor = Color.FromArgb(50, 70, 50); // Yeşilimsi 
                }
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        // Container'dan çıkıldığında
        
        private void Container_DragLeave(object? sender, EventArgs e)
        {
            if (sender is GroupBox gb)
            {
                gb.BackColor = Color.FromArgb(28, 28, 28); // Orijinal renge dön
            }
        }
  
        // Container üzerine bırakıldığında panel yerleştir
        
        private void Container_DragDrop(object? sender, DragEventArgs e)
        {
            if (sender is not GroupBox targetBox) return;

            // Rengi geri al
            targetBox.BackColor = Color.FromArgb(28, 28, 28);

            // Data'yı al
            if (e.Data?.GetData(typeof(PanelTypeInfo)) is not PanelTypeInfo panelInfo) return;

            // Önceki paneli temizle
            RemovePanelFromContainer(targetBox);

            // Yeni panel oluştur ve yerleştir
            var panel = PanelFactory.CreatePanel(panelInfo.Type);
            var control = panel.CreateControl();

            Log.Information($"Drag&Drop: {panel.PanelName} paneli {targetBox.Name} üzerine yerleştirildi.");

            targetBox.Controls.Add(control);
            control.BringToFront();

            _placedPanels[targetBox] = panel;
            
            // Panel başarıyla bırakıldığında event fırlat
            OnPanelDropped?.Invoke(panel);
        }

        /// Container'dan paneli kaldırır
        
        public void RemovePanelFromContainer(GroupBox container)
        {
            if (_placedPanels.TryGetValue(container, out var existingPanel) && existingPanel != null)
            {
                existingPanel.Dispose();
                _placedPanels[container] = null;
            }

            // Tüm child kontrolleri temizle
            var controlsToRemove = container.Controls.Cast<Control>().ToList();
            foreach (var ctrl in controlsToRemove)
            {
                container.Controls.Remove(ctrl);
                ctrl.Dispose();
            }
        }

        ///  
        /// Tüm panelleri temizler
        
        public void ClearAllPanels()
        {
            foreach (var container in _targetContainers)
            {
                RemovePanelFromContainer(container);
            }
        }
    }
}
