using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

public class FileReader
{
    public string FilePath { get; private set; }
    private long _lastFilePosition;

    public FileReader(string filePath)
    {
        FilePath = filePath;
        _lastFilePosition = 0;

        // Eðer dosya varsa, son konumunu kaydet
        if (File.Exists(FilePath))
        {
            _lastFilePosition = new FileInfo(FilePath).Length;
        }
    }

    // Dosyaya eklenen yeni satýrlarý okur ve TData listesi döner
    public List<TData> ReadNewLines()
    {
        List<TData> newDataList = new List<TData>();

        if (!File.Exists(FilePath))
            return newDataList;

        try
        {
            FileInfo fileInfo = new FileInfo(FilePath);
            long currentFileSize = fileInfo.Length;

            // Dosya boyutu küçülmüþse (dosya yeniden baþlatýlmýþsa), baþtan oku
            if (currentFileSize < _lastFilePosition)
            {
                _lastFilePosition = 0;
            }

            // Sadece yeni eklenen kýsýmlarý oku
            if (currentFileSize > _lastFilePosition)
            {
                using (FileStream fs = new FileStream(FilePath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
                {
                    fs.Seek(_lastFilePosition, SeekOrigin.Begin);

                    using (StreamReader reader = new StreamReader(fs, Encoding.UTF8))
                    {
                        string line;
                        while ((line = reader.ReadLine()) != null)
                        {
                            if (!string.IsNullOrWhiteSpace(line))
                            {
                                TData data = TData.Parse(line);
                                if (data != null)
                                {
                                    newDataList.Add(data);
                                }
                            }
                        }
                    }
                }

                // Yeni dosya konumunu kaydet
                _lastFilePosition = currentFileSize;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"FileReader hatasýndan oluþtu: {ex.Message}");
        }

        return newDataList;
    }

    // Belirtilen dizindeki dosyayý okur ve TData listesi döner
    public List<TData> ReadAllLines()
    {
        List<TData> allDataList = new List<TData>();

        if (!File.Exists(FilePath))
            return allDataList;

        try
        {
            using (StreamReader reader = new StreamReader(FilePath, Encoding.UTF8))
            {
                string line;
                bool isFirstLine = true;

                while ((line = reader.ReadLine()) != null)
                {
                    // Baþlýk satýrlarýný atla (ilk iki satýr)
                    if (isFirstLine)
                    {
                        isFirstLine = false;
                        continue;
                    }

                    if (line.StartsWith("-,"))
                    {
                        continue; // Birim satýrýný atla
                    }

                    if (!string.IsNullOrWhiteSpace(line))
                    {
                        TData data = TData.Parse(line);
                        if (data != null)
                        {
                            allDataList.Add(data);
                        }
                    }
                }
            }

            _lastFilePosition = new FileInfo(FilePath).Length;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"FileReader hatasýndan oluþtu: {ex.Message}");
        }

        return allDataList;
    }
}
