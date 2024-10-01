#include <iostream>
#include <stdint.h>
#include "../../include/Utils.h"
#include "SYNC_DATA.h"
#include "tclap/CmdLine.h"
#include <algorithm>
using namespace TCLAP;
using namespace std;
bool ConvertRecFile( string ifile, string ofile );
bool ReadPacketFromFile(uint8_t bytePacketType, FILE* file, void* buffer, size_t sizeofbuffer, uint8_t* bytesRead);
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
#define NPC_RECFILE_IDENTIFIER_V5 1005 //From December 2023 onwards
//#define NPC_RECFILE_IDENTIFIER_V6 1006 //From August 2024 onwards
#define PLAYER_RECORDING_TYPE_DRIVER	2
#define PLAYER_RECORDING_TYPE_ONFOOT	1
#define PLAYER_RECORDING_TYPE_ALL       3 //Since V5
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
	cout << ".rec file version is " << identifier<<"\n";
	if (identifier == NPC_RECFILE_IDENTIFIER_V1
		|| identifier==NPC_RECFILE_IDENTIFIER_V2
		||identifier==NPC_RECFILE_IDENTIFIER_V3
		||identifier==NPC_RECFILE_IDENTIFIER_V4
	//	||identifier==NPC_RECFILE_IDENTIFIER_V5
		)
	{
		//int newidentifier = NPC_RECFILE_IDENTIFIER_V6;
		int newidentifier = NPC_RECFILE_IDENTIFIER_V5;
		int rectype;
		m = fread(&rectype, sizeof(int), 1, pFile);
		if (m != 1)
			return 0;
		if (rectype != PLAYER_RECORDING_TYPE_ONFOOT && 
			rectype != PLAYER_RECORDING_TYPE_DRIVER &&
			rectype != PLAYER_RECORDING_TYPE_ALL)
		{
			cout << "Recording type is neither ONFOOT(1) or DRIVER(2) or ALL(3). Aborting.." << endl;
			return 0;
		}
		else
		{
			cout << "Recording type is " << rectype << endl;
			size_t count = fwrite(&newidentifier, sizeof(newidentifier), 1, oFile);
			if (count != 1)return 0;
			count = fwrite(&rectype, sizeof(rectype), 1, oFile);
			if (count != 1)return 0;
			uint32_t flags = 0; char name[24];

			/*if (identifier == NPC_RECFILE_IDENTIFIER_V5)
			{
				m = fread(&flags, sizeof(flags), 1, pFile);
				if (m != 1)
					return 0;
				if (flags & 1)
				{
					//Name is present
					m = fread(name, sizeof(char), sizeof(name), pFile);
					if (m != (sizeof(name)))return 0;
				}

			}*/
			count = fwrite(&flags, sizeof(uint32_t), 1, oFile);
			if (flags & 1)
			{
				count = fwrite(name, sizeof(char), sizeof(name), oFile);
				if (count != sizeof(name))return 0;
			}
			if (count != 1)return 0;
			cout << "*Written flags " <<flags << endl;
		}
		
		if (rectype == PLAYER_RECORDING_TYPE_DRIVER)
		{
			size_t count, t;
			uint16_t wAmmo;
			INCAR_DATABLOCK_BEFOREV3 m_pIcDatablock_bv3;
			INCAR_DATABLOCK_V3 m_pIcDatablock_v3;

			do
			{
				if (identifier == NPC_RECFILE_IDENTIFIER_V1 ||
					identifier == NPC_RECFILE_IDENTIFIER_V2)
				{
					count = fread(&m_pIcDatablock_bv3, sizeof(m_pIcDatablock_bv3), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}
					t = fwrite((void*)&m_pIcDatablock_bv3, sizeof(m_pIcDatablock_bv3), 1, oFile);
					if (t != 1)return 0;
					wAmmo = 1; // 2 byte
					count = fwrite(&wAmmo, sizeof(wAmmo), 1, oFile);
					if (count != 1)return 0;
				}
				else if (identifier == NPC_RECFILE_IDENTIFIER_V3||
					identifier==NPC_RECFILE_IDENTIFIER_V4
				//	||identifier== NPC_RECFILE_IDENTIFIER_V5
				)
				{
					//do nothing new
					count = fread(&m_pIcDatablock_v3, sizeof(m_pIcDatablock_v3), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}
					t = fwrite((void*)&m_pIcDatablock_v3, sizeof(m_pIcDatablock_v3), 1, oFile);
					if (t != 1)return 0;
					//no new fields
				}
			} while (true);

		}else if (rectype == PLAYER_RECORDING_TYPE_ONFOOT)
		{
			size_t count;
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
					/*VECTOR vecMatrixRight = VECTOR(0, 0, 0);
					VECTOR vecMatrixUp = VECTOR(0, 0, 0);
					VECTOR vecMatrixPosition = VECTOR(0, 0, 0);
					count = fwrite(&vecMatrixRight, sizeof(vecMatrixRight), 1, oFile);
					if (count != 1)return 0;
					count = fwrite(&vecMatrixUp, sizeof(vecMatrixUp), 1, oFile);
					if (count != 1)return 0;
					count = fwrite(&vecMatrixPosition, sizeof(vecMatrixPosition), 1, oFile);
					if (count != 1)return 0;*/
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
					/*VECTOR vecMatrixRight = VECTOR(0, 0, 0);
					VECTOR vecMatrixUp = VECTOR(0, 0, 0);
					VECTOR vecMatrixPosition = VECTOR(0, 0, 0);
					count = fwrite(&vecMatrixRight, sizeof(vecMatrixRight), 1, oFile);
					if (count != 1)return 0;
					count = fwrite(&vecMatrixUp, sizeof(vecMatrixUp), 1, oFile);
					if (count != 1)return 0;
					count = fwrite(&vecMatrixPosition, sizeof(vecMatrixPosition), 1, oFile);
					if (count != 1)return 0;*/
				}
				else if (identifier == NPC_RECFILE_IDENTIFIER_V4
					//||	identifier == NPC_RECFILE_IDENTIFIER_V5
					)
				{
					ONFOOT_DATABLOCK_V4 m_pOfDatablock;

					count = fread(&m_pOfDatablock, sizeof(m_pOfDatablock), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}
					size_t t = fwrite((void*)&m_pOfDatablock, sizeof(m_pOfDatablock), 1, oFile);
					if (t != 1)return 0;
					/*VECTOR vecMatrixRight = VECTOR(0, 0, 0);
					VECTOR vecMatrixUp = VECTOR(0, 0, 0);
					VECTOR vecMatrixPosition = VECTOR(0, 0, 0);
					count = fwrite(&vecMatrixRight, sizeof(vecMatrixRight), 1, oFile);
					if (count != 1)return 0;
					count = fwrite(&vecMatrixUp, sizeof(vecMatrixUp), 1, oFile);
					if (count != 1)return 0;
					count = fwrite(&vecMatrixPosition, sizeof(vecMatrixPosition), 1, oFile);
					if (count != 1)return 0;*/
				}
			} while (true);
		}/*
		else if (rectype == PLAYER_RECORDING_TYPE_ALL)
		{
			size_t count;
			void* buffer = malloc(1024 * sizeof(char));
			if (!buffer)
			{
				cout << "Error when allocating memory" << endl;
				return 0;
			}
			do
			{
				//Read general block.
				if (identifier == NPC_RECFILE_IDENTIFIER_V5)
				{
					GENERALDATABLOCK genblock;
					count = fread(&genblock, sizeof(genblock), 1, pFile);
					if (count != 1)
					{
						if (feof(pFile))return true; //end of file reached.
						return false;
					}
					size_t t = fwrite((void*)&genblock, sizeof(genblock), 1, oFile);
					if (t != 1)return 0;
					uint8_t bytesRead;
					if (!ReadPacketFromFile(genblock.bytePacketType, pFile, buffer, 1024,&bytesRead))
					{
						cout << "Error occured when reading packet-block from file" << endl;
						free(buffer);
						return 0;
					}
					//We have data in buffer;
					//Write it first.
					if (bytesRead)
					{
						size_t t = fwrite(buffer, sizeof(char), bytesRead, oFile);
						if (t != 1)return 0;
					}
					//Now, was it onfoot_packet/onfoot_aim
					if (genblock.bytePacketType == PACKET_ONFOOT ||
						genblock.bytePacketType == PACKET_ONFOOT_AIM)
					{
						//VECTOR vecMatrixRight = VECTOR(0, 0, 0);
						//VECTOR vecMatrixUp = VECTOR(0, 0, 0);
						//VECTOR vecMatrixPosition = VECTOR(0, 0, 0);
						//count = fwrite(&vecMatrixRight, sizeof(vecMatrixRight), 1, oFile);
						//if (count != 1)return 0;
						//count = fwrite(&vecMatrixUp, sizeof(vecMatrixUp), 1, oFile);
						//if (count != 1)return 0;
						//count = fwrite(&vecMatrixPosition, sizeof(vecMatrixPosition), 1, oFile);
						//if (count != 1)return 0;
					}
					//done
				}
				else
				{
					cout << "Unknown identifier" << endl;
					free(buffer);
					return 0;
				}
			} while (true);
			free(buffer);
		}*/
		else
		{
			//cout << "Recording type is neither ONFOOT(1) or DRIVER(2) or ALL(3). Aborting." << endl;
			cout << "Recording type is neither ONFOOT(1) or DRIVER(2). Aborting." << endl;
			return 0;
		}
		
	}
	else
	{
        //NOT NPC first version recording
        cout<< "This file does not belong to old npc .rec files) \n"<<endl;
        return 0;
    }
	return true;
}

bool ReadPacketFromFile(uint8_t bytePacketType, FILE* file, void* buffer, size_t sizeofbuffer, uint8_t* bytesRead)
{
	size_t count; uint16_t len;
	switch (bytePacketType)
	{
	case PACKET_SPAWN:
		if (sizeof(SPAWNDATA) > sizeofbuffer)return 0;
		count = fread(buffer, sizeof(SPAWNDATA), 1, file);
		if (count != 1)return 0;
		*bytesRead = sizeof(SPAWNDATA);
		break;
	case PACKET_DEATH:
		if (sizeof(DEATHDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(DEATHDATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(DEATHDATA);
		break;
	case PACKET_VEHICLE_EXIT:*bytesRead = 0;
		break;
	case PACKET_VEHICLE_REQ_ENTER:
		//same as vehicle enter
	case PACKET_VEHICLE_ENTER:
		if (sizeof(VEHICLEENTER_REQUEST) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(VEHICLEENTER_REQUEST), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(VEHICLEENTER_REQUEST);
		break;
	case PACKET_ONFOOT:
	case PACKET_ONFOOT_AIM:
		if (sizeof(ONFOOT_SYNC_DATA_V4) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(ONFOOT_SYNC_DATA_V4), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(ONFOOT_SYNC_DATA_V4);
		break;
	case PACKET_DRIVER:
		if (sizeof(INCAR_SYNC_DATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(INCAR_SYNC_DATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(INCAR_SYNC_DATA);
		break;
	case PACKET_PASSENGER:
		if (sizeof(PASSENGERDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(PASSENGERDATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(PASSENGERDATA);
		break;
	case PACKET_PLAYER_COMMAND:
	case PACKET_PLAYER_CHAT:
	case PACKET_CLIENTSCRIPT_DATA:
		if (sizeof(COMMANDDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(COMMANDDATA), 1, file);
		if (count != 1)
			return 0;
		len = *(uint16_t*)buffer;
		count = fread((void*)((uint8_t*)buffer + sizeof(COMMANDDATA)),
			sizeof(char), len, file);
		if (count != len)
			return 0;
		*bytesRead = sizeof(COMMANDDATA)+len;
		break;
	case PACKET_PLAYER_PM:
		if (sizeof(PRIVMSGDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(PRIVMSGDATA), 1, file);
		if (count != 1)
			return 0;
		len = *(uint16_t*)((uint8_t*)buffer+24);
		count = fread((void*)((uint8_t*)buffer + sizeof(PRIVMSGDATA)),
			sizeof(char), len, file);
		if (count != len)
			return 0;
		*bytesRead = sizeof(PRIVMSGDATA) + len;
		break;
	case PACKET_PICKUP_PICKED:
		if (sizeof(PICKUPATTEMPTDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(PICKUPATTEMPTDATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(PICKUPATTEMPTDATA);
		break;
	case PACKET_CHECKPOINT_ENTERED:
	case PACKET_CHECKPOINT_EXITED:
		if (sizeof(CHECKPOINTENTEREDDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(CHECKPOINTENTEREDDATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(CHECKPOINTENTEREDDATA);
		break;
	case PACKET_OBJECT_TOUCH:
		if (sizeof(OBJECTTOUCH_DATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(OBJECTTOUCH_DATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(OBJECTTOUCH_DATA);
		break;
	case PACKET_OBJECT_SHOT:
		if (sizeof(OBJECTSHOT_DATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(OBJECTSHOT_DATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(OBJECTSHOT_DATA);
		break;
	case PACKET_DISCONNECT:
		*bytesRead = 0;
		break;
	case PACKET_CHOOSECLASS:
		if (sizeof(CLASSCHOOSEDATA) > sizeofbuffer)
			return 0;
		count = fread(buffer, sizeof(CLASSCHOOSEDATA), 1, file);
		if (count != 1)
			return 0;
		*bytesRead = sizeof(CLASSCHOOSEDATA);
		break;
	 default:
		 cout << "Error: Packet type " << bytePacketType << " is unknown" << endl;
		 return 0;
		break;
	}
	
	return 1;
}