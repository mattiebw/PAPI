using System.Text.Json;
using Microsoft.Win32.SafeHandles;

namespace PAPIPreprocessor;

static class Program
{
    private static Dictionary<string, DateTime> _cachedLastWriteTimes = new();
    
    static void Main(string[] args)
    {
        Console.WriteLine("We're running the PAPI preprocessor!");
        
        LoadLastWriteTimes();
        
        var dir = $"{Environment.CurrentDirectory}/Include/";
        Console.WriteLine($"Looking for header files in {dir}");
        var files = Directory.GetFiles(dir, "*.h", SearchOption.AllDirectories);
        foreach (var file in files)
        {
            Console.WriteLine($"Processing {file}");
            var handle = File.OpenHandle(file);
            
            var lastWriteTime = File.GetLastWriteTime(file);
            if (_cachedLastWriteTimes.TryGetValue(file, out var cachedLastWriteTime))
            {
                if (lastWriteTime == cachedLastWriteTime)
                    continue;
            }
            
            ProcessHeader(handle, file);
        }
        
        SaveLastWriteTimes();
    }

    static void LoadLastWriteTimes()
    {
        if (!File.Exists("lastWriteTimes.json"))
        {
            return;
        }
        var json = File.ReadAllText("lastWriteTimes.json");
        _cachedLastWriteTimes = JsonSerializer.Deserialize<Dictionary<string, DateTime>>(json)!;

        if (_cachedLastWriteTimes is null)
        {
            Console.WriteLine("Failed to load last write times, starting fresh!");
            _cachedLastWriteTimes = new();
        }
    }
    
    static void SaveLastWriteTimes()
    {
        var json = JsonSerializer.Serialize(_cachedLastWriteTimes);
        File.WriteAllText("lastWriteTimes.json", json);
    }
    
    static void ProcessHeader(SafeFileHandle file, string path)
    {
        if (_cachedLastWriteTimes.ContainsKey(path))
            _cachedLastWriteTimes[path] = File.GetLastWriteTime(path);
        else
            _cachedLastWriteTimes.Add(path, File.GetLastWriteTime(path));
    }
}
