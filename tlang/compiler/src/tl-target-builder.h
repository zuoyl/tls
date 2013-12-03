//
//  CodeGenerator.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_CODEGENERATOR_H
#define TCC_CODEGENERATOR_H


#include "Common.h"

class CodeStream;
class CodeGenerator;

class CodeGeneratorFactory {
public:
    static CodeGeneratorFactory *getInstance();
    static CodeGenerator *createGenerator(const string& arch);
private:
    CodeGeneratorFactory(){}
    ~CodeGeneratorFactory(){}
};

class CodeStream {
public:
    CodeStream(const string &fileName):m_name(fileName){}
    virtual ~CodeStream(){}
protected:
    string m_name;
};

// public code generator interface
class CodeGenerator {
public:
    CodeGenerator(const string &arch):m_arch(arch){}
    virtual ~CodeGenerator(){}

protected:
    string m_arch;
};






#endif // TCC_CODEGENERATOR_H
