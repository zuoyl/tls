//
//  BultinType.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_BUILTINTYPE_H
#define TCC_BUILTINTYPE_H

#include "Type.h"

class MethodType;
class ProtocolType;

class ClassType : public Type 
{
public:
    ClassType();
    ClassType(const string &name, bool isPublic, bool isFinal = false);
    ~ClassType();

    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
    bool isFinal() { return m_isFinal;}
  
private:
    vector<pair<const string, MethodType*> > m_methods;
    vector<pair<const string, Type *> > m_vars;
    vector<pair<const string, ClassType*> > m_baseClass;
    vector<pair<const string, ProtocolType*> > m_baseProtocol;
    ObjectVirtualTable *m_vtbl;
    string m_name;
    int m_size;
    bool m_isPublic;
    bool m_isFinal;
};

class ProtocolType : public Type 
{
public:
    ProtocolType();
    ProtocolType(const string &name, bool isPublic);
    ~ProtocolType();
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
   
private:
    ObjectVirtualTable *m_vtbl;
};

class MethodType : public Type 
{
public:
    MethodType();
    MethodType(const string &name, bool isPublic);
    ~MethodType();

    
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
   
    void setLinkAddress(int addr) { m_linkAddress = addr; }
    int getLinkAddress() const { return m_linkAddress; }
    
    Type *getReturnType() { return m_retType; }
    void setReturnType(Type *type) { m_retType = type; }
    bool isOfClassMember() { return m_isOfClass; }
    void setOfClassMember(bool isOfClass) { m_isOfClass = isOfClass; }
private:
    string m_name;
    int m_size;  
    bool m_isPublic;
    bool m_isOfProtocol;
    bool m_isOfClass;
    Type *m_retType;
    int m_linkAddress;
};


class IntType : public Type 
{
public:
    IntType(){}
    virtual ~IntType(){}
};

class BoolType : public Type 
{
    
};


class StringType : public Type 
{
public:
    StringType(){}
    StringType(const string &name){}
    ~StringType(){}
    
};


class FloatType : public Type 
{
public:
    FloatType(){}
    ~FloatType(){}
    Type *getValType();
    
};

class VoidType : public Type
{
public:
    VoidType(){}
    ~VoidType(){}
};
class MapType : public Type 
{
public:
    MapType(){} 
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

class SetType : public Type 
{
public:
    Type *getValType() { return m_valType; }

private:
    Type *m_valType;
    
};

#endif // TCC_BUILTINTYPE_H
