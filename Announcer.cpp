// Player Login & Logout Announcement Script (Cata 4.3.4)
// Should be able to be modified for any xpac up to 11.2.5 with some edits
// Ported from JerCore's Lua Script
// Updated with on/off toggle in worldserver config
// Worldserver: SpyAnnouncer.Enable = 1 or SpyAnnouncer.Enable = 0

#include "Config.h"  
#include "DBCStores.h"
#include "ScriptPCH.h"
#include "SharedDefines.h"
#include "World.h"

struct AreaTableEntry;

namespace
{
    std::string GetClassColored(Player* player)
    {
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
                return "|cffC79C6EWarrior|r";
            case CLASS_PALADIN:
                return "|cffF58CBAPaladin|r";
            case CLASS_HUNTER:
                return "|cffABD473Hunter|r";
            case CLASS_ROGUE:
                return "|cffFFF569Rogue|r";
            case CLASS_PRIEST:
                return "|cffFFFFFFPriest|r";
            case CLASS_DEATH_KNIGHT:
                return "|cffC41F3BDeath Knight|r";
            case CLASS_SHAMAN:
                return "|cff0070DEShaman|r";
            case CLASS_MAGE:
                return "|cff69CCF0Mage|r";
            case CLASS_WARLOCK:
                return "|cff9482C9Warlock|r";
            case CLASS_DRUID:
                return "|cffFF7D0ADruid|r";
                // Future classes for other ports:
                // case CLASS_MONK:         return "|cff00FF96Monk|r";
                // case CLASS_DEMON_HUNTER: return "|cffA330C9Demon Hunter|r";
                // case CLASS_EVOKER:       return "|cff33937FEvoker|r";
            default:
                return "|cffFF0000Redacted Class|r";
        }
    }

    std::string GetRaceColored(Player* player)
    {
        switch (player->getRace())
        {
            case RACE_HUMAN:
                return "|cff00ff00Human|r";  // 1
            case RACE_ORC:
                return "|cff00ff00Orc|r";  // 2
            case RACE_DWARF:
                return "|cff00ff00Dwarf|r";  // 3
            case RACE_NIGHTELF:
                return "|cff00ff00Night Elf|r";  // 4
            case RACE_UNDEAD_PLAYER:
                return "|cff00ff00Undead|r";  // 5
            case RACE_TAUREN:
                return "|cff00ff00Tauren|r";  // 6
            case RACE_GNOME:
                return "|cff00ff00Gnome|r";  // 7
            case RACE_TROLL:
                return "|cff00ff00Troll|r";  // 8
            case RACE_GOBLIN:
                return "|cff00ff00Goblin|r";  // 9
            case RACE_BLOODELF:
                return "|cff00ff00Blood Elf|r";  // 10
            case RACE_DRAENEI:
                return "|cff00ff00High Elf|r";  // 11 (your swap)
            // case 12:               return "|cff00ff00Fel Orc|r";
            case RACE_WORGEN:
                return "|cff00ff00Worgen|r";
            // For future xpac ports:
            case 25:
                return "|cff00ff00Pandaren|r";
            case 26:
                return "|cff00ff00Pandaren|r";
            case 27:
                return "|cff00ff00Nightbourne|r";
            case 28:
                return "|cff00ff00Highmountain Tauren|r";
            case 29:
                return "|cff00ff00Void Elf|r";
            case 30:
                return "|cff00ff00Lightforged Draenei|r";
            case 31:
                return "|cff00ff00Zandalari Troll|r";
            case 32:
                return "|cff00ff00Kul Tiran|r";
            case 34:
                return "|cff00ff00Dark Iron Dwarf|r";
            case 35:
                return "|cff00ff00Vulpera|r";
            case 36:
                return "|cff00ff00Mag'har Orc|r";
            case 37:
                return "|cff00ff00Mechagnome|r";
            case 52:
                return "|cff00ff00Dracthyr|r";
            case 70:
                return "|cff00ff00Dracthyr|r";
            case 84:
                return "|cff00ff00Earthen Dwarf|r";
            case 85:
                return "|cff00ff00Earthen Dwarf|r";
            default:
                return "Unknown Race";
        }
    }

    std::string ColorName(Player* player) { return "|cff00ff00" + std::string(player->GetName()) + "|r"; }

    std::string ColorFaction(Player* player)
    {
        if (player->GetTeam() == ALLIANCE)
            return "|cff0070FFAlliance|r";
        else
            return "|cffFF0000Horde|r";
    }

    std::string GetPlayerZone(Player* player)
    {
        uint32 zoneId = player->GetZoneId();
        AreaTableEntry const* zoneEntry = sAreaTableStore.LookupEntry(zoneId);

        if (!zoneEntry) return "|cff00ff00Deep in the Wilderness|r";

        LocaleConstant locale = DEFAULT_LOCALE;
        if (WorldSession* session = player->GetSession()) locale = session->GetSessionDbcLocale();

        std::string name = zoneEntry->AreaName ? std::string(zoneEntry->AreaName) : std::string();

        return "|cff00ff00" + name + "|r";
    }

    void SendSpyMessage(Player* player, bool login)
    {
        std::string race = GetRaceColored(player);
        std::string name = ColorName(player);
        std::string cls = GetClassColored(player);
        std::string faction = ColorFaction(player);
        std::string area = GetPlayerZone(player);

        std::string msg;
        if (login)
        {
            msg = "Local spies report that a " + race + " " + cls + " named " + name + " has logged in near " + area +
                  "! Intelligence indicates they are loyal to the " + faction + ".";
        }
        else
        {
            msg = "Local spies report that a " + race + " " + cls + " named " + name + " has logged out from " + area +
                  "! Agents confirm their allegiance to the " + faction + ".";
        }

        sWorld->SendServerMessage(SERVER_MSG_STRING, msg.c_str());
    }
}  

class player_login_logout_spy : public PlayerScript
{
   public:
    player_login_logout_spy() : PlayerScript("player_login_logout_spy") {}

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (!sConfigMgr->GetBoolDefault("SpyAnnouncer.Enable", true)) return;

        SendSpyMessage(player, true);
    }

    void OnLogout(Player* player) override
    {
        if (!sConfigMgr->GetBoolDefault("SpyAnnouncer.Enable", true)) return;

        SendSpyMessage(player, false);
    }
};

void AddSC_Announcer() { new player_login_logout_spy(); }


