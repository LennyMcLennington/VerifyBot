namespace VerifyBot;
public class EventSystem
{
    private readonly CommandService _commands;
    private readonly DiscordSocketClient _client;
    private readonly ServiceProvider _serviceProvider;

    public EventSystem(ServiceProvider serviceProvider)
    {
        _commands = serviceProvider.GetRequiredService<CommandService>();
        _client = serviceProvider.GetRequiredService<DiscordSocketClient>();
        _serviceProvider = serviceProvider;
    }

    public void SubscribeEvents()
    {
        _client.Log += Client_Log;
        _client.MessageReceived += Client_MessageReceived;
        _commands.CommandExecuted += Commands_CommandExecuted;
    }

    private static Task Client_Log(LogMessage arg)
    {
        Console.WriteLine(arg);
        return Task.CompletedTask;
    }

    private async Task Client_MessageReceived(SocketMessage arg)
    {
        SocketUserMessage message = arg as SocketUserMessage;
        SocketCommandContext context = new(_client, message);
        if (message.Author.IsBot || string.IsNullOrWhiteSpace(message.Content)) return;

        int argPos = 0;
        if (message.HasCharPrefix('$', ref argPos))
            await _commands.ExecuteAsync(context, argPos, _serviceProvider);
    }

    private static async Task Commands_CommandExecuted(Optional<CommandInfo> command, ICommandContext context, IResult result)
    {
        if (result is CommandResult { IsSuccess: false } cResult)
            await context.Channel.SendMessageAsync(cResult.Reason, allowedMentions: new AllowedMentions(AllowedMentionTypes.Users));
        else if (!result.IsSuccess)
            Console.WriteLine(result.ErrorReason);
    }
}