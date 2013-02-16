//
//  Local.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_LOCAL_H
#define TCC_LOCAL_H

#include "Common.h"
#include "Runtime/Type.h"
#include "Runtime/BuiltinType.h"

/// 'class Local
/// Local is an object used for locals in frame/stack
class Local {
public:
    Local();
    Local(Type *type, bool inreg = false);
    Local(int offset, int size, bool inreg = false);
	Local(int regidx);
    ~Local();
	
	bool hasValidValue() const{ return true; /* temp */ }
	bool isInReg() const { return m_inreg; }
	Type* getType() const { return m_type; }
	int getOffset() const { return m_offset; }
	int getSize() const { return m_size; }
    void initializeWithType(ClassType *type);
    void initializeWithType(InterfaceType *type);
    void initializeWithType(FunctionType *type);
    void initializeWithType(StructType *type);
    void initializeWithType(const string &builtinType);
 
private:
    int m_offset;
    int m_size;
    Type *m_type;
	bool m_inreg;
	int m_regidx;
};

#endif // TCC_VALUE_H
