# Verify Bot
The bot for the [BTD6 Mods & Discussion Discord server](https://discord.gg/nuMvgkP).

<p align="center">
    <a href="https://discord.gg/nuMvgkP" alt="Discord">
        <img src="https://img.shields.io/discord/504782676331331584" />
    </a>
    <img src="https://img.shields.io/codefactor/grade/github/BowDown097/VerifyBot" />
</p>

## Running your own instance of the bot
You'll need to create a file named config.json in the bot's binary directory. It needs to look EXACTLY like this:
```json
{
    "botToken": "[DISCORD BOT TOKEN]",
    "steamApiKey": "[STEAM API KEY]",
    "userToken": "[TOKEN FOR A USER ACCOUNT THAT WILL BE USED TO ACCESS CONNECTIONS]"
}
```
What to plug in can be figured out through some simple googling.
