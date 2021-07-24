#include "my_header.hpp"
#include <iostream>

int foobar(int argc, char** argv) {
    using namespace csv;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [file]" << std::endl;
        exit(1);
    }

    std::string file = argv[1];
    auto info = get_file_info(file);

    std::cout << file << std::endl
        << "Columns: " << internals::format_row(info.col_names, ", ")
        << "Dimensions: " << info.n_rows << " rows x " << info.n_cols << " columns" << std::endl
        << "Delimiter: " << info.delim << std::endl;

    return 0;
}