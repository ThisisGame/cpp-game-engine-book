#define PB_STATIC_API
#include "pb.h"

PB_NS_BEGIN


#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>


#include <stdio.h>
#include <errno.h>


/* Lua util routines */

#define PB_STATE     "pb.State"
#define PB_BUFFER    "pb.Buffer"
#define PB_SLICE     "pb.Slice"

#define check_buffer(L,idx) ((pb_Buffer*)luaL_checkudata(L,idx,PB_BUFFER))
#define test_buffer(L,idx)  ((pb_Buffer*)luaL_testudata(L,idx,PB_BUFFER))
#define check_slice(L,idx)  ((lpb_SliceEx*)luaL_checkudata(L,idx,PB_SLICE))
#define test_slice(L,idx)   ((lpb_SliceEx*)luaL_testudata(L,idx,PB_SLICE))
#define return_self(L) { lua_settop(L, 1); return 1; }

#if LUA_VERSION_NUM < 502
#include <assert.h>

# define LUA_OK        0
# define lua_rawlen    lua_objlen
# define luaL_setfuncs(L,l,n) (assert(n==0), luaL_register(L,NULL,l))
# define luaL_setmetatable(L, name) \
    (luaL_getmetatable((L), (name)), lua_setmetatable(L, -2))

static int relindex(int idx, int offset)
{ return idx < 0 && idx > LUA_REGISTRYINDEX ? idx - offset : idx; }

static void lua_rawgetp(lua_State *L, int idx, const void *p) {
    lua_pushlightuserdata(L, (void*)p);
    lua_rawget(L, relindex(idx, 1));
}

static void lua_rawsetp(lua_State *L, int idx, const void *p) {
    lua_pushlightuserdata(L, (void*)p);
    lua_insert(L, -2);
    lua_rawset(L, relindex(idx, 1));
}

#ifndef luaL_newlib /* not LuaJIT 2.1 */
#define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))

static lua_Integer lua_tointegerx(lua_State *L, int idx, int *isint) {
    lua_Integer i = lua_tointeger(L, idx);
    if (isint) *isint = (i != 0 || lua_type(L, idx) == LUA_TNUMBER);
    return i;
}

static lua_Number lua_tonumberx(lua_State *L, int idx, int *isnum) {
    lua_Number i = lua_tonumber(L, idx);
    if (isnum) *isnum = (i != 0 || lua_type(L, idx) == LUA_TNUMBER);
    return i;
}

static void *luaL_testudata(lua_State *L, int idx, const char *type) {
    void *p = lua_touserdata(L, idx);
    if (p != NULL && lua_getmetatable(L, idx)) {
        lua_getfield(L, LUA_REGISTRYINDEX, type);
        if (!lua_rawequal(L, -2, -1))
            p = NULL;
        lua_pop(L, 2);
        return p;
    }
    return NULL;
}

#endif

#ifdef LUAI_BITSINT /* not LuaJIT */
#include <errno.h>

static int luaL_fileresult(lua_State *L, int stat, const char *fname) {
    int en = errno;
    if (stat) { lua_pushboolean(L, 1); return 1; }
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", fname, strerror(en));
    /*if (fname) lua_pushfstring(L, "%s: %s", fname, strerror(en));
      else       lua_pushstring(L, strerror(en));*//* NOT USED */
    lua_pushinteger(L, en);
    return 3;
}

#endif /* not LuaJIT */

#endif

#if LUA_VERSION_NUM >= 503
# define lua53_getfield lua_getfield
# define lua53_rawgeti  lua_rawgeti
# define lua53_rawgetp  lua_rawgetp
#else /* not Lua 5.3 */
static int lua53_getfield(lua_State *L, int idx, const char *field)
{ lua_getfield(L, idx, field); return lua_type(L, -1); }
static int lua53_rawgeti(lua_State *L, int idx, lua_Integer i)
{ lua_rawgeti(L, idx, i); return lua_type(L, -1); }
static int lua53_rawgetp(lua_State *L, int idx, const void *p)
{ lua_rawgetp(L, idx, p); return lua_type(L, -1); }
#endif


/* protobuf global state */

#define default_state(L) ((pb_State*)default_lstate(L))

static const char state_name[] = PB_STATE;

enum lpb_Int64Mode { LPB_NUMBER, LPB_STRING, LPB_HEXSTRING };
enum lpb_DefMode   { LPB_DEFDEF, LPB_COPYDEF, LPB_METADEF, LPB_NODEF };

typedef struct lpb_State {
    pb_State  base;
    pb_Buffer buffer;
    int defs_index;
    unsigned enum_as_value : 1;
    unsigned default_mode  : 2; /* lpb_DefMode */
    unsigned int64_mode    : 2; /* lpb_Int64Mode */
} lpb_State;

static void lpb_pushdeftable(lua_State *L, lpb_State *LS) {
    if (LS->defs_index != LUA_NOREF)
        lua_rawgeti(L, LUA_REGISTRYINDEX, LS->defs_index);
    else {
        lua_newtable(L);
        lua_pushvalue(L, -1);
        LS->defs_index = luaL_ref(L, LUA_REGISTRYINDEX);
    }
}

static int Lpb_delete(lua_State *L) {
    lpb_State *LS = (lpb_State*)luaL_testudata(L, 1, PB_STATE);
    if (LS != NULL) {
        pb_free(&LS->base);
        pb_resetbuffer(&LS->buffer);
        luaL_unref(L, LUA_REGISTRYINDEX, LS->defs_index);
    }
    return 0;
}

static lpb_State *default_lstate(lua_State *L) {
    lpb_State *LS;
    if (lua53_rawgetp(L, LUA_REGISTRYINDEX, state_name) == LUA_TUSERDATA) {
        LS = (lpb_State*)lua_touserdata(L, -1);
        lua_pop(L, 1);
    } else {
        lua_pop(L, 1);
        LS = (lpb_State*)lua_newuserdata(L, sizeof(lpb_State));
        memset(LS, 0, sizeof(lpb_State));
        LS->defs_index = LUA_NOREF;
        pb_init(&LS->base);
        pb_initbuffer(&LS->buffer);
        luaL_setmetatable(L, PB_STATE);
        lua_rawsetp(L, LUA_REGISTRYINDEX, state_name);
    }
    return LS;
}

static int Lpb_state(lua_State *L) {
    int top = lua_gettop(L);
    default_lstate(L);
    lua_rawgetp(L, LUA_REGISTRYINDEX, state_name);
    if (top != 0) {
        if (lua_isnil(L, 1))
            lua_pushnil(L);
        else {
            luaL_checkudata(L, 1, PB_STATE);
            lua_pushvalue(L, 1);
        }
        lua_rawsetp(L, LUA_REGISTRYINDEX, state_name);
    }
    return 1;
}


/* protobuf util routines */

typedef struct lpb_SliceEx {
    pb_Slice    base;
    const char *head;
} lpb_SliceEx;

static int lpb_offset(lpb_SliceEx *s) { return (int)(s->base.p-s->head) + 1; }

static lpb_SliceEx lpb_initext(pb_Slice s)
{ lpb_SliceEx ext; ext.base = s, ext.head = s.p; return ext; }

static void lpb_addlength(lua_State *L, pb_Buffer *b, size_t len)
{ if (pb_addlength(b, len) == 0) luaL_error(L, "encode bytes fail"); }

static int typeerror(lua_State *L, int idx, const char *type) {
    lua_pushfstring(L, "%s expected, got %s", type, luaL_typename(L, idx));
    return luaL_argerror(L, idx, lua_tostring(L, -1));
}

static lua_Integer posrelat(lua_Integer pos, size_t len) {
    if (pos >= 0) return pos;
    else if (0u - (size_t)pos > len) return 0;
    else return (lua_Integer)len + pos + 1;
}

static lua_Integer rangerelat(lua_State *L, int idx, lua_Integer r[2], size_t len) {
    r[0] = posrelat(luaL_optinteger(L, idx, 1), len);
    r[1] = posrelat(luaL_optinteger(L, idx+1, len), len);
    if (r[0] < 1) r[0] = 1;
    if (r[1] > (lua_Integer)len) r[1] = len;
    return r[0] <= r[1] ? r[1] - r[0] + 1 : 0;
}

static int argcheck(lua_State *L, int cond, int idx, const char *fmt, ...) {
    if (!cond) {
        va_list l;
        va_start(l, fmt);
        lua_pushvfstring(L, fmt, l);
        va_end(l);
        return luaL_argerror(L, idx, lua_tostring(L, -1));
    }
    return 1;
}

static pb_Slice lpb_toslice(lua_State *L, int idx) {
    int type = lua_type(L, idx);
    pb_Slice ret = { NULL, NULL };
    if (type == LUA_TSTRING) {
        size_t len;
        const char *s = lua_tolstring(L, idx, &len);
        ret = pb_lslice(s, len);
    } else if (type == LUA_TUSERDATA) {
        pb_Buffer *buffer;
        lpb_SliceEx *s;
        if ((buffer = test_buffer(L, idx)) != NULL)
            ret = pb_result(buffer);
        else if ((s = test_slice(L, idx)) != NULL)
            ret = s->base;
    }
    return ret;
}

static pb_Slice lpb_checkslice(lua_State *L, int idx) {
    pb_Slice ret = lpb_toslice(L, idx);
    if (ret.p == NULL) typeerror(L, idx, "string/buffer/slice");
    return ret;
}

static void lpb_readbytes(lua_State *L, lpb_SliceEx *s, lpb_SliceEx *pv) {
    uint64_t len = 0;
    if (pb_readvarint64(&s->base, &len) == 0 || len > PB_MAX_SIZET)
        luaL_error(L, "invalid bytes length: %d (at offset %d)",
                (int)len, lpb_offset(s));
    if (pb_readslice(&s->base, (size_t)len, &pv->base) == 0 && len != 0)
        luaL_error(L, "un-finished bytes (len %d at offset %d)",
                (int)len, lpb_offset(s));
    pv->head = s->head;
}

static int lpb_hexchar(char ch) {
    switch (ch) {
    case '0': return 0;
    case '1': return 1; case '2': return 2; case '3': return 3;
    case '4': return 4; case '5': return 5; case '6': return 6;
    case '7': return 7; case '8': return 8; case '9': return 9;
    case 'a': case 'A': return 10; case 'b': case 'B': return 11;
    case 'c': case 'C': return 12; case 'd': case 'D': return 13;
    case 'e': case 'E': return 14; case 'f': case 'F': return 15;
    }
    return -1;
}

static uint64_t lpb_tointegerx(lua_State *L, int idx, int *isint) {
    int neg = 0;
    const char *s, *os;
#if LUA_VERSION_NUM >= 503
    uint64_t v = (uint64_t)lua_tointegerx(L, idx, isint);
    if (*isint) return v;
#else
    uint64_t v = 0;
    lua_Number nv = lua_tonumberx(L, idx, isint);
    if (*isint) {
        if (nv < (lua_Number)INT64_MIN || nv > (lua_Number)INT64_MAX)
            luaL_error(L, "number has no integer representation");
        return (uint64_t)(int64_t)nv;
    }
#endif
    if ((os = s = lua_tostring(L, idx)) == NULL) return 0;
    while (*s == '#' || *s == '+' || *s == '-')
        neg = (*s == '-') ^ neg, ++s;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        for (s += 2; *s != '\0'; ++s) {
            int n = lpb_hexchar(*s);
            if (n < 0) break;
            v = v << 4 | n;
        }
    } else {
        for (; *s != '\0'; ++s) {
            int n = lpb_hexchar(*s);
            if (n < 0 || n > 10) break;
            v = v * 10 + n;
        }
    }
    if (*s != '\0') luaL_error(L, "integer format error: '%s'", os);
    *isint = 1;
    return neg ? ~v + 1 : v;
}

static uint64_t lpb_checkinteger(lua_State *L, int idx) {
    int isint;
    uint64_t v = lpb_tointegerx(L, idx, &isint);
    if (!isint) typeerror(L, idx, "number/string");
    return v;
}

static void lpb_pushinteger(lua_State *L, int64_t n, int mode) {
    if (mode != LPB_NUMBER && (n < INT_MIN || n > INT_MAX)) {
        char buff[32], *p = buff + sizeof(buff) - 1;
        int neg = n < 0;
        uint64_t un = neg ? ~(uint64_t)n + 1 : (uint64_t)n;
        if (mode == LPB_STRING) {
            for (*p = '\0'; un > 0; un /= 10)
                *--p = "0123456789"[un % 10];
        } else if (mode == LPB_HEXSTRING) {
            for (*p = '\0'; un > 0; un >>= 4)
                *--p = "0123456789ABCDEF"[un & 0xF];
            *--p = 'x', *--p = '0';
        }
        if (neg) *--p = '-';
        *--p = '#';
        lua_pushstring(L, p);
    } else if (LUA_VERSION_NUM >= 503 && sizeof(lua_Integer) >= 8)
        lua_pushinteger(L, (lua_Integer)n);
    else
        lua_pushnumber(L, (lua_Number)n);
}

typedef union lpb_Value {
    lpb_SliceEx s[1];
    uint32_t u32;
    uint64_t u64;
    lua_Integer lint;
    lua_Number lnum;
} lpb_Value;

static int lpb_addtype(lua_State *L, pb_Buffer *b, int idx, int type, size_t *plen) {
    int ret = 0, expected = LUA_TNUMBER;
    lpb_Value v;
    size_t len = 0;
    switch (type) {
    case PB_Tbool:
        len = pb_addvarint32(b, ret = lua_toboolean(L, idx));
        if (ret) len = 0;
        ret = 1;
        break;
    case PB_Tdouble:
        v.lnum = lua_tonumberx(L, idx, &ret);
        if (ret) len = pb_addfixed64(b, pb_encode_double((double)v.lnum));
        if (v.lnum != 0.0) len = 0;
        break;
    case PB_Tfloat:
        v.lnum = lua_tonumberx(L, idx, &ret);
        if (ret) len = pb_addfixed32(b, pb_encode_float((float)v.lnum));
        if (v.lnum != 0.0) len = 0;
        break;
    case PB_Tfixed32:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addfixed32(b, v.u32);
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tsfixed32:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addfixed32(b, v.u32);
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tint32:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addvarint64(b, pb_expandsig((uint32_t)v.u64));
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tuint32:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addvarint32(b, v.u32);
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tsint32:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addvarint32(b, pb_encode_sint32(v.u32));
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tfixed64:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addfixed64(b, v.u64);
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tsfixed64:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addfixed64(b, v.u64);
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tint64: case PB_Tuint64:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addvarint64(b, v.u64);
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tsint64:
        v.u64 = lpb_tointegerx(L, idx, &ret);
        if (ret) len = pb_addvarint64(b, pb_encode_sint64(v.u64));
        if (v.u64 != 0) len = 0;
        break;
    case PB_Tbytes: case PB_Tstring:
        v.s->base = lpb_toslice(L, idx);
        if ((ret = (v.s->base.p != NULL))) len = pb_addbytes(b, v.s->base);
        if (pb_len(v.s->base) != 0) len = 0;
        expected = LUA_TSTRING;
        break;
    default:
        lua_pushfstring(L, "unknown type %s", pb_typename(type, "<unknown>"));
        if (idx > 0) argcheck(L, 0, idx, lua_tostring(L, -1));
        lua_error(L);
    }
    if (plen) *plen = len;
    return ret ? 0 : expected;
}

static void lpb_readtype(lua_State *L, lpb_State *LS, int type, lpb_SliceEx *s) {
    lpb_Value v;
    switch (type) {
#define pushinteger(n) lpb_pushinteger((L), (n), LS->int64_mode)
    case PB_Tbool:  case PB_Tenum:
    case PB_Tint32: case PB_Tuint32: case PB_Tsint32:
    case PB_Tint64: case PB_Tuint64: case PB_Tsint64:
        if (pb_readvarint64(&s->base, &v.u64) == 0)
            luaL_error(L, "invalid varint value at offset %d", lpb_offset(s));
        switch (type) {
        case PB_Tbool:   lua_pushboolean(L, v.u64 != 0); break;
         /*case PB_Tenum:   pushinteger(v.u64); break; [> NOT REACHED <]*/
        case PB_Tint32:  pushinteger((int32_t)v.u64); break;
        case PB_Tuint32: pushinteger((uint32_t)v.u64); break;
        case PB_Tsint32: pushinteger(pb_decode_sint32((uint32_t)v.u64)); break;
        case PB_Tint64:  pushinteger((int64_t)v.u64); break;
        case PB_Tuint64: pushinteger((uint64_t)v.u64); break;
        case PB_Tsint64: pushinteger(pb_decode_sint64(v.u64)); break;
        }
        break;
    case PB_Tfloat:
    case PB_Tfixed32:
    case PB_Tsfixed32:
        if (pb_readfixed32(&s->base, &v.u32) == 0)
            luaL_error(L, "invalid fixed32 value at offset %d", lpb_offset(s));
        switch (type) {
        case PB_Tfloat:    lua_pushnumber(L, pb_decode_float(v.u32)); break;
        case PB_Tfixed32:  pushinteger(v.u32); break;
        case PB_Tsfixed32: pushinteger((int32_t)v.u32); break;
        }
        break;
    case PB_Tdouble:
    case PB_Tfixed64:
    case PB_Tsfixed64:
        if (pb_readfixed64(&s->base, &v.u64) == 0)
            luaL_error(L, "invalid fixed64 value at offset %d", lpb_offset(s));
        switch (type) {
        case PB_Tdouble:   lua_pushnumber(L, pb_decode_double(v.u64)); break;
        case PB_Tfixed64:  pushinteger(v.u64); break;
        case PB_Tsfixed64: pushinteger((int64_t)v.u64); break;
        }
        break;
    case PB_Tbytes:
    case PB_Tstring:
    case PB_Tmessage:
        lpb_readbytes(L, s, v.s);
        lua_pushlstring(L, v.s->base.p, pb_len(v.s->base));
        break;
    default:
        luaL_error(L, "unknown type %s (%d)", pb_typename(type, NULL), type);
    }
}


/* io routines */

#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
#else
# define setmode(a,b)  ((void)0)
#endif

static int io_read(lua_State *L) {
    FILE *fp = (FILE*)lua_touserdata(L, 1);
    size_t nr;
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    do {  /* read file in chunks of LUAL_BUFFERSIZE bytes */
        char *p = luaL_prepbuffer(&b);
        nr = fread(p, sizeof(char), LUAL_BUFFERSIZE, fp);
        luaL_addsize(&b, nr);
    } while (nr == LUAL_BUFFERSIZE);
    luaL_pushresult(&b);  /* close buffer */
    return 1;
}

static int io_write(lua_State *L, FILE *f, int idx) {
    int nargs = lua_gettop(L) - idx + 1;
    int status = 1;
    for (; nargs--; idx++) {
        pb_Slice s = lpb_checkslice(L, idx);
        size_t l = pb_len(s);
        status = status && (fwrite(s.p, sizeof(char), l, f) == l);
    }
    return status ? 1 : luaL_fileresult(L, 0, NULL);
}

static int Lio_read(lua_State *L) {
#if defined(WINAPI_FAMILY_PARTITION)
  return luaL_error(L, "unsupport api in uwp platform");
#else
    const char *fname = luaL_optstring(L, 1, NULL);
    FILE *fp = stdin;
    int ret;
    if (fname == NULL)
        (void)setmode(fileno(stdin), O_BINARY);
    else if ((fp = fopen(fname, "rb")) == NULL)
        return luaL_fileresult(L, 0, fname);
    lua_pushcfunction(L, io_read);
    lua_pushlightuserdata(L, fp);
    ret = lua_pcall(L, 1, 1, 0);
    if (fp != stdin) fclose(fp);
    else (void)setmode(fileno(stdin), O_TEXT);
    if (ret != LUA_OK) { lua_pushnil(L); lua_insert(L, -2); return 2; }
    return 1;
#endif
}

static int Lio_write(lua_State *L) {
#if defined(WINAPI_FAMILY_PARTITION)
    return luaL_error(L, "unsupport api in uwp platform");
#else
    int res;
    (void)setmode(fileno(stdout), O_BINARY);
    res = io_write(L, stdout, 1);
    fflush(stdout);
    (void)setmode(fileno(stdout), O_TEXT);
    return res;
#endif
}

static int Lio_dump(lua_State *L) {
#if defined(WINAPI_FAMILY_PARTITION)
    return luaL_error(L, "unsupport api in uwp platform");
#else
    int res;
    const char *fname = luaL_checkstring(L, 1);
    FILE *fp = fopen(fname, "wb");
    if (fp == NULL) return luaL_fileresult(L, 0, fname);
    res = io_write(L, fp, 2);
    fclose(fp);
    return res;
#endif
}

LUALIB_API int luaopen_pb_io(lua_State *L) {
    luaL_Reg libs[] = {
#define ENTRY(name) { #name, Lio_##name }
        ENTRY(read),
        ENTRY(write),
        ENTRY(dump),
#undef  ENTRY
        { NULL, NULL }
    };
    luaL_newlib(L, libs);
    return 1;
}


/* protobuf integer conversion */

static int Lconv_encode_int32(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    uint64_t v = pb_expandsig((int32_t)lpb_checkinteger(L, 1));
    lpb_pushinteger(L, v, mode);
    return 1;
}

static int Lconv_encode_uint32(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, (uint32_t)lpb_checkinteger(L, 1), mode);
    return 1;
}

static int Lconv_encode_sint32(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, pb_encode_sint32((int32_t)lpb_checkinteger(L, 1)), mode);
    return 1;
}

static int Lconv_decode_sint32(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, pb_decode_sint32((uint32_t)lpb_checkinteger(L, 1)), mode);
    return 1;
}

static int Lconv_encode_sint64(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, pb_encode_sint64(lpb_checkinteger(L, 1)), mode);
    return 1;
}

static int Lconv_decode_sint64(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, pb_decode_sint64(lpb_checkinteger(L, 1)), mode);
    return 1;
}

static int Lconv_encode_float(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, pb_encode_float((float)luaL_checknumber(L, 1)), mode);
    return 1;
}

static int Lconv_decode_float(lua_State *L) {
    lua_pushnumber(L, pb_decode_float((uint32_t)lpb_checkinteger(L, 1)));
    return 1;
}

static int Lconv_encode_double(lua_State *L) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_pushinteger(L, pb_encode_double(luaL_checknumber(L, 1)), mode);
    return 1;
}

static int Lconv_decode_double(lua_State *L) {
    lua_pushnumber(L, pb_decode_double(lpb_checkinteger(L, 1)));
    return 1;
}

LUALIB_API int luaopen_pb_conv(lua_State *L) {
    luaL_Reg libs[] = {
        { "decode_uint32", Lconv_encode_uint32 },
        { "decode_int32", Lconv_encode_int32 },
#define ENTRY(name) { #name, Lconv_##name }
        ENTRY(encode_int32),
        ENTRY(encode_uint32),
        ENTRY(encode_sint32),
        ENTRY(encode_sint64),
        ENTRY(decode_sint32),
        ENTRY(decode_sint64),
        ENTRY(decode_float),
        ENTRY(decode_double),
        ENTRY(encode_float),
        ENTRY(encode_double),
#undef  ENTRY
        { NULL, NULL }
    };
    luaL_newlib(L, libs);
    return 1;
}


/* protobuf encode routine */

static int lpb_typefmt(const char *fmt) {
    switch (*fmt) {
    case 'b': return PB_Tbool;
    case 'f': return PB_Tfloat;
    case 'F': return PB_Tdouble;
    case 'i': return PB_Tint32;
    case 'j': return PB_Tsint32;
    case 'u': return PB_Tuint32;
    case 'x': return PB_Tfixed32;
    case 'y': return PB_Tsfixed32;
    case 'I': return PB_Tint64;
    case 'J': return PB_Tsint64;
    case 'U': return PB_Tuint64;
    case 'X': return PB_Tfixed64;
    case 'Y': return PB_Tsfixed64;
    }
    return -1;
}

static int lpb_packfmt(lua_State *L, int idx, pb_Buffer *b, const char **pfmt, int level) {
    const char *fmt = *pfmt;
    int type, ltype;
    size_t len;
    argcheck(L, level <= 100, 1, "format level overflow");
    for (; *fmt != '\0'; ++fmt) {
        switch (*fmt) {
        case 'v': pb_addvarint64(b, (uint64_t)lpb_checkinteger(L, idx++)); break;
        case 'd': pb_addfixed32(b, (uint32_t)lpb_checkinteger(L, idx++)); break;
        case 'q': pb_addfixed64(b, (uint64_t)lpb_checkinteger(L, idx++)); break;
        case 'c': pb_addslice(b, lpb_checkslice(L, idx++)); break;
        case 's': pb_addbytes(b, lpb_checkslice(L, idx++)); break;
        case '#': lpb_addlength(L, b, (size_t)lpb_checkinteger(L, idx++)); break;
        case '(':
            len = pb_bufflen(b);
            ++fmt;
            idx = lpb_packfmt(L, idx, b, &fmt, level+1);
            lpb_addlength(L, b, len);
            break;
        case ')':
            if (level == 0) luaL_argerror(L, 1, "unexpected ')' in format");
            *pfmt = fmt;
            return idx;
        case '\0':
        default:
            argcheck(L, (type = lpb_typefmt(fmt)) >= 0,
                    1, "invalid formater: '%c'", *fmt);
            ltype = lpb_addtype(L, b, idx, type, NULL);
            argcheck(L, ltype == 0, idx, "%s expected for type '%s', got %s",
                    lua_typename(L, ltype), pb_typename(type, "<unknown>"),
                    luaL_typename(L, idx));
            ++idx;
        }
    }
    if (level != 0) luaL_argerror(L, 2, "unmatch '(' in format");
    *pfmt = fmt;
    return idx;
}

static int Lpb_tohex(lua_State *L) {
    pb_Slice s = lpb_checkslice(L, 1);
    const char *hexa = "0123456789ABCDEF";
    char hex[4] = "XX ";
    lua_Integer r[2] = { 1, -1 };
    luaL_Buffer lb;
    rangerelat(L, 2, r, pb_len(s));
    luaL_buffinit(L, &lb);
    for (; r[0] <= r[1]; ++r[0]) {
        unsigned int ch = s.p[r[0]-1];
        hex[0] = hexa[(ch>>4)&0xF];
        hex[1] = hexa[(ch   )&0xF];
        if (r[0] == r[1]) hex[2] = '\0';
        luaL_addstring(&lb, hex);
    }
    luaL_pushresult(&lb);
    return 1;
}

static int Lpb_result(lua_State *L) {
    pb_Slice s = lpb_checkslice(L, 1);
    lua_Integer r[2] = {1, -1}, range = rangerelat(L, 2, r, pb_len(s));
    lua_pushlstring(L, s.p+r[0]-1, (size_t)range);
    return 1;
}

static int Lbuf_new(lua_State *L) {
    int i, top = lua_gettop(L);
    pb_Buffer *buf = (pb_Buffer*)lua_newuserdata(L, sizeof(pb_Buffer));
    pb_initbuffer(buf);
    luaL_setmetatable(L, PB_BUFFER);
    for (i = 1; i <= top; ++i)
        pb_addslice(buf, lpb_checkslice(L, i));
    return 1;
}

static int Lbuf_libcall(lua_State *L) {
    int i, top = lua_gettop(L);
    pb_Buffer *buf = (pb_Buffer*)lua_newuserdata(L, sizeof(pb_Buffer));
    pb_initbuffer(buf);
    luaL_setmetatable(L, PB_BUFFER);
    for (i = 2; i <= top; ++i)
        pb_addslice(buf, lpb_checkslice(L, i));
    return 1;
}

static int Lbuf_tostring(lua_State *L) {
    pb_Buffer *buf = check_buffer(L, 1);
    lua_pushfstring(L, "pb.Buffer: %p", buf);
    return 1;
}

static int Lbuf_reset(lua_State *L) {
    pb_Buffer *buf = check_buffer(L, 1);
    int i, top = lua_gettop(L);
    pb_resetbuffer(buf);
    for (i = 2; i <= top; ++i)
        pb_addslice(buf, lpb_checkslice(L, i));
    return_self(L);
}

static int Lbuf_len(lua_State *L) {
    pb_Buffer *buf = check_buffer(L, 1);
    lua_pushinteger(L, (lua_Integer)buf->size);
    return 1;
}

static int Lbuf_pack(lua_State *L) {
    pb_Buffer b, *pb = test_buffer(L, 1);
    int idx = 1 + (pb != NULL);
    const char *fmt = luaL_checkstring(L, idx++);
    if (pb == NULL) pb_initbuffer(pb = &b);
    lpb_packfmt(L, idx, pb, &fmt, 0);
    if (pb != &b)
        lua_settop(L, 1);
    else {
        pb_Slice ret = pb_result(pb);
        lua_pushlstring(L, ret.p, pb_len(ret));
        pb_resetbuffer(pb);
    }
    return 1;
}

LUALIB_API int luaopen_pb_buffer(lua_State *L) {
    luaL_Reg libs[] = {
        { "__tostring", Lbuf_tostring },
        { "__len",      Lbuf_len },
        { "__gc",       Lbuf_reset },
        { "delete",     Lbuf_reset },
        { "tohex",      Lpb_tohex },
        { "result",     Lpb_result },
#define ENTRY(name) { #name, Lbuf_##name }
        ENTRY(new),
        ENTRY(reset),
        ENTRY(pack),
#undef  ENTRY
        { NULL, NULL }
    };
    if (luaL_newmetatable(L, PB_BUFFER)) {
        luaL_setfuncs(L, libs, 0);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        lua_createtable(L, 0, 1);
        lua_pushcfunction(L, Lbuf_libcall);
        lua_setfield(L, -2, "__call");
        lua_setmetatable(L, -2);
    }
    return 1;
}


/* protobuf decode routine */

#define LPB_INITSTACKLEN 2

typedef struct lpb_Slice {
    lpb_SliceEx  curr;
    lpb_SliceEx *buff;
    size_t       used;
    size_t       size;
    lpb_SliceEx  init_buff[LPB_INITSTACKLEN];
} lpb_Slice;

static void lpb_resetslice(lua_State *L, lpb_Slice *s, size_t size) {
    if (size == sizeof(lpb_Slice)) {
        if (s->buff != s->init_buff)
            free(s->buff);
        memset(s, 0, sizeof(lpb_Slice));
        s->buff = s->init_buff;
        s->size = LPB_INITSTACKLEN;
    }
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, s);
}

static lpb_SliceEx lpb_checkview(lua_State *L, int idx, lpb_SliceEx *ps) {
    pb_Slice src = lpb_checkslice(L, idx);
    lua_Integer r[2] = {1, -1}, range = rangerelat(L, idx+1, r, pb_len(src));
    lpb_SliceEx ret;
    if (ps) ps->base = src, ps->head = src.p;
    ret.base.p   = src.p + r[0] - 1;
    ret.base.end = ret.base.p + range;
    ret.head     = src.p;
    return ret;
}

static void lpb_enterview(lua_State *L, lpb_Slice *s, lpb_SliceEx view) {
    if (s->used >= s->size) {
        size_t newsize = s->size * 2;
        lpb_SliceEx *oldp = s->buff != s->init_buff ? s->buff : NULL;
        lpb_SliceEx *newp = (lpb_SliceEx*)realloc(oldp, newsize*sizeof(lpb_SliceEx));
        if (newp == NULL) { luaL_error(L, "out of memory"); return; }
        if (oldp == NULL) memcpy(newp, s->buff, s->used*sizeof(lpb_SliceEx));
        s->buff = newp;
        s->size = newsize;
    }
    s->buff[s->used++] = s->curr;
    s->curr = view;
}

static void lpb_initslice(lua_State *L, int idx, lpb_Slice *s, size_t size) {
    if (size == sizeof(lpb_Slice)) {
        memset(s, 0, sizeof(lpb_Slice));
        s->buff = s->init_buff;
        s->size = LPB_INITSTACKLEN;
    }
    if (!lua_isnoneornil(L, idx)) {
        lpb_SliceEx base, view = lpb_checkview(L, idx, &base);
        s->curr = base;
        if (size == sizeof(lpb_Slice)) lpb_enterview(L, s, view);
        lua_pushvalue(L, idx);
        lua_rawsetp(L, LUA_REGISTRYINDEX, s);
    }
}

static int lpb_unpackscalar(lua_State *L, int *pidx, int top, int fmt, lpb_SliceEx *s) {
    unsigned mode = default_lstate(L)->int64_mode;
    lpb_Value v;
    switch (fmt) {
    case 'v':
        if (pb_readvarint64(&s->base, &v.u64) == 0)
            luaL_error(L, "invalid varint value at offset %d", lpb_offset(s));
        lpb_pushinteger(L, v.u64, mode);
        break;
    case 'd':
        if (pb_readfixed32(&s->base, &v.u32) == 0)
            luaL_error(L, "invalid fixed32 value at offset %d", lpb_offset(s));
        lpb_pushinteger(L, v.u32, mode);
        break;
    case 'q':
        if (pb_readfixed64(&s->base, &v.u64) == 0)
            luaL_error(L, "invalid fixed64 value at offset %d", lpb_offset(s));
        lpb_pushinteger(L, v.u64, mode);
        break;
    case 's':
        if (pb_readbytes(&s->base, &v.s->base) == 0)
            luaL_error(L, "invalid bytes value at offset %d", lpb_offset(s));
        lua_pushlstring(L, v.s->base.p, pb_len(v.s->base));
        break;
    case 'c':
        argcheck(L, *pidx <= top, 1, "format argument exceed");
        v.lint = luaL_checkinteger(L, *pidx++);
        if (pb_readslice(&s->base, (size_t)v.lint, &v.s->base) == 0)
            luaL_error(L, "invalid sub string at offset %d", lpb_offset(s));
        lua_pushlstring(L, v.s->base.p, pb_len(v.s->base));
        break;
    default:
        return 0;
    }
    return 1;
}

static int lpb_unpackloc(lua_State *L, int *pidx, int top, int fmt, lpb_SliceEx *s, int *prets) {
    lua_Integer li;
    size_t len = s->base.end - s->head;
    switch (fmt) {
    case '@':
        lua_pushinteger(L, lpb_offset(s));
        ++*prets;
        break;

    case '*': case '+':
        argcheck(L, *pidx <= top, 1, "format argument exceed");
        if (fmt == '*')
            li = posrelat(luaL_checkinteger(L, *pidx++), len);
        else
            li = lpb_offset(s) + luaL_checkinteger(L, *pidx++);
        if (li == 0) li = 1;
        if (li > (lua_Integer)len) li = (lua_Integer)len + 1;
        s->base.p = s->head + li - 1;
        break;

    default:
        return 0;
    }
    return 1;
}

static int lpb_unpackfmt(lua_State *L, int idx, const char *fmt, lpb_SliceEx *s) {
    int rets = 0, top = lua_gettop(L), type;
    for (; *fmt != '\0'; ++fmt) {
        if (lpb_unpackloc(L, &idx, top, *fmt, s, &rets))
            continue;
        if (s->base.p >= s->base.end) { lua_pushnil(L); return rets + 1; }
        luaL_checkstack(L, 1, "too many values");
        if (!lpb_unpackscalar(L, &idx, top, *fmt, s)) {
            argcheck(L, (type = lpb_typefmt(fmt)) >= 0,
                    1, "invalid formater: '%c'", *fmt);
            lpb_readtype(L, default_lstate(L), type, s);
        }
        ++rets;
    }
    return rets;
}

static lpb_Slice *check_lslice(lua_State *L, int idx) {
    lpb_SliceEx *s = check_slice(L, idx);
    argcheck(L, lua_rawlen(L, 1) == sizeof(lpb_Slice),
            idx, "unsupport operation for raw mode slice");
    return (lpb_Slice*)s;
}

static int Lslice_new(lua_State *L) {
    lpb_Slice *s;
    lua_settop(L, 3);
    s = (lpb_Slice*)lua_newuserdata(L, sizeof(lpb_Slice));
    lpb_initslice(L, 1, s, sizeof(lpb_Slice));
    luaL_setmetatable(L, PB_SLICE);
    return 1;
}

static int Lslice_libcall(lua_State *L) {
    lpb_Slice *s;
    lua_settop(L, 4);
    s = (lpb_Slice*)lua_newuserdata(L, sizeof(lpb_Slice));
    lpb_initslice(L, 2, s, sizeof(lpb_Slice));
    luaL_setmetatable(L, PB_SLICE);
    return 1;
}

static int Lslice_reset(lua_State *L) {
    lpb_Slice *s = check_lslice(L, 1);
    size_t size = lua_rawlen(L, 1);
    lpb_resetslice(L, s, size);
    if (!lua_isnoneornil(L, 2))
        lpb_initslice(L, 2, s, size);
    return_self(L);
}

static int Lslice_tostring(lua_State *L) {
    lpb_SliceEx *s = check_slice(L, 1);
    lua_pushfstring(L, "pb.Slice: %p%s", s,
            lua_rawlen(L, 1) == sizeof(lpb_Slice) ? "" : " (raw)");
    return 1;
}

static int Lslice_len(lua_State *L) {
    lpb_SliceEx *s = check_slice(L, 1);
    lua_pushinteger(L, (lua_Integer)pb_len(s->base));
    lua_pushinteger(L, (lua_Integer)lpb_offset(s));
    return 2;
}

static int Lslice_unpack(lua_State *L) {
    lpb_SliceEx view, *s = test_slice(L, 1);
    const char *fmt = luaL_checkstring(L, 2);
    if (s == NULL) view = lpb_initext(lpb_checkslice(L, 1)), s = &view;
    return lpb_unpackfmt(L, 3, fmt, s);
}

static int Lslice_level(lua_State *L) {
    lpb_Slice *s = check_lslice(L, 1);
    if (!lua_isnoneornil(L, 2)) {
        lpb_SliceEx *se;
        lua_Integer level = posrelat(luaL_checkinteger(L, 2), s->used);
        if (level > (lua_Integer)s->used)
            return 0;
        else if (level == (lua_Integer)s->used)
            se = &s->curr;
        else
            se = &s->buff[level];
        lua_pushinteger(L, (lua_Integer)(se->base.p   - s->buff[0].head) + 1);
        lua_pushinteger(L, (lua_Integer)(se->head     - s->buff[0].head) + 1);
        lua_pushinteger(L, (lua_Integer)(se->base.end - s->buff[0].head));
        return 3;
    }
    lua_pushinteger(L, s->used);
    return 1;
}

static int Lslice_enter(lua_State *L) {
    lpb_Slice *s = check_lslice(L, 1);
    lpb_SliceEx view;
    if (lua_isnoneornil(L, 2)) {
        argcheck(L, pb_readbytes(&s->curr.base, &view.base) != 0,
            1, "bytes wireformat expected at offset %d", lpb_offset(&s->curr));
        view.head = view.base.p;
        lpb_enterview(L, s, view);
    } else {
        lua_Integer r[] = {1, -1};
        lua_Integer range = rangerelat(L, 2, r, s->curr.base.end - s->curr.head);
        view.base.p   = s->curr.head + r[0] - 1;
        view.base.end = view.base.p + range;
        view.head     = s->curr.head;
        lpb_enterview(L, s, view);
    }
    return_self(L);
}

static int Lslice_leave(lua_State *L) {
    lpb_Slice *s = check_lslice(L, 1);
    lua_Integer count = posrelat(luaL_optinteger(L, 2, 1), s->used);
    if (count > (lua_Integer)s->used)
        argcheck(L, 0, 2, "level (%d) exceed max level %d",
                (int)count, (int)s->used);
    else if (count == (lua_Integer)s->used) {
        s->curr = s->buff[0];
        s->used = 1;
    } else {
        s->used -= (size_t)count;
        s->curr = s->buff[s->used];
    }
    lua_settop(L, 1);
    lua_pushinteger(L, s->used);
    return 2;
}

LUALIB_API int lpb_newslice(lua_State *L, const char *s, size_t len) {
    lpb_SliceEx *S = (lpb_SliceEx*)lua_newuserdata(L, sizeof(lpb_SliceEx));
    *S = lpb_initext(pb_lslice(s, len));
    luaL_setmetatable(L, PB_SLICE);
    return 1;
}

LUALIB_API int luaopen_pb_slice(lua_State *L) {
    luaL_Reg libs[] = {
        { "__tostring", Lslice_tostring },
        { "__len",      Lslice_len   },
        { "__gc",       Lslice_reset },
        { "delete",     Lslice_reset },
        { "tohex",      Lpb_tohex   },
        { "result",     Lpb_result  },
#define ENTRY(name) { #name, Lslice_##name }
        ENTRY(new),
        ENTRY(reset),
        ENTRY(level),
        ENTRY(enter),
        ENTRY(leave),
        ENTRY(unpack),
#undef  ENTRY
        { NULL, NULL }
    };
    if (luaL_newmetatable(L, PB_SLICE)) {
        luaL_setfuncs(L, libs, 0);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        lua_createtable(L, 0, 1);
        lua_pushcfunction(L, Lslice_libcall);
        lua_setfield(L, -2, "__call");
        lua_setmetatable(L, -2);
    }
    return 1;
}


/* high level typeinfo/encode/decode routines */

static pb_Type *lpb_type(pb_State *S, const char *name) {
    pb_Type *t;
    if (name == NULL || *name == '.')
        t = pb_type(S, pb_name(S, name));
    else {
        pb_Buffer b;
        pb_initbuffer(&b);
        pb_addchar(&b, '.');
        pb_addslice(&b, pb_slice(name));
        pb_addchar(&b, '\0');
        t = pb_type(S, pb_name(S, pb_buffer(&b)));
        pb_resetbuffer(&b);
    }
    return t;
}

static pb_Field *lpb_checkfield(lua_State *L, int idx, pb_Type *t) {
    int isint, number = (int)lua_tointegerx(L, idx, &isint);
    if (isint) return pb_field(t, number);
    return pb_fname(t, pb_name(default_state(L), luaL_checkstring(L, idx)));
}

static int Lpb_load(lua_State *L) {
    pb_State *S = default_state(L);
    lpb_SliceEx s = lpb_initext(lpb_checkslice(L, 1));
    lua_pushboolean(L, pb_load(S, &s.base) == PB_OK);
    lua_pushinteger(L, lpb_offset(&s));
    return 2;
}

static int Lpb_loadfile(lua_State *L) {
#if defined(WINAPI_FAMILY_PARTITION)
  return luaL_error(L, "unsupport api in uwp platform");
#else
    pb_State *S = default_state(L);
    const char *filename = luaL_checkstring(L, 1);
    size_t size;
    pb_Buffer b;
    lpb_SliceEx s;
    int ret;
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return luaL_fileresult(L, 0, filename);
    pb_initbuffer(&b);
    do {
        void *d = pb_prepbuffsize(&b, BUFSIZ);
        if (d == NULL) { fclose(fp); return luaL_error(L, "out of memory"); }
        size = fread(d, 1, BUFSIZ, fp);
        pb_addsize(&b, size);
    } while (size == BUFSIZ);
    fclose(fp);
    s = lpb_initext(pb_result(&b));
    ret = pb_load(S, &s.base);
    pb_resetbuffer(&b);
    lua_pushboolean(L, ret == PB_OK);
    lua_pushinteger(L, lpb_offset(&s));
    return 2;
#endif
}

static int lpb_pushtype(lua_State *L, pb_Type *t) {
    if (t == NULL) return 0;
    lua_pushstring(L, (char*)t->name);
    lua_pushstring(L, (char*)t->basename);
    lua_pushstring(L, t->is_map ? "map" : t->is_enum ? "enum" : "message");
    return 3;
}

static int lpb_pushfield(lua_State *L, pb_Type *t, pb_Field *f) {
    if (f == NULL) return 0;
    lua_pushstring(L, (char*)f->name);
    lua_pushinteger(L, f->number);
    lua_pushstring(L, f->type ? (char*)f->type->name :
            pb_typename(f->type_id, "<unknown>"));
    lua_pushstring(L, (char*)f->default_value);
    lua_pushstring(L, f->repeated ? f->packed ? "packed" : "repeated"
                                  : "optional");
    if (f->oneof_idx > 0) {
        lua_pushstring(L, (const char*)pb_oneofname(t, f->oneof_idx));
        lua_pushinteger(L, f->oneof_idx-1);
        return 7;
    }
    return 5;
}

static int Lpb_typesiter(lua_State *L) {
    pb_State *S = default_state(L);
    pb_Type *t = lpb_type(S, lua_tostring(L, 2));
    if ((t == NULL && !lua_isnoneornil(L, 2)))
        return 0;
    pb_nexttype(S, &t);
    return lpb_pushtype(L, t);
}

static int Lpb_types(lua_State *L) {
    lua_pushcfunction(L, Lpb_typesiter);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

static int Lpb_fieldsiter(lua_State *L) {
    pb_State *S = default_state(L);
    pb_Type *t = lpb_type(S, luaL_checkstring(L, 1));
    pb_Field *f = pb_fname(t, pb_name(S, lua_tostring(L, 2)));
    if ((f == NULL && !lua_isnoneornil(L, 2)) || !pb_nextfield(t, &f))
        return 0;
    return lpb_pushfield(L, t, f);
}

static int Lpb_fields(lua_State *L) {
    lua_pushcfunction(L, Lpb_fieldsiter);
    lua_pushvalue(L, 1);
    lua_pushnil(L);
    return 3;
}

static int Lpb_type(lua_State *L) {
    pb_State *S = default_state(L);
    pb_Type *t = lpb_type(S, luaL_checkstring(L, 1));
    if (t == NULL || t->is_dead)
        return 0;
    return lpb_pushtype(L, t);
}

static int Lpb_field(lua_State *L) {
    pb_State *S = default_state(L);
    pb_Type *t = lpb_type(S, luaL_checkstring(L, 1));
    return lpb_pushfield(L, t, lpb_checkfield(L, 2, t));
}

static int Lpb_enum(lua_State *L) {
    lpb_State *LS = default_lstate(L);
    pb_Type *t = lpb_type(&LS->base, luaL_checkstring(L, 1));
    pb_Field *f = lpb_checkfield(L, 2, t);
    if (f == NULL) return 0;
    if (lua_type(L, 2) == LUA_TNUMBER)
        lua_pushstring(L, (char*)f->name);
    else
        lpb_pushinteger(L, f->number, LS->int64_mode);
    return 1;
}

static int lpb_pushdefault(lua_State *L, lpb_State *LS, pb_Field *f, int is_proto3) {
    pb_Type *type = f->type;
    int ret = 0;
    char *end;
    if (f == NULL) return 0;
    if (is_proto3 && f->repeated) { lua_newtable(L); return 1; }
    switch (f->type_id) {
    case PB_Tbytes: case PB_Tstring:
        if (f->default_value)
            ret = 1, lua_pushstring(L, (char*)f->default_value);
        else if (is_proto3)
            ret = 1, lua_pushliteral(L, "");
        break;
    case PB_Tenum:
        if ((f = pb_fname(type, f->default_value)) != NULL) {
            if (LS->enum_as_value)
                ret = 1, lpb_pushinteger(L, f->number, LS->int64_mode);
            else
                ret = 1, lua_pushstring(L, (char*)f->name);
        } else if (is_proto3) {
            if ((f = pb_field(type, 0)) == NULL || LS->enum_as_value)
                ret = 1, lua_pushinteger(L, 0);
            else
                ret = 1, lua_pushstring(L, (char*)f->name);
        }
        break;
    case PB_Tmessage:
        return 0;
    case PB_Tbool:
        if (f->default_value) {
            if (f->default_value == pb_name(&LS->base, "true"))
                ret = 1, lua_pushboolean(L, 1);
            else if (f->default_value == pb_name(&LS->base, "false"))
                ret = 1, lua_pushboolean(L, 0);
        } else if (is_proto3) ret = 1, lua_pushboolean(L, 0);
        break;
    case PB_Tdouble: case PB_Tfloat:
        if (f->default_value) {
            lua_Number ln = (lua_Number)strtod((char*)f->default_value, &end);
            if ((char*)f->default_value == end) return 0;
            ret = 1, lua_pushnumber(L, ln);
        } else if (is_proto3) ret = 1, lua_pushnumber(L, 0.0);
        break;

    default:
        if (f->default_value) {
            lua_Integer li = (lua_Integer)strtol((char*)f->default_value, &end, 10);
            if ((char*)f->default_value == end) return 0;
            ret = 1, lpb_pushinteger(L, li, LS->int64_mode);
        } else if (is_proto3) ret = 1, lua_pushinteger(L, 0);
    }
    return ret;
}

static void lpb_pushdefaults(lua_State *L, lpb_State *LS, pb_Type *t) {
    lpb_pushdeftable(L, LS);
    if (lua53_rawgetp(L, -1, t) != LUA_TTABLE) {
        pb_Field *f = NULL;
        lua_pop(L, 1);
        lua_newtable(L);
        while (pb_nextfield(t, &f)) {
            if (!f->repeated && lpb_pushdefault(L, LS, f, t->is_proto3))
                lua_setfield(L, -2, (char*)f->name);
        }
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        lua_pushvalue(L, -1);
        lua_rawsetp(L, -3, t);
    }
    lua_remove(L, -2);
}

static void lpb_cleardefaults(lua_State *L, lpb_State *LS, pb_Type *t) {
    lpb_pushdeftable(L, LS);
    lua_pushnil(L);
    lua_rawsetp(L, -2, t);
    lua_pop(L, 1);
}

static int Lpb_defaults(lua_State *L) {
    lpb_State *LS = default_lstate(L);
    pb_Type *t = lpb_type(&LS->base, luaL_checkstring(L, 1));
    int clear = lua_toboolean(L, 2);
    lpb_pushdefaults(L, LS, t);
    if (clear) lpb_cleardefaults(L, LS, t);
    return 1;
}

static int Lpb_clear(lua_State *L) {
    lpb_State *LS = default_lstate(L);
    pb_State *S = &LS->base;
    pb_Type *t;
    if (lua_isnoneornil(L, 1)) {
        pb_free(S), pb_init(S);
        luaL_unref(L, LUA_REGISTRYINDEX, LS->defs_index);
        LS->defs_index = LUA_NOREF;
        return 0;
    }
    t = lpb_type(S, luaL_checkstring(L, 1));
    if (lua_isnoneornil(L, 2)) pb_deltype(S, t);
    else pb_delfield(S, t, lpb_checkfield(L, 2, t));
    lpb_cleardefaults(L, LS, t);
    return 0;
}


/* protobuf encode */

typedef struct lpb_Env {
    lua_State *L;
    lpb_State *LS;
    pb_Buffer *b;
    lpb_SliceEx *s;
} lpb_Env;

static void lpb_encode (lpb_Env *e, pb_Type *t);

static void lpb_checktable(lua_State *L, pb_Field *f) {
    argcheck(L, lua_istable(L, -1),
            2, "table expected at field '%s', got %s",
            (char*)f->name, luaL_typename(L, -1));
}

static void lpbE_enum(lpb_Env *e, pb_Field *f) {
    lua_State *L = e->L;
    pb_Buffer *b = e->b;
    pb_Field *ev;
    int type = lua_type(L, -1);
    if (type == LUA_TNUMBER)
        pb_addvarint64(b, (uint64_t)lua_tonumber(L, -1));
    else if ((ev = pb_fname(f->type,
                    pb_name(&e->LS->base, lua_tostring(L, -1)))) != NULL)
        pb_addvarint32(b, ev->number);
    else if (type != LUA_TSTRING)
        argcheck(L, 0, 2, "number/string expected at field '%s', got %s",
                (char*)f->name, luaL_typename(L, -1));
    else
        argcheck(L, 0, 2, "can not encode unknown enum '%s' at field '%s'",
                lua_tostring(L, -1), (char*)f->name);
}

static void lpbE_field(lpb_Env *e, pb_Field *f, size_t *plen) {
    lua_State *L = e->L;
    pb_Buffer *b = e->b;
    size_t len;
    int ltype;
    if (plen) *plen = 0;
    switch (f->type_id) {
    case PB_Tenum:
        lpbE_enum(e, f);
        break;

    case PB_Tmessage:
        lpb_checktable(L, f);
        len = pb_bufflen(b);
        lpb_encode(e, f->type);
        lpb_addlength(L, b, len);
        break;

    default:
        ltype = lpb_addtype(L, b, -1, f->type_id, plen);
        argcheck(L, ltype == 0,
                2, "%s expected for field '%s', got %s",
                lua_typename(L, ltype),
                (char*)f->name, luaL_typename(L, -1));
    }
}

static void lpbE_tagfield(lpb_Env *e, pb_Field *f, size_t *plen) {
    size_t klen = pb_addvarint32(e->b,
            pb_pair(f->number, pb_wtypebytype(f->type_id)));
    lpbE_field(e, f, plen);
    if (plen && *plen != 0) *plen += klen;
}

static void lpbE_map(lpb_Env *e, pb_Field *f) {
    lua_State *L = e->L;
    pb_Field *kf = pb_field(f->type, 1);
    pb_Field *vf = pb_field(f->type, 2);
    if (kf == NULL || vf == NULL) return;
    lpb_checktable(L, f);
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        size_t len, ignoredlen;
        pb_addvarint32(e->b, pb_pair(f->number, PB_TBYTES));
        len = pb_bufflen(e->b);
        lua_pushvalue(L, -2);
        lpbE_tagfield(e, kf, &ignoredlen);
        e->b->size -= ignoredlen;
        lua_pop(L, 1);
        lpbE_tagfield(e, vf, &ignoredlen);
        e->b->size -= ignoredlen;
        lua_pop(L, 1);
        lpb_addlength(L, e->b, len);
    }
}

static void lpbE_repeated(lpb_Env *e, pb_Field *f) {
    lua_State *L = e->L;
    pb_Buffer *b = e->b;
    int i;
    lpb_checktable(L, f);
    if (f->packed) {
        size_t len;
        pb_addvarint32(b, pb_pair(f->number, PB_TBYTES));
        len = pb_bufflen(b);
        for (i = 1; lua53_rawgeti(L, -1, i) != LUA_TNIL; ++i) {
            lpbE_field(e, f, NULL);
            lua_pop(L, 1);
        }
        lpb_addlength(L, b, len);
    } else {
        for (i = 1; lua53_rawgeti(L, -1, i) != LUA_TNIL; ++i) {
            lpbE_tagfield(e, f, NULL);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

static void lpb_encode(lpb_Env *e, pb_Type *t) {
    lua_State *L = e->L;
    luaL_checkstack(L, 3, "message too many levels");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            pb_Field *f = pb_fname(t,
                    pb_name(&e->LS->base, lua_tostring(L, -2)));
            if (f == NULL)
                /* skip */;
            else if (f->type && f->type->is_map)
                lpbE_map(e, f);
            else if (f->repeated)
                lpbE_repeated(e, f);
            else if (!f->type || !f->type->is_dead) {
                size_t ignoredlen;
                lpbE_tagfield(e, f, &ignoredlen);
                if (t->is_proto3 && !f->oneof_idx)
                    e->b->size -= ignoredlen;
            }
        }
        lua_pop(L, 1);
    }
}

static int Lpb_encode(lua_State *L) {
    lpb_State *LS = default_lstate(L);
    pb_Type *t = lpb_type(&LS->base, luaL_checkstring(L, 1));
    lpb_Env e;
    argcheck(L, t!=NULL, 1, "type '%s' does not exists", lua_tostring(L, 1));
    luaL_checktype(L, 2, LUA_TTABLE);
    e.L = L, e.LS = LS, e.b = test_buffer(L, 3);
    if (e.b == NULL) pb_resetbuffer(e.b = &LS->buffer);
    lua_pushvalue(L, 2);
    lpb_encode(&e, t);
    if (e.b != &LS->buffer)
        lua_settop(L, 3);
    else {
        lua_pushlstring(L, e.b->buff, e.b->size);
        pb_resetbuffer(e.b);
    }
    return 1;
}


/* protobuf decode */

#define lpb_withinput(e, ns, stmt) ((e)->s = (ns), (stmt), (e)->s = s)

static int lpb_decode(lpb_Env *e, pb_Type *t);

static void lpb_pushtypetable(lua_State *L, lpb_State *LS, pb_Type *t) {
    pb_Field *f = NULL;
    int mode = t ? LS->default_mode : LPB_NODEF;
    lua_newtable(L);
    switch (t && t->is_proto3 && mode == LPB_DEFDEF ? LPB_COPYDEF : mode) {
    case LPB_COPYDEF:
        while (pb_nextfield(t, &f))
            if (!f->oneof_idx && lpb_pushdefault(L, LS, f, t->is_proto3))
                lua_setfield(L, -2, (char*)f->name);
        break;
    case LPB_METADEF:
        while (pb_nextfield(t, &f)) {
            if (f->repeated) {
                lua_newtable(L);
                lua_setfield(L, -2, (char*)f->name);
            }
        }
        lpb_pushdefaults(L, LS, t);
        lua_setmetatable(L, -2);
        break;
    default: /* no default value */
        break;
    }
}

static void lpb_fetchtable(lpb_Env *e, pb_Field *f, pb_Type *t) {
    lua_State *L = e->L;
    if (lua53_getfield(L, -1, (char*)f->name) == LUA_TNIL) {
        lua_pop(L, 1);
        lpb_pushtypetable(L, e->LS, t);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, (char*)f->name);
    }
}

static int lpbD_mismatch(lua_State *L, pb_Field *f, lpb_SliceEx *s, uint32_t tag) {
    return luaL_error(L,
            "type mismatch for field '%s' at offset %d, "
            "%s expected for type %s, got %s",
            (char*)f->name,
            lpb_offset(s),
            pb_wtypename(pb_wtypebytype(f->type_id), NULL),
            pb_typename(f->type_id, NULL),
            pb_wtypename(pb_gettype(tag), NULL));
}

static void lpbD_field(lpb_Env *e, pb_Field *f, uint32_t tag) {
    lua_State *L = e->L;
    lpb_SliceEx sv, *s = e->s;
    pb_Field *ev = NULL;
    uint64_t u64;

    if (!f->packed && pb_wtypebytype(f->type_id) != (int)pb_gettype(tag))
        lpbD_mismatch(L, f, s, tag);

    switch (f->type_id) {
    case PB_Tenum:
        if (pb_readvarint64(&s->base, &u64) == 0)
            luaL_error(L, "invalid varint value at offset %d", lpb_offset(s));
        if (!default_lstate(L)->enum_as_value)
            ev = pb_field(f->type, (int32_t)u64);
        if (ev) lua_pushstring(L, (char*)ev->name);
        else lpb_pushinteger(L, (lua_Integer)u64, default_lstate(L)->int64_mode);
        break;

    case PB_Tmessage:
        lpb_readbytes(L, s, &sv);
        if (f->type == NULL || f->type->is_dead)
            lua_pushnil(L);
        else {
            lpb_pushtypetable(L, e->LS, f->type);
            lpb_withinput(e, &sv, lpb_decode(e, f->type));
        }
        break;

    default:
        lpb_readtype(L, e->LS, f->type_id, s);
    }
}

static void lpbD_map(lpb_Env *e, pb_Field *f) {
    lua_State *L = e->L;
    lpb_SliceEx p, *s = e->s;
    int mask = 0, top = lua_gettop(L) + 1;
    uint32_t tag;
    lpb_fetchtable(e, f, NULL);
    lpb_readbytes(L, s, &p);
    if (f->type == NULL) return;
    lua_pushnil(L);
    lua_pushnil(L);
    while (pb_readvarint32(&p.base, &tag)) {
        int n = pb_gettag(tag);
        if (n == 1 || n == 2) {
            mask |= n;
            lpb_withinput(e, &p, lpbD_field(e, pb_field(f->type, n), tag));
            lua_replace(L, top+n);
        }
    }
    if ((mask & 1) == 0
            && lpb_pushdefault(L, e->LS, pb_field(f->type, 1), 1)) {
        lua_replace(L, top + 1);
        mask |= 1;
    }
    if ((mask & 2) == 0
            && lpb_pushdefault(L, e->LS, pb_field(f->type, 2), 1)) {
        lua_replace(L, top + 2);
        mask |= 2;
    }
    if (mask == 3) lua_rawset(L, -3);
    else           lua_pop(L, 2);
    lua_pop(L, 1);
}

static void lpbD_repeated(lpb_Env *e, pb_Field *f, uint32_t tag) {
    lua_State *L = e->L;
    lpb_fetchtable(e, f, NULL);
    if (f->packed && pb_gettype(tag) == PB_TBYTES) {
        int len = (int)lua_rawlen(L, -1);
        lpb_SliceEx p, *s = e->s;
        lpb_readbytes(L, s, &p);
        while (p.base.p < p.base.end) {
            lpb_withinput(e, &p, lpbD_field(e, f, tag));
            lua_rawseti(L, -2, ++len);
        }
    } else {
        lpbD_field(e, f, tag);
        lua_rawseti(L, -2, (lua_Integer)lua_rawlen(L, -2) + 1);
    }
    lua_pop(L, 1);
}

static int lpb_decode(lpb_Env *e, pb_Type *t) {
    lua_State *L = e->L;
    lpb_SliceEx *s = e->s;
    uint32_t tag;
    while (pb_readvarint32(&s->base, &tag)) {
        pb_Field *f = pb_field(t, pb_gettag(tag));
        if (f == NULL)
            pb_skipvalue(&s->base, tag);
        else if (f->type && f->type->is_map)
            lpbD_map(e, f);
        else if (f->repeated)
            lpbD_repeated(e, f, tag);
        else {
            lua_pushstring(L, (char*)f->name);
            lpbD_field(e, f, tag);
            lua_rawset(L, -3);
        }
    }
    return 1;
}

static int lpb_decode_ex(lua_State *L, lpb_SliceEx s) {
    lpb_State *LS = default_lstate(L);
    pb_Type *t = lpb_type(&LS->base, luaL_checkstring(L, 1));
    lpb_Env e;
    argcheck(L, t!=NULL, 1, "type '%s' does not exists", lua_tostring(L, 1));
    lua_settop(L, 3);
    if (!lua_istable(L, 3)) {
        lua_pop(L, 1);
        lpb_pushtypetable(L, LS, t);
    }
    e.L = L, e.LS = LS, e.s = &s;
    return lpb_decode(&e, t);
}

static int Lpb_decode(lua_State *L) {
    lpb_SliceEx s = lua_isnoneornil(L, 2) ? lpb_initext(pb_lslice(NULL, 0))
                                          : lpb_initext(lpb_checkslice(L, 2));
    return lpb_decode_ex(L, s);
}

/* pb module interface */

static int Lpb_option(lua_State *L) {
#define OPTS(X) \
    X(0, enum_as_name,          LS->enum_as_value = 0)             \
    X(1, enum_as_value,         LS->enum_as_value = 1)             \
    X(2, int64_as_number,       LS->int64_mode = LPB_NUMBER)       \
    X(3, int64_as_string,       LS->int64_mode = LPB_STRING)       \
    X(4, int64_as_hexstring,    LS->int64_mode = LPB_HEXSTRING)    \
    X(5, auto_default_values,   LS->default_mode = LPB_DEFDEF)     \
    X(6, no_default_values,     LS->default_mode = LPB_NODEF)      \
    X(7, use_default_values,    LS->default_mode = LPB_COPYDEF)    \
    X(8, use_default_metatable, LS->default_mode = LPB_METADEF)    \

    static const char *opts[] = {
#define X(ID,NAME,CODE) #NAME,
        OPTS(X)
#undef  X
        NULL
    };
    lpb_State *LS = default_lstate(L);
    switch (luaL_checkoption(L, 1, NULL, opts)) {
#define X(ID,NAME,CODE) case ID: CODE; break;
        OPTS(X)
#undef  X
    }
    return 0;
#undef  OPTS
}

LUALIB_API int luaopen_pb(lua_State *L) {
    luaL_Reg libs[] = {
        { "pack",     Lbuf_pack     },
        { "unpack",   Lslice_unpack },
#define ENTRY(name) { #name, Lpb_##name }
        ENTRY(clear),
        ENTRY(load),
        ENTRY(loadfile),
        ENTRY(encode),
        ENTRY(decode),
        ENTRY(types),
        ENTRY(fields),
        ENTRY(type),
        ENTRY(field),
        ENTRY(enum),
        ENTRY(defaults),
        ENTRY(tohex),
        ENTRY(result),
        ENTRY(option),
        ENTRY(state),
#undef  ENTRY
        { NULL, NULL }
    };
    luaL_Reg meta[] = {
        { "__gc", Lpb_delete },
        { "setdefault", Lpb_state },
        { NULL, NULL }
    };
    if (luaL_newmetatable(L, PB_STATE)) {
        luaL_setfuncs(L, meta, 0);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
    }
    luaL_newlib(L, libs);
    return 1;
}

static int Lpb_decode_unsafe(lua_State *L) {
    return lpb_decode_ex(L,
            lpb_initext(pb_lslice(
                    (const char*)lua_touserdata(L, 2),
                    (size_t)luaL_checkinteger(L, 3))));
}

LUALIB_API int luaopen_pb_decode_unsafe(lua_State *L) {
    lua_pushcfunction(L, Lpb_decode_unsafe);
    return 1;
}


PB_NS_END

/* cc: flags+='-O3 -ggdb -pedantic -std=c90 -Wall -Wextra --coverage'
 * maccc: flags+='-v -shared -undefined dynamic_lookup' output='pb.so'
 * win32cc: flags+='-s -mdll -DLUA_BUILD_AS_DLL ' output='pb.dll' libs+='-llua53' */
