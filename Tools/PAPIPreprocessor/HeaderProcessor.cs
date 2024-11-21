using System.Text;

namespace PAPIPreprocessor;

public enum HeaderProcessorState
{
    LookingForClass,
    FoundPossibleClass,
    InClassInheritance,
    InEntityPrescope,
    InEntity,
    InIrrelevantScope,
    InProperty,
    InFunc,
    InRPC,
    SingleLineComment,
    MultiLineComment,
}

public class HeaderProcessor(string content)
{
    public List<Entity> Process()
    {
        var entities = new List<Entity>();
        StringBuilder buffer = new();
        Stack<HeaderProcessorState>
            stateStack = new(); // Each time we enter a new scope, we push a new state to the stack.
        string previousToken = "";
        string className = "";
        Entity? currentEntity = null;
        stateStack.Push(HeaderProcessorState.LookingForClass);

        int i = -1;
        foreach (char c in content)
        {
            i++;
            if (stateStack.Peek() == HeaderProcessorState.MultiLineComment)
            {
                buffer.Append(c);
                if (buffer.Length >= 2 && buffer[^2] == '*' && buffer[^1] == '/')
                {
                    stateStack.Pop();
                    buffer.Clear();
                }

                continue;
            }

            if (char.IsLetterOrDigit(c))
            {
                if (stateStack.Peek() == HeaderProcessorState.SingleLineComment)
                    continue;

                buffer.Append(c);
            }
            else if (stateStack.Peek() == HeaderProcessorState.SingleLineComment)
            {
                if (c == '\n')
                {
                    stateStack.Pop();
                    buffer.Clear();
                }
            }
            else
                switch (c)
                {
                    case ' ' or '\n':
                    {
                        string spacedWord = buffer.ToString();
                        buffer.Clear();

                        switch (stateStack.Peek())
                        {
                            case HeaderProcessorState.LookingForClass:
                                if (spacedWord == "class")
                                    stateStack.Push(HeaderProcessorState.FoundPossibleClass);
                                break;
                            case HeaderProcessorState.InClassInheritance:
                                if (spacedWord == "Entity")
                                {
                                    stateStack.Pop();
                                    stateStack.Push(HeaderProcessorState.InEntityPrescope);
                                }

                                break;
                        }

                        previousToken = spacedWord;

                        break;
                    }
                    case '(':
                        string parameteredWord = buffer.ToString();
                        buffer.Clear();

                        switch (parameteredWord)
                        {
                            case "PAPI_PROP":
                                stateStack.Push(HeaderProcessorState.InProperty);
                                break;
                            case "PAPI_FUNC":
                                stateStack.Push(HeaderProcessorState.InFunc);
                                break;
                            case "PAPI_RPC":
                                stateStack.Push(HeaderProcessorState.InRPC);
                                break;
                        }

                        previousToken = parameteredWord;

                        break;
                    case ')':
                        break;
                    case '{':
                        switch (stateStack.Peek())
                        {
                            case HeaderProcessorState.FoundPossibleClass:
                                if (buffer.Length != 0)
                                {
                                    className = buffer.ToString();
                                    buffer.Clear();
                                }
                                else
                                    className = previousToken;

                                if (className == "Entity")
                                {
                                    // We're in the Entity class, so we need to preprocess it anyway.
                                    stateStack.Pop();
                                    stateStack.Push(HeaderProcessorState.InEntity);
                                    currentEntity = new Entity(className);
                                }
                                else
                                    stateStack.Push(HeaderProcessorState.InIrrelevantScope);

                                break;
                            case HeaderProcessorState.InEntityPrescope:
                                stateStack.Pop();
                                stateStack.Push(HeaderProcessorState.InEntity);
                                currentEntity = new Entity(className);
                                break;
                            default:
                                stateStack.Push(HeaderProcessorState.InIrrelevantScope);
                                break;
                        }

                        break;
                    case '}':
                        switch (stateStack.Peek())
                        {
                            case HeaderProcessorState.InEntity:
                                entities.Add(currentEntity!);
                                currentEntity = null;
                                className = "";
                                stateStack.Pop();
                                break;
                            case HeaderProcessorState.InIrrelevantScope:
                                stateStack.Pop();
                                break;
                            default:
                                Console.WriteLine("Unaccounted scope closing!");
                                break;
                        }

                        break;
                    case ':':
                        switch (stateStack.Peek())
                        {
                            case HeaderProcessorState.FoundPossibleClass:
                                if (buffer.Length != 0)
                                {
                                    className = buffer.ToString();
                                    buffer.Clear();
                                }
                                else
                                    className = previousToken;

                                stateStack.Pop();
                                stateStack.Push(HeaderProcessorState.InClassInheritance);
                                break;
                        }

                        break;
                    case ';':
                        switch (stateStack.Peek())
                        {
                            case HeaderProcessorState.FoundPossibleClass:
                                // It was a forward declaration, so we don't care about it.
                                stateStack.Pop();
                                break;
                            case HeaderProcessorState.InProperty:
                                break;
                            case HeaderProcessorState.InFunc:
                                break;
                            case HeaderProcessorState.InRPC:
                                break;
                        }

                        break;
                    case '/':
                        buffer.Append(c);
                        if (buffer.Length >= 2 && buffer[^2] == '/')
                            stateStack.Push(HeaderProcessorState.SingleLineComment);
                        break;
                    case '*':
                        switch (stateStack.Peek())
                        {
                            case HeaderProcessorState.InProperty:
                                buffer.Append(c);
                                break;
                            case HeaderProcessorState.InFunc:
                                buffer.Append(c);
                                break;
                            case HeaderProcessorState.InRPC:
                                buffer.Append(c);
                                break;
                            default:
                                buffer.Append(c);
                                if (buffer.Length >= 2 && buffer[^2] == '/')
                                    stateStack.Push(HeaderProcessorState.MultiLineComment);
                                break;
                        }

                        break;
                }
        }

        return entities;
    }
}
