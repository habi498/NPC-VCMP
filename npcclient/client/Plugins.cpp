/* Thanks to the sa-mp server plugin system*/
#include "main.h"
#ifndef WIN32
#include <dlfcn.h>
#define LoadLibrary(p) dlopen(p, RTLD_LAZY)
#define FreeLibrary dlclose
#define GetProcAddress dlsym
#define MAX_PATH 260
#endif
extern CFunctions* m_pFunctions;
extern funcError fn_GetLastError();
extern HSQUIRRELVM v;
void* GetSquirrelVM()
{
    return v;
}
CPlugins::CPlugins()
{

}
CPlugins::~CPlugins()
{

    PluginPoolVector::iterator itor;

    for (itor = m_Plugins.begin(); itor != m_Plugins.end(); itor++)
    {
        PluginPool_s* pPlugin = *itor;
        FreeLibrary(pPlugin->hInstance);
        delete pPlugin;
    }
}
void CPlugins::LoadPlugins(char* szSearchPath, std::string PluginsList)
{
    char szPath[MAX_PATH];
    char szFullPath[MAX_PATH];
    strcpy(szPath, szSearchPath);
#ifdef LINUX
    if (szPath[strlen(szPath) - 1] != '/')
        strcat(szPath, "/");
#else
    if (szPath[strlen(szPath) - 1] != '\\')
        strcat(szPath, "\\");
#endif
    int iPluginCount = 0;
    std::vector<char> tempPluginsList(PluginsList.begin(), PluginsList.end());
    tempPluginsList.push_back(0);

    char* szFilename = strtok(&tempPluginsList[0], " ");
    while (szFilename)
    {
        

        strcpy(szFullPath, szPath);
        strcat(szFullPath, szFilename);

        if (LoadSinglePlugin(szFullPath))
        {
            printf(" Loaded plugin: %s\n", szFilename);
        }
        else
        {
            printf("Failed loading plugin: %s\n", szFilename);
        }

        szFilename = strtok(NULL, " ");
    }
    printf(" Loaded %d plugins.\n", GetPluginCount());
}

BOOL CPlugins::LoadSinglePlugin(char* szPluginPath)
{
    PluginPool_s* pPlugin;
    pPlugin = new PluginPool_s();
    pPlugin->hInstance = LoadLibrary(szPluginPath);
    if (pPlugin->hInstance==NULL) {
        delete pPlugin;
        return FALSE;
    }
    pPlugin->Init = (PluginInit)GetProcAddress(pPlugin->hInstance, "PluginInit");
    if (pPlugin->Init == NULL )
    {
        printf("PluginInit function not found in plugin %s", szPluginPath);
        FreeLibrary(pPlugin->hInstance);
        delete pPlugin;
        return FALSE;
    }
    //Allocate memory
    pPlugin->pPluginInfo = (PluginInfo*)calloc(1,sizeof(PluginInfo));
    pPlugin->pPluginCalls = (PluginCallbacks*)calloc(1,sizeof(PluginCallbacks));
    pPlugin->pPluginFuncs = (PluginFuncs*)calloc(1,sizeof(PluginFuncs));
    if(pPlugin->pPluginCalls==NULL||
        pPlugin->pPluginFuncs==NULL||
        pPlugin->pPluginInfo==NULL)
        {
            printf("Memory allocation failed\n");
            return FALSE;
        }
    
    if (m_pFunctions)
    {
        pPlugin->pPluginFuncs->SendCommandToServer = m_pFunctions->SendCommandToServer;
        pPlugin->pPluginFuncs->GetLastError = fn_GetLastError;
        pPlugin->pPluginFuncs->FireSniperRifle = m_pFunctions->FireSniperRifle;
        pPlugin->pPluginFuncs->GetAngle = m_pFunctions->GetAngle;
        pPlugin->pPluginFuncs->GetPlayerName = m_pFunctions->GetPlayerName;
        pPlugin->pPluginFuncs->GetPosition = m_pFunctions->GetPosition;
        pPlugin->pPluginFuncs->RequestVehicleEnter = m_pFunctions->RequestVehicleEnter;
        pPlugin->pPluginFuncs->SendChatMessage = m_pFunctions->SendChatMessage;
        pPlugin->pPluginFuncs->SendDeathInfo = m_pFunctions->SendDeathInfo;
        pPlugin->pPluginFuncs->SendInCarSyncData = m_pFunctions->SendInCarSyncData;
        pPlugin->pPluginFuncs->SendOnFootSyncData = m_pFunctions->SendOnFootSyncData;
        pPlugin->pPluginFuncs->SendPassengerSync = m_pFunctions->SendPassengerSync;
        pPlugin->pPluginFuncs->SendShotInfo = m_pFunctions->SendShotInfo;
        pPlugin->pPluginFuncs->SetAngle = m_pFunctions->SetAngle;
        pPlugin->pPluginFuncs->SetPosition = m_pFunctions->SetPosition;
        pPlugin->pPluginFuncs->SetWeapon = m_pFunctions->SetWeapon;
        pPlugin->pPluginFuncs->GetPlayerName = m_pFunctions->GetPlayerName;
        pPlugin->pPluginFuncs->GetPlayerPosition = m_pFunctions->GetPlayerPosition;
        pPlugin->pPluginFuncs->GetPlayerAngle = m_pFunctions->GetPlayerAngle;
        pPlugin->pPluginFuncs->GetPlayerHealth = m_pFunctions->GetPlayerHealth;
        pPlugin->pPluginFuncs->GetPlayerArmour = m_pFunctions->GetPlayerArmour;
        pPlugin->pPluginFuncs->GetPlayerWeapon = m_pFunctions->GetPlayerWeapon;
        pPlugin->pPluginFuncs->IsPlayerCrouching = m_pFunctions->IsPlayerCrouching;
        pPlugin->pPluginFuncs->IsPlayerReloading = m_pFunctions->IsPlayerReloading;
        pPlugin->pPluginFuncs->GetPlayerKeys = m_pFunctions->GetPlayerKeys;
        pPlugin->pPluginFuncs->GetPlayerSpeed = m_pFunctions->GetPlayerSpeed;
        pPlugin->pPluginFuncs->GetPlayerAimDir = m_pFunctions->GetPlayerAimDir;
        pPlugin->pPluginFuncs->GetPlayerAimPos = m_pFunctions->GetPlayerAimPos;
        pPlugin->pPluginFuncs->GetPlayerWeaponAmmo = m_pFunctions->GetPlayerWeaponAmmo;
        pPlugin->pPluginFuncs->GetPlayerState = m_pFunctions->GetPlayerState;
        pPlugin->pPluginFuncs->GetPlayerVehicle = m_pFunctions->GetPlayerVehicle;
        pPlugin->pPluginFuncs->GetPlayerSeat = m_pFunctions->GetPlayerSeat;
        pPlugin->pPluginFuncs->GetPlayerSkin = m_pFunctions->GetPlayerSkin;
        pPlugin->pPluginFuncs->GetPlayerTeam = m_pFunctions->GetPlayerTeam;
        pPlugin->pPluginFuncs->GetPlayerWeaponAtSlot = m_pFunctions->GetPlayerWeaponAtSlot;
        pPlugin->pPluginFuncs->GetPlayerAmmoAtSlot = m_pFunctions->GetPlayerAmmoAtSlot;
        pPlugin->pPluginFuncs->GetVehicleRotation = m_pFunctions->GetVehicleRotation;
        pPlugin->pPluginFuncs->GetVehicleModel = m_pFunctions->GetVehicleModel;
        pPlugin->pPluginFuncs->GetVehiclePosition = m_pFunctions->GetVehiclePosition;
        pPlugin->pPluginFuncs->GetVehicleDriver = m_pFunctions->GetVehicleDriver;
        pPlugin->pPluginFuncs->GetVehicleHealth = m_pFunctions->GetVehicleHealth;
        pPlugin->pPluginFuncs->GetVehicleDamage = m_pFunctions->GetVehicleDamage;
        pPlugin->pPluginFuncs->GetVehicleSpeed = m_pFunctions->GetVehicleSpeed;
        pPlugin->pPluginFuncs->GetVehicleTurretRotation = m_pFunctions->GetVehicleTurretRotation;
        pPlugin->pPluginFuncs->IsVehicleStreamedIn = m_pFunctions->IsVehicleStreamedIn;
        pPlugin->pPluginFuncs->IsPlayerStreamedIn = m_pFunctions->IsPlayerStreamedIn;
        pPlugin->pPluginFuncs->IsPlayerSpawned = m_pFunctions->IsPlayerSpawned;
        pPlugin->pPluginFuncs->IsPlayerConnected = m_pFunctions->IsPlayerConnected;
        pPlugin->pPluginFuncs->GetSquirrelVM = GetSquirrelVM;
    }
    else return FALSE;
    if (!pPlugin->Init(pPlugin->pPluginFuncs, pPlugin->pPluginCalls, pPlugin->pPluginInfo))
    {
        // Initialize failed!
        FreeLibrary(pPlugin->hInstance);
        free(pPlugin->pPluginCalls);
        free(pPlugin->pPluginFuncs);
        free(pPlugin->pPluginInfo);
        delete pPlugin;
        return FALSE;
    }
    m_Plugins.push_back(pPlugin);
    return TRUE;
}

DWORD CPlugins::GetPluginCount()
{
    return (DWORD)m_Plugins.size();
}

PluginPool_s* CPlugins::GetPlugin(DWORD index)
{
    return m_Plugins[index];
}
