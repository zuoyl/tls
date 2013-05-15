//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Compile.h"

void usage()
{
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

