namespace VerifyBot;
public class Commands : ModuleBase<SocketCommandContext>
{
    [Command("jukeboxfix")]
    public async Task<RuntimeResult> JukeboxFix()
    {
        if (Context.Message.Attachments.Count == 0)
            return CommandResult.FromError($"{Context.User.Mention}, you need to attach a Profile.Save file. It can be found at ``Program Files (x86)/Steam/userdata/[user-id]/960090/local/link/[player-id]/Profile.Save``.");

        Attachment save = Context.Message.Attachments.First();
        if (!save.Filename.Equals("Profile.Save", StringComparison.OrdinalIgnoreCase))
            return CommandResult.FromError($"{Context.User.Mention}, you need to attach a Profile.Save file. It can be found at ``Program Files (x86)/Steam/userdata/[user-id]/960090/local/link/[player-id]/Profile.Save``.");

        using HttpClient client = new();
        byte[] buffer = await client.GetByteArrayAsync(save.Url);
        File.WriteAllBytes("Profile.Save.temp", buffer);

        if (!NKSave.UsesNKSaveSystem("Profile.Save.temp"))
        {
            File.Delete("Profile.Save.temp");
            return CommandResult.FromError($"{Context.User.Mention}, you have attached an invalid save file!");
        }

        using FileStream outstream = File.Open("Profile.Save", FileMode.Create);
        using FileStream instream = File.Open("Profile.Save.temp", FileMode.Open);
        string json = NKSave.ReadFileContent(instream);
        dynamic jsonObj = JObject.Parse(json);
        jsonObj.savedPlayList = new JArray();
        string outJson = Convert.ToString(jsonObj);

        NKSave.WriteFileContent(instream, outJson, outstream);
        await Context.Channel.SendFileAsync("Profile.Save", "Here's your new save! Replace the file at ``Program Files (x86)/Steam/userdata/[user-id]/960090/local/link/[player-id]/Profile.Save`` with this one.");
        File.Delete("Profile.Save.temp");
        File.Delete("Profile.Save");

        return CommandResult.FromSuccess();
    }

    [Command("verify")]
    public async Task<RuntimeResult> Verify()
    {
        if (Context.Channel.Id != Constants.VERIFY_CHANNEL)
            return CommandResult.FromError($"{Context.User.Mention}, please execute this command in {MentionUtils.MentionChannel(Constants.VERIFY_CHANNEL)}.");

        try
        {
            using HttpClient client = new();
            client.DefaultRequestHeaders.Add("Authorization", Credentials.USER_TOKEN);
            string response = await client.GetStringAsync($"https://discordapp.com/api/v6/users/{Context.User.Id}/profile");
            JObject responseJson = JObject.Parse(response);
            JArray connectedAccounts = JArray.Parse(responseJson["connected_accounts"].ToString());

            if (!connectedAccounts.Children().Any(x => x["type"].ToString() == "steam"))
                return CommandResult.FromError($"{Context.User.Mention}, I could not find Steam in your connected accounts. Do you perhaps not have it connected? Make sure you do in Settings -> Connections, then try again.");

            foreach (JObject item in connectedAccounts.Children().Where(x => x["type"].ToString() == "steam"))
            {
                SteamWebInterfaceFactory webInterfaceFactory = new(Credentials.STEAM_API_KEY);
                PlayerService player = webInterfaceFactory.CreateSteamWebInterface<PlayerService>();
                ulong id = item["id"].ToObject<ulong>();
                ISteamWebResponse<ulong?> playingSharedBtd6 = await player.IsPlayingSharedGameAsync(id, Constants.BTD6_ID);
                ISteamWebResponse<OwnedGamesResultModel> ownedGames = await player.GetOwnedGamesAsync(id);
                IReadOnlyCollection<OwnedGameModel> ownedGamesData = ownedGames.Data.OwnedGames;

                if (!ownedGamesData.Any(x => x.AppId == Constants.BTD6_ID) && !(playingSharedBtd6.Data.Value != 0 && playingSharedBtd6.Data != null))
                    return CommandResult.FromError($"{Context.User.Mention}, I could not find BTD6 in your Steam games list. Do you perhaps not have your account/games public, or not have BTD6? Make sure you do, then try again. To learn how to make your account/games public, go to <https://help.challengermode.com/en/articles/2579734-make-your-steam-profile-public>.");

                await ReplyAsync($"{Context.User.Mention}, you were verified successfully and received the Purchased BTD6 on Steam role!");
                await Task.Delay(1500);
                await (Context.User as IGuildUser)?.AddRoleAsync(Context.Guild.GetRole(Constants.VERIFIED_ROLE));

                IEnumerable<IMessage> msgs = await Context.Channel.GetMessagesAsync().FlattenAsync();
                await (Context.Channel as SocketTextChannel)?.DeleteMessagesAsync(msgs);
            }
        }
        catch (Exception ex)
        {
            EmbedBuilder embed = new EmbedBuilder()
                .WithTitle("Shit Code Exception")
                .WithColor(Color.Red)
                .WithDescription(ex.StackTrace);
            await ReplyAsync($"{Context.User.Mention}, there seems to be a problem with the bot. Here's some details. {MentionUtils.MentionUser(Constants.BOT_OWNER)} fix your shit!!!", embed: embed.Build());
        }

        await Context.Message.DeleteAsync();
        return CommandResult.FromSuccess();
    }
}