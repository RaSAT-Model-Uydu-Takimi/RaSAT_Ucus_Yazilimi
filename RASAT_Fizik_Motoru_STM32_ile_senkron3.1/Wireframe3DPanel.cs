using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using RASAT_Fizik_Motoru_STM32_ile_senkron3._1;

public class Wireframe3DPanel : Panel
{
    private Kuaterniyon_t _q = Kuaterniyon_t.BirimKuaterniyon;
    
    private class Face3D
    {
        public Vektor_t[] Vertices;
        public Color BaseColor;
    }
    
    private List<Face3D> _modelFaces = new List<Face3D>();

    private readonly Font _titleFont = new Font("Segoe UI", 8.5f, FontStyle.Bold);
    private readonly Brush _titleBrush = new SolidBrush(Color.FromArgb(200, 200, 200));

    public Wireframe3DPanel()
    {
        this.DoubleBuffered = true;
        this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer, true);
        this.BackColor = Color.FromArgb(20, 20, 25);
        
        BuildModel();
    }
    
    private void BuildModel()
    {
        // Merkezi Gövde (Silindir)
        double R = 18;
        double H = 35;
        int segs = 16;
        Vektor_t[] topV = new Vektor_t[segs];
        Vektor_t[] botV = new Vektor_t[segs];
        
        for (int i = 0; i < segs; i++) 
        {
            double a = i * Math.PI * 2 / segs;
            topV[i] = new Vektor_t(R * Math.Cos(a), R * Math.Sin(a), H, Kordinat_Sistemi_t.GOVDE_BODY);
            botV[i] = new Vektor_t(R * Math.Cos(a), R * Math.Sin(a), -H, Kordinat_Sistemi_t.GOVDE_BODY);
        }
        
        // Üst Yüzey (CCW)
        _modelFaces.Add(new Face3D { Vertices = topV, BaseColor = Color.Silver });
        // Alt Yüzey (CW -> normali dışarı)
        Vektor_t[] botRev = new Vektor_t[segs];
        for (int i = 0; i < segs; i++) botRev[i] = botV[segs - 1 - i];
        _modelFaces.Add(new Face3D { Vertices = botRev, BaseColor = Color.Silver });
        
        // Yan Yüzeyler (Altın / Kapton Bant Rengi)
        for (int i = 0; i < segs; i++) 
        {
            int n = (i + 1) % segs;
            _modelFaces.Add(new Face3D { 
                Vertices = new Vektor_t[] { topV[i], botV[i], botV[n], topV[n] },
                BaseColor = Color.FromArgb(200, 150, 20) 
            });
        }
        
        // 4 Kanat (Güneş Panelleri X Şeklinde)
        double[] angles = { Math.PI / 4, 3 * Math.PI / 4, 5 * Math.PI / 4, 7 * Math.PI / 4 };
        double pLen = 65; // Kanat uzunluğu
        double pWid = 25; // Kanat genişliği
        double pThick = 2; // Kanat kalınlığı
        double offset = R - 2; // Gövdeden başlangıç
        
        foreach (double a in angles) 
        {
            Vektor_t fwd = new Vektor_t(Math.Cos(a), Math.Sin(a), 0, Kordinat_Sistemi_t.GOVDE_BODY);
            Vektor_t right = new Vektor_t(Math.Cos(a - Math.PI / 2), Math.Sin(a - Math.PI / 2), 0, Kordinat_Sistemi_t.GOVDE_BODY);
            Vektor_t up = new Vektor_t(0, 0, 1, Kordinat_Sistemi_t.GOVDE_BODY);
            
            // Kanatları gövdenin ortası yerine üst yüzeye yakın bir hizaya taşıyoruz (H - 3)
            Vektor_t center = fwd * (offset + pLen / 2) + up * (H - 3);
            AddBox(center, fwd, right, up, pLen, pWid, pThick);
        }
    }
    
    private void AddBox(Vektor_t center, Vektor_t fwd, Vektor_t right, Vektor_t up, double L, double W, double T)
    {
        Vektor_t[] c = new Vektor_t[8];
        c[0] = center + fwd * (L / 2) + right * (W / 2) + up * (T / 2);
        c[1] = center + fwd * (L / 2) - right * (W / 2) + up * (T / 2);
        c[2] = center - fwd * (L / 2) - right * (W / 2) + up * (T / 2);
        c[3] = center - fwd * (L / 2) + right * (W / 2) + up * (T / 2);
        
        c[4] = center + fwd * (L / 2) + right * (W / 2) - up * (T / 2);
        c[5] = center + fwd * (L / 2) - right * (W / 2) - up * (T / 2);
        c[6] = center - fwd * (L / 2) - right * (W / 2) - up * (T / 2);
        c[7] = center - fwd * (L / 2) + right * (W / 2) - up * (T / 2);
        
        Color pColor = Color.FromArgb(20, 40, 100); // Panel üstü (Lacivert)
        Color pBack = Color.FromArgb(50, 50, 50);   // Panel altı (Gri)
        Color edge = Color.Silver;                  // Kenarlar (Gümüş)
        
        _modelFaces.Add(new Face3D { Vertices = new Vektor_t[] { c[0], c[1], c[2], c[3] }, BaseColor = pColor }); // Üst
        _modelFaces.Add(new Face3D { Vertices = new Vektor_t[] { c[7], c[6], c[5], c[4] }, BaseColor = pBack });  // Alt
        _modelFaces.Add(new Face3D { Vertices = new Vektor_t[] { c[0], c[4], c[5], c[1] }, BaseColor = edge });   // Ön
        _modelFaces.Add(new Face3D { Vertices = new Vektor_t[] { c[2], c[6], c[7], c[3] }, BaseColor = edge });   // Arka
        _modelFaces.Add(new Face3D { Vertices = new Vektor_t[] { c[3], c[7], c[4], c[0] }, BaseColor = edge });   // Sağ
        _modelFaces.Add(new Face3D { Vertices = new Vektor_t[] { c[1], c[5], c[6], c[2] }, BaseColor = edge });   // Sol
    }

    public void DurumGuncelle(Kuaterniyon_t q)
    {
        _q = q;
        this.Invalidate();
    }
    
    private class RenderFace
    {
        public PointF[] Points;
        public double AvgZ;
        public Color Color;
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        base.OnPaint(e);
        Graphics g = e.Graphics;
        g.SmoothingMode = SmoothingMode.AntiAlias;
        g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.ClearTypeGridFit;

        int w = this.ClientSize.Width;
        int h = this.ClientSize.Height;
        
        int cx = w / 2;
        int cy = h / 2 + 10;
        double scale = 1.5; 
        
        // Kamera Ayarları (Perspektif)
        double zCam = 400.0;
        double viewDist = 400.0;
        
        // Işık Ayarları
        Vektor_t lightDir = new Vektor_t(0.5, -0.7, 1.0, Kordinat_Sistemi_t.DUNYA_ENU);
        double lLen = Math.Sqrt(lightDir.X * lightDir.X + lightDir.Y * lightDir.Y + lightDir.Z * lightDir.Z);
        lightDir = new Vektor_t(lightDir.X / lLen, lightDir.Y / lLen, lightDir.Z / lLen, Kordinat_Sistemi_t.DUNYA_ENU);

        List<RenderFace> renderList = new List<RenderFace>();

        foreach (var face in _modelFaces)
        {
            Vektor_t[] wPts = new Vektor_t[face.Vertices.Length];
            PointF[] sPts = new PointF[face.Vertices.Length];
            double sumZ = 0;
            
            for (int i = 0; i < face.Vertices.Length; i++) 
            {
                wPts[i] = _q.GovdedenDunyayaCevir(face.Vertices[i]);
                sumZ += wPts[i].Z;
                
                // Perspektif İzdüşüm
                double zDist = zCam - wPts[i].Z;
                if (zDist < 1) zDist = 1;
                double factor = viewDist / zDist;
                
                sPts[i] = new PointF((float)(cx + wPts[i].X * scale * factor), (float)(cy - wPts[i].Y * scale * factor));
            }
            
            // Normal Hesaplama
            Vektor_t v1 = wPts[1] - wPts[0];
            Vektor_t v2 = wPts[2] - wPts[0];
            Vektor_t n = new Vektor_t(
                v1.Y * v2.Z - v1.Z * v2.Y,
                v1.Z * v2.X - v1.X * v2.Z,
                v1.X * v2.Y - v1.Y * v2.X,
                Kordinat_Sistemi_t.DUNYA_ENU
            );
            
            double nLen = Math.Sqrt(n.X * n.X + n.Y * n.Y + n.Z * n.Z);
            if (nLen < 1e-6) continue;
            
            // Backface Culling (Arkada kalan yüzeyleri sil)
            Vektor_t view = new Vektor_t(0 - wPts[0].X, 0 - wPts[0].Y, zCam - wPts[0].Z, Kordinat_Sistemi_t.DUNYA_ENU);
            double viewDot = n.X * view.X + n.Y * view.Y + n.Z * view.Z;
            
            if (viewDot <= 0) continue; 
            
            // Düz Gölgelendirme (Flat Shading)
            n = new Vektor_t(n.X / nLen, n.Y / nLen, n.Z / nLen, Kordinat_Sistemi_t.DUNYA_ENU);
            double dot = n.X * lightDir.X + n.Y * lightDir.Y + n.Z * lightDir.Z;
            double intensity = 0.4 + 0.6 * dot; // Ortam ışığı + Yönlü ışık
            intensity = Math.Max(0.0, Math.Min(1.0, intensity));
            
            int R = (int)(face.BaseColor.R * intensity);
            int G = (int)(face.BaseColor.G * intensity);
            int B = (int)(face.BaseColor.B * intensity);
            
            renderList.Add(new RenderFace {
                Points = sPts,
                AvgZ = sumZ / wPts.Length,
                Color = Color.FromArgb(255, R, G, B)
            });
        }
        
        // Z-Sorting (Painter's Algorithm) -> Uzaktan Yakına doğru çiz
        renderList.Sort((a, b) => a.AvgZ.CompareTo(b.AvgZ));
        
        foreach (var rf in renderList) 
        {
            using (Brush b = new SolidBrush(rf.Color)) 
            {
                g.FillPolygon(b, rf.Points);
            }
            using (Pen p = new Pen(Color.FromArgb(80, 0, 0, 0), 1f)) 
            {
                g.DrawPolygon(p, rf.Points); 
            }
        }
        
        // 2D Kartezyen Eksenler (Grid)
        using (Pen axisPen = new Pen(Color.FromArgb(50, 255, 255, 255), 1))
        {
            axisPen.DashStyle = DashStyle.Dash;
            g.DrawLine(axisPen, cx, 20, cx, h - 20); // Y ekseni
            g.DrawLine(axisPen, 20, cy, w - 20, cy); // X ekseni
        }
        
        double modelH = 35; // Modeldeki silindir yüksekliği

        // Açıları (Derece) Eksenlere İşaretle (Sadece 90 ve -90)
        int[] anglesToMark = { 90 };
        using (Pen tickPen = new Pen(Color.FromArgb(100, 255, 255, 255), 1))
        using (Brush tickBrush = new SolidBrush(Color.FromArgb(150, 255, 255, 255)))
        {
            Font tickFont = new Font("Segoe UI", 7f);
            float dist = (float)(modelH * scale); // sin(90) = 1
            
            // X ekseni
            g.DrawLine(tickPen, cx + dist, cy - 3, cx + dist, cy + 3);
            g.DrawLine(tickPen, cx - dist, cy - 3, cx - dist, cy + 3);
            g.DrawString("90°", tickFont, tickBrush, cx + dist - 8, cy + 3);
            g.DrawString("-90°", tickFont, tickBrush, cx - dist - 12, cy + 3);
            
            // Y ekseni
            g.DrawLine(tickPen, cx - 3, cy + dist, cx + 3, cy + dist);
            g.DrawLine(tickPen, cx - 3, cy - dist, cx + 3, cy - dist);
            g.DrawString("-90°", tickFont, tickBrush, cx + 3, cy + dist - 4); // Y ters olduğu için alt taraf -90
            g.DrawString("90°", tickFont, tickBrush, cx + 3, cy - dist - 14); // Üst taraf +90
            
            tickFont.Dispose();
        }

        // Ortogonal İzdüşüm (Çentik ve Merkez Noktası)
        Vektor_t topCenterBody = new Vektor_t(0, 0, modelH, Kordinat_Sistemi_t.GOVDE_BODY);
        Vektor_t topCenterWorld = _q.GovdedenDunyayaCevir(topCenterBody);
        
        float orthoX = (float)(cx + topCenterWorld.X * scale);
        float orthoY = (float)(cy - topCenterWorld.Y * scale);

        // Ortogonal Çentik Çizimi
        double R_silindir = 18;
        Vektor_t tipBody = new Vektor_t(0, R_silindir + 10, modelH, Kordinat_Sistemi_t.GOVDE_BODY);
        Vektor_t base1Body = new Vektor_t(-6, R_silindir - 5, modelH, Kordinat_Sistemi_t.GOVDE_BODY);
        Vektor_t base2Body = new Vektor_t(6, R_silindir - 5, modelH, Kordinat_Sistemi_t.GOVDE_BODY);

        Vektor_t tWorld = _q.GovdedenDunyayaCevir(tipBody);
        Vektor_t b1World = _q.GovdedenDunyayaCevir(base1Body);
        Vektor_t b2World = _q.GovdedenDunyayaCevir(base2Body);

        PointF pTip = new PointF((float)(cx + tWorld.X * scale), (float)(cy - tWorld.Y * scale));
        PointF pBase1 = new PointF((float)(cx + b1World.X * scale), (float)(cy - b1World.Y * scale));
        PointF pBase2 = new PointF((float)(cx + b2World.X * scale), (float)(cy - b2World.Y * scale));

        using (Brush notchBrush = new SolidBrush(Color.LimeGreen))
        {
            g.FillPolygon(notchBrush, new PointF[] { pTip, pBase1, pBase2 });
        }

        // Ortogonal Merkez Noktası Çizimi (Kırmızı Nokta)
        using (Brush dotBrush = new SolidBrush(Color.Red))
        using (Pen dotPen = new Pen(Color.White, 1.5f))
        {
            g.FillEllipse(dotBrush, orthoX - 4, orthoY - 4, 8, 8);
            g.DrawEllipse(dotPen, orthoX - 4, orthoY - 4, 8, 8);
        }
        
        // Açı (Derece) Kordinatlarını Hesapla ve Yazdır
        Vektor_t zBody = new Vektor_t(0, 0, 1, Kordinat_Sistemi_t.GOVDE_BODY);
        Vektor_t zWorld = _q.GovdedenDunyayaCevir(zBody);
        double theta = Math.Acos(Math.Max(-1.0, Math.Min(1.0, zWorld.Z)));
        double thetaDeg = theta * 180.0 / Math.PI;

        double distXY = Math.Sqrt(zWorld.X * zWorld.X + zWorld.Y * zWorld.Y);
        double dx = 0, dy = 0;
        if (distXY > 1e-6)
        {
            dx = zWorld.X / distXY;
            dy = zWorld.Y / distXY;
        }

        // NED/ENU kargaşasından kaçınmak için eksenleri doğrudan X, Y açısı olarak veriyoruz
        double visX = dx * thetaDeg;
        double visY = -dy * thetaDeg;

        string coordText = $"(X: {visX:+0.0;-0.0}°, Y: {visY:+0.0;-0.0}°)";
        using (Brush textBrush = new SolidBrush(Color.White))
        {
            g.DrawString(coordText, _titleFont, textBrush, orthoX + 8, orthoY + 8);
        }

        // ======================= NED VE ENU EULER AÇILARI =======================
        // q_ned: ENU uzayından NED uzayına dönüşüm
        Kuaterniyon_t q_ned = new Kuaterniyon_t(_q.W, _q.Y, _q.X, -_q.Z);
        q_ned.ToEulerDerece(out double roll, out double pitch, out double yaw);
        
        string nedText = $"NED   R: {roll:+0.0;-0.0}°   P: {pitch:+0.0;-0.0}°   Y: {yaw:+0.0;-0.0}°";
        string enuText = $"ENU   R: {roll:+0.0;-0.0}°   P: {pitch:+0.0;-0.0}°   Y: {-yaw:+0.0;-0.0}°";

        using (Brush bgBrush = new SolidBrush(Color.FromArgb(180, 0, 0, 0)))
        {
            g.FillRectangle(bgBrush, 0, h - 38, w, 38);
        }

        using (Brush textBrush = new SolidBrush(Color.Gold))
        {
            g.DrawString(nedText, _titleFont, textBrush, 5, h - 35);
            g.DrawString(enuText, _titleFont, textBrush, 5, h - 18);
        }
        
        // Başlık
        string title = "3D GÖREV YÜKÜ MODELİ";
        SizeF titleSize = g.MeasureString(title, _titleFont);
        g.DrawString(title, _titleFont, _titleBrush, cx - titleSize.Width / 2, 2);
        
        using (Pen border = new Pen(Color.FromArgb(80, 255, 255, 255), 1))
        {
            g.DrawRectangle(border, 0, 0, w - 1, h - 1);
        }
    }
}
