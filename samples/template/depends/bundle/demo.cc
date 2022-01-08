#include <fstream>
#include <sstream>
#include "bundle.h"

#ifdef _MSC_VER
#include <omp.h>
#endif

int main(int argc, const char **argv) {
    using namespace bundle;
    using namespace std;

    // 23 mb dataset
    string original( "There's a lady who's sure all that glitters is gold" );
    for (int i = 0; i < 18; ++i) original += original + string( i + 1, 32 + i );

    // argument file?
    if( argc > 1 ) {
        ifstream ifs( argv[1], ios::binary );
        stringstream ss;
        ss << ifs.rdbuf();
        original = ss.str();
    }

    // pack, unpack & verify all encoders
    vector<unsigned> libs { 
        RAW, SHOCO, LZ4F, MINIZ, LZIP, LZMA20,
        ZPAQ, LZ4, BROTLI9, ZSTD, LZMA25,
        BSC, BROTLI11, SHRINKER, CSC20, BCM,
        ZLING, MCM, TANGELO, ZMOLLY, CRUSH, LZJB
    };
    for( auto &lib : libs ) {
        #ifdef _MSC_VER
        double t = omp_get_wtime();
        #endif
        string packed = pack(lib, original);
        string unpacked = unpack(packed);
        cout << original.size() << " <--> " << packed.size() << " bytes (" << name_of(lib) << ")";
        #ifdef _MSC_VER
        cout << " " << ((omp_get_wtime() - t)*1000) << "ms.";
        #endif
        if( original == unpacked ) cout << endl; else cout << " (failed)" << endl;
    }

    cout << "All ok." << endl;
}
