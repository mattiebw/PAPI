namespace PAPIPreprocessor;

public class PAPIFunc
{
    public PAPIFunc(string content)
    {
    }
}

public class PAPIProperty
{
    public PAPIProperty(string content)
    {
    }
}

public enum RPCType
{
    Multicast,
    Client,
    Server
}

public class PAPIRPC
{
    public RPCType Type;
    
    public PAPIRPC(string content)
    {
        var parts = content.Split(',');
        for (var i = 0; i < parts.Length; i++)
            parts[i] = parts[i].Trim();
        
        Type = parts[0].ToLower() switch
        {
            "multicast" => RPCType.Multicast,
            "client"    => RPCType.Client,
            "server"    => RPCType.Server,
            _           => throw new Exception($"Unknown RPC type: {parts[0]}")
        };  
    }
}

public class Entity(string name)
{
    public String Name = name;
    public List<PAPIFunc> Functions = new();
    public List<PAPIProperty> Properties = new();
    public List<PAPIRPC> RPCs = new();
}
