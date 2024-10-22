#include "main.h"
//Uncompresses a bitstream. The Read pointer should be proper
void UnCompressBitStream(RakNet::BitStream& bsIn, RakNet::BitStream& bsUncompressedOut)
{
	bsIn.ReadBit();//read one bit which is zero.
	uint32_t sourcelen;//The original no:of bytes after messageid
	bsIn.Read(sourcelen);
	bsIn.ReadBit();//read one bit which is zero
	uint32_t lenCompressedData;//The length of the compressed data
	bsIn.Read(lenCompressedData);
	unsigned char* compressedData = (unsigned char*)malloc(lenCompressedData);
	if (!compressedData)
	{
		printf("Could not allocate memory\n"); exit(0);
	}
	for (int i = 0; i < lenCompressedData; i++)
	{
		if (!bsIn.Read(compressedData[i]))
		{
			printf("Could not read bitstream\n"); exit(0);
		}
	}
	unsigned char* data = (unsigned char*)malloc(sourcelen);//here sourcelen,but precaution.
	if (!data) {
		printf("Error allocating memory. Exiting\n"); exit(0);
	}
	uLongf datalen = (uLongf)sourcelen;
	int ret = uncompress(data, &datalen, compressedData, lenCompressedData);
	if (ret == Z_OK)
	{
		if (datalen == sourcelen)
		{
			//Decompression was 100% successful.
			// 
			//Create a bitstream so that rest of code works.
			//RakNet::BitStream bsIn2(sourcelen);
			for (int i = 0; i < sourcelen; i++)
				bsUncompressedOut.Write(data[i]);
			//Now free previous allocated memories
			free(data); free(compressedData);
			bsUncompressedOut.ResetReadPointer();
		}
		else {
			printf("Error. Decompress was not completely successful.\n");
			exit(0);
		}
	}
	else {
		printf("An error occured during decompressing data.\n");
		exit(0);
	}

}
//uint32_t GetLengthOfData()
void CompressBitStream(RakNet::BitStream& bsOut)
{
	unsigned char* data = bsOut.GetData();
	uint32_t sourcelen = bsOut.GetNumberOfBytesUsed();
	uLongf destLen = compressBound(sourcelen - 1);//An upper bound, we do not compress message id 
	unsigned char* cmpdata = (unsigned char*)malloc(destLen);
	if (cmpdata)
	{
		int c = compress2(cmpdata, (uLongf*)&destLen, data + 1, sourcelen - 1,
			Z_DEFAULT_COMPRESSION);
		if (c == Z_OK)
		{
			unsigned char messageid = data[0];
			bsOut.Reset();
			bsOut.Write(data[0]);//Write Message ID 0x30 or 0x31
			WriteBit0(&bsOut);//Write a zero bit
			//NOw write thelength of original data
			//printf("sourcelen adjusted is %u. ", sourcelen - 1);
			bsOut.Write(sourcelen - 1);
			WriteBit0(&bsOut);//Write a zero bit
			//Write length of upcoming compressed data
			//printf("destLen is %u\n", destLen);
			bsOut.Write((uint32_t)destLen);
			//for (int i = 0; i < destLen; i++)printf("%x ", cmpdata[i]);
			//printf("\n");
			for (int i = 0; i < destLen; i++)
				bsOut.Write(cmpdata[i]);
			free(cmpdata);
			return;
			
		}
		else {
			printf("Error. An error occured during compressing data\n");
			exit(0);
		}
		
	}
	else
	{
		printf("Error: Memory allocation failed\n"); exit(0);
	}
}