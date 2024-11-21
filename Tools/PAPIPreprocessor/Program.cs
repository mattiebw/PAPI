using System.Text.Json;

namespace PAPIPreprocessor;

static class Program
{
    private static Dictionary<string, DateTime> _cachedLastWriteTimes = new();
    
    static void Main(string[] args)
    {
        Console.ForegroundColor = ConsoleColor.White;
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
        
        Console.WriteLine($"Looking for header files in {dir}/PAPI/Include/");
        var files = Directory.GetFiles($"{dir}/PAPI/Include/", "*.h", SearchOption.AllDirectories);
        foreach (var file in files)
        {
            var lastWriteTime = File.GetLastWriteTime(file);
            if (_cachedLastWriteTimes.TryGetValue(file, out var cachedLastWriteTime) && !args.Contains("--force"))
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
            return;
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
        FixUpDirectorySeparator(ref path);
        
        if (_cachedLastWriteTimes.ContainsKey(path))
            _cachedLastWriteTimes[path] = File.GetLastWriteTime(path);
        else
            _cachedLastWriteTimes.Add(path, File.GetLastWriteTime(path));

        if (path.ToLower().Contains("vendor")) // Lets not process vendor headers!
            return;
        
        Console.WriteLine($"Processing {path}");
        
        HeaderProcessor processor = new(File.ReadAllText(path));
        var entities = processor.Process();
        if (entities.Count == 0)
            return;
        Console.WriteLine($"Found {entities.Count} entities in {path}:");
        foreach (var entity in entities) Console.WriteLine($"    {entity.Name}");
        GenerateSourceForFile(path, entities);
    }

    static void GenerateSourceForFile(string path, List<Entity> entities)
    {
        var sourcePath = path.Replace("Include", "Source").Replace(".h", ".cpp");
        if (!Path.Exists(sourcePath))
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"Couldn't find source file at {sourcePath}");
            Console.ForegroundColor = ConsoleColor.White;
            _cachedLastWriteTimes.Remove(path);
            return;
        }
        
        Console.WriteLine($"Writing reflection code to {sourcePath}");
    }

    static void FixUpDirectorySeparator(ref string path)
    {
        path = path.Replace('/', Path.DirectorySeparatorChar).Replace('\\', Path.DirectorySeparatorChar);
    }
}
