using System.Text.Json;
using CppAst;

namespace PAPIPreprocessor;

static class Program
{
    private static Dictionary<string, DateTime> _cachedLastWriteTimes = new();
    private static CppParserOptions _options = new();
    
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
        
        _options.IncludeFolders.AddRange(new []
        {
            $"{dir}/PAPI/Include/",
            $"{dir}/PAPI/Vendor/spdlog/include/",
            $"{dir}/PAPI/Vendor/SDL/include/",
            $"{dir}/PAPI/Vendor/imgui/",
            $"{dir}/PAPI/Vendor/msdf-atlas-gen/msdf-atlas-gen/",
            $"{dir}/PAPI/Vendor/msdf-atlas-gen/msdfgen/",
            $"{dir}/PAPI/Vendor/glm/Include/",
            $"{dir}/PAPI/Vendor/stb/",
        });
        
        _options.Defines.AddRange(args[1].Split(';'));

        _options.AdditionalArguments.Add("-std=c++20");
        
        Console.WriteLine($"Looking for header files in {dir}/PAPI/Include/");
        var files = Directory.GetFiles($"{dir}/PAPI/Include/", "*.h", SearchOption.AllDirectories);
        foreach (var file in files)
        {
            ProcessHeader(file);
            var lastWriteTime = File.GetLastWriteTime(file);
            if (_cachedLastWriteTimes.TryGetValue(file, out var cachedLastWriteTime))
            {
                if (lastWriteTime == cachedLastWriteTime)
                    continue;
            }
            
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
        
        Console.WriteLine($"Processing {path}");
        
        var headerData = CppParser.Parse(File.ReadAllText(path), _options);
        if (headerData.HasErrors)
        {
            Console.WriteLine("Error!");
            foreach (var error in headerData.Diagnostics.Messages)
            {
                Console.WriteLine(error);
            }
        }
        foreach (var cppClass in headerData.Classes)
        {
            Console.WriteLine($"Found class: {cppClass.Name}");
            if (cppClass.BaseTypes.Any(type => type.Type.FullName == "Entity"))
            {
                Console.WriteLine($"Found entity class {cppClass.Name}");
            }
        }
    }
}
