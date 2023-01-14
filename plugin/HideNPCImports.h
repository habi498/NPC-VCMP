#ifndef HIDENPCIMPORTS_H
#define HIDENPCIMPORTS_H

typedef bool (*fn_ShowMaxPlayersAs)(uint16_t wmaxplayers);

struct NPCHideImports {
	unsigned int uStructSize;
	fn_ShowMaxPlayersAs ShowMaxPlayersAs;
};
#endif