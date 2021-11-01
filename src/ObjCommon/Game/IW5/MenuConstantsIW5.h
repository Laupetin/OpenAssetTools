#pragma once

#include "Game/IW5/IW5.h"

namespace IW5
{
    inline const char* g_expFunctionNames[]
    {
        "NOOP",
        ")",
        "*",
        "/",
        "%",
        "+",
        "-",
        "!",
        "<",
        "<=",
        ">",
        ">=",
        "==",
        "!=",
        "&&",
        "||",
        "(",
        ",",
        "&",
        "|",
        "~",
        "<<",
        ">>",
        "dvarint(static)",
        "dvarbool(static)",
        "dvarfloat(static)",
        "dvarstring(static)",
        "int",
        "string",
        "float",
        "sin",
        "cos",
        "min",
        "max",
        "milliseconds",
        "localclientuimilliseconds",
        "dvarint",
        "dvarbool",
        "dvarfloat",
        "dvarstring",
        "ui_active",
        "flashbanged",
        "usingvehicle",
        "missilecam",
        "scoped",
        "scopedthermal",
        "scoreboard_visible",
        "inkillcam",
        "inkillcamnpc",
        "player",
        "getperk",
        "selecting_location",
        "selecting_direction",
        "team",
        "otherteam",
        "marinesfield",
        "opforfield",
        "menuisopen",
        "writingdata",
        "inlobby",
        "ingamelobby",
        "inprivateparty",
        "privatepartyhost",
        "privatepartyhostinlobby",
        "aloneinparty",
        "adsjavelin",
        "weaplockblink",
        "weapattacktop",
        "weapattackdirect",
        "weaplocking",
        "weaplocked",
        "weaplocktooclose",
        "weaplockscreenposx",
        "weaplockscreenposy",
        "secondsastime",
        "tablelookup",
        "tablelookupbyrow",
        "tablegetrownum",
        "locstring",
        "localvarint",
        "localvarbool",
        "localvarfloat",
        "localvarstring",
        "timeleft",
        "secondsascountdown",
        "gamemsgwndactive",
        "gametypename",
        "gametype",
        "gametypedescription",
        "scoreatrank",
        "spectatingclient",
        "spectatingfree",
        "keybinding",
        "actionslotusable",
        "hudfade",
        "maxrecommendedplayers",
        "acceptinginvite",
        "isintermission",
        "gamehost",
        "partyismissingmappack",
        "partymissingmappackerror",
        "anynewmappacks",
        "amiselected",
        "partystatusstring",
        "attachedcontrollercount",
        "issplitscreenonlinepossible",
        "splitscreenplayercount",
        "getplayerdata",
        "getplayerdatasplitscreen",
        "getmatchrulesdata",
        "getsavedmatchrulesmetadata",
        "levelforexperiencemp",
        "levelforexperienceso",
        "isitemunlocked",
        "isitemunlockedsplitscreen",
        "iscardiconunlocked",
        "iscardtitleunlocked",
        "iscardiconnew",
        "iscardtitlenew",
        "iscardiconunlockedsplitscreen",
        "iscardtitleunlockedsplitscreen",
        "iscardiconnewsplitscreen",
        "iscardtitlenewsplitscreen",
        "isprofileitemunlocked",
        "isprofileitemunlockedsplitscreen",
        "isprofileitemnew",
        "isprofileitemnewsplitscreen",
        "debugprint",
        "getplayerdataanybooltrue",
        "getprofileanybooltrue",
        "weaponclassnew",
        "weaponname",
        "isreloading",
        "savegameavailable",
        "unlockeditemcount",
        "unlockeditemcountsplitscreen",
        "unlockeditem",
        "unlockeditemsplitscreen",
        "radarisjammed",
        "radarjamintensity",
        "radarisenabled",
        "isempjammed",
        "playerads",
        "weaponheatactive",
        "weaponheatvalue",
        "weaponheatoverheated",
        "getsplashtext",
        "getsplashdescription",
        "getsplashmaterial",
        "splashhasicon",
        "splashrownum",
        "getfocuseditemname",
        "getfocuseditemx",
        "getfocuseditemy",
        "getfocuseditemwidth",
        "getfocuseditemheight",
        "getitemx",
        "getitemy",
        "getitemwidth",
        "getitemheight",
        "playlist",
        "scoreboardexternalmutenotice",
        "getclientmatchdata",
        "getclientmatchdatadef",
        "getmapname",
        "getmapimage",
        "getmapcustom",
        "getmigrationstatus",
        "getplayercardinfo",
        "isofflineprofileselected",
        "coopplayer",
        "iscoop",
        "getpartystatus",
        "getsearchparams",
        "gettimeplayed",
        "isselectedplayerfriend",
        "getcharbyindex",
        "getprofiledata",
        "getprofiledatasplitscreen",
        "isprofilesignedin",
        "getwaitpopupstatus",
        "getnattype",
        "getlocalizednattype",
        "getadjustedsafeareahorizontal",
        "getadjustedsafeareavertical",
        "connectioninfo",
        "offlineprofilecansave",
        "userwithoutofflineprofilewarning",
        "allsplitscreenprofilescansave",
        "allsplitscreenprofilesaresignedin",
        "dowehavemappack",
        "mayinviteplayertoparty",
        "getpatchnotes",
        "getgameinfos",
        "coopready",
        "votecast",
        "votepassed",
        "getmapvotemapimage",
        "getmapvotemapname",
        "mapvotegametypename",
        "isfriendinvitable",
        "isfriendjoinable",
        "getsortedchallengeindex",
        "getsortedchallengename",
        "getsortedchallengecount",
        "getfilterchallengecount",
        "getfilterchallengelockedcount",
        "getfilterchallengecompletecount",
        "issortedchallengetiered",
        "getchallengefiltercachecount",
        "getchallengefiltercachecompletecount",
        "iscoopsearching",
        "coopshowpublictype",
        "coopdisplayablegroupnum",
        "coophasrequiredonlinefiles",
        "getTextWidth",
        "getTextHeight",
        "isdeveloper",
        "isusingairburst",
        "getairburstmeters",
        "getcrosshairtracemeters",
        "getfacebookstatustext",
        "isfacebookloggedin",
        "isfacebookchecking",
        "isfacebookallowed",
        "getprivatepartystatus",
        "includedinmaprotation",
        "select",
        "isdemoplaying",
        "getusergrouptext",
        "getusergroupcommoninteresttotal",
        "isdemofollowcamera",
        "isdemofreecamera",
        "isdemocapturingscreenshot",
        "ispartyhostwaitingonmembers",
        "ispopuppartymemberaway",
        "isselectedpartymemberaway",
        "gettime",
        "gameendtime",
        "hasfocus",
        "menuhasfocus",
        "getdemosegmentcount",
        "getdemosegmentinformation",
        "isclipmodified",
        "isusingmatchrulesdata",
        "isguest",
        "getfacebookhelptext",
        "iseliteclanallowed",
        "isentitlementsallowed",
        "isusergroupsallowed",
        "iswaitingforonlineservices",
        "getTextWidthModCase",
        "getSaveScreenTitle",
        "getSaveScreenDescription",
        "getOnlineVaultSelectedItemData",
        "isOnlineVaultRestricted",
        "isContentServerTaskInProgress",
        "getContentServerTaskProgress",
        "getRecentGamesSelectedItemData",
        "gametypenameAbbreviated",
        "mapvotegametypenameAbbreviated",
        "isusersignedintolive",
        "usercanplayonline",
        "getPastTitleRank",
        "getFeederData",
        "partyclientsuptodate",
        "truncateTextWithEllipsis",
        "uistarted",
        "canRenderClip",
        "getpreviewmapcustom",
        "getdlcmapsavailablecount",
        "isusersignedin",
        "isUsingIntermissionTimer",
        "isUsingCustomMapRotation",
        "menuistopmost",
        "facebook_isplatfromfriend",
        "eliteclan_isplatfromfriend",
        "eliteclan_isme",
        "eliteclan_isleader",
        "isusersignedinforvault",
        "getusingmatchrulesdata",
        "canuseraccessonlinevault",
        "friend_getgamertag",
        "recentplayer_getgamertag",
        "liveparty_getgamertag",
        "facebook_getgamertag",
        "eliteclan_getgamertag",
        "liveparty_isme",
        "liveparty_islocal",
        "doubleclickwasrightclick",
        "isdemocliprecording",
        "getIndexFromString",
        "getStringWithoutIndex",
        "eliteclan_getname",
        "eliteclan_gethelp",
        "eliteclan_getmotd",
        "eliteclan_ismember",
        "eliteclan_isemblem_ok",
        "facebook_friends_show_next",
        "facebook_friends_show_prev",
        "getOnlineVaultFriendGamerTag",
        "getObjectiveListHeight",
        "isClientDemoEnabled",
        "isusersignedintodemonware",
        "customClassIsRestricted",
        "weaponIsRestricted",
        "anysplitscreenprofilesaresignedin",
        "isguestsplitscreen",
        "isitemunlockedbyclient",
        "isanyusersignedintolive",
        "getPastTitlePrestige",
        "splitscreenactivegamepadcount",
        "showFriendPlayercard",
        "getFriendPlayercardPresence",
        "showRecentPlayerGroupIcon",
        "getwrappedtextheight",
        "canClientSave",
        "getgameinvitescount",
        "issplitscreengamerliveenabled",
        "so_coopShowCommonGroupIcon",
        "stripColorsFromString",
        "DEPRECATED",
        "ischallengeperiodic",
        "getchallengedata",
        "iseliteapppresent",
        "eliteclan_selectedisme",
        "enoughStorageSpaceForClientDemo",
        "isusersignedinforcommerce",
        "getfacebookmenutext",
        "getfacebookisposting",
        "meetplayer_isplatformfriend",
        "isselectedplayerguest",
        "getsplitscreencontrollerclientnum",
        "isClientDemoEnabledSplitScreen",
        "ItemCanTakeFocus",
        "getTimeSinceLastDoubleClick",
        "isServerListRefreshing",
        "isRecipeNameValid",
        "recipeExists",
        "getfacebookoptionshelptext",
        "dowehaveallavailablemappacks",
        "isThereNewEliteItems",
        "isPayingSubscriber",
        "localuser_ismissingmappack",
        "localuser_missingmappackerror",
        "getFirstSpecOpsDLCMap",
        "localuser_missingmapname",
        "showStoreNew",
        "commerce_getstatus",
        "isManifestDownloaded",
        "areAllItemsUnlocked",
        "commerce_getstatuscond",
        "doWeHaveMissingOwnedContent",
    };

    inline const ItemExpressionTargetBinding floatExpressionTargetBindings[ITEM_FLOATEXP_TGT_COUNT]
    {
        {ITEM_FLOATEXP_TGT_RECT_X, "rect", "x"},
        {ITEM_FLOATEXP_TGT_RECT_Y, "rect", "y"},
        {ITEM_FLOATEXP_TGT_RECT_W, "rect", "w"},
        {ITEM_FLOATEXP_TGT_RECT_H, "rect", "h"},
        {ITEM_FLOATEXP_TGT_FORECOLOR_R, "forecolor", "r"},
        {ITEM_FLOATEXP_TGT_FORECOLOR_G, "forecolor", "g"},
        {ITEM_FLOATEXP_TGT_FORECOLOR_B, "forecolor", "b"},
        {ITEM_FLOATEXP_TGT_FORECOLOR_RGB, "forecolor", "rgb"},
        {ITEM_FLOATEXP_TGT_FORECOLOR_A, "forecolor", "a"},
        {ITEM_FLOATEXP_TGT_GLOWCOLOR_R, "glowcolor", "r"},
        {ITEM_FLOATEXP_TGT_GLOWCOLOR_G, "glowcolor", "g"},
        {ITEM_FLOATEXP_TGT_GLOWCOLOR_B, "glowcolor", "b"},
        {ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB, "glowcolor", "rgb"},
        {ITEM_FLOATEXP_TGT_GLOWCOLOR_A, "glowcolor", "a"},
        {ITEM_FLOATEXP_TGT_BACKCOLOR_R, "backcolor", "r"},
        {ITEM_FLOATEXP_TGT_BACKCOLOR_G, "backcolor", "g"},
        {ITEM_FLOATEXP_TGT_BACKCOLOR_B, "backcolor", "b"},
        {ITEM_FLOATEXP_TGT_BACKCOLOR_RGB, "backcolor", "rgb"},
        {ITEM_FLOATEXP_TGT_BACKCOLOR_A, "backcolor", "a"},
    };
}