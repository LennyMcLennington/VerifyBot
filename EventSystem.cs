namespace VerifyBot;
public class EventSystem
{
    private readonly CommandService commands;
    private readonly DiscordSocketClient client;
    private readonly ServiceProvider serviceProvider;

    public EventSystem(ServiceProvider serviceProvider)
    {
        this.serviceProvider = serviceProvider;
        commands = serviceProvider.GetRequiredService<CommandService>();
        client = serviceProvider.GetRequiredService<DiscordSocketClient>();
    }

    public void SubscribeEvents()
    {
        client.Log += Client_Log;
        client.MessageReceived += Client_MessageReceived;
        client.ReactionAdded += Client_ReactionAdded;
        client.ReactionRemoved += Client_ReactionRemoved;
        commands.CommandExecuted += Commands_CommandExecuted;
    }

    private static async Task HandleReactionAsync(Cacheable<IMessageChannel, ulong> channelCached, SocketReaction reaction, bool addedReaction)
    {
        IMessageChannel channel = await channelCached.GetOrDownloadAsync();
        SocketGuildUser user = await channel.GetUserAsync(reaction.UserId) as SocketGuildUser;
        if (user.IsBot || reaction.MessageId != Constants.SELF_ROLES_MSG) return;

        if (Constants.SELF_ROLES.ContainsKey(reaction.Emote.Name))
        {
            if (addedReaction)
                await user.AddRoleAsync(Constants.SELF_ROLES[reaction.Emote.Name]);
            else
                await user.RemoveRoleAsync(Constants.SELF_ROLES[reaction.Emote.Name]);
        }
    }

    private Task Client_Log(LogMessage arg)
    {
        Console.WriteLine(arg);
        return Task.CompletedTask;
    }

    private async Task Client_MessageReceived(SocketMessage arg)
    {
        SocketUserMessage message = arg as SocketUserMessage;
        SocketCommandContext context = new(client, message);
        if (message.Author.IsBot || string.IsNullOrWhiteSpace(message.Content)) return;

        await FilterSystem.DoNWordCheckAsync(context.Message, context.Channel);

        int argPos = 0;
        if (message.HasCharPrefix('$', ref argPos))
            await commands.ExecuteAsync(context, argPos, serviceProvider);
    }

    private static async Task Client_ReactionAdded(Cacheable<IUserMessage, ulong> msg, Cacheable<IMessageChannel, ulong> channel,
        SocketReaction reaction) => await HandleReactionAsync(channel, reaction, true);

    private static async Task Client_ReactionRemoved(Cacheable<IUserMessage, ulong> msg, Cacheable<IMessageChannel, ulong> channel,
        SocketReaction reaction) => await HandleReactionAsync(channel, reaction, false);

    private static async Task Commands_CommandExecuted(Optional<CommandInfo> command, ICommandContext context, IResult result)
    {
        string reason = Format.Sanitize(result.ErrorReason);
        string response = result.Error switch {
            CommandError.BadArgCount => $"{context.User.Mention}, you must specify {command.Value.Parameters.Count(p => !p.IsOptional)} argument(s)!\nCommand usage: ``{command.Value.Remarks}``",
            CommandError.ObjectNotFound => reason,
            _ => !result.IsSuccess && result is CommandResult ? reason : ""
        };

        if (response != "")
            await context.Channel.SendMessageAsync(response, allowedMentions: new AllowedMentions(AllowedMentionTypes.Users));
        else if (!result.IsSuccess)
            Console.WriteLine(reason);
    }
}