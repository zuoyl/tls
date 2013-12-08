#include "tl-type.h"
#include "tl-type-builtin.h"

using namespace tlang;

MapType::MapType()
    :BuiltinType("map")
{
    m_keyType = NULL;
    m_valType = NULL;
}

MapType::MapType(const string& keyType, const string& valType)
    :BuiltinType("map") 
{
    m_keyType = NULL;
    m_valType = NULL;
}
MapType::MapType(Type* keyType, Type* valType)
    :BuiltinType("map")
{
    m_keyType = keyType;
    m_valType = valType;
    if (keyType)
        m_keyTypeName = keyType->getName();
    if (valType)
        m_valTypeName = valType->getName();
}
void MapType::setItemType(const string& keyType, const string& valType)
{
    m_keyTypeName = keyType;
    m_valTypeName = valType;
}
void MapType::setItemType(Type* keyType, Type* valType)
{
    m_keyType = keyType;
    m_valType = valType;
    if (keyType)
        m_keyTypeName = keyType->getName();
    if (valType)
        m_valTypeName = valType->getName();

}


