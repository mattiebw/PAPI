using System.Text.Json;
using CppAst;
using Microsoft.Win32.SafeHandles;

namespace PAPIPreprocessor;

static class Program
{
    private static Dictionary<string, DateTime> _cachedLastWriteTimes = new();
    
    static void Main(string[] args)
    {
        Console.WriteLine("We're running the PAPI preprocessor!");
        
        LoadLastWriteTimes();

        if (args.Length < 1)
        {
            Console.WriteLine("Please provide a directory to search for header files.");
            return;    
        }
        
        var dir = args[0];
        Console.WriteLine(dir);
        dir = Path.GetFullPath(dir);
        Console.WriteLine($"Looking for header files in {dir}");
        var files = Directory.GetFiles(dir, "*.h", SearchOption.AllDirectories);
        foreach (var file in files)
        {
            Console.WriteLine($"Processing {file}");
            
            var lastWriteTime = File.GetLastWriteTime(file);
            if (_cachedLastWriteTimes.TryGetValue(file, out var cachedLastWriteTime))
            {
                if (lastWriteTime == cachedLastWriteTime)
                    continue;
            }
            
            ProcessHeader(file);
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
    
    static void ProcessHeader(string path)
    {
        if (_cachedLastWriteTimes.ContainsKey(path))
            _cachedLastWriteTimes[path] = File.GetLastWriteTime(path);
        else
            _cachedLastWriteTimes.Add(path, File.GetLastWriteTime(path));

        if (path.ToLower().Contains("vendor")) // Lets not process vendor headers!
            return;
        
        var headerData = CppParser.ParseFile(path);
        Console.WriteLine($"We have some header data: {headerData}");
    }
}
