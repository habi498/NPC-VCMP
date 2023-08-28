#include <iostream>
#include <stdint.h>
#include "SYNC_DATA.h"

#include "tclap/CmdLine.h"
#include <algorithm>
using namespace TCLAP;
using namespace std;
bool ConvertRecFile( string ifile, string ofile );
#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif
#define NPC_RECFILE_IDENTIFIER_V1 1000
#define NPC_RECFILE_IDENTIFIER_V2 1001
#define NPC_RECFILE_IDENTIFIER_V3 1002
#define NPC_RECFILE_IDENTIFIER_V4 1004 //v4==1004
#define PLAYER_RECORDING_TYPE_DRIVER	2
#define PLAYER_RECORDING_TYPE_ONFOOT	1

/* Action IDs Credits: vito https ://forum.vc-mp.org/index.php?msg=53263*/
#define ACTION_NONE 0x00
#define ACTION_NORMAL 0x01
#define ACTION_AIMING 0x0c
#define ACTION_FIRE 0x10
#define ACTION_MELEE_ATTACK 0x11

bool FileExists( const std::string &Filename )
{
    return access( Filename.c_str(), 0 ) == 0;
}
int main(int argc, char** argv)
{
	try{
		// Define the command line object.
        CmdLine cmd("npcclient rec-file updater ", ' ', "4.0");

        // Define a value argument and add it to the command line.
        ValueArg<string> fileNameArg("i", "file", "The input file with .rec file", true, "" ,
                                 "string");
        cmd.add(fileNameArg);
		
		ValueArg<string> outputfileNameArg("o", "outputfile", "The output file name with .rec extension", true, "" ,
                                 "string");
        
        
        cmd.add(outputfileNameArg);
        // Parse the args.
        cmd.parse(argc, argv);
        // Get the value parsed by each arg.
        string inputfileName = fileNameArg.getValue();
		string outputfileName = outputfileNameArg.getValue();
		bool success = ConvertRecFile( inputfileName, outputfileName );
		if( success )cout << "Done. Wrote to " << outputfileName.c_str() <<endl;
		else cout << "An error occured";
	return 1;
	}catch (TCLAP::ArgException &e)  // catch exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}

bool ConvertRecFile( string ifile, string ofile )
{
	bool bfexists = FileExists( ifile );
	if( !bfexists ){
		cout << ifile <<" does not exist."<<std::endl;
		return false;
	}
	FILE* pFile = fopen(ifile.c_str(), "rb");
	if(! pFile)
		return false;
	FILE* oFile = fopen(ofile.c_str(), "wb");
	if(!oFile)
		return false;
	int identifier;
    size_t m = fread(&identifier, sizeof(int), 1, pFile);
    if (m != 1)
		return false;
	if (identifier == NPC_RECFILE_IDENTIFIER_V1
		|| identifier==NPC_RECFILE_IDENTIFIER_V2
		||identifier==NPC_RECFILE_IDENTIFIER_V3)
	{
		int newidentifier = NPC_RECFILE_IDENTIFIER_V4;
		int rectype;
		m = fread(&rectype, sizeof(int), 1, pFile);
		if (m != 1)
			return 0;
		if (rectype != PLAYER_RECORDING_TYPE_ONFOOT && rectype != PLAYER_RECORDING_TYPE_DRIVER)
		{
			cout << "Recording type mismatch. Aborting.." << endl;
			return 0;
		}
		else cout << "Recording type is " << rectype << endl;
		if (rectype == PLAYER_RECORDING_TYPE_DRIVER)
		{
			size_t count = fwrite(&newidentifier, sizeof(newidentifier), 1, oFile);
			if (count != 1)return 0;
			count = fwrite(&rectype, sizeof(rectype), 1, oFile);
			if (count != 1)return 0;
			do
			{
				if (identifier == NPC_RECFILE_IDENTIFIER_V1 ||
					identifier == NPC_RECFILE_IDENTIFIER_V2)
				{
					INCAR_DATABLOCK_BEFOREV3 m_pIcDatablock;
					count = fread(&m_pIcDatablock, sizeof(m_pIcDatablock), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}

					size_t t = fwrite((void*)&m_pIcDatablock, sizeof(m_pIcDatablock), 1, oFile);
					if (t != 1)return 0;
					uint16_t wAmmo = 1; // 2 byte
					size_t count = fwrite(&wAmmo, sizeof(wAmmo), 1, oFile);
					if (count != 1)return 0;
				}
				else if (identifier == NPC_RECFILE_IDENTIFIER_V3)
				{
					//do nothing new
					INCAR_DATABLOCK_V3 m_pIcDatablock;
					count = fread(&m_pIcDatablock, sizeof(m_pIcDatablock), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}

					size_t t = fwrite((void*)&m_pIcDatablock, sizeof(m_pIcDatablock), 1, oFile);
					if (t != 1)return 0;
					//no new fields
				}
			} while (true);

		}
		else
		{
			size_t count = fwrite(&newidentifier, sizeof(newidentifier), 1, oFile);
			if (count != 1)return 0;
			count = fwrite(&rectype, sizeof(rectype), 1, oFile);
			if (count != 1)return 0;
			
			do
			{
				
				if (identifier == NPC_RECFILE_IDENTIFIER_V1)
				{
					
					ONFOOT_DATABLOCK_V1 m_pOfDatablock;

					count = fread(&m_pOfDatablock, sizeof(m_pOfDatablock), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}

					size_t t = fwrite((void*)&m_pOfDatablock, sizeof(m_pOfDatablock), 1, oFile);
					if (t != 1)return 0;
					bool bIsReloading = false; //1 byte
					count = fwrite(&bIsReloading, sizeof(bIsReloading), 1, oFile);
					if (count != 1)return 0;
					uint16_t wAmmo = 1; // 2 byte
					count = fwrite(&wAmmo, sizeof(wAmmo), 1, oFile);
					if (count != 1)return 0;
					//No reloading ability
					uint8_t byteAction = ACTION_NONE;
					ONFOOT_SYNC_DATA_V1 m_pOfSyncData = m_pOfDatablock.m_pOfSyncData;
					if ((m_pOfSyncData.dwKeys&64)&&!(m_pOfSyncData.dwKeys&512))
						byteAction = ACTION_NORMAL; //when reloading keys=64, firing 572
					else
					{
						if ((m_pOfSyncData.byteCurrentWeapon == 26
							|| m_pOfSyncData.byteCurrentWeapon == 27
							|| m_pOfSyncData.byteCurrentWeapon == 28
							|| m_pOfSyncData.byteCurrentWeapon == 29
							|| m_pOfSyncData.byteCurrentWeapon == 30
							|| m_pOfSyncData.byteCurrentWeapon == 32) &&
							m_pOfSyncData.dwKeys & 0x01) //aiming
						{
							byteAction = ACTION_AIMING;
						}
						else if (m_pOfSyncData.dwKeys & 0x40) //attacking
						{
							if (m_pOfSyncData.byteCurrentWeapon <= 10)//melee weapons
								byteAction = ACTION_MELEE_ATTACK;
							else
								byteAction = ACTION_FIRE;
						}
					}

					count = fwrite(&byteAction, sizeof(byteAction), 1, oFile);
					if (count != 1)return 0;
					uint8_t byteReserved = 0; //1 byte
					count = fwrite(&byteReserved, sizeof(byteReserved), 1, oFile);
					if (count != 1)return 0;

				}
				else if(identifier==NPC_RECFILE_IDENTIFIER_V2||identifier==NPC_RECFILE_IDENTIFIER_V3)
				{
					ONFOOT_DATABLOCK_V2ANDV3 m_pOfDatablock;

					count = fread(&m_pOfDatablock, sizeof(m_pOfDatablock), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}
					size_t t = fwrite((void*)&m_pOfDatablock, sizeof(m_pOfDatablock), 1, oFile);
					if (t != 1)return 0;
					//Now write byteAction and byteReserved. If we write both as 0, then
					//when playback, it cause weapons not to fire 

					//Determining actual player action using player keys
					uint8_t byteAction = ACTION_NONE;
					ONFOOT_SYNC_DATA_V2ANDV3 m_pOfSyncData = m_pOfDatablock.m_pOfSyncData;
					if (m_pOfSyncData.bIsReloading)
						byteAction = ACTION_NORMAL;
					else
					{
						if ((m_pOfSyncData.byteCurrentWeapon == 26
							|| m_pOfSyncData.byteCurrentWeapon == 27
							|| m_pOfSyncData.byteCurrentWeapon == 28
							|| m_pOfSyncData.byteCurrentWeapon == 29
							|| m_pOfSyncData.byteCurrentWeapon == 30
							|| m_pOfSyncData.byteCurrentWeapon == 32)&&
							m_pOfSyncData.dwKeys & 0x01) //aiming
						{
							byteAction = ACTION_AIMING;
						}
						else if (m_pOfSyncData.dwKeys & 0x40) //attacking
						{
							if (m_pOfSyncData.byteCurrentWeapon <= 10)//melee weapons
								byteAction = ACTION_MELEE_ATTACK;
							else
								byteAction = ACTION_FIRE;
						}
					}
					
					count = fwrite(&byteAction, sizeof(byteAction), 1, oFile);
					if (count != 1)return 0;
					uint8_t byteReserved = 0; //1 byte
					count = fwrite(&byteReserved, sizeof(byteReserved), 1, oFile);
					if (count != 1)return 0;
				}
				
			} while (true);
		}
		
	}
	else {
        //NOT NPC first version recording
        cout<< "This file does not belong to old npc .rec files) \n"<<endl;
        return 0;
    }
	return true;
}

