//
//  Frame.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_IRREGISTER_H
#define TCC_IRREGISTER_H

#include "Common.h"

enum {
	IRREG_0,
	IRREG_1,
	IRREG_2,
	IRREG_3,
	IRREG_4
	IRREG_5,
	IRREG_6,
	IRREG_7,
	IRREG_8,
	IRREG_9
	IRREG_10,
	IRREG_11,
	IRREG_12,
	IRREG_13,
	IRREG_14
};

class RegisterWindow {
public:
	static Value* allocValue();
	static Value* allocValue(Type* type);
private:
	RegisterWindow();
	~RegisterWindow();
};

#endif // TCC_IRREGISTER_H