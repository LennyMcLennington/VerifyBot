namespace VerifyBot;
internal static class Program
{
    private static async Task Main()
    {
        DiscordSocketClient client = new(new DiscordSocketConfig
        {
            AlwaysDownloadUsers = true,
            LargeThreshold = 250,
            MessageCacheSize = 1500
        });
        ServiceProvider serviceProvider = new ServiceCollection()
            .AddSingleton(client)
            .AddSingleton<CommandService>()
            .BuildServiceProvider();

        CommandService commands = serviceProvider.GetRequiredService<CommandService>();
        await commands.AddModulesAsync(Assembly.GetEntryAssembly(), serviceProvider);
        new EventSystem(serviceProvider).SubscribeEvents();
        await client.LoginAsync(TokenType.Bot, Credentials.TOKEN);
        await client.SetGameAsync("(〃´∀｀〃)ε｀●)chu♪");
        await client.StartAsync();
        await Task.Delay(-1);
    }
}