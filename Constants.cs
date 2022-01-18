namespace VerifyBot;
public static class Constants
{
    // ********************
    //       COMMANDS
    // ********************
    public const ulong BOT_OWNER = 371476477335699457;
    public const uint BTD6_ID = 960090;
    public const ulong VERIFY_CHANNEL = 715598300505702440;
    public const ulong VERIFIED_ROLE = 511848372999553035;
    // ********************
    //     SELF ROLES
    // ********************
    public static readonly Dictionary<string, ulong> SELF_ROLES = new()
    {
        { "enjoyer", 571496379164131346 }, // mod releases role
        { "KEKW", 887482554217820160 }, // comedian role
        { "pingsock", 887483570912899082 }, // announcements ping role
        { "WeirdChamp", 504789477781405707 }, // degenerate role
    };
    public const ulong SELF_ROLES_MSG = 887484331285700658;
}