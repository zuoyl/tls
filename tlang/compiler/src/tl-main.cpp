//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "tl-common.h"
#include "tl-compile.h"


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
static void 
dumpAllOptions()
{
    int optionsMax = sizeof(optionItems) / sizeof(optionItems[0]);
    for (int i = 0; i < optionsMax; i++) {
        cout << "\t" << optionItems[i].key << "\t" ;
        cout << optionItems[i].val << "\t" ;
        cout << optionItems[i].sample << endl;
    }
    cout << endl;
}

/// check compile options's validity
static bool 
checkOptionValidity(const string &key, string &val)
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
static bool 
getOneOption(const char* argv[], int &leftArgc,  string &key, string &val)
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
static void 
parseAllOptions(map<string, string> &options)
{
    CompileOption &compileOption = CompileOption::getInstance();
    string val;

    if (options.find("-W") != options.end()) {
        val = options["-W"];
        compileOption.setCompileLevel(atoi(val.c_str()));
    }

    if (options.find("-g") != options.end()) {
        compileOption.setDebug(true);
    }

    if (options.find("-S") != options.end())
        compileOption.setOutputAssembleFile(true);
    else
        compileOption.setOutputAssembleFile(false);

    if (options.find("-P") != options.end()) {
        val = options["-P"];
        if (val.size() == 2) { 
            if (val == "ap" || val == "pa") {
                compileOption.setOutputParseTree(true);
                compileOption.setOutputAST(true);
            }
        }
        else if (val == "a")
            compileOption.setOutputAST(true);
        else if (val == "p")
            compileOption.setOutputParseTree(true);
        else 
            cout << "unknow options for -P " << endl;
    }
}
/// dumpy compiler usage in console
static void 
usage()
{
    cout << "unknow tcc  options" << endl;
    dumpAllOptions();
    cout << endl; 
}
/// comapiler main entry
int 
tlangCompile(int argc, const char *argv[])
{   
    // check the arguments count
    if (argc < 2) {
        usage();
        return 0;
    }

    // parse options
    int leftArgc = argc - 1;
    int index = 1;
    map<string, string> options;
    vector<string> sourceFiles;

    while (leftArgc > 0) {
        if (argv[index][0] == '-') {
            string key, val;
            if (getOneOption(argv + index, leftArgc, key, val)) {
                if (!checkOptionValidity(key, val)) {
                    cout << "unknown options:" << key << endl;
                    usage();
                    return 0;
                }
                if (val.empty())
                    index += 1;
                else
                    index += 2;

                if (options.find(key) != options.end()) {
                    cout << "there are sample options" << key << "," 
                        << val << "ignore the second" << endl;
                }
                else
                    options.insert(make_pair(key, val));
            }
        }
        else {
            // source files
            string sourceFile = argv[index];
            index++;
            unsigned found = sourceFile.find_last_of(".");
            string extension = sourceFile.substr(found);
            if (extension.empty() || extension != ".tl") {
                cout << "the source file can not be compiled: " 
                    << sourceFile << endl;
            }
            else 
                sourceFiles.push_back(sourceFile);
            leftArgc--;
        }
    }
    // if there are no source files, dump compile options 
    if (sourceFiles.empty()) {
        cout << "There are no source files to complile." << endl;
        usage();
        return 0;
    }
    // parse all compile options 
    parseAllOptions(options);
    // to debug easily, turn on the parse tree and ast xml output
    CompileOption::getInstance().setOutputParseTree(true);
    CompileOption::getInstance().setOutputAST(true);
    CompileOption::getInstance().setOutputAssembleFile(true); 
    // compile source files 
    Compiler& compiler = Compiler::getInstance();
    compiler.compile(sourceFiles);
    
    return 0;
}


/// application entry
int main (int argc, const char*  argv[])
{
    return tlangCompile(argc, argv);
}
