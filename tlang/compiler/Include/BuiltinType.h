//
//  BultinType.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_BUILTINTYPE_H
#define TCC_BUILTINTYPE_H

#include "Type.h"

class IntType : public Type {
public:
    IntType(){}
    virtual ~IntType(){}
};

class BoolType : public Type {
    
};


class StringType : public Type {
public:
    StringType();
    StringType(const string &name, Scope *scope);
    ~StringType();
    
};


class FloatType : public Type {
public:
    Type *getValType();
    
};

class MapType : public Type {
public:
    MapType(const string &keyType, const string &valType);
    MapType(const Type *keyType, const Type *valType);
    void setTypeInfo(const string &keyType, const string &valType);
    void setTypeInfo(const Type *keyType, const Type *valType);
    Type *getKeyType() { return m_keyType; }
    Type *getValType() { return m_valType; }
private:
    Type *m_keyType;
    Type *m_valType;
    
    
};

class SetType : public Type {
public:
    Type *getValType() { return m_valType; }

private:
    Type *m_valType;
    
};

#endif // TCC_BUILTINTYPE_H
