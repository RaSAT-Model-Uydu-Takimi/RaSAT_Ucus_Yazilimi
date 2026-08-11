namespace _10OcakRASAT.Panels
{
    /// Tüm sürüklenebilir panellerin implement etmesi gereken interface
    
    public interface IPanelContent
    {
        string PanelName { get; }
 
        
        string Description { get; }

        
        Control CreateControl();

        /// Panel kaldırıldığında temizlik yapar
        void Dispose();
    }
}
