////
/// tl-compile-option.cpp 
///  A toyable language compiler (like a simple c++)

#include "tl-compile-option.h"

using namespace tlang;

/// compiler option items
struct CompileOptionItem {
    const char *key;
    const char *val;
    const char *sample;
};

CompileOptionItem optionItems[] = 
{
    {"-W", "1,2,3",  "Warning level"},
    {"-g", NULL,     "Wether debug" },
    {"-S", NULL,    "Output the assemble file"},
    {"-P", "p,a",   "Print parse tree or abstrat syntax tree in xml"},
};

/// dump all compiler options in console
void 
CompileOption::dumpAllOptions(ostream &ios)
{
    int optionsMax = sizeof(optionItems) / sizeof(optionItems[0]);
    for (int i = 0; i < optionsMax; i++) {
        ios << "\t" << optionItems[i].key << "\t" ;
        ios << optionItems[i].val << "\t" ;
        ios << optionItems[i].sample << endl;
    }
    ios << endl;
}

/// check compile options's validity
bool 
CompileOption::checkOptionValidity(const string &key, string &val)
{
    int optionsMax = sizeof(optionItems) / sizeof(optionItems[0]);
    for (int i = 0; i < optionsMax; i++) {
        if (key == optionItems[i].key) {
            if (optionItems[i].val)
                val =  optionItems[i].val;
            return true;
        }
    }
    return false;
}

/// parse one item, such as "-W 1"
bool 
CompileOption::getOneOption(const char* argv[], int &leftArgc, string &key, string &val)
{
    if (leftArgc >= 2) {
        key = argv[0];
        // check wether the key is valid
        if (checkOptionValidity(key, val)) {
            if (!val.empty()) { // with option val, sucha as -W 1
                leftArgc -= 2;
                val = argv[1];
            }
            else 
                leftArgc -= 1; // no options val, such as -g
        }
        return true;
    }
    else if (leftArgc == 1) {
        key = &argv[0][1];
        // check wether the key is valid
        if (checkOptionValidity(key, val)) {
            if (!val.empty()) 
                return false;
            else {
                leftArgc -= 1;
                return true;
            }
        }
    }
    return false;
}

/// iterate options map table, find key and val
void 
CompileOption::parseAllOptions(map<string, string> &options)
{
    string val;

    if (options.find("-W") != options.end()) {
        val = options["-W"];
        setCompileLevel(atoi(val.c_str()));
    }

    if (options.find("-g") != options.end()) {
        setDebug(true);
    }

    if (options.find("-S") != options.end())
        setOutputAssembleFile(true);
    else
        setOutputAssembleFile(false);

    if (options.find("-P") != options.end()) {
        val = options["-P"];
        setOutputParseTree(true);
    }
}
/// dumpy compiler usage in console
void 
CompileOption::usage(ostream &ios)
{
    ios << "unknow tcc  options" << endl;
    dumpAllOptions(ios);
    ios << endl; 
}

/// CompileOption
CompileOption::CompileOption()
{
    m_compileLevel = 1;
    m_isDebug = true;
    m_isOutputAssembleFile = false;
    m_isOutputAST = false;
    m_isOutputParseTree = false;
}
CompileOption::~CompileOption()
{}

CompileOption& CompileOption::getInstance() 
{
    static CompileOption option;
    return option;
}


bool 
CompileOption::parse(const char* argv[], int argc)
{
    // check the arguments count
    if (argc < 2) 
        return false;
    // parse options
    int leftArgc = argc - 1;
    int index = 1;
    map<string, string> options;

    while (leftArgc > 0) {
        if (argv[index][0] == '-') {
            string key, val;
            if (getOneOption(argv + index, leftArgc, key, val)) {
                if (!checkOptionValidity(key, val)) 
                    return false;
                if (val.empty())
                    index += 1;
                else
                    index += 2;

                if (options.find(key) != options.end()) 
                    return false;
                else
                    options.insert(make_pair(key, val));
            }
        }
        else {
            // source files
            string sourceFile = argv[index];
            index++;
            unsigned found = sourceFile.find_last_of(".");
            if (found != string::npos) {
                string extension = sourceFile.substr(found);
                if (extension.empty() || extension != ".tl")
                    return false;
                else 
                    m_sourceFiles.push_back(sourceFile);
            }
            leftArgc--;
        }
    }
    // if there are no source files, dump compile options 
    if (m_sourceFiles.empty())
        return false;
    // parse all compile options 
    parseAllOptions(options);
    // to debug easily, turn on the parse tree and ast xml output
    setOutputParseTree(true);
    setOutputAST(true);
    setOutputAssembleFile(true); 
    return true;
}

void
CompileOption::getSourceFiles(vector<string> &files)
{
    std::vector<string>::iterator ite = m_sourceFiles.begin();
    for (; ite != m_sourceFiles.end(); ite++)
        files.push_back(*ite);
}
