///std
#include <iostream>
#include <memory>
#include "converter.h"

using namespace profiler::reader;

int main(int argc, char* argv[])
{
    std::string filename, output_json_filename;

    if (argc > 1 && argv[1])
    {
        filename = argv[1];
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " INPUT_PROF_FILE [OUTPUT_JSON_FILE]\n"
                                             "where:\n"
                                             "INPUT_PROF_FILE // Required\n"
                                             "OUTPUT_JSON_FILE (if not specified output will be print in stdout) // Optional\n";
        return 1;
    }

    if (argc > 2 && argv[2])
    {
        output_json_filename = argv[2];
    }

    JsonExporter js;
    js.convert(filename, output_json_filename);

    return 0;
}
