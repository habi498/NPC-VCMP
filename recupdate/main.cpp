#include <iostream>
#include <stdint.h>
#include "SYNC_DATA.h"
#include <tclap/CmdLine.h>
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
#define NPC_RECFILE_IDENTIFIER_OLD 1000
#define NPC_RECFILE_IDENTIFIER_NEW 1001
#define PLAYER_RECORDING_TYPE_DRIVER	2
#define PLAYER_RECORDING_TYPE_ONFOOT	1
bool FileExists( const std::string &Filename )
{
    return access( Filename.c_str(), 0 ) == 0;
}
int main(int argc, char** argv)
{
	try{
		// Define the command line object.
        CmdLine cmd("npcclient rec-file updater ", ' ', "2.0");

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
    if (identifier != NPC_RECFILE_IDENTIFIER_OLD)
    {
        //NOT NPC first version recording
        cout<< "This file does not belong to old npc .rec file viz ( npcclient v.1.1) \n"<<endl;
        return 0;
    }
	int newidentifier = NPC_RECFILE_IDENTIFIER_NEW;
    int rectype;
    m = fread(&rectype, sizeof(int), 1, pFile);
    if (m != 1)
		return 0;
    if (rectype != PLAYER_RECORDING_TYPE_ONFOOT && rectype!=PLAYER_RECORDING_TYPE_DRIVER)
    {
        cout << "Recording type mismatch. Aborting.."<<endl;
        return 0;
    }else cout << "Recording type is " << rectype <<endl;
	if( rectype == PLAYER_RECORDING_TYPE_DRIVER )
	{
		cout << "Nothing to do for PLAYER_RECORDING_TYPE_DRIVER recordings" <<endl;
	}else 
	{
		size_t count = fwrite(&newidentifier, sizeof(newidentifier), 1, oFile);
		if (count != 1)return 0;
		count = fwrite(&rectype, sizeof(rectype), 1, oFile);
		if (count != 1)return 0;
		do
		{
			ONFOOT_DATABLOCK m_pOfDatablock;
		
			count=fread(&m_pOfDatablock, sizeof(m_pOfDatablock), 1, pFile);
			if (count != 1)
			{
				if (feof(pFile))return true; //end of file reached.
				return false;
			}
			
			size_t t = fwrite((void*)&m_pOfDatablock, sizeof(m_pOfDatablock), 1, oFile);
			if(t!=1)return 0;
			bool bIsReloading=false; //1 byte
			count = fwrite(&bIsReloading, sizeof(bIsReloading), 1, oFile );
			if(count!=1)return 0;
			uint16_t wAmmo = 1; // 2 byte
			count = fwrite(&wAmmo, sizeof(wAmmo), 1, oFile );
			if(count!=1)return 0;
		}while(true);
	}
	return true;
}

