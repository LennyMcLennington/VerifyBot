# Verify Bot
The bot for the [BTD6 Mods & Discussion Discord server](https://discord.gg/nuMvgkP).

<p align="center">
    <a href="https://discord.gg/nuMvgkP" alt="Discord">
        <img src="https://img.shields.io/discord/504782676331331584" />
    </a>
    <img src="https://img.shields.io/codefactor/grade/github/BowDown097/VerifyBot" />
</p>

## Running your own instance of the bot
You'll need to create a file named Credentials.cs in the bot's main directory. The code needs to look exactly like this (of course, not in syntax, but you get the point):
```cs
namespace VerifyBot;
public static class Credentials
{
    public const string SteamApiKey = "[STEAM API KEY]";
    public const string Token = "[DISCORD BOT TOKEN]";
    public const string UserToken = "[TOKEN FOR A USER ACCOUNT THAT WILL BE USED TO ACCESS CONNECTIONS]";
}
```
What to plug into these variables can be figured out through some simple googling.
