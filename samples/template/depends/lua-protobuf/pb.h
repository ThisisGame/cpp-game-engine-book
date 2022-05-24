#ifndef pb_h
#define pb_h

#ifndef PB_NS_BEGIN
# ifdef __cplusplus
#   define PB_NS_BEGIN extern "C" {
#   define PB_NS_END   }
# else
#   define PB_NS_BEGIN
#   define PB_NS_END
# endif
#endif /* PB_NS_BEGIN */

#ifndef PB_STATIC
# if __GNUC__
#   define PB_STATIC static __attribute((unused))
# else
#   define PB_STATIC static
# endif
#endif

#ifdef PB_STATIC_API
# ifndef PB_IMPLEMENTATION
#  define PB_IMPLEMENTATION
# endif
# define PB_API PB_STATIC
#endif

#if !defined(PB_API) && defined(_WIN32)
# ifdef PB_IMPLEMENTATION
#  define PB_API __declspec(dllexport)
# else
#  define PB_API __declspec(dllimport)
# endif
#endif

#ifndef PB_API
# define PB_API extern
#endif

#if defined(_MSC_VER) || defined(__UNIXOS2__) || defined(__SOL64__)
typedef unsigned char      uint8_t;
typedef signed   char       int8_t;
typedef unsigned short     uint16_t;
typedef signed   short      int16_t;
typedef unsigned int       uint32_t;
typedef signed   int        int32_t;
typedef unsigned long long uint64_t;
typedef signed   long long  int64_t;
#define INT64_MIN LLONG_MIN
#define INT64_MAX LLONG_MAX

#elif defined(__SCO__) || defined(__USLC__) || defined(__MINGW32__)
# include <stdint.h>
#else
# include <inttypes.h>
# if (defined(__sun__) || defined(__digital__))
#   if defined(__STDC__) && (defined(__arch64__) || defined(_LP64))
typedef unsigned long int  uint64_t;
typedef signed   long int   int64_t;
#   else
typedef unsigned long long uint64_t;
typedef signed   long long  int64_t;
#   endif /* LP64 */
# endif /* __sun__ || __digital__ */
#endif

#include <stddef.h>
#include <limits.h>

PB_NS_BEGIN


/* types */

#define PB_WIRETYPES(X) /* X(name, index) */\
    X(VARINT, "varint", 0) X(64BIT,  "64bit", 1) X(BYTES, "bytes", 2)  \
    X(GSTART, "gstart", 3) X(GEND,   "gend",  4) X(32BIT, "32bit", 5)  \

#define PB_TYPES(X)           /* X(name, type, index) */\
    X(double,   double,   1)  X(float,    float,    2)  \
    X(int64,    int64_t,  3)  X(uint64,   uint64_t, 4)  \
    X(int32,    int32_t,  5)  X(fixed64,  uint64_t, 6)  \
    X(fixed32,  uint32_t, 7)  X(bool,     int,      8)  \
    X(string,   pb_Slice, 9)  X(group,    pb_Slice, 10) \
    X(message,  pb_Slice, 11) X(bytes,    pb_Slice, 12) \
    X(uint32,   uint32_t, 13) X(enum,     int32_t,  14) \
    X(sfixed32, int32_t,  15) X(sfixed64, int64_t,  16) \
    X(sint32,   int32_t,  17) X(sint64,   int64_t,  18) \

typedef enum pb_WireType {
#define X(name, s, index) PB_T##name,
    PB_WIRETYPES(X)
#undef  X
    PB_TWIRECOUNT
} pb_WireType;

typedef enum pb_FieldType {
#define X(name, type, index) PB_T##name = index,
    PB_TYPES(X)
#undef  X
    PB_TYPECOUNT
} pb_FieldType;


/* conversions */

PB_API uint64_t pb_expandsig     (uint32_t v);
PB_API uint32_t pb_encode_sint32 ( int32_t v);
PB_API  int32_t pb_decode_sint32 (uint32_t v);
PB_API uint64_t pb_encode_sint64 ( int64_t v);
PB_API  int64_t pb_decode_sint64 (uint64_t v);
PB_API uint32_t pb_encode_float  (float    v);
PB_API float    pb_decode_float  (uint32_t v);
PB_API uint64_t pb_encode_double (double   v);
PB_API double   pb_decode_double (uint64_t v);


/* decode */

typedef struct pb_Slice { const char *p, *end; } pb_Slice;
#define pb_gettype(v)       ((v) &  7)
#define pb_gettag(v)        ((v) >> 3)
#define pb_pair(tag, type)  ((tag) << 3 | ((type) & 7))

PB_API pb_Slice pb_slice  (const char *p);
PB_API pb_Slice pb_lslice (const char *p, size_t len);
PB_API size_t   pb_len    (pb_Slice s);

PB_API size_t pb_readvarint32 (pb_Slice *s, uint32_t *pv);
PB_API size_t pb_readvarint64 (pb_Slice *s, uint64_t *pv);
PB_API size_t pb_readfixed32 (pb_Slice *s, uint32_t *pv);
PB_API size_t pb_readfixed64 (pb_Slice *s, uint64_t *pv);

PB_API size_t pb_readslice (pb_Slice *s, size_t len, pb_Slice *pv);
PB_API size_t pb_readbytes (pb_Slice *s, pb_Slice *pv);
PB_API size_t pb_readgroup (pb_Slice *s, uint32_t tag, pb_Slice *pv);

PB_API size_t pb_skipvarint (pb_Slice *s);
PB_API size_t pb_skipbytes  (pb_Slice *s);
PB_API size_t pb_skipslice  (pb_Slice *s, size_t len);
PB_API size_t pb_skipvalue  (pb_Slice *s, uint32_t tag);

PB_API const char *pb_wtypename (int wiretype, const char *def);
PB_API const char *pb_typename  (int type,     const char *def);

PB_API int pb_typebyname  (const char *name, int def);
PB_API int pb_wtypebyname (const char *name, int def);
PB_API int pb_wtypebytype (int type);


/* encode */

#define PB_BUFFERSIZE   (1024)

typedef struct pb_Buffer {
    size_t size;
    size_t capacity;
    char  *buff;
    char   init_buff[PB_BUFFERSIZE];
} pb_Buffer;

#define pb_buffer(b)      ((b)->buff)
#define pb_bufflen(b)     ((b)->size)
#define pb_addsize(b, sz) ((void)((b)->size += (sz)))
#define pb_addchar(b, ch) \
    ((void)((b)->size < (b)->capacity || pb_prepbuffsize((b), 1)), \
     ((b)->buff[(b)->size++] = (ch)))

PB_API void   pb_initbuffer   (pb_Buffer *b);
PB_API void   pb_resetbuffer  (pb_Buffer *b);
PB_API size_t pb_resizebuffer (pb_Buffer *b, size_t len);
PB_API void  *pb_prepbuffsize (pb_Buffer *b, size_t len);

PB_API pb_Slice pb_result (pb_Buffer *b);

PB_API size_t pb_addvarint32 (pb_Buffer *b, uint32_t v);
PB_API size_t pb_addvarint64 (pb_Buffer *b, uint64_t v);
PB_API size_t pb_addfixed32  (pb_Buffer *b, uint32_t v);
PB_API size_t pb_addfixed64  (pb_Buffer *b, uint64_t v);

PB_API size_t pb_addslice  (pb_Buffer *b, pb_Slice s);
PB_API size_t pb_addbytes  (pb_Buffer *b, pb_Slice s);
PB_API size_t pb_addlength (pb_Buffer *b, size_t len);


/* type info database state and name table */

typedef struct pb_State pb_State;
typedef struct pb_Name  pb_Name;

PB_API void pb_init (pb_State *S);
PB_API void pb_free (pb_State *S);

PB_API pb_Name *pb_newname (pb_State *S, pb_Slice    s);
PB_API void     pb_delname (pb_State *S, pb_Name    *name);
PB_API pb_Name *pb_name    (pb_State *S, const char *name);

PB_API pb_Name *pb_usename (pb_Name *name);


/* type info */

typedef struct pb_Type  pb_Type;
typedef struct pb_Field pb_Field;

#define PB_OK     0
#define PB_ERROR  1
#define PB_ENOMEM 2

PB_API int pb_load (pb_State *S, pb_Slice *s);

PB_API pb_Type  *pb_newtype  (pb_State *S, pb_Name *tname);
PB_API void      pb_deltype  (pb_State *S, pb_Type *t);
PB_API pb_Field *pb_newfield (pb_State *S, pb_Type *t, pb_Name *fname, int32_t number);
PB_API void      pb_delfield (pb_State *S, pb_Type *t, pb_Field *f);

PB_API pb_Type  *pb_type   (pb_State *S, pb_Name *tname);
PB_API pb_Field *pb_fname  (pb_Type *t,  pb_Name *tname);
PB_API pb_Field *pb_field  (pb_Type *t,  int32_t number);

PB_API pb_Name *pb_oneofname (pb_Type *t, int oneof_index);

PB_API int pb_nexttype  (pb_State *S, pb_Type **ptype);
PB_API int pb_nextfield (pb_Type *t, pb_Field **pfield);


/* util: memory pool */

#define PB_POOLSIZE 4096

typedef struct pb_Pool {
    void  *pages;
    void  *freed;
    size_t obj_size;
} pb_Pool;

PB_API void pb_initpool (pb_Pool *pool, size_t obj_size);
PB_API void pb_freepool (pb_Pool *pool);

PB_API void *pb_poolalloc (pb_Pool *pool);
PB_API void  pb_poolfree  (pb_Pool *pool, void *obj);

/* util: hash table */

typedef struct pb_Table pb_Table;
typedef struct pb_Entry pb_Entry;
typedef ptrdiff_t pb_Key;

PB_API void pb_inittable (pb_Table *t, size_t entrysize);
PB_API void pb_freetable (pb_Table *t);

PB_API size_t pb_resizetable (pb_Table *t, size_t size);

PB_API pb_Entry *pb_gettable (pb_Table *t, pb_Key key);
PB_API pb_Entry *pb_settable (pb_Table *t, pb_Key key);
PB_API int pb_nextentry (pb_Table *t, pb_Entry **pentry);

struct pb_Table {
    size_t    size;
    size_t    lastfree;
    unsigned  entry_size : sizeof(unsigned)*CHAR_BIT - 1;
    unsigned  has_zero   : 1;
    pb_Entry *hash;
};

struct pb_Entry {
    ptrdiff_t next;
    pb_Key    key;
};


/* fields */

typedef struct pb_NameEntry {
    struct pb_NameEntry *next;
    unsigned       hash     : 32;
    unsigned       length   : 16;
    unsigned       refcount : 16;
} pb_NameEntry;

typedef struct pb_NameTable {
    size_t         size;
    size_t         count;
    pb_NameEntry **hash;
} pb_NameTable;

struct pb_State {
    pb_Table     types;
    pb_NameTable nametable;
    pb_Pool      typepool;
    pb_Pool      fieldpool;
};

struct pb_Field {
    pb_Name *name;
    pb_Type *type;
    pb_Name *default_value;
    int32_t  number;
    unsigned oneof_idx : 24;
    unsigned type_id   : 5; /* PB_T* enum */
    unsigned repeated  : 1;
    unsigned packed    : 1;
    unsigned scalar    : 1;
};

struct pb_Type {
    pb_Name    *name;
    const char *basename;
    pb_Table    field_tags;
    pb_Table    field_names;
    pb_Table    oneof_index;
    unsigned    field_count : 28;
    unsigned    is_enum   : 1;
    unsigned    is_map    : 1;
    unsigned    is_proto3 : 1;
    unsigned    is_dead   : 1;
};


PB_NS_END

#endif /* pb_h */


#if defined(PB_IMPLEMENTATION) && !defined(pb_implemented)
#define pb_implemented

#define PB_MAX_SIZET          ((size_t)~0 - 100)
#define PB_MIN_STRTABLE_SIZE  16
#define PB_MIN_HASHTABLE_SIZE 8
#define PB_HASHLIMIT          5

#include <assert.h>
#include <stdlib.h>
#include <string.h>

PB_NS_BEGIN


/* conversions */

PB_API uint32_t pb_encode_sint32(int32_t value)
{ return ((uint32_t)value << 1) ^ -(value < 0); }

PB_API int32_t pb_decode_sint32(uint32_t value)
{ return (value >> 1) ^ -(int32_t)(value & 1); }

PB_API uint64_t pb_encode_sint64(int64_t value)
{ return ((uint64_t)value << 1) ^ -(value < 0); }

PB_API int64_t pb_decode_sint64(uint64_t value)
{ return (value >> 1) ^ -(int64_t)(value & 1); }

PB_API uint64_t pb_expandsig(uint32_t value)
{ uint64_t m = (uint64_t)1U << 31; return (value ^ m) - m; }

PB_API uint32_t pb_encode_float(float value)
{ union { uint32_t u32; float f; } u; u.f = value; return u.u32; }

PB_API float pb_decode_float(uint32_t value)
{ union { uint32_t u32; float f; } u; u.u32 = value; return u.f; }

PB_API uint64_t pb_encode_double(double value)
{ union { uint64_t u64; double d; } u; u.d = value; return u.u64; }

PB_API double pb_decode_double(uint64_t value)
{ union { uint64_t u64; double d; } u; u.u64 = value; return u.d; }


/* decode */

PB_API pb_Slice pb_slice(const char *s)
{ return pb_lslice(s, strlen(s)); }

PB_API pb_Slice pb_lslice(const char *s, size_t len)
{ pb_Slice slice; slice.p = s; slice.end = s + len; return slice; }

PB_API size_t pb_len(pb_Slice s)
{ return s.end - s.p; }

static size_t pb_readvarint_slow(pb_Slice *s, uint64_t *pv) {
    const char *p = s->p;
    uint64_t n = 0;
    int i = 0;
    while (s->p < s->end && i < 10) {
        int b = *s->p++;
        n |= ((uint64_t)b & 0x7F) << (7*i++);
        if ((b & 0x80) == 0) {
            *pv = n;
            return i;
        }
    }
    s->p = p;
    return 0;
}

static size_t pb_readvarint32_fallback(pb_Slice *s, uint32_t *pv) {
    const uint8_t *p = (const uint8_t*)s->p, *o = p;
    uint32_t b, n;
    for (;;) {
        n = *p++ - 0x80, n += (b = *p++) <<  7; if (!(b & 0x80)) break;
        n -= 0x80 <<  7, n += (b = *p++) << 14; if (!(b & 0x80)) break;
        n -= 0x80 << 14, n += (b = *p++) << 21; if (!(b & 0x80)) break;
        n -= 0x80 << 21, n += (b = *p++) << 28; if (!(b & 0x80)) break;
        /* n -= 0x80 << 28; */
        if (!(*p++ & 0x80)) break;
        if (!(*p++ & 0x80)) break;
        if (!(*p++ & 0x80)) break;
        if (!(*p++ & 0x80)) break;
        if (!(*p++ & 0x80)) break;
        return 0;
    }
    *pv = n;
    s->p = (const char*)p;
    return p - o;
}

static size_t pb_readvarint64_fallback(pb_Slice *s, uint64_t *pv) {
    const uint8_t *p = (const uint8_t*)s->p, *o = p;
    uint32_t b, n1, n2 = 0, n3 = 0;
    for (;;) {
        n1 = *p++ - 0x80, n1 += (b = *p++) <<  7; if (!(b & 0x80)) break;
        n1 -= 0x80 <<  7, n1 += (b = *p++) << 14; if (!(b & 0x80)) break;
        n1 -= 0x80 << 14, n1 += (b = *p++) << 21; if (!(b & 0x80)) break;
        n1 -= 0x80 << 21, n2 += (b = *p++)      ; if (!(b & 0x80)) break;
        n2 -= 0x80      , n2 += (b = *p++) <<  7; if (!(b & 0x80)) break;
        n2 -= 0x80 <<  7, n2 += (b = *p++) << 14; if (!(b & 0x80)) break;
        n2 -= 0x80 << 14, n2 += (b = *p++) << 21; if (!(b & 0x80)) break;
        n2 -= 0x80 << 21, n3 += (b = *p++)      ; if (!(b & 0x80)) break;
        n3 -= 0x80      , n3 += (b = *p++) <<  7; if (!(b & 0x80)) break;
        return 0;
    }
    *pv = n1 | ((uint64_t)n2 << 28) | ((uint64_t)n3 << 56);
    s->p = (const char*)p;
    return p - o;
}

PB_API size_t pb_readvarint32(pb_Slice *s, uint32_t *pv) {
    uint64_t u64;
    size_t ret;
    if (s->p >= s->end)  return 0;
    if (!(*s->p & 0x80)) { *pv = *s->p++; return 1; }
    if (pb_len(*s) >= 10 || !(s->end[-1] & 0x80))
        return pb_readvarint32_fallback(s, pv);
    if ((ret = pb_readvarint_slow(s, &u64)) != 0)
        *pv = (uint32_t)u64;
    return ret;
}

PB_API size_t pb_readvarint64(pb_Slice *s, uint64_t *pv) {
    if (s->p >= s->end)  return 0;
    if (!(*s->p & 0x80)) { *pv = *s->p++; return 1; }
    if (pb_len(*s) >= 10 || !(s->end[-1] & 0x80))
        return pb_readvarint64_fallback(s, pv);
    return pb_readvarint_slow(s, pv);
}

PB_API size_t pb_readfixed32(pb_Slice *s, uint32_t *pv) {
    int i;
    uint32_t n = 0;
    if (s->p + 4 > s->end)
        return 0;
    for (i = 3; i >= 0; --i) {
        n <<= 8;
        n |= s->p[i] & 0xFF;
    }
    s->p += 4;
    *pv = n;
    return 4;
}

PB_API size_t pb_readfixed64(pb_Slice *s, uint64_t *pv) {
    int i;
    uint64_t n = 0;
    if (s->p + 8 > s->end)
        return 0;
    for (i = 7; i >= 0; --i) {
        n <<= 8;
        n |= s->p[i] & 0xFF;
    }
    s->p += 8;
    *pv = n;
    return 8;
}

PB_API size_t pb_readslice(pb_Slice *s, size_t len, pb_Slice *pv) {
    if (pb_len(*s) < len)
        return 0;
    pv->p   = s->p;
    pv->end = s->p + len;
    s->p = pv->end;
    return len;
}

PB_API size_t pb_readbytes(pb_Slice *s, pb_Slice *pv) {
    const char *p = s->p;
    uint64_t len;
    if (pb_readvarint64(s, &len) == 0 || pb_len(*s) < len) {
        s->p = p;
        return 0;
    }
    pv->p   = s->p;
    pv->end = s->p + len;
    s->p = pv->end;
    return s->p - p;
}

PB_API size_t pb_readgroup(pb_Slice *s, uint32_t tag, pb_Slice *pv) {
    const char *p = s->p;
    uint32_t newtag = 0;
    size_t count;
    assert(pb_gettype(tag) == PB_TGSTART);
    while ((count = pb_readvarint32(s, &newtag)) != 0) {
        if (pb_gettype(newtag) == PB_TGEND) {
            if (pb_gettag(newtag) != pb_gettag(tag))
                break;
            pv->p = p;
            pv->end = s->p - count;
            return s->p - p;
        }
        pb_skipvalue(s, newtag);
    }
    s->p = p;
    return 0;
}

PB_API size_t pb_skipvalue(pb_Slice *s, uint32_t tag) {
    const char *p = s->p;
    size_t ret = 0;
    pb_Slice data;
    switch (pb_gettype(tag)) {
    default: break;
    case PB_TVARINT: ret = pb_skipvarint(s); break;
    case PB_T64BIT:  ret = pb_skipslice(s, 8); break;
    case PB_TBYTES:  ret = pb_skipbytes(s); break;
    case PB_T32BIT:  ret = pb_skipslice(s, 4); break;
    case PB_TGSTART: ret = pb_readgroup(s, tag, &data); break;
    }
    if (!ret) s->p = p;
    return ret;
}

PB_API size_t pb_skipvarint(pb_Slice *s) {
    const char *p = s->p, *op = p;
    while (p < s->end && (*p & 0x80) != 0) ++p;
    if (p >= s->end) return 0;
    s->p = ++p;
    return p - op;
}

PB_API size_t pb_skipbytes(pb_Slice *s) {
    const char *p = s->p;
    uint64_t var;
    if (!pb_readvarint64(s, &var)) return 0;
    if (pb_len(*s) < var) {
        s->p = p;
        return 0;
    }
    s->p += var;
    return s->p - p;
}

PB_API size_t pb_skipslice(pb_Slice *s, size_t len) {
    if (s->p + len > s->end) return 0;
    s->p += len;
    return len;
}

PB_API int pb_wtypebytype(int type) {
    switch (type) {
    case PB_Tdouble:    return PB_T64BIT;
    case PB_Tfloat:     return PB_T32BIT;
    case PB_Tint64:     return PB_TVARINT;
    case PB_Tuint64:    return PB_TVARINT;
    case PB_Tint32:     return PB_TVARINT;
    case PB_Tfixed64:   return PB_T64BIT;
    case PB_Tfixed32:   return PB_T32BIT;
    case PB_Tbool:      return PB_TVARINT;
    case PB_Tstring:    return PB_TBYTES;
    case PB_Tmessage:   return PB_TBYTES;
    case PB_Tbytes:     return PB_TBYTES;
    case PB_Tuint32:    return PB_TVARINT;
    case PB_Tenum:      return PB_TVARINT;
    case PB_Tsfixed32:  return PB_T32BIT;
    case PB_Tsfixed64:  return PB_T64BIT;
    case PB_Tsint32:    return PB_TVARINT;
    case PB_Tsint64:    return PB_TVARINT;
    default:            return PB_TWIRECOUNT;
    }
}

PB_API const char *pb_wtypename(int wiretype, const char *def) {
    switch (wiretype) {
#define X(id, name, v) case v: return name;
        PB_WIRETYPES(X)
#undef  X
    default: return def ? def : "<unknown>";
    }
}

PB_API const char *pb_typename(int type, const char *def) {
    switch (type) {
#define X(name, t, v) case v: return #name;
        PB_TYPES(X)
#undef  X
    default: return def ? def : "<unknown>";
    }
}

PB_API int pb_typebyname(const char *name, int def) {
    static struct entry { const char *name; int value; } names[] = {
#define X(name, t, v) { #name, v },
        PB_TYPES(X)
#undef  X
        { NULL, 0 }
    };
    struct entry *p;
    for (p = names; p->name != NULL; ++p)
        if (strcmp(p->name, name) == 0)
            return p->value;
    return def;
}

PB_API int pb_wtypebyname(const char *name, int def) {
    static struct entry { const char *name; int value; } names[] = {
#define X(id, name, v) { name, v },
        PB_WIRETYPES(X)
#undef  X
        { NULL, 0 }
    };
    struct entry *p;
    for (p = names; p->name != NULL; ++p)
        if (strcmp(p->name, name) == 0)
            return p->value;
    return def;
}


/* encode */

PB_API pb_Slice pb_result(pb_Buffer *b)
{ pb_Slice slice = pb_lslice(b->buff, b->size); return slice; }

PB_API void pb_initbuffer(pb_Buffer *b)
{ b->buff = b->init_buff, b->capacity = PB_BUFFERSIZE, b->size = 0; }

PB_API void pb_resetbuffer(pb_Buffer *b)
{ if (b->buff != b->init_buff) free(b->buff); pb_initbuffer(b); }

static int pb_write32(char *buff, uint32_t n) {
    int p, c = 0;
    do {
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n;
    } while (0);
    return *buff++ = p, ++c;
}

static int pb_write64(char *buff, uint64_t n) {
    int p, c = 0;
    do {
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F; if ((n >>= 7) == 0) break; *buff++ = p | 0x80, ++c;
        p = n & 0x7F;
    } while (0);
    return *buff++ = p, ++c;
}

PB_API size_t pb_resizebuffer(pb_Buffer *b, size_t len) {
    size_t newsize = PB_BUFFERSIZE;
    while (newsize < PB_MAX_SIZET/2 && newsize < len)
        newsize += newsize >> 1;
    if (newsize > b->size) {
        char *buff = b->buff == b->init_buff ? NULL : b->buff;
        char *newbuff = (char*)realloc(buff, newsize);
        if (newbuff == NULL) return 0;
        if (b->buff == b->init_buff) memcpy(newbuff, b->buff, b->size);
        b->buff     = newbuff;
        b->capacity = newsize;
    }
    return b->capacity;
}

PB_API void* pb_prepbuffsize(pb_Buffer *b, size_t len) {
    if (b->size + len > b->capacity) {
        size_t oldsize = b->size;
        if (pb_resizebuffer(b, oldsize + len) == 0)
            return NULL;
    }
    return &b->buff[b->size];
}

PB_API size_t pb_addslice(pb_Buffer *b, pb_Slice s) {
    size_t len = pb_len(s);
    void *buff = pb_prepbuffsize(b, len);
    if (buff == NULL) return 0;
    memcpy(buff, s.p, len);
	pb_addsize(b, len);
	return len;
}

PB_API size_t pb_addlength(pb_Buffer *b, size_t len) {
    char buff[10], *s;
    size_t bl, ml;
    if ((bl = pb_bufflen(b)) < len)
        return 0;
    ml = pb_write64(buff, bl - len);
    if (pb_prepbuffsize(b, ml) == 0) return 0;
    s = b->buff + len;
    memmove(s+ml, s, bl - len);
    memcpy(s, buff, ml);
    pb_addsize(b, ml);
    return ml;
}

PB_API size_t pb_addbytes(pb_Buffer *b, pb_Slice s) {
    size_t ret, len = pb_len(s);
    if (pb_prepbuffsize(b, len+5) == NULL) return 0;
    ret = pb_addvarint32(b, (uint32_t)len);
    return ret + pb_addslice(b, s);
}

PB_API size_t pb_addvarint32(pb_Buffer *b, uint32_t n) {
    char *buff = (char*)pb_prepbuffsize(b, 5);
	size_t l;
    if (buff == NULL) return 0;
	pb_addsize(b, l = pb_write32(buff, n));
	return l;
}

PB_API size_t pb_addvarint64(pb_Buffer *b, uint64_t n) {
    char *buff = (char*)pb_prepbuffsize(b, 10);
	size_t l;
    if (buff == NULL) return 0;
	pb_addsize(b, l = pb_write64(buff, n));
	return l;
}

PB_API size_t pb_addfixed32(pb_Buffer *b, uint32_t n) {
    char *ch = (char*)pb_prepbuffsize(b, 4);
    if (ch == NULL) return 0;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch   = n & 0xFF;
	pb_addsize(b, 4);
	return 4;
}

PB_API size_t pb_addfixed64(pb_Buffer *b, uint64_t n) {
    char *ch = (char*)pb_prepbuffsize(b, 8);
    if (ch == NULL) return 0;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch++ = n & 0xFF; n >>= 8;
    *ch   = n & 0xFF;
	pb_addsize(b, 8);
	return 8;
}


/* memory pool */

PB_API void pb_initpool(pb_Pool *pool, size_t obj_size) {
    memset(pool, 0, sizeof(pb_Pool));
    pool->obj_size = obj_size;
    assert(obj_size > sizeof(void*) && obj_size < PB_POOLSIZE/4);
}

PB_API void pb_freepool(pb_Pool *pool) {
    void *page = pool->pages;
    while (page) {
        void *next = *(void**)((char*)page + PB_POOLSIZE - sizeof(void*));
        free(page);
        page = next;
    }
    pb_initpool(pool, pool->obj_size);
}

PB_API void *pb_poolalloc(pb_Pool *pool) {
    void *obj = pool->freed;
    if (obj == NULL) {
        size_t objsize = pool->obj_size, offset;
        void *newpage = malloc(PB_POOLSIZE);
        if (newpage == NULL) return NULL;
        offset = ((PB_POOLSIZE - sizeof(void*)) / objsize - 1) * objsize;
        for (; offset > 0; offset -= objsize) {
            void **entry = (void**)((char*)newpage + offset);
            *entry = pool->freed, pool->freed = (void*)entry;
        }
        *(void**)((char*)newpage + PB_POOLSIZE - sizeof(void*)) = pool->pages;
        pool->pages = newpage;
        return newpage;
    }
    pool->freed = *(void**)obj;
    return obj;
}

PB_API void pb_poolfree(pb_Pool *pool, void *obj)
{ *(void**)obj = pool->freed, pool->freed = obj; }


/* hash table */

#define pbT_offset(a, b) ((char*)(a) - (char*)(b))
#define pbT_index(a, b)  ((pb_Entry*)((char*)(a) + (b)))

PB_API void pb_inittable(pb_Table *t, size_t entrysize)
{ memset(t, 0, sizeof(pb_Table)), t->entry_size = (unsigned)entrysize; }

PB_API void pb_freetable(pb_Table *t)
{ free(t->hash); pb_inittable(t, t->entry_size); }

static pb_Entry *pbT_hash(pb_Table *t, pb_Key key) {
    size_t h = ((size_t)key*2654435761U)&(t->size-1);
    if (key && h == 0) h = 1;
    return pbT_index(t->hash, h*t->entry_size);
}

static pb_Entry *pbT_newkey(pb_Table *t, pb_Key key) {
    pb_Entry *mp, *othern, *next, *f = NULL;
    if (t->size == 0 && pb_resizetable(t, t->size*2) == 0) return NULL;
    if (key == 0) {
        mp = t->hash;
        t->has_zero = 1;
    }
    else if ((mp = pbT_hash(t, key))->key != 0) {
        while (t->lastfree > t->entry_size) {
            pb_Entry *cur = pbT_index(t->hash, t->lastfree -= t->entry_size);
            if (cur->key == 0 && cur->next == 0) { f = cur; break; }
        }
        if (f == NULL) return pb_resizetable(t, t->size*2) ?
            pbT_newkey(t, key) : NULL;
        if ((othern = pbT_hash(t, mp->key)) != mp) {
            while ((next = pbT_index(othern, othern->next)) != mp)
                othern = next;
            othern->next = pbT_offset(f, othern);
            memcpy(f, mp, t->entry_size);
            if (mp->next != 0) f->next += pbT_offset(mp, f), mp->next = 0;
        }
        else {
            if (mp->next != 0) f->next = pbT_offset(mp, f) + mp->next;
            else assert(f->next == 0);
            mp->next = pbT_offset(f, mp);
            mp = f;
        }
    }
    mp->key = key;
    if (t->entry_size != sizeof(pb_Entry))
        memset(mp+1, 0, t->entry_size - sizeof(pb_Entry));
    return mp;
}

PB_API size_t pb_resizetable(pb_Table *t, size_t size) {
    pb_Table nt = *t;
    size_t i, rawsize = t->size*t->entry_size;
    size_t newsize = PB_MIN_HASHTABLE_SIZE;
    while (newsize < PB_MAX_SIZET/t->entry_size && newsize < size)
        newsize <<= 1;
    if (newsize < size) return 0;
    nt.size     = newsize;
    nt.lastfree = nt.entry_size * newsize;
    nt.hash     = (pb_Entry*)malloc(nt.lastfree);
    if (nt.hash == NULL) return 0;
    memset(nt.hash, 0, nt.lastfree);
    for (i = 0; i < rawsize; i += t->entry_size) {
        pb_Entry *olde = (pb_Entry*)((char*)t->hash + i);
        pb_Entry *newe = pbT_newkey(&nt, olde->key);
        if (nt.entry_size > sizeof(pb_Entry))
            memcpy(newe+1, olde+1, nt.entry_size - sizeof(pb_Entry));
    }
    free(t->hash);
    *t = nt;
    return newsize;
}

PB_API pb_Entry *pb_gettable(pb_Table *t, pb_Key key) {
    pb_Entry *entry;
    if (t == NULL || t->size == 0)
        return NULL;
    if (key == 0)
        return t->has_zero ? t->hash : NULL;
    for (entry = pbT_hash(t, key);
            entry->key != key;
            entry = pbT_index(entry, entry->next))
        if (entry->next == 0) return NULL;
    return entry;
}

PB_API pb_Entry *pb_settable(pb_Table *t, pb_Key key) {
    pb_Entry *entry;
    if ((entry = pb_gettable(t, key)) != NULL)
        return entry;
    return pbT_newkey(t, key);
}

PB_API int pb_nextentry(pb_Table *t, pb_Entry **pentry) {
    size_t i = *pentry ? pbT_offset(*pentry, t->hash) : 0;
    size_t size = t->size*t->entry_size;
    if (*pentry == NULL && t->has_zero) {
        *pentry = t->hash;
        return 1;
    }
    while (i += t->entry_size, i < size) {
        pb_Entry *entry = pbT_index(t->hash, i);
        if (entry->key != 0) {
            *pentry = entry;
            return 1;
        }
    }
    *pentry = NULL;
    return 0;
}


/* name table */

static void pbN_init(pb_State *S)
{ memset(&S->nametable, 0, sizeof(pb_NameTable)); }

static void pbN_free(pb_State *S) {
    pb_NameTable *nt = &S->nametable;
    size_t i;
    for (i = 0; i < nt->size; ++i) {
        pb_NameEntry *ne = nt->hash[i];
        while (ne != NULL) {
            pb_NameEntry *next = ne->next;
            free(ne);
            ne = next;
        }
    }
    free(nt->hash);
    pbN_init(S);
}

static unsigned pbN_calchash(const char *s, size_t len) {
    unsigned h = (unsigned)len;
    size_t step = (len >> PB_HASHLIMIT) + 1;
    for (; len >= step; len -= step)
        h ^= ((h<<5) + (h>>2) + (unsigned char)(s[len - 1]));
    return h;
}

static size_t pbN_resize(pb_State *S, size_t size) {
    pb_NameTable *nt = &S->nametable;
    pb_NameEntry **hash;
    size_t i, newsize = PB_MIN_STRTABLE_SIZE;
    while (newsize < PB_MAX_SIZET/sizeof(pb_NameEntry*) && newsize < size)
        newsize <<= 1;
    if (newsize < size) return 0;
    hash = (pb_NameEntry**)malloc(newsize * sizeof(pb_NameEntry*));
    if (hash == NULL) return 0;
    memset(hash, 0, newsize * sizeof(pb_NameEntry*));
    for (i = 0; i < nt->size; ++i) {
        pb_NameEntry *entry = nt->hash[i];
        while (entry != NULL) {
            pb_NameEntry *next = entry->next;
            pb_NameEntry **newh = &hash[entry->hash & (newsize - 1)];
            entry->next = *newh, *newh = entry;
            entry = next;
        }
    }
    free(nt->hash);
    nt->hash = hash;
    nt->size = newsize;
    return newsize;
}

static pb_NameEntry *pbN_newname(pb_State *S, const char *name, size_t len, unsigned hash) {
    pb_NameTable *nt = &S->nametable;
    pb_NameEntry **list, *newobj;
    if (nt->count >= nt->size && !pbN_resize(S, nt->size * 2)) return NULL;
    list = &nt->hash[hash & (nt->size - 1)];
    newobj = (pb_NameEntry*)malloc(sizeof(pb_NameEntry) + len + 1);
    if (newobj == NULL) return NULL;
    newobj->next = *list;
    newobj->length = (unsigned)len;
    newobj->refcount = 1;
    newobj->hash = hash;
    memcpy(newobj+1, name, len);
    ((char*)(newobj+1))[len] = '\0';
    *list = newobj;
    ++nt->count;
    return newobj;
}

static void pbN_delname(pb_State *S, pb_NameEntry *name) {
    pb_NameTable *nt = &S->nametable;
    pb_NameEntry **list = &nt->hash[name->hash & (nt->size - 1)];
    while (*list != NULL) {
        if (*list != name)
            list = &(*list)->next;
        else {
            *list = (*list)->next;
            free(name);
            --nt->count;
            break;
        }
    }
}

static pb_NameEntry *pbN_getname(pb_State *S, const char *name, size_t len, unsigned hash) {
    pb_NameTable *nt = &S->nametable;
    if (nt->hash) {
        pb_NameEntry *entry = nt->hash[hash & (nt->size - 1)];
        for (; entry != NULL; entry = entry->next)
            if (entry->hash == hash && entry->length == len
                    && memcmp(name, entry + 1, len) == 0)
                return entry;
    }
    return NULL;
}

PB_API pb_Name *pb_newname(pb_State *S, pb_Slice s) {
    if (s.p != NULL) {
        size_t size = pb_len(s);
        const char *name = s.p;
        unsigned hash = pbN_calchash(name, size);
        pb_NameEntry *entry = pbN_getname(S, name, size, hash);
        if (entry) return pb_usename((pb_Name*)(entry + 1));
        entry = pbN_newname(S, name, size, hash);
        return entry ? (pb_Name*)(entry + 1) : NULL;
    }
    return NULL;
}

PB_API pb_Name *pb_usename(pb_Name *name) {
    if (name != NULL)
        ++((pb_NameEntry*)name-1)->refcount;
    return name;
}

PB_API void pb_delname(pb_State *S, pb_Name *name) {
    if (name != NULL) {
        pb_NameEntry *ne = (pb_NameEntry*)name - 1;
        if (ne->refcount <= 1)
        { pbN_delname(S, ne); return; }
        --ne->refcount;
    }
}

PB_API pb_Name *pb_name(pb_State *S, const char *name) {
    if (name != NULL) {
        size_t size = strlen(name);
        unsigned hash = pbN_calchash(name, size);
        pb_NameEntry *entry = pbN_getname(S, name, size, hash);
        return entry ? (pb_Name*)(entry + 1) : NULL;
    }
    return NULL;
}


/* state */

typedef struct pb_TypeEntry { pb_Entry entry; pb_Type *value; } pb_TypeEntry;
typedef struct pb_FieldEntry { pb_Entry entry; pb_Field *value; } pb_FieldEntry;

typedef struct pb_OneofEntry {
    pb_Entry entry;
    pb_Name *name;
    unsigned index;
} pb_OneofEntry;

PB_API void pb_init(pb_State *S) {
    memset(S, 0, sizeof(pb_State));
    S->types.entry_size = sizeof(pb_TypeEntry);
    pb_initpool(&S->typepool, sizeof(pb_Type));
    pb_initpool(&S->fieldpool, sizeof(pb_Field));
}

PB_API void pb_free(pb_State *S) {
    if (S != NULL) {
        pb_TypeEntry *te = NULL;
        while (pb_nextentry(&S->types, (pb_Entry**)&te))
            if (te->value != NULL) pb_deltype(S, te->value);
        pb_freetable(&S->types);
        pb_freepool(&S->typepool);
        pb_freepool(&S->fieldpool);
        pbN_free(S);
    }
}

PB_API pb_Type *pb_type(pb_State *S, pb_Name *tname) {
    pb_TypeEntry *te = NULL;
    if (S != NULL && tname != NULL)
        te = (pb_TypeEntry*)pb_gettable(&S->types, (pb_Key)tname);
    return te && !te->value->is_dead ? te->value : NULL;
}

PB_API pb_Field *pb_fname(pb_Type *t, pb_Name *name) {
    pb_FieldEntry *fe = NULL;
    if (t != NULL && name != NULL)
        fe = (pb_FieldEntry*)pb_gettable(&t->field_names, (pb_Key)name);
    return fe ? fe->value : NULL;
}

PB_API pb_Field *pb_field(pb_Type *t, int32_t number) {
    pb_FieldEntry *fe = NULL;
    if (t != NULL) fe = (pb_FieldEntry*)pb_gettable(&t->field_tags, number);
    return fe ? fe->value : NULL;
}

PB_API pb_Name *pb_oneofname(pb_Type *t, int idx) {
    pb_OneofEntry *oe = NULL;
    if (t != NULL) oe = (pb_OneofEntry*)pb_gettable(&t->oneof_index, idx);
    return oe ? oe->name : NULL;
}

PB_API int pb_nexttype(pb_State *S, pb_Type **ptype) {
    pb_TypeEntry *e = NULL;
    if (S != NULL) {
        if (*ptype != NULL)
            e = (pb_TypeEntry*)pb_gettable(&S->types, (pb_Key)(*ptype)->name);
        while (pb_nextentry(&S->types, (pb_Entry**)&e))
            if ((*ptype = e->value) != NULL && !(*ptype)->is_dead)
                return 1;
    }
    *ptype = NULL;
    return 0;
}

PB_API int pb_nextfield(pb_Type *t, pb_Field **pfield) {
    pb_FieldEntry *e = NULL;
    if (t != NULL) {
        if (*pfield != NULL)
            e = (pb_FieldEntry*)pb_gettable(&t->field_tags, (*pfield)->number);
        while (pb_nextentry(&t->field_tags, (pb_Entry**)&e))
            if ((*pfield = e->value) != NULL)
                return 1;
    }
    *pfield = NULL;
    return 0;
}


/* new type/field */

static const char *pbT_basename(const char *tname) {
    const char *end = tname + strlen(tname);
    while (tname < end && *--end != '.')
        ;
    return *end != '.' ? end : end + 1;
}

static void pbT_inittype(pb_Type *t) {
    memset(t, 0, sizeof(pb_Type));
    pb_inittable(&t->field_names, sizeof(pb_FieldEntry));
    pb_inittable(&t->field_tags, sizeof(pb_FieldEntry));
    pb_inittable(&t->oneof_index, sizeof(pb_OneofEntry));
}

static void pbT_freefield(pb_State *S, pb_Field *f) {
    pb_delname(S, f->default_value);
    pb_delname(S, f->name);
    pb_poolfree(&S->fieldpool, f);
}

PB_API pb_Type *pb_newtype(pb_State *S, pb_Name *tname) {
    if (tname != NULL) {
        pb_TypeEntry *te = (pb_TypeEntry*)pb_settable(
                &S->types, (pb_Key)tname);
        pb_Type *t;
        if (te == NULL) return NULL;
        if ((t = te->value) != NULL) {
            t->is_dead = 0;
            return t;
        }
        if (!(t = (pb_Type*)pb_poolalloc(&S->typepool))) return NULL;
        pbT_inittype(t);
        t->name = tname;
        t->basename = pbT_basename((const char*)tname);
        return te->value = t;
    }
    return NULL;
}

PB_API void pb_deltype(pb_State *S, pb_Type *t) {
    if (S && t) {
        pb_FieldEntry *nf = NULL;
        pb_OneofEntry *ne = NULL;
        while (pb_nextentry(&t->field_names, (pb_Entry**)&nf)) {
            if (nf->value != NULL) {
                pb_FieldEntry *of = (pb_FieldEntry*)pb_gettable(
                        &t->field_tags, nf->value->number);
                if (of && of->value == nf->value)
                    of->entry.key = 0, of->value = NULL;
                pbT_freefield(S, nf->value);
            }
        }
        while (pb_nextentry(&t->field_tags, (pb_Entry**)&nf))
            if (nf->value != NULL) pbT_freefield(S, nf->value);
        while (pb_nextentry(&t->oneof_index, (pb_Entry**)&ne))
            pb_delname(S, ne->name);
        pb_freetable(&t->field_tags);
        pb_freetable(&t->field_names);
        pb_freetable(&t->oneof_index);
        t->field_count = 0;
        t->is_dead = 1;
        /*pb_delname(S, t->name); */
        /*pb_poolfree(&S->typepool, t); */
    }
}

PB_API pb_Field *pb_newfield(pb_State *S, pb_Type *t, pb_Name *fname, int32_t number) {
    if (fname != NULL) {
        pb_FieldEntry *nf = (pb_FieldEntry*)pb_settable(
                &t->field_names, (pb_Key)fname);
        pb_FieldEntry *tf = (pb_FieldEntry*)pb_settable(
                &t->field_tags, number);
        pb_Field *f;
        if (nf == NULL || tf == NULL) return NULL;
        if ((f = nf->value) != NULL && tf->value == f) {
            pb_delname(S, f->default_value);
            f->default_value = NULL;
            return f;
        }
        if (!(f = (pb_Field*)pb_poolalloc(&S->fieldpool))) return NULL;
        memset(f, 0, sizeof(pb_Field));
        f->name   = fname;
        f->type   = t;
        f->number = number;
        if (nf->value && pb_field(t, nf->value->number) != nf->value)
            pbT_freefield(S, nf->value), --t->field_count;
        if (tf->value && pb_fname(t, tf->value->name) != tf->value)
            pbT_freefield(S, tf->value), --t->field_count;
        ++t->field_count;
        return nf->value = tf->value = f;
    }
    return NULL;
}

PB_API void pb_delfield(pb_State *S, pb_Type *t, pb_Field *f) {
    if (S && t && f) {
        pb_FieldEntry *nf = (pb_FieldEntry*)pb_gettable(&t->field_names,
                (pb_Key)f->name);
        pb_FieldEntry *tf = (pb_FieldEntry*)pb_gettable(&t->field_tags,
                (pb_Key)f->number);
        int count = 0;
        if (nf && nf->value == f) nf->entry.key = 0, nf->value = NULL, ++count;
        if (tf && tf->value == f) tf->entry.key = 0, tf->value = NULL, ++count;
        if (count) pbT_freefield(S, f), --t->field_count;
    }
}


/* .pb proto loader */

#include <setjmp.h>

typedef struct pb_Loader         pb_Loader;
typedef struct pbL_FieldInfo     pbL_FieldInfo;
typedef struct pbL_EnumValueInfo pbL_EnumValueInfo;
typedef struct pbL_EnumInfo      pbL_EnumInfo;
typedef struct pbL_TypeInfo      pbL_TypeInfo;
typedef struct pbL_FileInfo      pbL_FileInfo;

#define pbL_rawh(A)   ((size_t*)(A) - 2)
#define pbL_size(A)   ((A) ? pbL_rawh(A)[0] : 0)
#define pbL_count(A)  ((A) ? pbL_rawh(A)[1] : 0)
#define pbL_add(A)    (pbL_grow(L, (void**)&(A), sizeof(*(A))), &(A)[pbL_rawh(A)[1]++])
#define pbL_delete(A) ((A) ? (void)free(pbL_rawh(A)) : (void)0)

struct pb_Loader {
    jmp_buf   jbuf;
    pb_Slice  s;
    pb_Buffer b;
    int       is_proto3;
};

/* parsers */

struct pbL_EnumValueInfo {
    pb_Slice name;
    int32_t  number;
};

struct pbL_EnumInfo {
    pb_Slice           name;
    pbL_EnumValueInfo *value;
};

struct pbL_FieldInfo {
    pb_Slice name;
    pb_Slice type_name;
    pb_Slice extendee;
    pb_Slice default_value;
    int32_t  number;
    int32_t  label;
    int32_t  type;
    int32_t  oneof_index;
    int32_t  packed;
};

struct pbL_TypeInfo {
    pb_Slice       name;
    int32_t        is_map;
    pbL_FieldInfo *field;
    pbL_FieldInfo *extension;
    pbL_EnumInfo  *enum_type;
    pbL_TypeInfo  *nested_type;
    pb_Slice      *oneof_decl;
};

struct pbL_FileInfo {
    pb_Slice       package;
    pb_Slice       syntax;
    pbL_EnumInfo  *enum_type;
    pbL_TypeInfo  *message_type;
    pbL_FieldInfo *extension;
};

static void pbL_readbytes(pb_Loader *L, pb_Slice *pv)
{ if (pb_readbytes(&L->s, pv) == 0) longjmp(L->jbuf, 1); }

static void pbL_beginmsg(pb_Loader *L, pb_Slice *pv)
{ pb_Slice v; pbL_readbytes(L, &v); *pv = L->s, L->s = v; }

static void pbL_endmsg(pb_Loader *L, pb_Slice *pv)
{ L->s = *pv; }

static void pbL_readint32(pb_Loader *L, int32_t *pv) {
    uint32_t v;
    if (pb_readvarint32(&L->s, &v) == 0)
        longjmp(L->jbuf, 1);
    *pv = (int32_t)v;
}

static void pbL_grow(pb_Loader *L, void **pp, size_t obj_size) {
    enum { SIZE, COUNT, FIELDS };
    size_t *h = *pp ? pbL_rawh(*pp) : NULL;
    if (h == NULL || h[SIZE] <= h[COUNT]) {
        size_t used = (h ? h[COUNT] : 0);
		size_t size = (h ? h[SIZE] : 2), newsize = size + (size >> 1);
		size_t *nh  = (size_t*)realloc(h, sizeof(size_t)*FIELDS + newsize*obj_size);
        if (nh == NULL || newsize < size) longjmp(L->jbuf, PB_ENOMEM);
        nh[SIZE]  = newsize;
        nh[COUNT] = used;
        *pp = nh + FIELDS;
        memset((char*)*pp + used*obj_size, 0, (newsize - used)*obj_size);
    }
}

static void pbL_FieldOptions(pb_Loader *L, pbL_FieldInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(2, PB_TVARINT): /* bool packed */
            pbL_readint32(L, &info->packed); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_FieldDescriptorProto(pb_Loader *L, pbL_FieldInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    info->packed = -1;
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(1, PB_TBYTES): /* string name */
            pbL_readbytes(L, &info->name); break;
        case pb_pair(3, PB_TVARINT): /* int32 number */
            pbL_readint32(L, &info->number); break;
        case pb_pair(4, PB_TVARINT): /* Label label */
            pbL_readint32(L, &info->label); break;
        case pb_pair(5, PB_TVARINT): /* Type type */
            pbL_readint32(L, &info->type); break;
        case pb_pair(6, PB_TBYTES): /* string type_name */
            pbL_readbytes(L, &info->type_name); break;
        case pb_pair(2, PB_TBYTES): /* string extendee */
            pbL_readbytes(L, &info->extendee); break;
        case pb_pair(7, PB_TBYTES): /* string default_value */
            pbL_readbytes(L, &info->default_value); break;
        case pb_pair(8, PB_TBYTES): /* FieldOptions options */
            pbL_FieldOptions(L, info); break;
        case pb_pair(9, PB_TVARINT): /* int32 oneof_index */
            pbL_readint32(L, &info->oneof_index);
            ++info->oneof_index; break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_EnumValueDescriptorProto(pb_Loader *L, pbL_EnumValueInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(1, PB_TBYTES): /* string name */
            pbL_readbytes(L, &info->name); break;
        case pb_pair(2, PB_TVARINT): /* int32 number */
            pbL_readint32(L, &info->number); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_EnumDescriptorProto(pb_Loader *L, pbL_EnumInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(1, PB_TBYTES): /* string name */
            pbL_readbytes(L, &info->name); break;
        case pb_pair(2, PB_TBYTES): /* EnumValueDescriptorProto value */
            pbL_EnumValueDescriptorProto(L, pbL_add(info->value)); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_MessageOptions(pb_Loader *L, pbL_TypeInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(7, PB_TVARINT): /* bool map_entry */
            pbL_readint32(L, &info->is_map); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_OneofDescriptorProto(pb_Loader *L, pbL_TypeInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(1, PB_TBYTES): /* string name */
            pbL_readbytes(L, pbL_add(info->oneof_decl)); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_DescriptorProto(pb_Loader *L, pbL_TypeInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(1, PB_TBYTES): /* string name */
            pbL_readbytes(L, &info->name); break;
        case pb_pair(2, PB_TBYTES): /* FieldDescriptorProto field */
            pbL_FieldDescriptorProto(L, pbL_add(info->field)); break;
        case pb_pair(6, PB_TBYTES): /* FieldDescriptorProto extension */
            pbL_FieldDescriptorProto(L, pbL_add(info->extension)); break;
        case pb_pair(3, PB_TBYTES): /* DescriptorProto nested_type */
            pbL_DescriptorProto(L, pbL_add(info->nested_type)); break;
        case pb_pair(4, PB_TBYTES): /* EnumDescriptorProto enum_type */
            pbL_EnumDescriptorProto(L, pbL_add(info->enum_type)); break;
        case pb_pair(8, PB_TBYTES): /* OneofDescriptorProto oneof_decl */
            pbL_OneofDescriptorProto(L, info); break;
        case pb_pair(7, PB_TBYTES): /* MessageOptions options */
            pbL_MessageOptions(L, info); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_FileDescriptorProto(pb_Loader *L, pbL_FileInfo *info) {
    pb_Slice s;
    uint32_t tag;
    pbL_beginmsg(L, &s);
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(2, PB_TBYTES): /* string package */
            pbL_readbytes(L, &info->package); break;
        case pb_pair(4, PB_TBYTES): /* DescriptorProto message_type */
            pbL_DescriptorProto(L, pbL_add(info->message_type)); break;
        case pb_pair(5, PB_TBYTES): /* EnumDescriptorProto enum_type */
            pbL_EnumDescriptorProto(L, pbL_add(info->enum_type)); break;
        case pb_pair(7, PB_TBYTES): /* FieldDescriptorProto extension */
            pbL_FieldDescriptorProto(L, pbL_add(info->extension)); break;
        case pb_pair(12, PB_TBYTES): /* string syntax */
            pbL_readbytes(L, &info->syntax); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
    pbL_endmsg(L, &s);
}

static void pbL_FileDescriptorSet(pb_Loader *L, pbL_FileInfo **pinfo) {
    uint32_t tag;
    while (pb_readvarint32(&L->s, &tag)) {
        switch (tag) {
        case pb_pair(1, PB_TBYTES): /* FileDescriptorProto file */
            pbL_FileDescriptorProto(L, pbL_add(pinfo[0])); break;
        default: pb_skipvalue(&L->s, tag);
        }
    }
}

/* loader */

static void pbL_delTypeInfo(pbL_TypeInfo *info) {
    size_t i, count;
    for (i = 0, count = pbL_count(info->nested_type); i < count; ++i)
        pbL_delTypeInfo(&info->nested_type[i]);
    for (i = 0, count = pbL_count(info->enum_type); i < count; ++i)
        pbL_delete(info->enum_type[i].value);
    pbL_delete(info->nested_type);
    pbL_delete(info->enum_type);
    pbL_delete(info->field);
    pbL_delete(info->extension);
}

static void pbL_delFileInfo(pbL_FileInfo *files) {
    size_t i, count, j, jcount;
    for (i = 0, count = pbL_count(files); i < count; ++i) {
        for (j = 0, jcount = pbL_count(files[i].message_type); j < jcount; ++j)
            pbL_delTypeInfo(&files[i].message_type[j]);
        for (j = 0, jcount = pbL_count(files[i].enum_type); j < jcount; ++j)
            pbL_delete(files[i].enum_type[j].value);
        pbL_delete(files[i].message_type);
        pbL_delete(files[i].enum_type);
        pbL_delete(files[i].extension);
    }
    pbL_delete(files);
}

static pb_Slice pbL_prefixname(pb_Buffer *b, pb_Slice s, size_t *ps) {
    *ps = b->size;
    pb_addchar(b, '.');
    pb_addslice(b, s);
    return pb_result(b);
}

static void pbL_loadEnum(pb_State *S, pbL_EnumInfo *info, pb_Loader *L) {
    size_t i, count, curr;
    pb_Type *t = pb_newtype(S, pb_newname(S,
                pbL_prefixname(&L->b, info->name, &curr)));
    t->is_enum = 1;
    for (i = 0, count = pbL_count(info->value); i < count; ++i) {
        pbL_EnumValueInfo *ev = &info->value[i];
        pb_newfield(S, t, pb_newname(S, ev->name), ev->number);
    }
    L->b.size = curr;
}

static void pbL_loadField(pb_State *S, pbL_FieldInfo *info, pb_Loader *L, pb_Type *t) {
    pb_Type *ft = pb_newtype(S, pb_newname(S, info->type_name));
    pb_Field *f;
    if (!ft && (info->type == PB_Tmessage || info->type == PB_Tenum))
        return;
    if (t == NULL && !(t = pb_newtype(S, pb_newname(S, info->extendee))))
        return;
    if (!(f = pb_newfield(S, t, pb_newname(S, info->name), info->number)))
        return;
    f->default_value = pb_newname(S, info->default_value);
    f->type      = ft;
    f->oneof_idx = info->oneof_index;
    f->type_id   = info->type;
    f->repeated  = info->label == 3; /* repeated */
    f->packed    = info->packed >= 0 ? info->packed : L->is_proto3 && f->repeated;
    if (f->type_id >= 9 && f->type_id <= 12) f->packed = 0;
    f->scalar = (f->type == NULL);
}

static void pbL_loadType(pb_State *S, pbL_TypeInfo *info, pb_Loader *L) {
    size_t i, count, curr;
    pb_Type *t = pb_newtype(S, pb_newname(S,
                pbL_prefixname(&L->b, info->name, &curr)));
    t->is_map = info->is_map;
    t->is_proto3 = L->is_proto3;
    for (i = 0, count = pbL_count(info->oneof_decl); i < count; ++i) {
        pb_OneofEntry *e = (pb_OneofEntry*)pb_settable(&t->oneof_index, i+1);
        e->name = pb_newname(S, info->oneof_decl[i]);
        e->index = (int)i+1;
    }
    for (i = 0, count = pbL_count(info->field); i < count; ++i)
        pbL_loadField(S, &info->field[i], L, t);
    for (i = 0, count = pbL_count(info->extension); i < count; ++i)
        pbL_loadField(S, &info->extension[i], L, NULL);
    for (i = 0, count = pbL_count(info->enum_type); i < count; ++i)
        pbL_loadEnum(S, &info->enum_type[i], L);
    for (i = 0, count = pbL_count(info->nested_type); i < count; ++i)
        pbL_loadType(S, &info->nested_type[i], L);
    L->b.size = curr;
}

static void pbL_loadFile(pb_State *S, pbL_FileInfo *info, pb_Loader *L) {
    size_t i, count, j, jcount, curr = 0;
    for (i = 0, count = pbL_count(info); i < count; ++i) {
        if (info[i].package.p) pbL_prefixname(&L->b, info[i].package, &curr);
        if (pb_newname(S, info[i].syntax) == pb_newname(S, pb_slice("proto3")))
            L->is_proto3 = 1;
        for (j = 0, jcount = pbL_count(info[i].enum_type); j < jcount; ++j)
            pbL_loadEnum(S, &info[i].enum_type[j], L);
        for (j = 0, jcount = pbL_count(info[i].message_type); j < jcount; ++j)
            pbL_loadType(S, &info[i].message_type[j], L);
        for (j = 0, jcount = pbL_count(info[i].extension); j < jcount; ++j)
            pbL_loadField(S, &info[i].extension[j], L, NULL);
        L->b.size = curr;
    }
}

PB_API int pb_load(pb_State *S, pb_Slice *s) {
    volatile int ret = PB_ERROR;
    pbL_FileInfo *files = NULL;
    pb_Loader L;
    if (!setjmp(L.jbuf)) {
        L.s = *s;
        L.is_proto3 = 0;
        pb_initbuffer(&L.b);
        pbL_FileDescriptorSet(&L, &files);
        pbL_loadFile(S, files, &L);
        ret = PB_OK;
    }
    pbL_delFileInfo(files);
    pb_resetbuffer(&L.b);
    s->p = L.s.p;
    return ret;
}


PB_NS_END

#endif /* PB_IMPLEMENTATION */

/* cc: flags+='-shared -DPB_IMPLEMENTATION -xc' output='pb.so' */

