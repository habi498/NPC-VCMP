#include "main.h"
#include <mutex>
#ifdef _WIN32
#include <direct.h> // For _mkdir on Windows
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755) // POSIX systems use mkdir() with permissions
#endif
TestCB testCB;
RakNet::FileListTransfer flt;
extern std::string store_download_location;
extern std::mutex flt_mtx; //for filelisttransfer
extern bool shared_dwnld_cmpltd_signal; //flt_mtx
extern bool debug_mode;
void CreateDirectories(const std::string& path)
{
	size_t pos = 0;
	std::string dir;

	// Traverse the path to create each intermediate directory
	while ((pos = path.find('/', pos)) != std::string::npos) {
		dir = path.substr(0, pos++);
		if (dir.empty()) continue; // Skip if no valid directory part is found

		// Create the directory if it doesn't exist
		MKDIR(dir.c_str());
	}

	// Create the final directory if the path does not end with a slash
	if (!path.empty() && path.back() != '/') {
		MKDIR(path.c_str());
	}
}

bool TestCB::OnFile(OnFileStruct* onFileStruct)
{
	/*printf("OnFile: %i. (100%%) %i/%i %s %ib / %ib\n",
		onFileStruct->setID,
		onFileStruct->fileIndex + 1,
		onFileStruct->numberOfFilesInThisSet,
		onFileStruct->fileName,
		onFileStruct->byteLengthOfThisFile,
		onFileStruct->byteLengthOfThisSet);*/

	//Code by ChatGPT 4.0
	//Create necessary directories 
	std::string filePath(onFileStruct->fileName);
	filePath = store_download_location + filePath;
	// Extract the directory path (without the file name)
	size_t lastSlash = filePath.find_last_of('/');
	if (lastSlash != std::string::npos) {
		std::string dirPath = filePath.substr(0, lastSlash);
		CreateDirectories(dirPath);
	}

	//printf("Filepath is %s\n", filePath.c_str());
	FILE* fp = fopen(filePath.c_str(), "wb");
	if (!fp)
	{
		printf("Error opening file for writing\n"); exit(1);
	}
	fwrite(onFileStruct->fileData, onFileStruct->byteLengthOfThisFile, 1, fp);
	fclose(fp);

	if(debug_mode)
		printf("[FILES]Finished downloading %s\n", onFileStruct->fileName);
	
	// Return true to have RakNet delete the memory allocated to hold this file.
	// False if you hold onto the memory, and plan to delete it yourself later
	return true;
}


void TestCB::OnFileProgress(FileProgressStruct* fps)
{
	/*printf("OnFileProgress: %i partCount=%i partTotal=%i (%i%%) %i/%i %s %ib/%ib %ib/%ib total\n",
		fps->onFileStruct->setID,
		fps->partCount, fps->partTotal, (int)(100.0 * (double)fps->onFileStruct->bytesDownloadedForThisFile / (double)fps->onFileStruct->byteLengthOfThisFile),
		fps->onFileStruct->fileIndex + 1,
		fps->onFileStruct->numberOfFilesInThisSet,
		fps->onFileStruct->fileName,
		fps->onFileStruct->bytesDownloadedForThisFile,
		fps->onFileStruct->byteLengthOfThisFile,
		fps->onFileStruct->bytesDownloadedForThisSet,
		fps->onFileStruct->byteLengthOfThisSet,
		fps->firstDataChunk);*/
	if(debug_mode)
		printf("[FILES]Received contents for %s. (%u%%)\n", fps->onFileStruct->fileName, (unsigned int)(100.0 * (double)fps->onFileStruct->bytesDownloadedForThisFile / (double)fps->onFileStruct->byteLengthOfThisFile));

}

bool TestCB::OnDownloadComplete(DownloadCompleteStruct* dcs)
{
	if(debug_mode)
		printf("[FILES]Finished downloading all files.\n");
	std::lock_guard<std::mutex> lock(flt_mtx);
	shared_dwnld_cmpltd_signal = true;
	// Returning false automatically deallocates the automatically allocated handler that was created by DirectoryDeltaTransfer
	return false;
}


void Setup_FileTransfer(RakNet::RakPeerInterface* peer, RakNet::SystemAddress systemAddress)
{
	peer->AttachPlugin(&flt);
	flt.SetupReceive(&testCB, false, systemAddress);
}

void DetachPlugin(RakNet::RakPeerInterface* peer)
{
	peer->DetachPlugin(&flt);
}