#ifndef HIDENPCIMPORTS_H
#define HIDENPCIMPORTS_H

typedef bool (*fn_ShowMaxPlayersAs)(uint16_t wmaxplayers);
typedef bool (*fn_SetAllNPCNames)(const char* name, uint8_t size);
struct NPCHideImports {
	unsigned int uStructSize;
	fn_ShowMaxPlayersAs ShowMaxPlayersAs;
	fn_SetAllNPCNames SetAllNPCNames;
};
#endif