namespace VerifyBot;
public class EventSystem
{
    private readonly CommandService commands;
    private readonly DiscordSocketClient client;
    private readonly ServiceProvider serviceProvider;

    public EventSystem(ServiceProvider serviceProvider)
    {
        commands = serviceProvider.GetRequiredService<CommandService>();
        client = serviceProvider.GetRequiredService<DiscordSocketClient>();
        this.serviceProvider = serviceProvider;
    }

    public void SubscribeEvents()
    {
        client.Log += Client_Log;
        client.MessageReceived += Client_MessageReceived;
        commands.CommandExecuted += Commands_CommandExecuted;
    }

    private static Task Client_Log(LogMessage arg)
    {
        Console.WriteLine(arg);
        return Task.CompletedTask;
    }

    private async Task Client_MessageReceived(SocketMessage arg)
    {
        SocketUserMessage message = arg as SocketUserMessage;
        SocketCommandContext context = new(client, message);
        if (message.Author.IsBot || string.IsNullOrWhiteSpace(message.Content)) return;

        int argPos = 0;
        if (message.HasCharPrefix('$', ref argPos))
            await commands.ExecuteAsync(context, argPos, serviceProvider);
    }

    private static async Task Commands_CommandExecuted(Optional<CommandInfo> command, ICommandContext context, IResult result)
    {
        if (result is CommandResult { IsSuccess: false } cResult)
            await context.Channel.SendMessageAsync(cResult.Reason, allowedMentions: new AllowedMentions(AllowedMentionTypes.Users));
        else if (!result.IsSuccess)
            Console.WriteLine(result.ErrorReason);
    }
}