namespace VerifyBot;
public static class FilterSystem
{
    public static readonly Regex NWORD_REGEX = new("[nɴⁿₙñńņňÑŃŅŇ][i1!¡ɪᶦᵢ¹₁jįīïîíìl|;:¡][g9ɢᵍ𝓰𝓰qģğĢĞ][g9ɢᵍ𝓰𝓰qģğĢĞ][e3€ᴇᵉₑ³₃ĖĘĚĔėęěĕəèéêëē][rʀʳᵣŔŘŕř]");

    public static async Task DoNWordCheckAsync(SocketUserMessage message, IMessageChannel channel)
    {
        char[] cleaned = message.Content.Where(char.IsLetterOrDigit).ToArray();
        if (channel.Name != "extremely-funny" && NWORD_REGEX.IsMatch(new string(cleaned).ToLower()))
            await message.DeleteAsync();
    }
}