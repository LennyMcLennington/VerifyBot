# Verify Bot
The bot for the [BTD6 Mods & Discussion Discord server](https://discord.gg/nuMvgkP).

<p align="center">
    <a href="https://discord.gg/nuMvgkP" alt="Discord">
        <img src="https://img.shields.io/discord/504782676331331584" />
    </a>
    <img src="https://img.shields.io/codefactor/grade/github/BowDown097/VerifyBot" />
</p>

## Running your own instance of the bot
Well, you can't. At least not with the $jukeboxfix command, as it uses code that has been removed from this repository so that BTD6's save encryption algorithms aren't compromised. However, if (for whatever reason) you so wish, and you've removed all that stuff, there's only one thing of setup you need: you'll need to create a file named Credentials.cs in the bot's main directory. The code needs to look exactly like this (of course, not in syntax, but you get the point):
```cs
namespace VerifyBot;
public static class Credentials
{
    public const string STEAM_API_KEY = "[STEAM API KEY]";
    public const string TOKEN = "[DISCORD BOT TOKEN]";
    public const string USER_TOKEN = "[TOKEN FOR A USER ACCOUNT THAT WILL BE USED TO ACCESS CONNECTIONS]";
}
```
What to plug into these variables can be figured out through some simple googling.
