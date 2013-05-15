//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Compile.h"

struct CompileOptionItem {
    const char *key;
    const char *val;
    const char *sample;
};

CompileOptionItem optionItems[] = 
{
    {"W", "1,2,3",  "Warning level"},

};

void dumpAllOptions()
{

}

bool isValidOption(const string &key)
{
    int optionsMax = sizeof(optionItems) / sizeof(optionItems[0]);
    for (int i = 0; i < optionsMax; i++) {
        if (key == optionItems[i].key)
            return true;
    }
    
    return false;
}

// parse one item, such as "-W 1"
bool parseOneOption(int leftArgc, const char*argv[], string &key, string &val)
{
    if (leftArgc >= 2) {
        key = &argv[0][1];
        val = argv[1];
        return true;
    }
    else
        return false;

}

void usage()
{
    std::cout << "unknow options" << std::endl;
    dumpAllOptions();
}

int main (int argc, const char * argv[])
{    

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
            if (parseOneOption(leftArgc, argv + index, key, val)) {
                leftArgc -= 2;
                index += 2;
                if (!isValidOption(key)) {
                    std::cout << "unknown options:" << key << std::endl;
                    usage();
                    return 0;
                }
                if (options.find(key) != options.end()) {
                    std::cout << "there are sample options" << key << "," << val << "ignore the second" << std::endl;
                }
                else
                    options.insert(make_pair(key, val));
            }
        }
        else {
            // source files
            string sourceFile = argv[index];
            sourceFiles.push_back(sourceFile);
            index++;
            leftArgc--;
        }
    }


    Compiler &compiler = Compiler::getInstance();
    compiler.parseOptions(options);
    compiler.compile(sourceFiles);
    
    return 0;
}

