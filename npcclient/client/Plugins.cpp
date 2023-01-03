/* Thanks to the sa-mp server plugin system*/
#include "main.h"
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
        printf(" Loading plugin: %s", szFilename);

        strcpy(szFullPath, szPath);
        strcat(szFullPath, szFilename);

        if (LoadSinglePlugin(szFullPath))
        {
            printf("  Loaded.");
        }
        else
        {
            printf("Failed");
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
    pPlugin->pPluginInfo = (PluginInfo*)malloc(sizeof(PluginInfo));
    pPlugin->pPluginCalls = (PluginCallbacks*)malloc(sizeof(PluginCallbacks));
    pPlugin->pPluginFuncs = (PluginFuncs*)malloc(sizeof(PluginFuncs));
    if(pPlugin->pPluginCalls==NULL||
        pPlugin->pPluginFuncs==NULL||
        pPlugin->pPluginInfo==NULL)
        {
            printf("Memory allocation failed\n");
            return FALSE;
        }
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
