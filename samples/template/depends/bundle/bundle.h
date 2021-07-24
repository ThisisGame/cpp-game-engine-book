/* Simple compression interface.
 * Copyright (c) 2013, 2014, 2015, 2016, 2017 r-lyeh.
 * ZLIB/libPNG licensed.

 * - rlyeh ~~ listening to Boris / Missing Pieces
 */

#ifndef BUNDLE_HPP
#define BUNDLE_HPP

#define BUNDLE_VERSION "2.1.0" /* (2017/06/10) C API and DLL bindings; Pump up libcsc; Rename .bnd to .bun
#define BUNDLE_VERSION "2.0.5" // (2016/02/06) Bring back BZIP2 support
#define BUNDLE_VERSION "2.0.4" // (2015/12/04) Add padding support; Fix reentrant CRUSH; Optimizations & fixes
#define BUNDLE_VERSION "2.0.3" // (2015/12/02) Add LZJB and CRUSH; Add BUNDLE_NO_CDDL directive
#define BUNDLE_VERSION "2.0.2" // (2015/11/07) Fix ZMolly segmentation fault (OSX)
#define BUNDLE_VERSION "2.0.1" // (2015/11/04) Fix clang warnings and compilation errors
#define BUNDLE_VERSION "2.0.0" // (2015/11/03) Add BCM,ZLING,MCM,Tangelo,ZMolly,ZSTDf support; Change archive format /!\
#define BUNDLE_VERSION "1.0.2" // (2015/10/29) Skip extra copy during archive decompression; add extra archive meta-info
#define BUNDLE_VERSION "1.0.1" // (2015/10/10) Shrink to fit during measures() function
#define BUNDLE_VERSION "1.0.0" // (2015/10/09) Change benchmark API to sort multiples values as well /!\
#define BUNDLE_VERSION "0.9.8" // (2015/10/07) Remove confusing bundle::string variant class from API
#define BUNDLE_VERSION "0.9.7" // (2015/10/07) Add license configuration directives { BUNDLE_NO_BSD2, BUNDLE_NO_BSD3, ... }
#define BUNDLE_VERSION "0.9.6" // (2015/10/03) Add library configuration directives { BUNDLE_NO_ZSTD, BUNDLE_NO_CSC, ... }
#define BUNDLE_VERSION "0.9.5" // (2015/09/28) Add missing prototypes; bugfix helper function
#define BUNDLE_VERSION "0.9.4" // (2015/09/26) Add CSC20 + Shrinker support; rename enums LZ4->LZ4F/LZ4HC->LZ4
#define BUNDLE_VERSION "0.9.3" // (2015/09/25) Add a few missing API calls
#define BUNDLE_VERSION "0.9.2" // (2015/09/22) Pump up Brotli; split BROTLI enum into BROTLI9/11 pair
#define BUNDLE_VERSION "0.9.1" // (2015/05/10) Switch to ZLIB/LibPNG license
#define BUNDLE_VERSION "0.9.0" // (2015/04/08) BSC support
#define BUNDLE_VERSION "0.8.1" // (2015/04/07) Pump up Brotli+ZSTD, LZMA20/25 dict, unify FOURCCs
#define BUNDLE_VERSION "0.8.0" // (2015/01/27) ZSTD support, reorder enums, simplify API
#define BUNDLE_VERSION "0.7.1" // (2015/01/26) Fix LZMA, verify DEFLATEs, new AUTO enum
#define BUNDLE_VERSION "0.7.0" // (2014/10/22) Brotli support, pump up LZ4
#define BUNDLE_VERSION "0.6.3" // (2014/09/27) Switch to BOOST license
#define BUNDLE_VERSION "0.6.2" // (2014/09/02) Fix 0-byte streams, deflate alignment
#define BUNDLE_VERSION "0.6.1" // (2014/06/30) Safer lz4 decompression, pump up lz4+zpaq
#define BUNDLE_VERSION "0.6.0" // (2014/06/26) LZ4HC support, optimize in-place decompression
#define BUNDLE_VERSION "0.5.0" // (2014/06/09) ZPAQ support, UBER encoding, fixes
#define BUNDLE_VERSION "0.4.1" // (2014/06/05) Switch to lzmasdk
#define BUNDLE_VERSION "0.4.0" // (2014/05/30) Maximize compression (lzma)
#define BUNDLE_VERSION "0.3.0" // (2014/05/28) Fix alignment (deflate), change stream header
#define BUNDLE_VERSION "0.2.1" // (2014/05/23) Fix overflow bug
#define BUNDLE_VERSION "0.2.0" // (2014/05/14) Add VLE header, fix vs201x compilation errors
#define BUNDLE_VERSION "0.1.0" // (2014/05/13) Add high-level API, iOS support
#define BUNDLE_VERSION "0.0.0" // (2014/05/09) Initial commit */

// C Api {

#include <stdio.h>   // size_t
#include <stdbool.h> // bool

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define BUNDLE_API_EXPORT __declspec(dllexport)
#define BUNDLE_API_IMPORT __declspec(dllimport)
#define BUNDLE_API_STATIC
#else
#define BUNDLE_API_EXPORT
#define BUNDLE_API_IMPORT
#define BUNDLE_API_STATIC
#endif

#ifndef BUNDLE_API
#define BUNDLE_API BUNDLE_API_EXPORT
#endif

// libraries and/or encoders 
enum { BUNDLE_RAW, BUNDLE_SHOCO, BUNDLE_LZ4F, BUNDLE_MINIZ, BUNDLE_LZIP, BUNDLE_LZMA20, BUNDLE_ZPAQ, BUNDLE_LZ4,      //  0..7
       BUNDLE_BROTLI9, BUNDLE_ZSTD, BUNDLE_LZMA25, BUNDLE_BSC, BUNDLE_BROTLI11, BUNDLE_SHRINKER, BUNDLE_CSC20,        //  7..14
       BUNDLE_ZSTDF, BUNDLE_BCM, BUNDLE_ZLING, BUNDLE_MCM, BUNDLE_TANGELO, BUNDLE_ZMOLLY, BUNDLE_CRUSH, BUNDLE_LZJB,  // 15..22
       BUNDLE_BZIP2                                                                                                   // 23..
};

// constant: 32 = [0..10] padding + [1] 0x70 + [1] q + [1..10] vle(in) + [1..10] vle(out)
enum { BUNDLE_MAX_HEADER_SIZE = 32 }; 

// algorithm properties
BUNDLE_API const char *const bundle_name_of( unsigned q );
BUNDLE_API const char *const bundle_version_of( unsigned q );
BUNDLE_API const char *const bundle_ext_of( unsigned q );
BUNDLE_API size_t bundle_unc_payload( unsigned q );
BUNDLE_API size_t bundle_bound( unsigned q, size_t len );

// low level API (raw pointers)
BUNDLE_API bool bundle_is_packed( const void *mem, size_t size );
BUNDLE_API bool bundle_is_unpacked( const void *mem, size_t size );
BUNDLE_API unsigned bundle_type_of( const void *mem, size_t size );
BUNDLE_API unsigned bundle_guess_type_of( const void *mem, size_t size );
BUNDLE_API size_t bundle_len( const void *mem, size_t size );
BUNDLE_API size_t bundle_zlen( const void *mem, size_t size );
BUNDLE_API size_t bundle_padding( const void *mem, size_t size );
BUNDLE_API const void *bundle_zptr( const void *mem, size_t size );
BUNDLE_API bool bundle_pack( unsigned q, const void *in, size_t len, void *out, size_t *zlen );
BUNDLE_API bool bundle_unpack( unsigned q, const void *in, size_t len, void *out, size_t *zlen );

// variable-length encoder
BUNDLE_API size_t bundle_enc_vlebit( char *buf, size_t val );      // return number of bytes written
BUNDLE_API size_t bundle_dec_vlebit( const char *i, size_t *val ); // return number of bytes read

#ifdef __cplusplus
}
#endif

// } C API

#ifdef __cplusplus  // C++ API

#if ( defined(_MSC_VER) && _MSC_VER >= 1800 ) || __cplusplus >= 201103L
#define BUNDLE_USE_CXX11 1
#else
#define BUNDLE_USE_CXX11 0
#endif

#if BUNDLE_USE_CXX11
#include <chrono>
#endif

#if BUNDLE_USE_OMP_TIMER
#include <omp.h>
#endif

#include <cassert>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace bundle
{
    // libraries and/or encoders 
    enum { RAW, SHOCO, LZ4F, MINIZ, LZIP, LZMA20, ZPAQ, LZ4,      //  0..7
           BROTLI9, ZSTD, LZMA25, BSC, BROTLI11, SHRINKER, CSC20, //  7..14
           ZSTDF, BCM, ZLING, MCM, TANGELO, ZMOLLY, CRUSH, LZJB,  // 15..22
           BZIP2                                                  // 23..
    };

    enum { MAX_HEADER_SIZE = BUNDLE_MAX_HEADER_SIZE };

    // algorithm properties
    inline const char *const name_of( unsigned q ) { return bundle_name_of( q ); }
    inline const char *const version_of( unsigned q ) { return bundle_version_of( q ); }
    inline const char *const ext_of( unsigned q ) { return bundle_ext_of( q ); }
    inline size_t unc_payload( unsigned q ) { return bundle_unc_payload( q ); }
    inline size_t bound( unsigned q, size_t len ) { return bundle_bound( q, len ); }

    // low level API (raw pointers)

    inline bool is_packed( const void *mem, size_t size ) { return bundle_is_packed( mem, size ); }
    inline bool is_unpacked( const void *mem, size_t size ) { return bundle_is_unpacked( mem, size ); }
    inline unsigned type_of( const void *ptr, size_t len ) { return bundle_type_of( ptr, len ); }
    inline unsigned guess_type_of( const void *ptr, size_t len ) { return bundle_guess_type_of( ptr, len ); }
    inline size_t len( const void *ptr, size_t len ) { return bundle_len( ptr, len ); }
    inline size_t zlen( const void *ptr, size_t len ) { return bundle_zlen( ptr, len ); }
    inline size_t padding( const void *mem, size_t size ) { return bundle_padding( mem, size ); }
    inline const void *zptr( const void *ptr, size_t len ) { return bundle_zptr( ptr, len ); }
    inline bool pack( unsigned q, const void *in, size_t inlen, void *out, size_t &outlen ) { return bundle_pack( q, in, inlen, out, &outlen ); }
    inline bool unpack( unsigned q, const void *in, size_t inlen, void *out, size_t &outlen ) { return bundle_unpack( q, in, inlen, out, &outlen ); }

    inline std::string vlebit( size_t i ) {
        char buf[16] = {0};
        int bytes = bundle_enc_vlebit( buf, i );
        return std::string( buf, bytes );
    }
    inline size_t vlebit( const char *&i ) {
        size_t out;
        i += bundle_dec_vlebit( i, &out );
        return out;
    }

    // medium level API, templates (in-place)

    template<typename container>
    static inline bool is_packed( const container &input ) {
        return is_packed( &input[0], input.size() );
    }

    template<typename container>
    static inline bool is_unpacked( const container &input ) {
        return !is_packed( &input[0], input.size() );
    }

    template<typename T>
    static inline unsigned guess_type_of( const T &input ) {
        return guess_type_of( &input[0], input.size() );
    }
    template<typename T>
    static inline unsigned type_of( const T &input ) {
        return type_of( &input[0], input.size() );
    }
    template<typename T>
    static inline const char *const name_of( const T &input ) {
        return name_of( type_of( input ) );
    }
    template<typename T>
    static inline const char *const version_of( const T &input ) {
        return version_of( type_of( input ) );
    }
    template<typename T>
    static inline const char *const ext_of( const T &input ) {
        return ext_of( type_of( input ) );
    }
    template<typename T>
    static inline size_t padding( const T &input ) {
        return padding( &input[0], input.size() );
    }
    template<typename T>
    static inline size_t len( const T &input ) {
        return len( &input[0], input.size() );
    }
    template<typename T>
    static inline size_t zlen( const T &input ) {
        return zlen( &input[0], input.size() );
    }
    template<typename T>
    static inline const void *zptr( const T &input ) {
        return zptr( &input[0], input.size() );
    }

    template < class T1, class T2 >
    static inline bool unpack( T2 &output, const T1 &input ) {
        // sanity checks
        assert( sizeof(input[0]) == 1 && "size of input elements != 1" );
        assert( sizeof(output[0]) == 1 && "size of output elements != 1" );

        if( is_packed( input ) ) {
            // decapsulate, skip header (initial byte), and grab Q (compression algorithm)
            size_t pad = padding( input );
            unsigned Q = input[pad + 1];

            // retrieve uncompressed and compressed size
            const char *ptr = (const char *)&input[pad + 2];
            size_t size1 = vlebit(ptr);
            size_t size2 = vlebit(ptr);

            // decompress
            size1 += unc_payload(Q);
            output.resize( size1 );

            // note: output must be resized properly before calling this function!! (see bound() function)
            if( unpack( Q, ptr, size2, &output[0], size1 ) ) {
                output.resize( size1 );
                return true;
            }
        }

        output = input;
        return false;
    }

    template < class T1, class T2 >
    static inline bool pack( unsigned q, T2 &output, const T1 &input ) {
        // sanity checks
        assert( sizeof(input[0]) == 1 && "size of input elements != 1" );
        assert( sizeof(output[0]) == 1 && "size of output elements != 1" );

        if( input.empty() ) {
            output = input;
            return true;
        }

        if( 1 /* is_unpacked( input ) */ ) {
            // resize to worst case
            size_t zlen = bound(q, input.size());
            output.resize( MAX_HEADER_SIZE + zlen );

            // compress
            if( pack( q, &input[0], input.size(), &output[MAX_HEADER_SIZE], zlen ) ) {
                // resize properly (new zlen)
                output.resize( MAX_HEADER_SIZE + zlen );

                // encapsulate
                std::string header = std::string() + char(0x70) + char(q) + vlebit(input.size()) + vlebit(output.size() - MAX_HEADER_SIZE);
                size_t header_len = header.size();
                memset( &output[0], 0, MAX_HEADER_SIZE );
                memcpy( &output[MAX_HEADER_SIZE-header_len], &header[0], header_len );
                return true;
            }
        }

        output = input;
        return false;
    }

    // high level API, templates (copy)

    template < class T1 >
    static inline T1 unpack( const T1 &input ) {
        T1 output;
        unpack( output, input );
        return output;
    }


    template<typename container>
    static inline container pack( unsigned q, const container &input ) {
        container output;
        pack( q, output, input );
        return output;
    }

    // helpers

    static inline std::vector<unsigned> fast_encodings() {
        static std::vector<unsigned> all;
        if( all.empty() ) {
            all.push_back( RAW );
            all.push_back( LZ4F );
            all.push_back( SHOCO );
            all.push_back( MINIZ );
            all.push_back( LZMA20 );
            all.push_back( LZMA25 );
            all.push_back( LZ4 );
            all.push_back( ZSTD );
            all.push_back( SHRINKER );
            all.push_back( ZLING );
            all.push_back( ZSTDF );
            all.push_back( CRUSH );
            all.push_back( LZJB );
            all.push_back( BZIP2 );
#if 0
            // for archival purposes
            all.push_back( LZFX );
            all.push_back( LZHAM );
            all.push_back( LZP1 );
            all.push_back( FSE );
            all.push_back( BLOSC );
            all.push_back( YAPPY );
#endif
        }
        return all;
    }

    static inline std::vector<unsigned> slow_encodings() {
        static std::vector<unsigned> all;
        if( all.empty() ) {
            all.push_back( BCM );
            all.push_back( BROTLI11 );
            all.push_back( BROTLI9 );
            all.push_back( BSC );
            all.push_back( CSC20 );
            all.push_back( LZIP );
            all.push_back( MCM );
            all.push_back( TANGELO );
            all.push_back( ZMOLLY );
            all.push_back( ZPAQ );
        }
        return all;
    }

    static inline std::vector<unsigned> encodings() {
        static std::vector<unsigned> all;
        if( all.empty() ) {
            all = fast_encodings();
            all.push_back( BCM );
            all.push_back( BROTLI11 );
            all.push_back( BROTLI9 );
            all.push_back( BSC );
            all.push_back( CSC20 );
            all.push_back( LZIP );
            all.push_back( MCM );
            all.push_back( TANGELO );
            all.push_back( ZMOLLY );
            all.push_back( ZPAQ );
        }
        return all;
    }

#if BUNDLE_USE_CXX11

    // measures for all given encodings
    template<typename T>
    struct measure {
        unsigned q = RAW;
        bool pass = 0;
        double ratio = 0;
        double enctime = 0;
        double dectime = 0;
        double memusage = 0;
        unsigned long long bytes = 0;
        T packed;
        double encspeed() const {
            double mbytes = bytes / 1024.0 / 1024.0;
            auto secs = []( double x ) { return x / 1000000.0; };
            mbytes /= secs(enctime > 0.0 ? enctime : 1.0);
            return int( mbytes * 100 ) / 100.0;
        }
        double decspeed() const {
            double mbytes = bytes / 1024.0 / 1024.0;
            auto secs = []( double x ) { return x / 1000000.0; };
            mbytes /= secs(dectime > 0.0 ? dectime : 1.0);
            return int( mbytes * 100 ) / 100.0;
        }
        double avgspeed() const {
            double mbytes = bytes / 1024.0 / 1024.0;
            auto secs = []( double x ) { return x / 1000000.0; };
            mbytes = mbytes * 2 / secs(dectime + enctime ? dectime + enctime : 1.0);
            return int( mbytes * 100 ) / 100.0;
        }
        std::string str() const {
            std::stringstream ss;
            ss << ( pass ? "[ OK ] " : "[FAIL] ") << name_of(q) << ": ratio=" << ratio << "% enctime=" << int(enctime) << "us dectime=" << int(dectime) << "us (zlen=" << packed.size() << " bytes)";
            ss << "(enc:" << encspeed() << " MB/s,dec:" << decspeed() << " MB/s,avg:" << avgspeed() << " MB/s)";
            return ss.str();
        }
    };

    template< class T, bool do_enc = true, bool do_dec = true, bool do_verify = true >
    std::vector< measure<T> > measures( const T& original, const std::vector<unsigned> &use_encodings = encodings() ) {
        std::vector< measure<T> > results;
        auto bytes = original.size();

        for( auto encoding : use_encodings ) {
            //std::cout << name_of(encoding) << std::endl;
            results.push_back( measure<T>() );
            auto &r = results.back();
            r.q = encoding;
            r.pass = true;
            r.bytes = bytes;

            if( r.pass && do_enc ) {
#if BUNDLE_USE_OMP_TIMER
                auto start = omp_get_wtime();
                pack( encoding, r.packed, original );
                auto end = omp_get_wtime();
                r.enctime = ( end - start ) * 1000000;
#else
                auto start = std::chrono::steady_clock::now();
                pack( encoding, r.packed, original );
                auto end = std::chrono::steady_clock::now();
                r.enctime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
#endif
                r.ratio = 100 - 100 * ( double( r.packed.size() ) / original.size() );
                if( encoding != RAW )
                r.pass = r.pass && is_packed(r.packed);
            }

            if( r.pass && do_dec ) {
#if BUNDLE_USE_OMP_TIMER
                std::string unpacked;
                auto start = omp_get_wtime();
                unpack( unpacked, r.packed );
                auto end = omp_get_wtime();
                r.dectime = ( end - start ) * 1000000;
#else
                std::string unpacked;
                auto start = std::chrono::steady_clock::now();
                unpack( unpacked, r.packed );
                auto end = std::chrono::steady_clock::now();
                r.dectime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
#endif
                if( encoding != RAW )
                r.pass = r.pass && (do_verify ? original == unpacked : r.pass);
            }

            if( !r.pass ) {
                r.ratio = r.enctime = r.dectime = 0;
            } else {
                // @todo: clean up, if !best ratio && !fastest dec && !best mem
            }
        }

        return results;
    }

    // sort_* functions return sorted slot indices (as seen order in measures vector)

    template< class T >
    std::vector<unsigned> sort_smallest_encoders( const std::vector< measure<T> > &measures, double pct_treshold_to_skip_compression = 0 ) {
        // skip compression results if compression ratio is below % (like <5%). default: 0 (do not skip)
        // also, sort in reverse order
        std::map<double,std::set<unsigned>,std::greater<double>> q;
        for( auto end = measures.size(), it = end - end; it < end; ++it ) {
            auto &r = measures[ it ];
            if( r.pass && r.ratio >= pct_treshold_to_skip_compression ) {
                q[ r.ratio ].insert( it );
            }
        }
        std::vector<unsigned> v;
        for( auto &set : q ) {
            for( auto &enc : set.second ) {
                v.push_back( enc );
            }
        }
        return v;
    }

    template< class T >
    std::vector<unsigned> sort_fastest_encoders( const std::vector< measure<T> > &measures ) {
        std::map<double,std::set<unsigned>> q;
        for( auto end = measures.size(), it = end - end; it < end; ++it ) {
            auto &r = measures[ it ];
            if( r.pass ) {
                q[ r.enctime ].insert( it );
            }
        }
        std::vector<unsigned> v;
        for( auto &set : q ) {
            for( auto &enc : set.second ) {
                v.push_back( enc );
            }
        }
        return v;
    }

    template< class T >
    std::vector<unsigned> sort_fastest_decoders( const std::vector< measure<T> > &measures ) {
        std::map<double,std::set<unsigned>> q;
        for( auto end = measures.size(), it = end - end; it < end; ++it ) {
            auto &r = measures[ it ];
            if( r.pass ) {
                q[ r.dectime ].insert( it );
            }
        }
        std::vector<unsigned> v;
        for( auto &set : q ) {
            for( auto &dec : set.second ) {
                v.push_back( dec );
            }
        }
        return v;
    }

    template< class T >
    std::vector<unsigned> sort_average_coders( const std::vector< measure<T> > &measures ) {
        std::map<double,std::set<unsigned>> q;
        for( auto end = measures.size(), it = end - end; it < end; ++it ) {
            auto &r = measures[ it ];
            if( r.pass ) {
                q[ r.enctime + r.dectime ].insert( it );
            }
        }
        std::vector<unsigned> v;
        for( auto &set : q ) {
            for( auto &avg : set.second ) {
                v.push_back( avg );
            }
        }
        return v;
    }

    // find_* functions return sorted encoding enums (as slot indices are traversed from measures vector)

    template< class T >
    std::vector<unsigned> find_smallest_encoders( const std::vector< measure<T> > &measures, double pct_treshold_to_skip_compression = 0 ) {
        std::vector<unsigned> v;
        for( auto &slot : sort_smallest_encoders( measures, pct_treshold_to_skip_compression ) ) {
            v.push_back( measures[slot].q );
        }
        return v;
    }

    template< class T >
    std::vector<unsigned> find_fastest_encoders( const std::vector< measure<T> > &measures ) {
        std::vector<unsigned> v;
        for( auto &slot : sort_fastest_encoders( measures ) ) {
            v.push_back( measures[slot].q );
        }
        return v;
    }

    template< class T >
    std::vector<unsigned> find_fastest_decoders( const std::vector< measure<T> > &measures ) {
        std::vector<unsigned> v;
        for( auto &slot : sort_fastest_decoders( measures ) ) {
            v.push_back( measures[slot].q );
        }
        return v;
    }

    template< class T >
    std::vector<unsigned> find_average_coders( const std::vector< measure<T> > &measures ) {
        std::vector<unsigned> v;
        for( auto &slot : sort_average_coders( measures ) ) {
            v.push_back( measures[slot].q );
        }
        return v;
    }

#endif
}

namespace bundle
{
    template< typename T >
    std::string itoa( const T &t ) {
        std::stringstream ss;
        ss.precision(20);
        return ss << t ? ss.str() : std::string();
    }

    template< typename T >
    T as( const std::string &text ) {
        T t;
        std::stringstream ss( text );
        return ss >> t ? t : T();
    }

    struct file : public std::map< std::string, std::string >
    {
        bool has( const std::string &property ) const;

        template <typename T>
        T get( const std::string &property ) const {
            return as<T>( (*this->find( property )).second );
        }

        // inspection (json doc)
        std::string toc() const {
            std::string ret, sep = "\t{\n";
            for( const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                const std::pair< std::string, std::string > &property = *it;
                if( property.first == "data" )
                    ret += sep + "\t\t\"data\":\"... (" + bundle::itoa( property.second.size() ) + " bytes)\"";
                else
                    ret += sep + "\t\t\"" + property.first + "\":\"" + property.second + "\"";
                sep = ",\n";
            }
            return ret + "\n\t}";
        }
    };

    class archive : public std::vector< bundle::file >
    {
        public:

        enum type { BUN, ZIP };

        // .bun binary serialization
        bool bun( const std::string &binary );
        std::string bun() const;

        // .zip binary serialization
        bool zip( const std::string &binary );
        std::string zip( unsigned level ) const; /* level [0(store)..100(max)] */

        // generic serialization
        bool bin( int type, const std::string &binary );
        std::string bin( int type, unsigned level ) const;

        // inspection (json doc)
        std::string toc() const {
            std::string ret, sep = "[\n";
            for( const_iterator it = this->begin(), end = this->end(); it != end; ++it ) {
                ret += sep + it->toc();
                sep = ",\n";
            }
            return ret + "\n]\n";
        }
    };
}

#endif // __cplusplus
#endif // BUNDLE_HPP

#if defined(BUNDLE_BUILD_TESTS) && defined(__cplusplus)

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// tiny unittest suite { // usage: int main() { /* orphan test */ test(1<2); suite("grouped tests") { test(1<2); test(1<2); } }
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define suite(...) if(printf("------ " __VA_ARGS__),puts(""),true)
#define test(...)  (errno=0,++tst,err+=!(ok=!!(__VA_ARGS__))),printf("[%s] %d %s (%s)\n",ok?" OK ":"FAIL",__LINE__,#__VA_ARGS__,strerror(errno))
unsigned tst=0,err=0,ok=atexit([]{ suite("summary"){ printf("[%s] %d tests = %d passed + %d errors\n",err?"FAIL":" OK ",tst,tst-err,err); }});
// } rlyeh, public domain.

void stream_tests(int argc, char **argv) {
    suite( "stream tests" ) {
        using namespace std;
        using namespace bundle;
        // 23 mb dataset
        string original( "There's a lady who's sure all that glitters is gold" );
        for (int i = 0; i < 18; ++i) original += original + string( i + 1, 32 + i );

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
            ZLING, MCM, TANGELO, ZMOLLY, CRUSH, LZJB,
            BZIP2
        };
        for( auto &lib : libs ) {
            suite( "stream test (%s)", name_of(lib) ) {
                string packed = pack(lib, original);
                string unpacked = unpack(packed);
                cout << "[ OK ] " << original.size() << " <-> " << packed.size() << " bytes" << endl;
                test( original == unpacked );
            }
        }
    }
}

void zip_tests() {
    std::string binary;

    suite( ".zip saving tests" ) {
        bundle::archive pak;

        pak.resize(2);

        pak[0]["name"] = "test.txt";
        pak[0]["data"] = "hello world";

        pak[1]["name"] = "test2.txt";
        pak[1]["data"] = "1337";

        //std::cout << "zipping files..." << std::endl;

        // save .zip archive to memory string (then optionally to disk)
        binary = pak.zip(60); // compression level = 60 (of 100)

        //std::cout << "saving test:\n" << pak.toc() << std::endl;
    }

    suite( ".zip loading tests" ) {
        //std::cout << "unzipping files..." << std::endl;

        bundle::archive pak;
        pak.zip( binary );

        //std::cout << "loading test:\n" << pak.toc() << std::endl;

        test( pak.size() == 2 );

        test( pak[0]["name"] == "test.txt" );
        test( pak[0]["data"] == "hello world" );

        test( pak[1]["name"] == "test2.txt" );
        test( pak[1]["data"] == "1337" );
    }
}

void bun_tests() {
    std::string binary;

    suite( ".bun saving tests" ) {
        bundle::archive pak;
        pak.resize(2);

        pak[0]["name"] = "test_lz4.txt";
        pak[0]["data"] = "hellohellohellohellohellohello";
        pak[0]["data"] = bundle::pack( bundle::LZ4, pak[0]["data"] );

        pak[1]["name"] = "test_shoco.txt";
        pak[1]["data"] = "hellohellohellohellohellohello";
        pak[1]["data"] = bundle::pack( bundle::SHOCO, pak[1]["data"] );

        //std::cout << "packing files..." << std::endl;

        // save .bun archive to memory string (then optionally to disk)
        binary = pak.bun();

        //std::cout << "saving test:\n" << pak.toc() << std::endl;
    }

    suite( ".bun loading tests" ) {
        //std::cout << "unpacking files..." << std::endl;

        bundle::archive pak;
        pak.bun( binary );

        //std::cout << "loading test:\n" << pak.toc() << std::endl;

        test( pak.size() == 2 );

        pak[0]["data"] = bundle::unpack( pak[0]["data"] );
        pak[1]["data"] = bundle::unpack( pak[1]["data"] );

        //std::cout << pak[0]["data"] << std::endl;

        test( pak[0]["name"] == "test_lz4.txt" );
        test( pak[1]["data"] == "hellohellohellohellohellohello" );

        test( pak[1]["name"] == "test_shoco.txt" );
        test( pak[1]["data"] == "hellohellohellohellohellohello" );

        //std::cout << pak.toc() << std::endl;
    }
}

int main(int argc, char **argv) {
    stream_tests(argc, argv);
    zip_tests();
    bun_tests();
}

#endif // BUNDLE_BUILD_TESTS
