//
//  Preproc.cpp
//  A toyable language compiler (like a simple c++)


#include "Preproc.h"
#include "Compile.h"
#include "Error.h"
#include "OSWraper.h"
#include "TokenStream.h"
#include "Lexer.h"
#include "Parser.h"
#include "ASTBuilder.h"
#include "Class.h"
#include "AST.h"
#include "TypeBuilder.h"

Preproc::Preproc(const string& file, const string& path)
{
    m_file = file;
    m_path = path;
}
Preproc::~Preproc()
{
}


void Preproc::build(TypeDomain &typeDomain)
{
    // at first, check wether the file is preprocessed  
    string fullName = m_path;
    if (!fullName.empty())
        fullName += "/";
    fullName += m_file;
    
    if (typeDomain.isTypeFileExist(fullName))
        return;

    TokenStream tokenStream; 
    Lexer lexer(m_path, m_file);
    Parser parser(m_path, m_file);
    ASTBuilder astBuilder(m_path, m_file);
   
    // create the AST
    lexer.parse(&tokenStream);
    Node *parseTree = parser.build(&tokenStream);
    AST *ast = astBuilder.build(parseTree);
    TypeBuilder typeBuilder(m_path, m_file);
    typeBuilder.setWetherIncludedFile(true);
    typeBuilder.build(ast, &typeDomain);
    delete ast;
} 
