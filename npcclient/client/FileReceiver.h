#ifndef FILERECEIVER_H
#define FILERECEIVER_H
class TestCB : public RakNet::FileListTransferCBInterface
{
public:
	bool OnFile(
		OnFileStruct* onFileStruct);
	

	virtual void OnFileProgress(FileProgressStruct* fps);

	virtual bool OnDownloadComplete(DownloadCompleteStruct* dcs);

};
void Setup_FileTransfer(RakNet::RakPeerInterface* peer, RakNet::SystemAddress systemAddress);
void CreateDirectories(const std::string& path);
void DetachPlugin(RakNet::RakPeerInterface* peer);
#endif