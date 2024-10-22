#ifndef COMPRESS_H
#define COMPRESS_H
void UnCompressBitStream(RakNet::BitStream& bsIn, RakNet::BitStream& bsUncompressedOut);
void CompressBitStream(RakNet::BitStream& bsOut);
#endif