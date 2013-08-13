//
// tof.h
// A toyable language compiler(like a simple c++)
//
#ifndef TOF_H
#define TOF_H

#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace tof {
    // type definitions
    typedef unsigned char u1;
    typedef unsigned short u2;
    typedef unsigned long u4;
    
    enum AccessFlag {
        AccPublicFlag = 0x0001, // object is public, may be accessed from outside
        AccFinalFlag = 0x0010, // no subclass allowed
        AccSuperFlag = 0x0020,
        AccInterfaceFlag = 0x0200, // it's interface, not class
        AccAbstractFlag = 0x0400 // may not be instantiated
    };
    
    // constant pool declarations
    struct CostantPoolInfo {
        u1 type;
        u1 info[];
    };
    // constant type definitons
    enum ConstantType {
        ConstantClassType = 0x01,
        ConstantFieldRefType = 0x02,
        ConstantMethodRefType = 0x03,
        ConstantInterfaceMethodRef = 0x04,
        ConstantString = 0x05,
        ConstantInt = 0x06,
        ConstantFloat = 0x07,
        ConstantLong = 0x08,
        ConstantDouble = 0x09,
        ConstantNameAndType = 0x10,
    };



    class TObjectFormat {
        public:
            TObjectFormat(const string& name){}
            ~TobjectFormat(){}
            const string& getName() { return m_name; }
            u2 getMinorVersion();
            u2 getMajorVersion();
            u2 getConstantPoolCount();
            void getConstantPool(u2 index, ConstantPool& pool);
            u2 getAccessFlag();
            u2 getThisClass();
            u2 getSuperClass();
            u2 getInterfaceCount();
            u2 getInterface(u2 index);
            u2 getFieldsCount();
            void getField(u2 index, FieldInfo& info);
            u2 getMethodCount();
            void getMethod(u2 index, MethodInfo& info);
            u2 getAttributeCount();
            void getAttribute(u2 index, AttributetInfo& info);
        private:
            u4 m_magic;
            u2 m_minorVersion;
            u2 m_majorVersion;
            vector<ConstantPoolInfo> m_constantPools; 
            u2 m_accessFlag;
            u2 m_thisClass;
            u2 m_superClass;
            u2 m_interfaceCount;
            vector<u2> m_interfaces;
            vector<FieldInfo> m_fields;
            vector<MethodInfo> m_methods;
            vector<AttributeInfo> m_attributes;
            const string m_name;
    };

}; // namespace tof
#endif // TOF_H

