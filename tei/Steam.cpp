#include "Steam.h"

#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <steam_api.h>
#pragma warning(pop)

#include <tei/internal/utility.h>

void OutputDebugString(std::string_view msg)
{
	std::cerr << "[DEBUG] Steam: " << msg << std::endl;
}

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }
struct Achievement_t
{
	int m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class CSteamAchievements
{
private:
	int64 m_iAppID; // Our current AppID
	Achievement_t *m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CSteamAchievements(Achievement_t *Achievements, int NumAchievements);
	~CSteamAchievements() = default;

	bool RequestStats();
	bool SetAchievement(const char* ID);

	STEAM_CALLBACK( CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t,
		m_CallbackUserStatsReceived );
	STEAM_CALLBACK( CSteamAchievements, OnUserStatsStored, UserStatsStored_t,
		m_CallbackUserStatsStored );
	STEAM_CALLBACK( CSteamAchievements, OnAchievementStored,
		UserAchievementStored_t, m_CallbackAchievementStored );
};

CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements):
 m_iAppID( 0 ),
 m_bInitialized( false ),
 m_CallbackUserStatsReceived( this, &CSteamAchievements::OnUserStatsReceived ),
 m_CallbackUserStatsStored( this, &CSteamAchievements::OnUserStatsStored ),
 m_CallbackAchievementStored( this, &CSteamAchievements::OnAchievementStored )
{
     m_iAppID = SteamUtils()->GetAppID();
     m_pAchievements = Achievements;
     m_iNumAchievements = NumAchievements;
     RequestStats();
}

bool CSteamAchievements::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	if ( NULL == SteamUserStats() || NULL == SteamUser() )
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if ( !SteamUser()->BLoggedOn() )
	{
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteamAchievements::SetAchievement(const char* ID)
{
	// Have we received a call back from Steam yet?
	if (m_bInitialized)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	// If not then we can't set achievements yet
	return false;
}

void CSteamAchievements::OnUserStatsReceived( UserStatsReceived_t *pCallback )
{
	// we may get callbacks for other games' stats arriving, ignore them
	if ( uint64_t(m_iAppID) == pCallback->m_nGameID )
	{
		if ( k_EResultOK == pCallback->m_eResult )
		{
			OutputDebugString("Received stats and achievements from Steam\n");
			m_bInitialized = true;

			// load achievements
			for ( int iAch = 0; iAch < m_iNumAchievements; ++iAch )
			{
				Achievement_t &ach = m_pAchievements[iAch];

				SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
				_snprintf( ach.m_rgchName, sizeof(ach.m_rgchName), "%s",
					SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
					"name"));
				_snprintf( ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s",
					SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
					"desc"));
			}
		}
		else
		{
			char buffer[128];
			_snprintf( buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult );
			OutputDebugString( buffer );
		}
	}
}

void CSteamAchievements::OnUserStatsStored( UserStatsStored_t *pCallback )
{
	// we may get callbacks for other games' stats arriving, ignore them
	if ( uint64_t(m_iAppID) == pCallback->m_nGameID )
	{
		if ( k_EResultOK == pCallback->m_eResult )
		{
			OutputDebugString( "Stored stats for Steam\n" );
		}
		else
		{
			char buffer[128];
			_snprintf( buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult );
			OutputDebugString( buffer );
		}
	}
}

void CSteamAchievements::OnAchievementStored( UserAchievementStored_t *pCallback )
{
     // we may get callbacks for other games' stats arriving, ignore them
     if ( uint64_t(m_iAppID) == pCallback->m_nGameID )
     {
          OutputDebugString( "Stored Achievement for Steam\n" );
     }
}

#include "steam_api.h"

// Defining our achievements
enum EAchievements
{
	ACH_WIN_ONE_GAME = 0,
	ACH_WIN_100_GAMES = 1,
	ACH_TRAVEL_FAR_ACCUM = 2,
	ACH_TRAVEL_FAR_SINGLE = 3,
};

// Achievement array which will hold data about the achievements and their state
Achievement_t g_Achievements[] =
{
	_ACH_ID( ACH_WIN_ONE_GAME, "Winner" ),
	_ACH_ID( ACH_WIN_100_GAMES, "Champion" ),
	_ACH_ID( ACH_TRAVEL_FAR_ACCUM, "Interstellar" ),
	_ACH_ID( ACH_TRAVEL_FAR_SINGLE, "Orbiter" ),
};

// Global access to Achievements object
std::unique_ptr<CSteamAchievements> g_SteamAchievements{};

bool g_Init = false;

bool SteamInitialize()
{
	// Initialize Steam
	g_Init = SteamAPI_Init();
	// Create the SteamAchievements object if Steam was successfully initialized
	if (g_Init)
	{
		g_SteamAchievements.reset(new CSteamAchievements(g_Achievements, 4));
	}

	return g_Init;
}

void SteamUpdate()
{
	if (g_Init)
		SteamAPI_RunCallbacks();
}

void SteamCleanup()
{
	if (g_Init)
	g_SteamAchievements.reset();
	SteamAPI_Shutdown();
}

bool SteamSetAchievement(std::string_view ach)
{
	if (g_Init)
		if (g_SteamAchievements)
			return g_SteamAchievements->SetAchievement(ach.data());
	return false;
}
