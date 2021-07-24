# bundle :package: <a href="https://travis-ci.org/r-lyeh/bundle"><img src="https://api.travis-ci.org/r-lyeh/bundle.svg?branch=master" align="right" /></a>

Bundle is an embeddable compression library that supports 23 compression algorithms and 2 archive formats. 

Distributed in two files.

### Features
- [x] Archive support: .bun, .zip
- [x] Stream support: DEFLATE, LZMA, LZIP, ZPAQ, LZ4, ZSTD, BROTLI, BSC, CSC, BCM, MCM, ZMOLLY, ZLING, TANGELO, SHRINKER, CRUSH, LZJB, BZIP2 and SHOCO 
- [x] Optimized for highest compression ratios on each compressor, where possible.
- [x] Optimized for fastest decompression times on each decompressor, where possible.
- [x] Configurable, redistributable, self-contained, amalgamated and cross-platform (C++03).
- [x] Optional benchmark infrastructure (C++11).
- [x] ZLIB/LibPNG licensed.

### Bundle stream format
```c++
[0x00  ...]          Optional zero padding (N bits)
[0x70 0x??]          Header (8 bits). De/compression algorithm (8 bits)
                     enum { RAW, SHOCO, LZ4F, MINIZ, LZIP, LZMA20, ZPAQ, LZ4,      //  0..7
                            BROTLI9, ZSTD, LZMA25, BSC, BROTLI11, SHRINKER, CSC20, //  7..14
                            ZSTDF, BCM, ZLING, MCM, TANGELO, ZMOLLY, CRUSH, LZJB,  // 15..22
                            BZIP2                                                  // 23..
                     };
[vle_unpacked_size]  Unpacked size of the stream (N bytes). Data is stored in a variable
                     length encoding value, where bytes are just shifted and added into a
                     big accumulator until MSB is found.
[vle_packed_size]    Packed size of the stream (N bytes). Data is stored in a variable
                     length encoding value, where bytes are just shifted and added into a
                     big accumulator until MSB is found.
[bitstream]          Compressed bitstream (N bytes). As returned by compressor.
                     If possible, header-less bitstreams are preferred.
```

### Bundle .bun archive format
```c++
- Files/datas are packed into streams by using any compression method (see above)
- Streams are archived into a standard ZIP file:
  - ZIP entry compression is (0) for packed streams and (1-9) for unpacked streams.
  - ZIP entry comment is a serialized JSON of (file) meta-datas (@todo).
- Note: you can mix streams of different algorithms into the very same ZIP archive.
```

### Showcase
```c++
#include <cassert>
#include "bundle.h"

int main() {
    using namespace bundle;
    using namespace std;

    // 23 mb dataset
    string original( "There's a lady who's sure all that glitters is gold" );
    for (int i = 0; i < 18; ++i) original += original + string( i + 1, 32 + i );

    // pack, unpack & verify all encoders
    vector<unsigned> libs { 
        RAW, SHOCO, LZ4F, MINIZ, LZIP, LZMA20,
        ZPAQ, LZ4, BROTLI9, ZSTD, LZMA25,
        BSC, BROTLI11, SHRINKER, CSC20, BCM,
        ZLING, MCM, TANGELO, ZMOLLY, CRUSH, LZJB
    };
    for( auto &lib : libs ) {
        string packed = pack(lib, original);
        string unpacked = unpack(packed);
        cout << original.size() << " -> " << packed.size() << " bytes (" << name_of(lib) << ")" << endl;
        assert( original == unpacked );
    }

    cout << "All ok." << endl;
}
```

### On choosing compressors (on a regular basis)
|Rank|Compression ratio      |Fastest compressors    |Fastest decompressors  |Average speed          |Memory efficiency|
|---:|:----------------------|:----------------------|:----------------------|:----------------------|:----------------|
| 1st|91.15% ZPAQ            |958.18MB/s RAW         |2231.20MB/s RAW        |1340.63MB/s RAW        |tbd
| 2nd|90.71% MCM             |358.41MB/s LZ4F        |993.68MB/s LZ4         |508.50MB/s LZ4F        |tbd
| 3rd|90.02% TANGELO         |240.87MB/s SHRINKER    |874.83MB/s LZ4F        |334.57MB/s SHRINKER    |tbd
| 4th|88.31% BSC             |223.28MB/s LZJB        |547.62MB/s SHRINKER    |267.57MB/s LZJB        |tbd
| 5th|87.74% LZMA25          |210.74MB/s ZSTDF       |382.52MB/s MINIZ       |246.66MB/s ZSTDF       |tbd
| 6th|87.74% LZIP            |159.59MB/s SHOCO       |380.39MB/s ZSTD        |209.32MB/s SHOCO       |tbd
| 7th|87.63% BROTLI11        |40.19MB/s ZLING        |333.76MB/s LZJB        |65.40MB/s ZLING        |tbd
| 8th|87.50% CSC20           |33.67MB/s CRUSH        |304.06MB/s SHOCO       |60.29MB/s CRUSH        |tbd
| 9th|87.15% BCM             |13.73MB/s ZSTD         |297.34MB/s ZSTDF       |26.51MB/s ZSTD         |tbd
|10th|86.44% ZMOLLY          |09.00MB/s BSC          |287.83MB/s CRUSH       |13.44MB/s BZIP2        |tbd
|11th|86.17% LZMA20          |08.51MB/s BZIP2        |287.58MB/s BROTLI9     |11.51MB/s BROTLI9      |tbd
|12th|86.05% BROTLI9         |06.77MB/s ZMOLLY       |246.88MB/s BROTLI11    |10.78MB/s BSC          |tbd
|13th|85.27% BZIP2           |05.87MB/s BROTLI9      |175.54MB/s ZLING       |08.13MB/s LZ4          |tbd
|14th|85.24% ZSTD            |05.21MB/s BCM          |118.49MB/s LZMA25      |07.24MB/s MINIZ        |tbd
|15th|82.89% ZLING           |04.08MB/s LZ4          |108.71MB/s LZMA20      |06.73MB/s ZMOLLY       |tbd
|16th|81.68% MINIZ           |03.65MB/s MINIZ        |72.72MB/s CSC20        |05.27MB/s LZMA20       |tbd
|17th|77.93% ZSTDF           |02.70MB/s LZMA20       |57.05MB/s LZIP         |04.90MB/s LZMA25       |tbd
|18th|77.57% LZ4             |02.50MB/s LZMA25       |31.88MB/s BZIP2        |04.83MB/s CSC20        |tbd
|19th|77.37% CRUSH           |02.50MB/s CSC20        |13.44MB/s BSC          |04.65MB/s BCM          |tbd
|20th|67.30% SHRINKER        |02.25MB/s MCM          |06.68MB/s ZMOLLY       |04.13MB/s LZIP         |tbd
|21th|63.30% LZ4F            |02.14MB/s LZIP         |04.20MB/s BCM          |02.29MB/s MCM          |tbd
|22th|59.37% LZJB            |01.15MB/s TANGELO      |02.34MB/s MCM          |01.17MB/s TANGELO      |tbd
|23th|06.42% SHOCO           |00.24MB/s BROTLI11     |01.18MB/s TANGELO      |00.48MB/s BROTLI11     |tbd
|24th|00.00% RAW             |00.23MB/s ZPAQ         |00.21MB/s ZPAQ         |00.22MB/s ZPAQ         |tbd
- Note: SHOCO is a _text_ compressor intended to be used for plain ascii IDs only.

### Charts

[@mavam](https://github.com/mavam) has an awesome R script that plots some fancy graphics in [his compbench repository](https://github.com/mavam/compbench). The following CC images are a few of his own showcasing an invocation for a 10,000 packet PCAP trace:

![Tradeoff](https://raw.githubusercontent.com/mavam/compbench/master/screenshots/bro-tradeoff-comp.png)
![Throughput Scatterplot](https://raw.githubusercontent.com/mavam/compbench/master/screenshots/bro-throughput-scatter.png)
![Compression Ratio](https://raw.githubusercontent.com/mavam/compbench/master/screenshots/bro-compression-ratio-decomp.png)

### API - data
```c++
namespace bundle
{
  // low level API (raw pointers)
  bool is_packed( *ptr, len );
  bool is_unpacked( *ptr, len );
  unsigned type_of( *ptr, len );
  size_t len( *ptr, len );
  size_t zlen( *ptr, len );
  const void *zptr( *ptr, len );
  bool pack( unsigned Q, *in, len, *out, &zlen );
  bool unpack( unsigned Q, *in, len, *out, &zlen );

  // medium level API, templates (in-place)
  bool is_packed( T );
  bool is_unpacked( T );
  unsigned type_of( T );
  size_t len( T );
  size_t zlen( T );
  const void *zptr( T );
  bool unpack( T &, T );
  bool pack( unsigned Q, T &, T );

  // high level API, templates (copy)
  T pack( unsigned Q, T );
  T unpack( T );
}
```
For a complete review check [bundle.hpp header](bundle.hpp)

### API - archives
```c++
namespace bundle
{
  struct file : map<string,string> { // ~map of properties
    bool    has(property);           // property check
    string &get(property);           // property access
    string  toc() const;             // inspection (json)
  };
  struct archive : vector<file>    { // ~sequence of files
    void   bun(string);              // .bun serialization
    string bun() const;              // .bun serialization
    void   zip(string);              // .zip serialization
    string zip() const;              // .zip serialization
    string toc() const;              // inspection (json)
  };
}
```

### Build Directives (Licensing)
|#define directive|Default value|Meaning|
|:-----------|:---------------|:---------|
|BUNDLE_NO_APACHE2|(undefined)|Define to remove any Apache 2.0 library from build
|BUNDLE_NO_BSD2|(undefined)|Define to remove any BSD-2 library from build
|BUNDLE_NO_BSD3|(undefined)|Define to remove any BSD-3 library from build
|BUNDLE_NO_CDDL|(undefined)|Define to remove any CDDL library from build
|BUNDLE_NO_GPL|(undefined)|Define to remove any GPL library from build
|BUNDLE_NO_MIT|(undefined)|Define to remove any MIT library from build
|BUNDLE_NO_UNLICENSE|(undefined)|Define to remove any Public Domain library from build (*)

(*): will disable `.bun` and `.zip` archive support as well.

### Build Directives (Libraries)
|#define directive|Default value|Meaning|
|:-----------|:---------------|:---------|
|BUNDLE_NO_BCM|(undefined)|Define to remove BCM library from build
|BUNDLE_NO_BROTLI|(undefined)|Define to remove Brotli library from build
|BUNDLE_NO_BSC|(undefined)|Define to remove LibBsc library from build
|BUNDLE_NO_BZIP2|(undefined)|Define to remove BZip2 library from build
|BUNDLE_NO_CRUSH|(undefined)|Define to remove CRUSH library from build
|BUNDLE_NO_CSC|(undefined)|Define to remove CSC library from build
|BUNDLE_NO_LZ4|(undefined)|Define to remove LZ4/LZ4 libraries 
|BUNDLE_NO_LZIP|(undefined)|Define to remove EasyLZMA library from build
|BUNDLE_NO_LZJB|(undefined)|Define to remove LZJB library from build
|BUNDLE_NO_LZMA|(undefined)|Define to remove LZMA library from build
|BUNDLE_NO_MCM|(undefined)|Define to remove MCM library from build
|BUNDLE_NO_MINIZ|(undefined)|Define to remove MiniZ library from build (*)
|BUNDLE_NO_SHOCO|(undefined)|Define to remove Shoco library from build
|BUNDLE_NO_SHRINKER|(undefined)|Define to remove Shrinker library from build
|BUNDLE_NO_TANGELO|(undefined)|Define to remove TANGELO library from build
|BUNDLE_NO_ZLING|(undefined)|Define to remove ZLING library from build
|BUNDLE_NO_ZMOLLY|(undefined)|Define to remove ZMOLLY library from build
|BUNDLE_NO_ZPAQ|(undefined)|Define to remove ZPAQ library from build
|BUNDLE_NO_ZSTD|(undefined)|Define to remove ZSTD library from build

(*): will disable `.bun` and `.zip` archive support as well.

## Build Directives (Other)
|#define directive|Default value|Meaning|
|:-----------|:---------------|:---------|
|BUNDLE_USE_OMP_TIMER|(undefined)|Define as 1 to use OpenMP timers
|BUNDLE_USE_CXX11|(autodetected)|Define as 0/1 to disable/enable C++11 features

### Licensing table
| Software | Author(s) | License | Version | Major changes? |
|:---------|:----------|:--------|--------:|:---------------------|
|[bundle](https://github.com/r-lyeh/bundle)|r-lyeh|ZLIB/LibPNG|latest||
|[bcm](http://sourceforge.net/projects/bcm/)|Ilya Muravyov|Public Domain|1.00|istream based now|
|[brotli](https://github.com/google/brotli)|Jyrki Alakuijala, Zoltan Szabadka|Apache 2.0|2015/11/03||
|[bzip2](http://www.bzip.org/)|Julian Seward|BSD-4||
|[crush](http://sourceforge.net/projects/crush/)|Ilya Muravyov|Public Domain|1.00|reentrant fix|
|[csc](https://github.com/fusiyuan2010/CSC)|Siyuan Fu|Public Domain|2015/06/16||
|[easylzma](https://github.com/lloyd/easylzma)|Igor Pavlov, Lloyd Hilaiel|Public Domain|0.0.7||
|[endian](https://gist.github.com/panzi/6856583)|Mathias Panzenböck|Public Domain||msvc fix|
|[libbsc](https://github.com/IlyaGrebnov/libbsc)|Ilya Grebnov|Apache 2.0|3.1.0||
|[libzling](https://github.com/richox/libzling)|Zhang Li|BSD-3|2015/09/16||
|[libzpaq](https://github.com/zpaq/zpaq)|Matt Mahoney|Public Domain|7.05||
|[lz4](https://github.com/Cyan4973/lz4)|Yann Collet|BSD-2|1.7.1||
|[lzjb](http://en.wikipedia.org/wiki/LZJB)|Jeff Bonwick|CDDL license|2010||
|[mcm](https://github.com/mathieuchartier/mcm)|Mathieu Chartier|GPL|0.84||
|[miniz](https://code.google.com/p/miniz/)|Rich Geldreich|Public Domain|v1.15 r.4.1|alignment fix|
|[shoco](https://github.com/Ed-von-Schleck/shoco)|Christian Schramm|MIT|2015/03/16||
|[shrinker](https://code.google.com/p/data-shrinker/)|Siyuan Fu|BSD-3|rev 3||
|[tangelo](http://encode.ru/threads/1738-TANGELO-new-compressor-(derived-from-PAQ8-FP8))|Matt Mahoney, Jan Ondrus|GPL|2.41|reentrant fixes, istream based now|
|[zmolly](https://github.com/richox/zmolly)|Zhang Li|BSD-3|0.0.1|reentrant and memstream fixes|
|[zstd](https://github.com/Cyan4973/zstd)|Yann Collet|BSD-2|0.3.2||

### Evaluated alternatives
[FastLZ](http://fastlz.org/), [FLZP](http://cs.fit.edu/~mmahoney/compression/#flzp), [LibLZF](http://freshmeat.net/projects/liblzf), [LZFX](https://code.google.com/p/lzfx/), [LZHAM](https://code.google.com/p/lzham/), [LZLIB](http://www.nongnu.org/lzip/lzlib.html), [LZO](http://www.oberhumer.com/opensource/lzo/), [LZP](http://www.cbloom.com/src/index_lz.html), [SMAZ](https://github.com/antirez/smaz), [Snappy](https://code.google.com/p/snappy/), [ZLIB](http://www.zlib.net/), [bzip2](http://www.bzip2.org/), [Yappy](http://blog.gamedeff.com/?p=371), [CMix](http://www.byronknoll.com/cmix.html), [M1](https://sites.google.com/site/toffer86/m1-project)

### Creating DLLs
```bash
cl bundle.cpp -DBUNDLE_API=BUNDLE_API_EXPORT /LD
cl demo.cc -DBUNDLE_API=BUNDLE_API_IMPORT bundle.lib
```

### Changelog
- v2.1.0 (2017/06/10): C API and DLL bindings; Pump up libcsc; Rename .bnd to .bun
- v2.0.5 (2016/02/06): Bring back BZIP2 support
- v2.0.4 (2015/12/04): Add padding support; Fix reentrant CRUSH; Optimizations & fixes
- v2.0.3 (2015/12/02): Add LZJB and CRUSH; Add BUNDLE_NO_CDDL directive
- v2.0.2 (2015/11/07): Fix ZMolly segmentation fault (OSX)
- v2.0.1 (2015/11/04): Fix clang warnings and compilation errors
- v2.0.0 (2015/11/03): Add BCM, ZLING, MCM, Tangelo, ZMolly, ZSTDf support
- v2.0.0 (2015/11/03): Change archive format (break change)
- v2.0.0 (2015/11/03): Disambiguate .bnd/.zip archive handling
- v2.0.0 (2015/11/03): Fix compilation errors (C++03)
- v2.0.0 (2015/11/03): Fix missing entries in benchmarks
- v2.0.0 (2015/11/03): Improve runtime C++ de/initialization stability
- v2.0.0 (2015/11/03): Optimize archive decompression
- v2.0.0 (2015/11/03): Remove obsolete enums
- v2.0.0 (2015/11/03): Upgrade Brotli, ZPAQ, LZ4, ZSTD and Shoco
- v1.0.2 (2015/10/29): Skip extra copy during archive decompression
- v1.0.2 (2015/10/29): Add extra archive meta-info
- v1.0.1 (2015/10/10): Shrink to fit during measures() function
- v1.0.0 (2015/10/09): Change benchmark API to sort multiples values as well (API break change)
- v0.9.8 (2015/10/07): Remove confusing bundle::string variant class from API
- v0.9.7 (2015/10/07): Add license configuration directives
- v0.9.6 (2015/10/03): Add library configuration directives
- v0.9.5 (2015/09/28): Add missing prototypes
- v0.9.5 (2015/09/28): Bugfix helper function
- v0.9.4 (2015/09/26): Add CSC20 + Shrinker support
- v0.9.4 (2015/09/26): Rename enums LZ4->LZ4F/LZ4HC->LZ4
- v0.9.3 (2015/09/25): Add a few missing API calls
- v0.9.2 (2015/09/22): Pump up Brotli
- v0.9.2 (2015/09/22): Split BROTLI enum into BROTLI9/11 pair
- v0.9.1 (2015/05/10): Switch to ZLIB/LibPNG license
- v0.9.0 (2015/04/08): BSC support
- v0.8.1 (2015/04/07): Pump up Brotli+ZSTD
- v0.8.1 (2015/04/07): LZMA20/25 dict
- v0.8.1 (2015/04/07): Unify FOURCCs
- v0.8.0 (2015/01/27): ZSTD support
- v0.8.0 (2015/01/27): Reorder enums
- v0.8.0 (2015/01/27): Simplify API
- v0.7.1 (2015/01/26): Fix LZMA
- v0.7.1 (2015/01/26): Verify DEFLATEs
- v0.7.1 (2015/01/26): New AUTO enum
- v0.7.0 (2014/10/22): Brotli support
- v0.7.0 (2014/10/22): Pump up LZ4
- v0.6.3 (2014/09/27): Switch to BOOST license
- v0.6.2 (2014/09/02): Fix 0-byte streams
- v0.6.2 (2014/09/02): Deflate alignment
- v0.6.1 (2014/06/30): Safer LZ4 decompression
- v0.6.1 (2014/06/30): Pump up LZ4 + ZPAQ
- v0.6.0 (2014/06/26): LZ4HC support
- v0.6.0 (2014/06/26): Optimize in-place decompression
- v0.5.0 (2014/06/09): ZPAQ support
- v0.5.0 (2014/06/09): UBER encoding
- v0.5.0 (2014/06/09): Fixes
- v0.4.1 (2014/06/05): Switch to lzmasdk
- v0.4.0 (2014/05/30): Maximize compression (lzma)
- v0.3.0 (2014/05/28): Fix alignment (deflate)
- v0.3.0 (2014/05/28): Change stream header
- v0.2.1 (2014/05/23): Fix overflow bug
- v0.2.0 (2014/05/14): Add VLE header
- v0.2.0 (2014/05/14): Fix vs201x compilation errors
- v0.1.0 (2014/05/13): Add high-level API
- v0.1.0 (2014/05/13): Add iOS support
- v0.0.0 (2014/05/09): Initial commit
