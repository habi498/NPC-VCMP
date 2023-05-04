#ifndef HIDENPCIMPORTS_H
#define HIDENPCIMPORTS_H

typedef bool (*fn_ShowMaxPlayersAs)(uint16_t wmaxplayers);
typedef bool (*fn_SetAllNPCNames)(const char* name, uint8_t size);
#pragma pack(push,1)
struct NPCHideImports {
	unsigned int uStructSize;
	fn_ShowMaxPlayersAs ShowMaxPlayersAs;
	fn_SetAllNPCNames SetAllNPCNames;
};
#pragma pack(pop)
#endif