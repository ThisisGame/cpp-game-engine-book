# Vince's CSV Parser
[![Build Status](https://travis-ci.org/vincentlaucsb/csv-parser.svg?branch=master)](https://travis-ci.org/vincentlaucsb/csv-parser)

 * [Motivation](#motivation)
 * [Documentation](#documentation)
 * [Integration](#integration)
   * [C++ Version](#c-version)
   * [Single Header](#single-header)
   * [CMake Instructions](#cmake-instructions)
 * [Features & Examples](#features--examples)
   * [Reading an Arbitrarily Large File (with Iterators)](#reading-an-arbitrarily-large-file-with-iterators)
      * [Memory Mapped Files vs. Streams](#memory-mapped-files-vs-streams)
   * [Indexing by Column Names](#indexing-by-column-names)
   * [Numeric Conversions](#numeric-conversions)
   * [Specifying the CSV Format](#specifying-the-csv-format)
      * [Trimming Whitespace](#trimming-whitespace)
      * [Handling Variable Numbers of Columns](#handling-variable-numbers-of-columns)
      * [Setting Column Names](#setting-column-names)
   * [Converting to JSON](#converting-to-json)
   * [Parsing an In-Memory String](#parsing-an-in-memory-string)
   * [Writing CSV Files](#writing-csv-files)
 * [Contributing](#contributing)

## Motivation
There's plenty of other CSV parsers in the wild, but I had a hard time finding what I wanted. Inspired by Python's `csv` module, I wanted a library with **simple, intuitive syntax**. Furthermore, I wanted support for special use cases such as calculating statistics on very large files. Thus, this library was created with these following goals in mind.

### Performance and Memory Requirements
With the deluge of large datasets available, a performant CSV parser is a necessity. By using overlapped threads, memory mapped IO, and 
efficient data structures, this parser can quickly tackle large CSV files. Furthermore, this parser has a minimal memory footprint and
can handle larger-than-RAM files.

#### Show me the numbers
On my computer (Intel Core i7-8550U @ 1.80GHz/Toshiba XG5 SSD), this parser can read
 * the [69.9 MB 2015_StateDepartment.csv](https://github.com/vincentlaucsb/csv-data/tree/master/real_data) in 0.26 seconds (269 MBps)
 * a [1.4 GB Craigslist Used Vehicles Dataset](https://www.kaggle.com/austinreese/craigslist-carstrucks-data/version/7) in 2.1 seconds (667 MBps)
 * a [1.24GB Car Accidents Dataset](https://www.kaggle.com/sobhanmoosavi/us-accidents) in 5 seconds (248 MBps)

### Robust Yet Flexible
#### RFC 4180 and Beyond
This CSV parser is much more than a fancy string splitter, and parses all files following [RFC 4180](https://www.rfc-editor.org/rfc/rfc4180.txt).

However, in reality we know that RFC 4180 is just a suggestion, and there's many "flavors" of CSV such as tab-delimited files. Thus, this library has:
 * Automatic delimiter guessing
 * Ability to ignore comments in leading rows and elsewhere
 * Ability to handle rows of different lengths

By default, rows of variable length are silently ignored, although you may elect to keep them or throw an error.

#### Encoding
This CSV parser is encoding-agnostic and will handle ANSI and UTF-8 encoded files.
It does not try to decode UTF-8, except for detecting and stripping UTF-8 byte order marks.

### Well Tested
This CSV parser has an extensive test suite and is checked for memory safety with Valgrind. If you still manage to find a bug,
do not hesitate to report it.

## Documentation

In addition to the [Features & Examples](#features--examples) below, a [fully-fledged online documentation](https://vincentlaucsb.github.io/csv-parser/html/) contains more examples, details, interesting features, and instructions for less common use cases.

## Integration

This library was developed with Microsoft Visual Studio and is compatible with >g++ 6.0 and clang.
All of the code required to build this library, aside from the C++ standard library, is contained under `include/`.

### C++ Version
While C++17 is recommended, C++11 is the minimum version required. This library makes extensive use of string views, and uses
[Martin Moene's string view library](https://github.com/martinmoene/string-view-lite) if `std::string_view` is not available.

### Single Header
This library is available as a single `.hpp` file under [`single_include/csv.hpp`](single_include/csv.hpp).

### CMake Instructions
If you're including this in another CMake project, you can simply clone this repo into your project directory, 
and add the following to your CMakeLists.txt:

```
# Optional: Defaults to C++ 17
# set(CSV_CXX_STANDARD 11)
add_subdirectory(csv-parser)

# ...

add_executable(<your program> ...)
target_link_libraries(<your program> csv)

```

## Features & Examples
### Reading an Arbitrarily Large File (with Iterators)
With this library, you can easily stream over a large file without reading its entirety into memory.

**C++ Style**
```cpp
# include "csv.hpp"

using namespace csv;

...

CSVReader reader("very_big_file.csv");

for (CSVRow& row: reader) { // Input iterator
    for (CSVField& field: row) {
        // By default, get<>() produces a std::string.
        // A more efficient get<string_view>() is also available, where the resulting
        // string_view is valid as long as the parent CSVRow is alive
        std::cout << field.get<>() << ...
    }
}

...
```

**Old-Fashioned C Style Loop**
```cpp
...

CSVReader reader("very_big_file.csv");
CSVRow row;
 
while (reader.read_row(row)) {
    // Do stuff with row here
}

...
```

#### Memory-Mapped Files vs. Streams
By default, passing in a file path string to the constructor of `CSVReader`
causes memory-mapped IO to be used. In general, this option is the most
performant.

However, `std::ifstream` may also be used as well as in-memory sources via `std::stringstream`.

**Note**: Currently CSV guessing only works for memory-mapped files. The CSV dialect
must be manually defined for other sources.

```cpp
CSVFormat format;
// custom formatting options go here

CSVReader mmap("some_file.csv", format);

std::ifstream infile("some_file.csv", std::ios::binary);
CSVReader ifstream_reader(infile, format);

std::stringstream my_csv;
CSVReader sstream_reader(my_csv, format);
```

### Indexing by Column Names
Retrieving values using a column name string is a cheap, constant time operation.

```cpp
# include "csv.hpp"

using namespace csv;

...

CSVReader reader("very_big_file.csv");
double sum = 0;

for (auto& row: reader) {
    // Note: Can also use index of column with [] operator
    sum += row["Total Salary"].get<double>();
}

...
```

### Numeric Conversions
If your CSV has lots of numeric values, you can also have this parser (lazily)
convert them to the proper data type.

 * Type checking is performed on conversions to prevent undefined behavior and integer overflow
   * Negative numbers cannot be blindly converted to unsigned integer types
 * `get<float>()`, `get<double>()`, and `get<long double>()` are capable of parsing numbers written in scientific notation.
 * **Note:** Conversions to floating point types are not currently checked for loss of precision.

```cpp
# include "csv.hpp"

using namespace csv;

...

CSVReader reader("very_big_file.csv");

for (auto& row: reader) {
    if (row["timestamp"].is_int()) {
        // Can use get<>() with any integer type, but negative
        // numbers cannot be converted to unsigned types
        row["timestamp"].get<int>();
        
        // ..
    }
}

```

### Converting to JSON
You can serialize individual rows as JSON objects, where the keys are column names, or as 
JSON arrays (which don't contain column names). The outputted JSON contains properly escaped
strings with minimal whitespace and no quoting for numeric values. How these JSON fragments are 
assembled into a larger JSON document is an exercise left for the user.

```cpp
# include <sstream>
# include "csv.hpp"

using namespace csv;

...

CSVReader reader("very_big_file.csv");
std::stringstream my_json;

for (auto& row: reader) {
    my_json << row.to_json() << std::endl;
    my_json << row.to_json_array() << std::endl;

    // You can pass in a vector of column names to
    // slice or rearrange the outputted JSON
    my_json << row.to_json({ "A", "B", "C" }) << std::endl;
    my_json << row.to_json_array({ "C", "B", "A" }) << std::endl;
}

```

### Specifying the CSV Format
Although the CSV parser has a decent guessing mechanism, in some cases it is preferrable to specify the exact parameters of a file.

```cpp
# include "csv.hpp"
# include ...

using namespace csv;

CSVFormat format;
format.delimiter('\t')
      .quote('~')
      .header_row(2);   // Header is on 3rd row (zero-indexed)
      // .no_header();  // Parse CSVs without a header row
      // .quote(false); // Turn off quoting 

// Alternatively, we can use format.delimiter({ '\t', ',', ... })
// to tell the CSV guesser which delimiters to try out

CSVReader reader("wierd_csv_dialect.csv", format);

for (auto& row: reader) {
    // Do stuff with rows here
}

```

#### Trimming Whitespace
This parser can efficiently trim off leading and trailing whitespace. Of course,
make sure you don't include your intended delimiter or newlines in the list of characters
to trim.

```cpp
CSVFormat format;
format.trim({ ' ', '\t'  });
```

#### Handling Variable Numbers of Columns
Sometimes, the rows in a CSV are not all of the same length. Whether this was intentional or not,
this library is built to handle all use cases.

```cpp
CSVFormat format;

// Default: Silently ignoring rows with missing or extraneous columns
format.variable_columns(false); // Short-hand
format.variable_columns(VariableColumnPolicy::IGNORE);

// Case 2: Keeping variable-length rows
format.variable_columns(true); // Short-hand
format.variable_columns(VariableColumnPolicy::KEEP);

// Case 3: Throwing an error if variable-length rows are encountered
format.variable_columns(VariableColumnPolicy::THROW);
```

#### Setting Column Names
If a CSV file does not have column names, you can specify your own:

```cpp
std::vector<std::string> col_names = { ... };
CSVFormat format;
format.column_names(col_names);
```

### Parsing an In-Memory String

```cpp
# include "csv.hpp"

using namespace csv;

...

// Method 1: Using parse()
std::string csv_string = "Actor,Character\r\n"
    "Will Ferrell,Ricky Bobby\r\n"
    "John C. Reilly,Cal Naughton Jr.\r\n"
    "Sacha Baron Cohen,Jean Giard\r\n";

auto rows = parse(csv_string);
for (auto& r: rows) {
    // Do stuff with row here
}
    
// Method 2: Using _csv operator
auto rows = "Actor,Character\r\n"
    "Will Ferrell,Ricky Bobby\r\n"
    "John C. Reilly,Cal Naughton Jr.\r\n"
    "Sacha Baron Cohen,Jean Giard\r\n"_csv;

for (auto& r: rows) {
    // Do stuff with row here
}

```

### Writing CSV Files

```cpp
# include "csv.hpp"
# include ...

using namespace csv;
using namespace std;

...

stringstream ss; // Can also use ofstream, etc.

auto writer = make_csv_writer(ss);
// auto writer = make_tsv_writer(ss);               // For tab-separated files
// DelimWriter<stringstream, '|', '"'> writer(ss);  // Your own custom format

writer << vector<string>({ "A", "B", "C" })
    << deque<string>({ "I'm", "too", "tired" })
    << list<string>({ "to", "write", "documentation." });

writer << array<string, 2>({ "The quick brown", "fox", "jumps over the lazy dog" });
writer << make_tuple(1, 2.0, "Three");
...
```

You can pass in arbitrary types into `DelimWriter` by defining a conversion function
for that type to `std::string`.

## Contributing
Bug reports, feature requests, and so on are always welcome. Feel free to leave a note in the Issues section.
