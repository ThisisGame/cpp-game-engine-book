--[[
        luaunit.lua

Description: A unit testing framework
Homepage: https://github.com/bluebird75/luaunit
Development by Philippe Fremy <phil@freehackers.org>
Based on initial work of Ryu, Gwang (http://www.gpgstudy.com/gpgiki/LuaUnit)
License: BSD License, see LICENSE.txt
Version: 3.2
]]--

require("math")
local M={}

-- private exported functions (for testing)
M.private = {}

M.VERSION='3.3'
M._VERSION=M.VERSION -- For LuaUnit v2 compatibility

--[[ Some people like assertEquals( actual, expected ) and some people prefer
assertEquals( expected, actual ).
]]--
M.ORDER_ACTUAL_EXPECTED = true
M.PRINT_TABLE_REF_IN_ERROR_MSG = false
M.TABLE_EQUALS_KEYBYCONTENT = true
M.LINE_LENGTH = 80
M.TABLE_DIFF_ANALYSIS_THRESHOLD = 10    -- display deep analysis for more than 10 items
M.LIST_DIFF_ANALYSIS_THRESHOLD  = 10    -- display deep analysis for more than 10 items

--[[ EPS is meant to help with Lua's floating point math in simple corner
cases like almostEquals(1.1-0.1, 1), which may not work as-is (e.g. on numbers
with rational binary representation) if the user doesn't provide some explicit
error margin.

The default margin used by almostEquals() in such cases is EPS; and since
Lua may be compiled with different numeric precisions (single vs. double), we
try to select a useful default for it dynamically. Note: If the initial value
is not acceptable, it can be changed by the user to better suit specific needs.

See also: https://en.wikipedia.org/wiki/Machine_epsilon
]]
M.EPS = 2^-52 -- = machine epsilon for "double", ~2.22E-16
if math.abs(1.1 - 1 - 0.1) > M.EPS then
    -- rounding error is above EPS, assume single precision
    M.EPS = 2^-23 -- = machine epsilon for "float", ~1.19E-07
end

-- set this to false to debug luaunit
local STRIP_LUAUNIT_FROM_STACKTRACE = true

M.VERBOSITY_DEFAULT = 10
M.VERBOSITY_LOW     = 1
M.VERBOSITY_QUIET   = 0
M.VERBOSITY_VERBOSE = 20
M.DEFAULT_DEEP_ANALYSIS = nil
M.FORCE_DEEP_ANALYSIS   = true
M.DISABLE_DEEP_ANALYSIS = false

-- set EXPORT_ASSERT_TO_GLOBALS to have all asserts visible as global values
-- EXPORT_ASSERT_TO_GLOBALS = true

-- we need to keep a copy of the script args before it is overriden
local cmdline_argv = rawget(_G, "arg")

M.FAILURE_PREFIX = 'LuaUnit test FAILURE: ' -- prefix string for failed tests

M.USAGE=[[Usage: lua <your_test_suite.lua> [options] [testname1 [testname2] ... ]
Options:
  -h, --help:             Print this help
  --version:              Print version information
  -v, --verbose:          Increase verbosity
  -q, --quiet:            Set verbosity to minimum
  -e, --error:            Stop on first error
  -f, --failure:          Stop on first failure or error
  -s, --shuffle:          Shuffle tests before running them
  -o, --output OUTPUT:    Set output type to OUTPUT
                          Possible values: text, tap, junit, nil
  -n, --name NAME:        For junit only, mandatory name of xml file
  -r, --repeat NUM:       Execute all tests NUM times, e.g. to trig the JIT
  -p, --pattern PATTERN:  Execute all test names matching the Lua PATTERN
                          May be repeated to include several patterns
                          Make sure you escape magic chars like +? with %
  -x, --exclude PATTERN:  Exclude all test names matching the Lua PATTERN
                          May be repeated to exclude several patterns
                          Make sure you escape magic chars like +? with %
  testname1, testname2, ... : tests to run in the form of testFunction,
                              TestClass or TestClass.testMethod
]]

local is_equal -- defined here to allow calling from mismatchFormattingPureList

----------------------------------------------------------------
--
--                 general utility functions
--
----------------------------------------------------------------

local function pcall_or_abort(func, ...)
    -- unpack is a global function for Lua 5.1, otherwise use table.unpack
    local unpack = rawget(_G, "unpack") or table.unpack
    local result = {pcall(func, ...)}
    if not result[1] then
        -- an error occurred
        print(result[2]) -- error message
        print()
        print(M.USAGE)
        os.exit(-1)
    end
    return unpack(result, 2)
end

local crossTypeOrdering = {
    number = 1, boolean = 2, string = 3, table = 4, other = 5
}
local crossTypeComparison = {
    number = function(a, b) return a < b end,
    string = function(a, b) return a < b end,
    other = function(a, b) return tostring(a) < tostring(b) end,
}

local function crossTypeSort(a, b)
    local type_a, type_b = type(a), type(b)
    if type_a == type_b then
        local func = crossTypeComparison[type_a] or crossTypeComparison.other
        return func(a, b)
    end
    type_a = crossTypeOrdering[type_a] or crossTypeOrdering.other
    type_b = crossTypeOrdering[type_b] or crossTypeOrdering.other
    return type_a < type_b
end

local function __genSortedIndex( t )
    -- Returns a sequence consisting of t's keys, sorted.
    local sortedIndex = {}

    for key,_ in pairs(t) do
        table.insert(sortedIndex, key)
    end

    table.sort(sortedIndex, crossTypeSort)
    return sortedIndex
end
M.private.__genSortedIndex = __genSortedIndex

local function sortedNext(state, control)
    -- Equivalent of the next() function of table iteration, but returns the
    -- keys in sorted order (see __genSortedIndex and crossTypeSort).
    -- The state is a temporary variable during iteration and contains the
    -- sorted key table (state.sortedIdx). It also stores the last index (into
    -- the keys) used by the iteration, to find the next one quickly.
    local key

    --print("sortedNext: control = "..tostring(control) )
    if control == nil then
        -- start of iteration
        state.count = #state.sortedIdx
        state.lastIdx = 1
        key = state.sortedIdx[1]
        return key, state.t[key]
    end

    -- normally, we expect the control variable to match the last key used
    if control ~= state.sortedIdx[state.lastIdx] then
        -- strange, we have to find the next value by ourselves
        -- the key table is sorted in crossTypeSort() order! -> use bisection
        local lower, upper = 1, state.count
        repeat
            state.lastIdx = math.modf((lower + upper) / 2)
            key = state.sortedIdx[state.lastIdx]
            if key == control then
                break -- key found (and thus prev index)
            end
            if crossTypeSort(key, control) then
                -- key < control, continue search "right" (towards upper bound)
                lower = state.lastIdx + 1
            else
                -- key > control, continue search "left" (towards lower bound)
                upper = state.lastIdx - 1
            end
        until lower > upper
        if lower > upper then -- only true if the key wasn't found, ...
            state.lastIdx = state.count -- ... so ensure no match in code below
        end
    end

    -- proceed by retrieving the next value (or nil) from the sorted keys
    state.lastIdx = state.lastIdx + 1
    key = state.sortedIdx[state.lastIdx]
    if key then
        return key, state.t[key]
    end

    -- getting here means returning `nil`, which will end the iteration
end

local function sortedPairs(tbl)
    -- Equivalent of the pairs() function on tables. Allows to iterate in
    -- sorted order. As required by "generic for" loops, this will return the
    -- iterator (function), an "invariant state", and the initial control value.
    -- (see http://www.lua.org/pil/7.2.html)
    return sortedNext, {t = tbl, sortedIdx = __genSortedIndex(tbl)}, nil
end
M.private.sortedPairs = sortedPairs

-- seed the random with a strongly varying seed
math.randomseed(os.clock()*1E11)

local function randomizeTable( t )
    -- randomize the item orders of the table t
    for i = #t, 2, -1 do
        local j = math.random(i)
        if i ~= j then
            t[i], t[j] = t[j], t[i]
        end
    end
end
M.private.randomizeTable = randomizeTable

local function strsplit(delimiter, text)
-- Split text into a list consisting of the strings in text, separated
-- by strings matching delimiter (which may _NOT_ be a pattern).
-- Example: strsplit(", ", "Anna, Bob, Charlie, Dolores")
    if delimiter == "" then -- this would result in endless loops
        error("delimiter matches empty string!")
    end
    local list, pos, first, last = {}, 1
    while true do
        first, last = text:find(delimiter, pos, true)
        if first then -- found?
            table.insert(list, text:sub(pos, first - 1))
            pos = last + 1
        else
            table.insert(list, text:sub(pos))
            break
        end
    end
    return list
end
M.private.strsplit = strsplit

local function hasNewLine( s )
    -- return true if s has a newline
    return (string.find(s, '\n', 1, true) ~= nil)
end
M.private.hasNewLine = hasNewLine

local function prefixString( prefix, s )
    -- Prefix all the lines of s with prefix
    return prefix .. string.gsub(s, '\n', '\n' .. prefix)
end
M.private.prefixString = prefixString

local function strMatch(s, pattern, start, final )
    -- return true if s matches completely the pattern from index start to index end
    -- return false in every other cases
    -- if start is nil, matches from the beginning of the string
    -- if final is nil, matches to the end of the string
    start = start or 1
    final = final or string.len(s)

    local foundStart, foundEnd = string.find(s, pattern, start, false)
    return foundStart == start and foundEnd == final
end
M.private.strMatch = strMatch

local function patternFilter(patterns, expr)
    -- Run `expr` through the inclusion and exclusion rules defined in patterns
    -- and return true if expr shall be included, false for excluded.
    -- Inclusion pattern are defined as normal patterns, exclusions 
    -- patterns start with `!` and are followed by a normal pattern

    -- result: nil = UNKNOWN (not matched yet), true = ACCEPT, false = REJECT
    -- default: true if no explicit "include" is found, set to false otherwise
    local default, result = true, nil

    if patterns ~= nil then
        for _, pattern in ipairs(patterns) do
            local exclude = pattern:sub(1,1) == '!'
            if exclude then
                pattern = pattern:sub(2)
            else
                -- at least one include pattern specified, a match is required
                default = false
            end
            -- print('pattern: ',pattern)
            -- print('exclude: ',exclude)
            -- print('default: ',default)

            if string.find(expr, pattern) then
                -- set result to false when excluding, true otherwise
                result = not exclude
            end
        end
    end

    if result ~= nil then
        return result
    end
    return default
end
M.private.patternFilter = patternFilter

local function xmlEscape( s )
    -- Return s escaped for XML attributes
    -- escapes table:
    -- "   &quot;
    -- '   &apos;
    -- <   &lt;
    -- >   &gt;
    -- &   &amp;

    return string.gsub( s, '.', {
        ['&'] = "&amp;",
        ['"'] = "&quot;",
        ["'"] = "&apos;",
        ['<'] = "&lt;",
        ['>'] = "&gt;",
    } )
end
M.private.xmlEscape = xmlEscape

local function xmlCDataEscape( s )
    -- Return s escaped for CData section, escapes: "]]>"
    return string.gsub( s, ']]>', ']]&gt;' )
end
M.private.xmlCDataEscape = xmlCDataEscape

local function stripLuaunitTrace( stackTrace )
    --[[
    -- Example of  a traceback:
    <<stack traceback:
        example_with_luaunit.lua:130: in function 'test2_withFailure'
        ./luaunit.lua:1449: in function <./luaunit.lua:1449>
        [C]: in function 'xpcall'
        ./luaunit.lua:1449: in function 'protectedCall'
        ./luaunit.lua:1508: in function 'execOneFunction'
        ./luaunit.lua:1596: in function 'runSuiteByInstances'
        ./luaunit.lua:1660: in function 'runSuiteByNames'
        ./luaunit.lua:1736: in function 'runSuite'
        example_with_luaunit.lua:140: in main chunk
        [C]: in ?>>

        Other example:
    <<stack traceback:
        ./luaunit.lua:545: in function 'assertEquals'
        example_with_luaunit.lua:58: in function 'TestToto.test7'
        ./luaunit.lua:1517: in function <./luaunit.lua:1517>
        [C]: in function 'xpcall'
        ./luaunit.lua:1517: in function 'protectedCall'
        ./luaunit.lua:1578: in function 'execOneFunction'
        ./luaunit.lua:1677: in function 'runSuiteByInstances'
        ./luaunit.lua:1730: in function 'runSuiteByNames'
        ./luaunit.lua:1806: in function 'runSuite'
        example_with_luaunit.lua:140: in main chunk
        [C]: in ?>>

    <<stack traceback:
        luaunit2/example_with_luaunit.lua:124: in function 'test1_withFailure'
        luaunit2/luaunit.lua:1532: in function <luaunit2/luaunit.lua:1532>
        [C]: in function 'xpcall'
        luaunit2/luaunit.lua:1532: in function 'protectedCall'
        luaunit2/luaunit.lua:1591: in function 'execOneFunction'
        luaunit2/luaunit.lua:1679: in function 'runSuiteByInstances'
        luaunit2/luaunit.lua:1743: in function 'runSuiteByNames'
        luaunit2/luaunit.lua:1819: in function 'runSuite'
        luaunit2/example_with_luaunit.lua:140: in main chunk
        [C]: in ?>>


    -- first line is "stack traceback": KEEP
    -- next line may be luaunit line: REMOVE
    -- next lines are call in the program under testOk: REMOVE
    -- next lines are calls from luaunit to call the program under test: KEEP

    -- Strategy:
    -- keep first line
    -- remove lines that are part of luaunit
    -- kepp lines until we hit a luaunit line
    ]]

    local function isLuaunitInternalLine( s )
        -- return true if line of stack trace comes from inside luaunit
        return s:find('[/\\]luaunit%.lua:%d+: ') ~= nil
    end

    -- print( '<<'..stackTrace..'>>' )

    local t = strsplit( '\n', stackTrace )
    -- print( prettystr(t) )

    local idx = 2

    -- remove lines that are still part of luaunit
    while t[idx] and isLuaunitInternalLine( t[idx] ) do
        -- print('Removing : '..t[idx] )
        table.remove(t, idx)
    end

    -- keep lines until we hit luaunit again
    while t[idx] and (not isLuaunitInternalLine(t[idx])) do
        -- print('Keeping : '..t[idx] )
        idx = idx + 1
    end

    -- remove remaining luaunit lines
    while t[idx] do
        -- print('Removing : '..t[idx] )
        table.remove(t, idx)
    end

    -- print( prettystr(t) )
    return table.concat( t, '\n')

end
M.private.stripLuaunitTrace = stripLuaunitTrace


local function prettystr_sub(v, indentLevel, printTableRefs, recursionTable )
    local type_v = type(v)
    if "string" == type_v  then
        -- use clever delimiters according to content:
        -- enclose with single quotes if string contains ", but no '
        if v:find('"', 1, true) and not v:find("'", 1, true) then
            return "'" .. v .. "'"
        end
        -- use double quotes otherwise, escape embedded "
        return '"' .. v:gsub('"', '\\"') .. '"'

    elseif "table" == type_v then
        --if v.__class__ then
        --    return string.gsub( tostring(v), 'table', v.__class__ )
        --end
        return M.private._table_tostring(v, indentLevel, printTableRefs, recursionTable)

    elseif "number" == type_v then
        -- eliminate differences in formatting between various Lua versions
        if v ~= v then
            return "#NaN" -- "not a number"
        end
        if v == math.huge then
            return "#Inf" -- "infinite"
        end
        if v == -math.huge then
            return "-#Inf"
        end
        if _VERSION == "Lua 5.3" then
            local i = math.tointeger(v)
            if i then
                return tostring(i)
            end
        end
    end

    return tostring(v)
end

local function prettystr( v )
    --[[ Pretty string conversion, to display the full content of a variable of any type.

    * string are enclosed with " by default, or with ' if string contains a "
    * tables are expanded to show their full content, with indentation in case of nested tables
    ]]--
    local recursionTable = {}
    local s = prettystr_sub(v, 1, M.PRINT_TABLE_REF_IN_ERROR_MSG, recursionTable)
    if recursionTable.recursionDetected and not M.PRINT_TABLE_REF_IN_ERROR_MSG then
        -- some table contain recursive references,
        -- so we must recompute the value by including all table references
        -- else the result looks like crap
        recursionTable = {}
        s = prettystr_sub(v, 1, true, recursionTable)
    end
    return s
end
M.prettystr = prettystr

local function tryMismatchFormatting( table_a, table_b, doDeepAnalysis )
    --[[
    Prepares a nice error message when comparing tables, performing a deeper 
    analysis.

    Arguments:
    * table_a, table_b: tables to be compared
    * doDeepAnalysis:
        M.DEFAULT_DEEP_ANALYSIS: (the default if not specified) perform deep analysis only for big lists and big dictionnaries
        M.FORCE_DEEP_ANALYSIS  : always perform deep analysis
        M.DISABLE_DEEP_ANALYSIS: never perform deep analysis

    Returns: {success, result}
    * success: false if deep analysis could not be performed 
               in this case, just use standard assertion message
    * result: if success is true, a multi-line string with deep analysis of the two lists
    ]]

    -- check if table_a & table_b are suitable for deep analysis
    if type(table_a) ~= 'table' or type(table_b) ~= 'table' then
        return false
    end

    if doDeepAnalysis == M.DISABLE_DEEP_ANALYSIS then
        return false
    end

    local len_a, len_b, isPureList = #table_a, #table_b, true

    for k1, v1 in pairs(table_a) do
        if type(k1) ~= 'number' or k1 > len_a then
            -- this table a mapping
            isPureList = false
            break
        end
    end

    if isPureList then
        for k2, v2 in pairs(table_b) do
            if type(k2) ~= 'number' or k2 > len_b then
                -- this table a mapping
                isPureList = false
                break
            end
        end
    end

    if isPureList and math.min(len_a, len_b) < M.LIST_DIFF_ANALYSIS_THRESHOLD then
        if not (doDeepAnalysis == M.FORCE_DEEP_ANALYSIS) then
            return false
        end
    end

    if isPureList then
        return M.private.mismatchFormattingPureList( table_a, table_b )
    else
        -- only work on mapping for the moment
        -- return M.private.mismatchFormattingMapping( table_a, table_b, doDeepAnalysis )
        return false
    end
end
M.private.tryMismatchFormatting = tryMismatchFormatting

local function getTaTbDescr()
    if not M.ORDER_ACTUAL_EXPECTED then
        return 'expected', 'actual'
    end
    return 'actual', 'expected'
end

local function extendWithStrFmt( res, ... )
    table.insert( res, string.format( ... ) )
end

local function mismatchFormattingMapping( table_a, table_b, doDeepAnalysis )
    --[[
    Prepares a nice error message when comparing tables which are not pure lists, performing a deeper 
    analysis.

    Returns: {success, result}
    * success: false if deep analysis could not be performed 
               in this case, just use standard assertion message
    * result: if success is true, a multi-line string with deep analysis of the two lists
    ]]

    -- disable for the moment
    --[[
    local result = {}
    local descrTa, descrTb = getTaTbDescr()

    local keysCommon = {}
    local keysOnlyTa = {}
    local keysOnlyTb = {}
    local keysDiffTaTb = {}

    local k, v

    for k,v in pairs( table_a ) do
        if is_equal( v, table_b[k] ) then
            table.insert( keysCommon, k )
        else 
            if table_b[k] == nil then
                table.insert( keysOnlyTa, k )
            else
                table.insert( keysDiffTaTb, k )
            end
        end
    end

    for k,v in pairs( table_b ) do
        if not is_equal( v, table_a[k] ) and table_a[k] == nil then
            table.insert( keysOnlyTb, k )
        end
    end

    local len_a = #keysCommon + #keysDiffTaTb + #keysOnlyTa
    local len_b = #keysCommon + #keysDiffTaTb + #keysOnlyTb
    local limited_display = (len_a < 5 or len_b < 5)

    if math.min(len_a, len_b) < M.TABLE_DIFF_ANALYSIS_THRESHOLD then
        return false
    end

    if not limited_display then
        if len_a == len_b then
            extendWithStrFmt( result, 'Table A (%s) and B (%s) both have %d items', descrTa, descrTb, len_a )
        else
            extendWithStrFmt( result, 'Table A (%s) has %d items and table B (%s) has %d items', descrTa, len_a, descrTb, len_b )
            end

        if #keysCommon == 0 and #keysDiffTaTb == 0 then
            table.insert( result, 'Table A and B have no keys in common, they are totally different')
        else
            local s_other = 'other '
            if #keysCommon then
                extendWithStrFmt( result, 'Table A and B have %d identical items', #keysCommon )
            else
                table.insert( result, 'Table A and B have no identical items' )
                s_other = ''
            end

            if #keysDiffTaTb ~= 0 then
                result[#result] = string.format( '%s and %d items differing present in both tables', result[#result], #keysDiffTaTb)
            else
                result[#result] = string.format( '%s and no %sitems differing present in both tables', result[#result], s_other, #keysDiffTaTb)
            end
        end

        extendWithStrFmt( result, 'Table A has %d keys not present in table B and table B has %d keys not present in table A', #keysOnlyTa, #keysOnlyTb ) 
    end

    local function keytostring(k)
        if "string" == type(k) and k:match("^[_%a][_%w]*$") then
            return k
        end
        return prettystr(k)
    end

    if #keysDiffTaTb ~= 0 then
        table.insert( result, 'Items differing in A and B:')
        for k,v in sortedPairs( keysDiffTaTb ) do
            extendWithStrFmt( result, '  - A[%s]: %s', keytostring(v), prettystr(table_a[v]) )
            extendWithStrFmt( result, '  + B[%s]: %s', keytostring(v), prettystr(table_b[v]) )
        end
    end    

    if #keysOnlyTa ~= 0 then
        table.insert( result, 'Items only in table A:' )
        for k,v in sortedPairs( keysOnlyTa ) do
            extendWithStrFmt( result, '  - A[%s]: %s', keytostring(v), prettystr(table_a[v]) )
        end
    end

    if #keysOnlyTb ~= 0 then
        table.insert( result, 'Items only in table B:' )
        for k,v in sortedPairs( keysOnlyTb ) do
            extendWithStrFmt( result, '  + B[%s]: %s', keytostring(v), prettystr(table_b[v]) )
        end
    end

    if #keysCommon ~= 0 then
        table.insert( result, 'Items common to A and B:')
        for k,v in sortedPairs( keysCommon ) do
            extendWithStrFmt( result, '  = A and B [%s]: %s', keytostring(v), prettystr(table_a[v]) )
        end
    end    

    return true, table.concat( result, '\n')
    ]]
end
M.private.mismatchFormattingMapping = mismatchFormattingMapping

local function mismatchFormattingPureList( table_a, table_b )
    --[[
    Prepares a nice error message when comparing tables which are lists, performing a deeper 
    analysis.

    Returns: {success, result}
    * success: false if deep analysis could not be performed 
               in this case, just use standard assertion message
    * result: if success is true, a multi-line string with deep analysis of the two lists
    ]]
    local result, descrTa, descrTb = {}, getTaTbDescr()

    local len_a, len_b, refa, refb = #table_a, #table_b, '', ''
    if M.PRINT_TABLE_REF_IN_ERROR_MSG then
        refa, refb = string.format( '<%s> ', tostring(table_a)), string.format('<%s> ', tostring(table_b) )
    end
    local longest, shortest = math.max(len_a, len_b), math.min(len_a, len_b)
    local deltalv  = longest - shortest

    local commonUntil = shortest
    for i = 1, shortest do
        if not is_equal(table_a[i], table_b[i]) then
            commonUntil = i - 1
            break
        end
    end

    local commonBackTo = shortest - 1
    for i = 0, shortest - 1 do
        if not is_equal(table_a[len_a-i], table_b[len_b-i]) then
            commonBackTo = i - 1
            break
        end
    end


    table.insert( result, 'List difference analysis:' )    
    if len_a == len_b then
        -- TODO: handle expected/actual naming
        extendWithStrFmt( result, '* lists %sA (%s) and %sB (%s) have the same size', refa, descrTa, refb, descrTb )
    else 
        extendWithStrFmt( result, '* list sizes differ: list %sA (%s) has %d items, list %sB (%s) has %d items', refa, descrTa, len_a, refb, descrTb, len_b )
    end

    extendWithStrFmt( result, '* lists A and B start differing at index %d', commonUntil+1 ) 
    if commonBackTo >= 0 then
        if deltalv > 0 then
            extendWithStrFmt( result, '* lists A and B are equal again from index %d for A, %d for B', len_a-commonBackTo, len_b-commonBackTo )
        else
            extendWithStrFmt( result, '* lists A and B are equal again from index %d', len_a-commonBackTo )
        end
    end

    local function insertABValue(ai, bi)
        bi = bi or ai
        if is_equal( table_a[ai], table_b[bi]) then
            return extendWithStrFmt( result, '  = A[%d], B[%d]: %s', ai, bi, prettystr(table_a[ai]) )
        else
            extendWithStrFmt( result, '  - A[%d]: %s', ai, prettystr(table_a[ai]))
            extendWithStrFmt( result, '  + B[%d]: %s', bi, prettystr(table_b[bi]))
        end
    end

    -- common parts to list A & B, at the beginning
    if commonUntil > 0 then
        table.insert( result, '* Common parts:' )
        for i = 1, commonUntil do
            insertABValue( i )
        end
    end

    -- diffing parts to list A & B
    if commonUntil < shortest - commonBackTo - 1 then
        table.insert( result, '* Differing parts:' )
        for i = commonUntil + 1, shortest - commonBackTo - 1 do
            insertABValue( i )
        end
    end

    -- display indexes of one list, with no match on other list
    if shortest - commonBackTo <= longest - commonBackTo - 1 then
        table.insert( result, '* Present only in one list:' )
        for i = shortest - commonBackTo, longest - commonBackTo - 1 do
            if len_a > len_b then
                extendWithStrFmt( result, '  - A[%d]: %s', i, prettystr(table_a[i]) )
                -- table.insert( result, '+ (no matching B index)')
            else
                -- table.insert( result, '- no matching A index')
                extendWithStrFmt( result, '  + B[%d]: %s', i, prettystr(table_b[i]) )
            end
        end
    end

    -- common parts to list A & B, at the end
    if commonBackTo >= 0 then
        table.insert( result, '* Common parts at the end of the lists' )
        for i = longest - commonBackTo, longest do
            if len_a > len_b then
                insertABValue( i, i-deltalv )
            else
                insertABValue( i-deltalv, i )
            end
        end
    end

    return true, table.concat( result, '\n')
end
M.private.mismatchFormattingPureList = mismatchFormattingPureList

local function prettystrPairs(value1, value2, suffix_a, suffix_b)
    --[[
    This function helps with the recurring task of constructing the "expected
    vs. actual" error messages. It takes two arbitrary values and formats
    corresponding strings with prettystr().

    To keep the (possibly complex) output more readable in case the resulting
    strings contain line breaks, they get automatically prefixed with additional
    newlines. Both suffixes are optional (default to empty strings), and get
    appended to the "value1" string. "suffix_a" is used if line breaks were
    encountered, "suffix_b" otherwise.

    Returns the two formatted strings (including padding/newlines).
    ]]
    local str1, str2 = prettystr(value1), prettystr(value2)
    if hasNewLine(str1) or hasNewLine(str2) then
        -- line break(s) detected, add padding
        return "\n" .. str1 .. (suffix_a or ""), "\n" .. str2
    end
    return str1 .. (suffix_b or ""), str2
end
M.private.prettystrPairs = prettystrPairs

local function _table_raw_tostring( t )
    -- return the default tostring() for tables, with the table ID, even if the table has a metatable
    -- with the __tostring converter
    local mt = getmetatable( t )
    if mt then setmetatable( t, nil ) end
    local ref = tostring(t)
    if mt then setmetatable( t, mt ) end
    return ref
end
M.private._table_raw_tostring = _table_raw_tostring

local TABLE_TOSTRING_SEP = ", "
local TABLE_TOSTRING_SEP_LEN = string.len(TABLE_TOSTRING_SEP)

local function _table_tostring( tbl, indentLevel, printTableRefs, recursionTable )
    printTableRefs = printTableRefs or M.PRINT_TABLE_REF_IN_ERROR_MSG
    recursionTable = recursionTable or {}
    recursionTable[tbl] = true

    local result, dispOnMultLines = {}, false

    -- like prettystr but do not enclose with "" if the string is just alphanumerical
    -- this is better for displaying table keys who are often simple strings
    local function keytostring(k)
        if "string" == type(k) and k:match("^[_%a][_%w]*$") then
            return k
        end
        return prettystr_sub(k, indentLevel+1, printTableRefs, recursionTable)
    end

    local mt = getmetatable( tbl )

    if mt and mt.__tostring then
        -- if table has a __tostring() function in its metatable, use it to display the table
        -- else, compute a regular table
        result = strsplit( '\n', tostring(tbl) )
        return M.private._table_tostring_format_multiline_string( result, indentLevel )

    else
        -- no metatable, compute the table representation

        local entry, count, seq_index = nil, 0, 1
        for k, v in sortedPairs( tbl ) do

            -- key part
            if k == seq_index then
                -- for the sequential part of tables, we'll skip the "<key>=" output
                entry = ''
                seq_index = seq_index + 1
            elseif recursionTable[k] then
                -- recursion in the key detected
                recursionTable.recursionDetected = true
                entry = "<".._table_raw_tostring(k)..">="
            else
                entry = keytostring(k) .. "="
            end

            -- value part 
            if recursionTable[v] then
                -- recursion in the value detected!
                recursionTable.recursionDetected = true
                entry = entry .. "<".._table_raw_tostring(v)..">"
            else
                entry = entry ..
                    prettystr_sub( v, indentLevel+1, printTableRefs, recursionTable )
            end
            count = count + 1
            result[count] = entry
        end
        return M.private._table_tostring_format_result( tbl, result, indentLevel, printTableRefs )
    end

end
M.private._table_tostring = _table_tostring -- prettystr_sub() needs it

local function _table_tostring_format_multiline_string( tbl_str, indentLevel )
    local indentString = '\n'..string.rep("    ", indentLevel - 1)
    return table.concat( tbl_str, indentString )

end
M.private._table_tostring_format_multiline_string = _table_tostring_format_multiline_string


local function _table_tostring_format_result( tbl, result, indentLevel, printTableRefs )
    -- final function called in _table_to_string() to format the resulting list of 
    -- string describing the table.

    local dispOnMultLines = false

    -- set dispOnMultLines to true if the maximum LINE_LENGTH would be exceeded with the values
    local totalLength = 0
    for k, v in ipairs( result ) do
        totalLength = totalLength + string.len( v )
        if totalLength >= M.LINE_LENGTH then
            dispOnMultLines = true
            break
        end
    end

    -- set dispOnMultLines to true if the max LINE_LENGTH would be exceeded
    -- with the values and the separators.
    if not dispOnMultLines then
        -- adjust with length of separator(s):
        -- two items need 1 sep, three items two seps, ... plus len of '{}'
        if #result > 0 then
            totalLength = totalLength + TABLE_TOSTRING_SEP_LEN * (#result - 1)
        end
        dispOnMultLines = (totalLength + 2 >= M.LINE_LENGTH)
    end

    -- now reformat the result table (currently holding element strings)
    if dispOnMultLines then
        local indentString = string.rep("    ", indentLevel - 1)
        result = {  
                    "{\n    ", 
                    indentString,
                    table.concat(result, ",\n    " .. indentString), 
                    "\n",
                    indentString, 
                    "}"
                }
    else
        result = {"{", table.concat(result, TABLE_TOSTRING_SEP), "}"}
    end
    if printTableRefs then
        table.insert(result, 1, "<".._table_raw_tostring(tbl).."> ") -- prepend table ref
    end
    return table.concat(result)
end
M.private._table_tostring_format_result = _table_tostring_format_result -- prettystr_sub() needs it

local function _table_contains(t, element)
    if type(t) == "table" then
        local type_e = type(element)
        for _, value in pairs(t) do
            if type(value) == type_e then
                if value == element then
                    return true
                end
                if type_e == 'table' then
                    -- if we wanted recursive items content comparison, we could use
                    -- _is_table_items_equals(v, expected) but one level of just comparing
                    -- items is sufficient
                    if M.private._is_table_equals( value, element ) then
                        return true
                    end
                end
            end
        end
    end
    return false
end

local function _is_table_items_equals(actual, expected )
    local type_a, type_e = type(actual), type(expected)

    if (type_a == 'table') and (type_e == 'table') then
        for k, v in pairs(actual) do
            if not _table_contains(expected, v) then
                return false
            end
        end
        for k, v in pairs(expected) do
            if not _table_contains(actual, v) then
                return false
            end
        end
        return true

    elseif type_a ~= type_e then
        return false

    elseif actual ~= expected then
        return false
    end

    return true
end

--[[
This is a specialized metatable to help with the bookkeeping of recursions
in _is_table_equals(). It provides an __index table that implements utility
functions for easier management of the table. The "cached" method queries
the state of a specific (actual,expected) pair; and the "store" method sets
this state to the given value. The state of pairs not "seen" / visited is
assumed to be `nil`.
]]
local _recursion_cache_MT = {
    __index = {
        -- Return the cached value for an (actual,expected) pair (or `nil`)
        cached = function(t, actual, expected)
            local subtable = t[actual] or {}
            return subtable[expected]
        end,

        -- Store cached value for a specific (actual,expected) pair.
        -- Returns the value, so it's easy to use for a "tailcall" (return ...).
        store = function(t, actual, expected, value, asymmetric)
            local subtable = t[actual]
            if not subtable then
                subtable = {}
                t[actual] = subtable
            end
            subtable[expected] = value

            -- Unless explicitly marked "asymmetric": Consider the recursion
            -- on (expected,actual) to be equivalent to (actual,expected) by
            -- default, and thus cache the value for both.
            if not asymmetric then
                t:store(expected, actual, value, true)
            end

            return value
        end
    }
}

local function _is_table_equals(actual, expected, recursions)
    local type_a, type_e = type(actual), type(expected)
    recursions = recursions or setmetatable({}, _recursion_cache_MT)

    if type_a ~= type_e then
        return false -- different types won't match
    end

    if (type_a == 'table') --[[ and (type_e == 'table') ]] then
        if actual == expected then
            -- Both reference the same table, so they are actually identical
            return recursions:store(actual, expected, true)
        end

        -- If we've tested this (actual,expected) pair before: return cached value
        local previous = recursions:cached(actual, expected)
        if previous ~= nil then
            return previous
        end

        -- Mark this (actual,expected) pair, so we won't recurse it again. For
        -- now, assume a "false" result, which we might adjust later if needed.
        recursions:store(actual, expected, false)

        -- Tables must have identical element count, or they can't match.
        if (#actual ~= #expected) then
            return false
        end

        local actualKeysMatched, actualTableKeys = {}, {}

        for k, v in pairs(actual) do
            if M.TABLE_EQUALS_KEYBYCONTENT and type(k) == "table" then
                -- If the keys are tables, things get a bit tricky here as we
                -- can have _is_table_equals(t[k1], t[k2]) despite k1 ~= k2. So
                -- we first collect table keys from "actual", and then later try
                -- to match each table key from "expected" to actualTableKeys.
                table.insert(actualTableKeys, k)
            else
                if not _is_table_equals(v, expected[k], recursions) then
                    return false -- Mismatch on value, tables can't be equal
                end
                actualKeysMatched[k] = true -- Keep track of matched keys
            end
        end

        for k, v in pairs(expected) do
            if M.TABLE_EQUALS_KEYBYCONTENT and type(k) == "table" then
                local found = false
                -- Note: DON'T use ipairs() here, table may be non-sequential!
                for i, candidate in pairs(actualTableKeys) do
                    if _is_table_equals(candidate, k, recursions) then
                        if _is_table_equals(actual[candidate], v, recursions) then
                            found = true
                            -- Remove the candidate we matched against from the list
                            -- of table keys, so each key in actual can only match
                            -- one key in expected.
                            actualTableKeys[i] = nil
                            break
                        end
                        -- keys match but values don't, keep searching
                    end
                end
                if not found then
                    return false -- no matching (key,value) pair
                end
            else
                if not actualKeysMatched[k] then
                    -- Found a key that we did not see in "actual" -> mismatch
                    return false
                end
                -- Otherwise actual[k] was already matched against v = expected[k].
            end
        end

        if next(actualTableKeys) then
            -- If there is any key left in actualTableKeys, then that is
            -- a table-type key in actual with no matching counterpart
            -- (in expected), and so the tables aren't equal.
            return false
        end

        -- The tables are actually considered equal, update cache and return result
        return recursions:store(actual, expected, true)

    elseif actual ~= expected then
        return false
    end

    return true
end
M.private._is_table_equals = _is_table_equals
is_equal = _is_table_equals

local function failure(msg, level)
    -- raise an error indicating a test failure
    -- for error() compatibility we adjust "level" here (by +1), to report the
    -- calling context
    error(M.FAILURE_PREFIX .. msg, (level or 1) + 1)
end

local function fail_fmt(level, ...)
     -- failure with printf-style formatted message and given error level
    failure(string.format(...), (level or 1) + 1)
end
M.private.fail_fmt = fail_fmt

local function error_fmt(level, ...)
     -- printf-style error()
    error(string.format(...), (level or 1) + 1)
end

----------------------------------------------------------------
--
--                     assertions
--
----------------------------------------------------------------

local function errorMsgEquality(actual, expected, doDeepAnalysis)

    if not M.ORDER_ACTUAL_EXPECTED then
        expected, actual = actual, expected
    end
    if type(expected) == 'string' or type(expected) == 'table' then
        local strExpected, strActual = prettystrPairs(expected, actual)
        local result = string.format("expected: %s\nactual: %s", strExpected, strActual)

        -- extend with mismatch analysis if possible:
        local success, mismatchResult
        success, mismatchResult = tryMismatchFormatting( actual, expected, doDeepAnalysis )
        if success then 
            result = table.concat( { result, mismatchResult }, '\n' )
        end
        return result
    end
    return string.format("expected: %s, actual: %s",
                         prettystr(expected), prettystr(actual))
end

function M.assertError(f, ...)
    -- assert that calling f with the arguments will raise an error
    -- example: assertError( f, 1, 2 ) => f(1,2) should generate an error
    if pcall( f, ... ) then
        failure( "Expected an error when calling function but no error generated", 2 )
    end
end

function M.fail( msg )
    -- stops a test due to a failure
    failure( msg, 2 )
end

function M.failIf( cond, msg )
    -- Fails a test with "msg" if condition is true
    if cond then
        failure( msg, 2 )
    end
end


------------------------------------------------------------------
--                  Equality assertion
------------------------------------------------------------------

function M.assertEquals(actual, expected, doDeepAnalysis)
    if type(actual) == 'table' and type(expected) == 'table' then
        if not _is_table_equals(actual, expected) then
            failure( errorMsgEquality(actual, expected, doDeepAnalysis), 2 )
        end
    elseif type(actual) ~= type(expected) then
        failure( errorMsgEquality(actual, expected), 2 )
    elseif actual ~= expected then
        failure( errorMsgEquality(actual, expected), 2 )
    end
end

function M.almostEquals( actual, expected, margin )
    if type(actual) ~= 'number' or type(expected) ~= 'number' or type(margin) ~= 'number' then
        error_fmt(3, 'almostEquals: must supply only number arguments.\nArguments supplied: %s, %s, %s',
            prettystr(actual), prettystr(expected), prettystr(margin))
    end
    if margin < 0 then
        error('almostEquals: margin must not be negative, current value is ' .. margin, 3)
    end
    return math.abs(expected - actual) <= margin
end

function M.assertAlmostEquals( actual, expected, margin )
    -- check that two floats are close by margin
    margin = margin or M.EPS
    if not M.almostEquals(actual, expected, margin) then
        if not M.ORDER_ACTUAL_EXPECTED then
            expected, actual = actual, expected
        end
        local delta = math.abs(actual - expected) 
        fail_fmt(2, 'Values are not almost equal\n' ..
                    'Actual: %s, expected: %s, delta %s above margin of %s',
                    actual, expected, delta, margin)
    end
end

function M.assertNotEquals(actual, expected)
    if type(actual) ~= type(expected) then
        return
    end

    if type(actual) == 'table' and type(expected) == 'table' then
        if not _is_table_equals(actual, expected) then
            return
        end
    elseif actual ~= expected then
        return
    end
    fail_fmt(2, 'Received the not expected value: %s', prettystr(actual))
end

function M.assertNotAlmostEquals( actual, expected, margin )
    -- check that two floats are not close by margin
    margin = margin or M.EPS
    if M.almostEquals(actual, expected, margin) then
        if not M.ORDER_ACTUAL_EXPECTED then
            expected, actual = actual, expected
        end
        local delta = math.abs(actual - expected)
        fail_fmt(2, 'Values are almost equal\nActual: %s, expected: %s' ..
                    ', delta %s below margin of %s',
                    actual, expected, delta, margin)
    end
end

function M.assertItemsEquals(actual, expected)
    -- checks that the items of table expected
    -- are contained in table actual. Warning, this function
    -- is at least O(n^2)
    if not _is_table_items_equals(actual, expected ) then
        expected, actual = prettystrPairs(expected, actual)
        fail_fmt(2, 'Contents of the tables are not identical:\nExpected: %s\nActual: %s',
                 expected, actual)
    end
end

------------------------------------------------------------------
--                  String assertion
------------------------------------------------------------------

function M.assertStrContains( str, sub, useRe )
    -- this relies on lua string.find function
    -- a string always contains the empty string
    if not string.find(str, sub, 1, not useRe) then
        sub, str = prettystrPairs(sub, str, '\n')
        fail_fmt(2, 'Error, %s %s was not found in string %s',
                 useRe and 'regexp' or 'substring', sub, str)
    end
end

function M.assertStrIContains( str, sub )
    -- this relies on lua string.find function
    -- a string always contains the empty string
    if not string.find(str:lower(), sub:lower(), 1, true) then
        sub, str = prettystrPairs(sub, str, '\n')
        fail_fmt(2, 'Error, substring %s was not found (case insensitively) in string %s',
                 sub, str)
    end
end

function M.assertNotStrContains( str, sub, useRe )
    -- this relies on lua string.find function
    -- a string always contains the empty string
    if string.find(str, sub, 1, not useRe) then
        sub, str = prettystrPairs(sub, str, '\n')
        fail_fmt(2, 'Error, %s %s was found in string %s',
                 useRe and 'regexp' or 'substring', sub, str)
    end
end

function M.assertNotStrIContains( str, sub )
    -- this relies on lua string.find function
    -- a string always contains the empty string
    if string.find(str:lower(), sub:lower(), 1, true) then
        sub, str = prettystrPairs(sub, str, '\n')
        fail_fmt(2, 'Error, substring %s was found (case insensitively) in string %s',
                 sub, str)
    end
end

function M.assertStrMatches( str, pattern, start, final )
    -- Verify a full match for the string
    -- for a partial match, simply use assertStrContains with useRe set to true
    if not strMatch( str, pattern, start, final ) then
        pattern, str = prettystrPairs(pattern, str, '\n')
        fail_fmt(2, 'Error, pattern %s was not matched by string %s',
                 pattern, str)
    end
end

function M.assertErrorMsgEquals( expectedMsg, func, ... )
    -- assert that calling f with the arguments will raise an error
    -- example: assertError( f, 1, 2 ) => f(1,2) should generate an error
    local no_error, error_msg = pcall( func, ... )
    if no_error then
        failure( 'No error generated when calling function but expected error: "'..expectedMsg..'"', 2 )
    end
    if type(expectedMsg) == "string" and type(error_msg) ~= "string" then
        error_msg = tostring(error_msg)
    end
    local differ = false
    if error_msg ~= expectedMsg then
        local tr = type(error_msg)
        local te = type(expectedMsg)
        if te == 'table' then
            if tr ~= 'table' then
                differ = true
            else
                 local ok = pcall(M.assertItemsEquals, error_msg, expectedMsg)
                 if not ok then
                     differ = true
                 end
            end
        else
           differ = true
        end
    end

    if differ then
        error_msg, expectedMsg = prettystrPairs(error_msg, expectedMsg)
        fail_fmt(2, 'Exact error message expected: %s\nError message received: %s\n',
                 expectedMsg, error_msg)
    end
end

function M.assertErrorMsgContains( partialMsg, func, ... )
    -- assert that calling f with the arguments will raise an error
    -- example: assertError( f, 1, 2 ) => f(1,2) should generate an error
    local no_error, error_msg = pcall( func, ... )
    if no_error then
        failure( 'No error generated when calling function but expected error containing: '..prettystr(partialMsg), 2 )
    end
    if type(error_msg) ~= "string" then
        error_msg = tostring(error_msg)
    end
    if not string.find( error_msg, partialMsg, nil, true ) then
        error_msg, partialMsg = prettystrPairs(error_msg, partialMsg)
        fail_fmt(2, 'Error message does not contain: %s\nError message received: %s\n',
                 partialMsg, error_msg)
    end
end

function M.assertErrorMsgMatches( expectedMsg, func, ... )
    -- assert that calling f with the arguments will raise an error
    -- example: assertError( f, 1, 2 ) => f(1,2) should generate an error
    local no_error, error_msg = pcall( func, ... )
    if no_error then
        failure( 'No error generated when calling function but expected error matching: "'..expectedMsg..'"', 2 )
    end
    if type(error_msg) ~= "string" then
        error_msg = tostring(error_msg)
    end
    if not strMatch( error_msg, expectedMsg ) then
        expectedMsg, error_msg = prettystrPairs(expectedMsg, error_msg)
        fail_fmt(2, 'Error message does not match: %s\nError message received: %s\n',
                 expectedMsg, error_msg)
    end
end

------------------------------------------------------------------
--              Type assertions
------------------------------------------------------------------

function M.assertEvalToTrue(value)
    if not value then
        failure("expected: a value evaluating to true, actual: " ..prettystr(value), 2)
    end
end

function M.assertEvalToFalse(value)
    if value then
        failure("expected: false or nil, actual: " ..prettystr(value), 2)
    end
end

function M.assertIsTrue(value)
    if value ~= true then
        failure("expected: true, actual: " ..prettystr(value), 2)
    end
end

function M.assertNotIsTrue(value)
    if value == true then
        failure("expected: anything but true, actual: " ..prettystr(value), 2)
    end
end

function M.assertIsFalse(value)
    if value ~= false then
        failure("expected: false, actual: " ..prettystr(value), 2)
    end
end

function M.assertNotIsFalse(value)
    if value == false then
        failure("expected: anything but false, actual: " ..prettystr(value), 2)
    end
end

function M.assertIsNil(value)
    if value ~= nil then
        failure("expected: nil, actual: " ..prettystr(value), 2)
    end
end

function M.assertNotIsNil(value)
    if value == nil then
        failure("expected non nil value, received nil", 2)
    end
end

--[[
Add type assertion functions to the module table M. Each of these functions
takes a single parameter "value", and checks that its Lua type matches the
expected string (derived from the function name):

M.assertIsXxx(value) -> ensure that type(value) conforms to "xxx"
]]
for _, funcName in ipairs(
    {'assertIsNumber', 'assertIsString', 'assertIsTable', 'assertIsBoolean',
     'assertIsFunction', 'assertIsUserdata', 'assertIsThread'}
) do
    local typeExpected = funcName:match("^assertIs([A-Z]%a*)$")
    -- Lua type() always returns lowercase, also make sure the match() succeeded
    typeExpected = typeExpected and typeExpected:lower()
                   or error("bad function name '"..funcName.."' for type assertion")

    M[funcName] = function(value)
        if type(value) ~= typeExpected then
            fail_fmt(2, 'Expected: a %s value, actual: type %s, value %s',
                     typeExpected, type(value), prettystrPairs(value))
        end
    end
end

--[[
Add shortcuts for verifying type of a variable, without failure (luaunit v2 compatibility)
M.isXxx(value) -> returns true if type(value) conforms to "xxx"
]]
for _, typeExpected in ipairs(
    {'Number', 'String', 'Table', 'Boolean',
     'Function', 'Userdata', 'Thread', 'Nil' }
) do
    local typeExpectedLower = typeExpected:lower()
    local isType = function(value)
        return (type(value) == typeExpectedLower)
    end
    M['is'..typeExpected] = isType
    M['is_'..typeExpectedLower] = isType
end

--[[
Add non-type assertion functions to the module table M. Each of these functions
takes a single parameter "value", and checks that its Lua type differs from the
expected string (derived from the function name):

M.assertNotIsXxx(value) -> ensure that type(value) is not "xxx"
]]
for _, funcName in ipairs(
    {'assertNotIsNumber', 'assertNotIsString', 'assertNotIsTable', 'assertNotIsBoolean',
     'assertNotIsFunction', 'assertNotIsUserdata', 'assertNotIsThread'}
) do
    local typeUnexpected = funcName:match("^assertNotIs([A-Z]%a*)$")
    -- Lua type() always returns lowercase, also make sure the match() succeeded
    typeUnexpected = typeUnexpected and typeUnexpected:lower()
                   or error("bad function name '"..funcName.."' for type assertion")

    M[funcName] = function(value)
        if type(value) == typeUnexpected then
            fail_fmt(2, 'Not expected: a %s type, actual: value %s',
                     typeUnexpected, prettystrPairs(value))
        end
    end
end

function M.assertIs(actual, expected)
    if actual ~= expected then
        if not M.ORDER_ACTUAL_EXPECTED then
            actual, expected = expected, actual
        end
        expected, actual = prettystrPairs(expected, actual, '\n', ', ')
        fail_fmt(2, 'Expected object and actual object are not the same\nExpected: %sactual: %s',
                 expected, actual)
    end
end

function M.assertNotIs(actual, expected)
    if actual == expected then
        if not M.ORDER_ACTUAL_EXPECTED then
            expected = actual
        end
        fail_fmt(2, 'Expected object and actual object are the same object: %s',
                 prettystrPairs(expected))
    end
end


------------------------------------------------------------------
--              Scientific assertions
------------------------------------------------------------------


function M.assertIsNaN(value)
    if type(value) ~= "number" or value == value then
        failure("expected: nan, actual: " ..prettystr(value), 2)
    end
end

function M.assertNotIsNaN(value)
    if type(value) == "number" and value ~= value then
        failure("expected non nan value, received nan", 2)
    end
end

function M.assertIsInf(value)
    if type(value) ~= "number" or math.abs(value) ~= math.huge then
        failure("expected: inf, actual: " ..prettystr(value), 2)
    end
end

function M.assertIsPlusInf(value)
    if type(value) ~= "number" or value ~= math.huge then
        failure("expected: +inf, actual: " ..prettystr(value), 2)
    end
end

function M.assertIsMinusInf(value)
    if type(value) ~= "number" or value ~= -math.huge then
        failure("expected: -inf, actual: " ..prettystr(value), 2)
    end
end

function M.assertNotIsPlusInf(value)
    if type(value) == "number" and value == math.huge then
        failure("expected not +inf value, received +inf", 2)
    end
end

function M.assertNotIsMinusInf(value)
    if type(value) == "number" and value == -math.huge then
        failure("expected not -inf value, received -inf", 2)
    end
end

function M.assertNotIsInf(value)
    if type(value) == "number" and math.abs(value) == math.huge then
        failure("expected non inf value, received ±inf", 2)
    end
end

function M.assertIsPlusZero(value)
    if type(value) ~= 'number' or value ~= 0 then
        failure("expected: +0.0, actual: " ..prettystr(value), 2)
    else if (1/value == -math.huge) then
            -- more precise error diagnosis
            failure("expected: +0.0, actual: -0.0", 2)
        else if (1/value ~= math.huge) then
                -- strange, case should have already been covered
                failure("expected: +0.0, actual: " ..prettystr(value), 2)
            end
        end
    end
end

function M.assertIsMinusZero(value)
    if type(value) ~= 'number' or value ~= 0 then
        failure("expected: -0.0, actual: " ..prettystr(value), 2)
    else if (1/value == math.huge) then
            -- more precise error diagnosis
            failure("expected: -0.0, actual: +0.0", 2)
        else if (1/value ~= -math.huge) then
                -- strange, case should have already been covered
                failure("expected: -0.0, actual: " ..prettystr(value), 2)
            end
        end
    end
end

function M.assertNotIsPlusZero(value)
    if type(value) == 'number' and value == 0 and (1/value ~= math.huge) then
        failure("expected: not +0.0, actual: +0.0", 2)
    end
end

function M.assertNotIsMinusZero(value)
    if type(value) == 'number' and value == 0 and (1/value ~= -math.huge) then
        failure("expected: not +0.0, actual: +0.0", 2)
    end
end

----------------------------------------------------------------
--                     Compatibility layer
----------------------------------------------------------------

-- for compatibility with LuaUnit v2.x
function M.wrapFunctions()
    -- In LuaUnit version <= 2.1 , this function was necessary to include
    -- a test function inside the global test suite. Nowadays, the functions
    -- are simply run directly as part of the test discovery process.
    -- so just do nothing !
    io.stderr:write[[Use of WrapFunctions() is no longer needed.
Just prefix your test function names with "test" or "Test" and they
will be picked up and run by LuaUnit.
]]
end

local list_of_funcs = {
    -- { official function name , alias }

    -- general assertions
    { 'assertEquals'            , 'assert_equals' },
    { 'assertItemsEquals'       , 'assert_items_equals' },
    { 'assertNotEquals'         , 'assert_not_equals' },
    { 'assertAlmostEquals'      , 'assert_almost_equals' },
    { 'assertNotAlmostEquals'   , 'assert_not_almost_equals' },
    { 'assertEvalToTrue'        , 'assert_eval_to_true' },
    { 'assertEvalToFalse'       , 'assert_eval_to_false' },
    { 'assertStrContains'       , 'assert_str_contains' },
    { 'assertStrIContains'      , 'assert_str_icontains' },
    { 'assertNotStrContains'    , 'assert_not_str_contains' },
    { 'assertNotStrIContains'   , 'assert_not_str_icontains' },
    { 'assertStrMatches'        , 'assert_str_matches' },
    { 'assertError'             , 'assert_error' },
    { 'assertErrorMsgEquals'    , 'assert_error_msg_equals' },
    { 'assertErrorMsgContains'  , 'assert_error_msg_contains' },
    { 'assertErrorMsgMatches'   , 'assert_error_msg_matches' },
    { 'assertIs'                , 'assert_is' },
    { 'assertNotIs'             , 'assert_not_is' },
    { 'wrapFunctions'           , 'WrapFunctions' },
    { 'wrapFunctions'           , 'wrap_functions' },

    -- type assertions: assertIsXXX -> assert_is_xxx
    { 'assertIsNumber'          , 'assert_is_number' },
    { 'assertIsString'          , 'assert_is_string' },
    { 'assertIsTable'           , 'assert_is_table' },
    { 'assertIsBoolean'         , 'assert_is_boolean' },
    { 'assertIsNil'             , 'assert_is_nil' },
    { 'assertIsTrue'            , 'assert_is_true' },
    { 'assertIsFalse'           , 'assert_is_false' },
    { 'assertIsNaN'             , 'assert_is_nan' },
    { 'assertIsInf'             , 'assert_is_inf' },
    { 'assertIsPlusInf'         , 'assert_is_plus_inf' },
    { 'assertIsMinusInf'        , 'assert_is_minus_inf' },
    { 'assertIsPlusZero'        , 'assert_is_plus_zero' },
    { 'assertIsMinusZero'       , 'assert_is_minus_zero' },
    { 'assertIsFunction'        , 'assert_is_function' },
    { 'assertIsThread'          , 'assert_is_thread' },
    { 'assertIsUserdata'        , 'assert_is_userdata' },

    -- type assertions: assertIsXXX -> assertXxx
    { 'assertIsNumber'          , 'assertNumber' },
    { 'assertIsString'          , 'assertString' },
    { 'assertIsTable'           , 'assertTable' },
    { 'assertIsBoolean'         , 'assertBoolean' },
    { 'assertIsNil'             , 'assertNil' },
    { 'assertIsTrue'            , 'assertTrue' },
    { 'assertIsFalse'           , 'assertFalse' },
    { 'assertIsNaN'             , 'assertNaN' },
    { 'assertIsInf'             , 'assertInf' },
    { 'assertIsPlusInf'         , 'assertPlusInf' },
    { 'assertIsMinusInf'        , 'assertMinusInf' },
    { 'assertIsPlusZero'        , 'assertPlusZero' },
    { 'assertIsMinusZero'       , 'assertMinusZero'},
    { 'assertIsFunction'        , 'assertFunction' },
    { 'assertIsThread'          , 'assertThread' },
    { 'assertIsUserdata'        , 'assertUserdata' },

    -- type assertions: assertIsXXX -> assert_xxx (luaunit v2 compat)
    { 'assertIsNumber'          , 'assert_number' },
    { 'assertIsString'          , 'assert_string' },
    { 'assertIsTable'           , 'assert_table' },
    { 'assertIsBoolean'         , 'assert_boolean' },
    { 'assertIsNil'             , 'assert_nil' },
    { 'assertIsTrue'            , 'assert_true' },
    { 'assertIsFalse'           , 'assert_false' },
    { 'assertIsNaN'             , 'assert_nan' },
    { 'assertIsInf'             , 'assert_inf' },
    { 'assertIsPlusInf'         , 'assert_plus_inf' },
    { 'assertIsMinusInf'        , 'assert_minus_inf' },
    { 'assertIsPlusZero'        , 'assert_plus_zero' },
    { 'assertIsMinusZero'       , 'assert_minus_zero' },
    { 'assertIsFunction'        , 'assert_function' },
    { 'assertIsThread'          , 'assert_thread' },
    { 'assertIsUserdata'        , 'assert_userdata' },

    -- type assertions: assertNotIsXXX -> assert_not_is_xxx
    { 'assertNotIsNumber'       , 'assert_not_is_number' },
    { 'assertNotIsString'       , 'assert_not_is_string' },
    { 'assertNotIsTable'        , 'assert_not_is_table' },
    { 'assertNotIsBoolean'      , 'assert_not_is_boolean' },
    { 'assertNotIsNil'          , 'assert_not_is_nil' },
    { 'assertNotIsTrue'         , 'assert_not_is_true' },
    { 'assertNotIsFalse'        , 'assert_not_is_false' },
    { 'assertNotIsNaN'          , 'assert_not_is_nan' },
    { 'assertNotIsInf'          , 'assert_not_is_inf' },
    { 'assertNotIsPlusInf'      , 'assert_not_plus_inf' },
    { 'assertNotIsMinusInf'     , 'assert_not_minus_inf' },
    { 'assertNotIsPlusZero'     , 'assert_not_plus_zero' },
    { 'assertNotIsMinusZero'    , 'assert_not_minus_zero' },
    { 'assertNotIsFunction'     , 'assert_not_is_function' },
    { 'assertNotIsThread'       , 'assert_not_is_thread' },
    { 'assertNotIsUserdata'     , 'assert_not_is_userdata' },

    -- type assertions: assertNotIsXXX -> assertNotXxx (luaunit v2 compat)
    { 'assertNotIsNumber'       , 'assertNotNumber' },
    { 'assertNotIsString'       , 'assertNotString' },
    { 'assertNotIsTable'        , 'assertNotTable' },
    { 'assertNotIsBoolean'      , 'assertNotBoolean' },
    { 'assertNotIsNil'          , 'assertNotNil' },
    { 'assertNotIsTrue'         , 'assertNotTrue' },
    { 'assertNotIsFalse'        , 'assertNotFalse' },
    { 'assertNotIsNaN'          , 'assertNotNaN' },
    { 'assertNotIsInf'          , 'assertNotInf' },
    { 'assertNotIsPlusInf'      , 'assertNotPlusInf' },
    { 'assertNotIsMinusInf'     , 'assertNotMinusInf' },
    { 'assertNotIsPlusZero'     , 'assertNotPlusZero' },
    { 'assertNotIsMinusZero'    , 'assertNotMinusZero' },
    { 'assertNotIsFunction'     , 'assertNotFunction' },
    { 'assertNotIsThread'       , 'assertNotThread' },
    { 'assertNotIsUserdata'     , 'assertNotUserdata' },

    -- type assertions: assertNotIsXXX -> assert_not_xxx
    { 'assertNotIsNumber'       , 'assert_not_number' },
    { 'assertNotIsString'       , 'assert_not_string' },
    { 'assertNotIsTable'        , 'assert_not_table' },
    { 'assertNotIsBoolean'      , 'assert_not_boolean' },
    { 'assertNotIsNil'          , 'assert_not_nil' },
    { 'assertNotIsTrue'         , 'assert_not_true' },
    { 'assertNotIsFalse'        , 'assert_not_false' },
    { 'assertNotIsNaN'          , 'assert_not_nan' },
    { 'assertNotIsInf'          , 'assert_not_inf' },
    { 'assertNotIsPlusInf'      , 'assert_not_plus_inf' },
    { 'assertNotIsMinusInf'     , 'assert_not_minus_inf' },
    { 'assertNotIsPlusZero'     , 'assert_not_plus_zero' },
    { 'assertNotIsMinusZero'    , 'assert_not_minus_zero' },
    { 'assertNotIsFunction'     , 'assert_not_function' },
    { 'assertNotIsThread'       , 'assert_not_thread' },
    { 'assertNotIsUserdata'     , 'assert_not_userdata' },

    -- all assertions with Coroutine duplicate Thread assertions
    { 'assertIsThread'          , 'assertIsCoroutine' },
    { 'assertIsThread'          , 'assertCoroutine' },
    { 'assertIsThread'          , 'assert_is_coroutine' },
    { 'assertIsThread'          , 'assert_coroutine' },
    { 'assertNotIsThread'       , 'assertNotIsCoroutine' },
    { 'assertNotIsThread'       , 'assertNotCoroutine' },
    { 'assertNotIsThread'       , 'assert_not_is_coroutine' },
    { 'assertNotIsThread'       , 'assert_not_coroutine' },
}

-- Create all aliases in M
for _,v in ipairs( list_of_funcs ) do
    local funcname, alias = v[1], v[2]
    M[alias] = M[funcname]

    if EXPORT_ASSERT_TO_GLOBALS then
        _G[funcname] = M[funcname]
        _G[alias] = M[funcname]
    end
end

----------------------------------------------------------------
--
--                     Outputters
--
----------------------------------------------------------------

-- A common "base" class for outputters
-- For concepts involved (class inheritance) see http://www.lua.org/pil/16.2.html

local genericOutput = { __class__ = 'genericOutput' } -- class
local genericOutput_MT = { __index = genericOutput } -- metatable
M.genericOutput = genericOutput -- publish, so that custom classes may derive from it

function genericOutput.new(runner, default_verbosity)
    -- runner is the "parent" object controlling the output, usually a LuaUnit instance
    local t = { runner = runner }
    if runner then
        t.result = runner.result
        t.verbosity = runner.verbosity or default_verbosity
        t.fname = runner.fname
    else
        t.verbosity = default_verbosity
    end
    return setmetatable( t, genericOutput_MT)
end

-- abstract ("empty") methods
function genericOutput:startSuite() end
function genericOutput:startClass(className) end
function genericOutput:startTest(testName) end
function genericOutput:addStatus(node) end
function genericOutput:endTest(node) end
function genericOutput:endClass() end
function genericOutput:endSuite() end


----------------------------------------------------------------
--                     class TapOutput
----------------------------------------------------------------

local TapOutput = genericOutput.new() -- derived class
local TapOutput_MT = { __index = TapOutput } -- metatable
TapOutput.__class__ = 'TapOutput'

    -- For a good reference for TAP format, check: http://testanything.org/tap-specification.html

    function TapOutput.new(runner)
        local t = genericOutput.new(runner, M.VERBOSITY_LOW)
        return setmetatable( t, TapOutput_MT)
    end
    function TapOutput:startSuite()
        print("1.."..self.result.testCount)
        print('# Started on '..self.result.startDate)
    end
    function TapOutput:startClass(className)
        if className ~= '[TestFunctions]' then
            print('# Starting class: '..className)
        end
    end

    function TapOutput:addStatus( node )
        io.stdout:write("not ok ", self.result.currentTestNumber, "\t", node.testName, "\n")
        if self.verbosity > M.VERBOSITY_LOW then
           print( prefixString( '#   ', node.msg ) )
        end
        if self.verbosity > M.VERBOSITY_DEFAULT then
           print( prefixString( '#   ', node.stackTrace ) )
        end
    end

    function TapOutput:endTest( node )
        if node:isPassed() then
            io.stdout:write("ok     ", self.result.currentTestNumber, "\t", node.testName, "\n")
        end
    end

    function TapOutput:endSuite()
        print( '# '..M.LuaUnit.statusLine( self.result ) )
        return self.result.notPassedCount
    end


-- class TapOutput end

----------------------------------------------------------------
--                     class JUnitOutput
----------------------------------------------------------------

-- See directory junitxml for more information about the junit format
local JUnitOutput = genericOutput.new() -- derived class
local JUnitOutput_MT = { __index = JUnitOutput } -- metatable
JUnitOutput.__class__ = 'JUnitOutput'

    function JUnitOutput.new(runner)
        local t = genericOutput.new(runner, M.VERBOSITY_LOW)
        t.testList = {}
        return setmetatable( t, JUnitOutput_MT )
    end

    function JUnitOutput:startSuite()
        -- open xml file early to deal with errors
        if self.fname == nil then
            error('With Junit, an output filename must be supplied with --name!')
        end
        if string.sub(self.fname,-4) ~= '.xml' then
            self.fname = self.fname..'.xml'
        end
        self.fd = io.open(self.fname, "w")
        if self.fd == nil then
            error("Could not open file for writing: "..self.fname)
        end

        print('# XML output to '..self.fname)
        print('# Started on '..self.result.startDate)
    end
    function JUnitOutput:startClass(className)
        if className ~= '[TestFunctions]' then
            print('# Starting class: '..className)
        end
    end
    function JUnitOutput:startTest(testName)
        print('# Starting test: '..testName)
    end

    function JUnitOutput:addStatus( node )
        if node:isFailure() then
            print( '#   Failure: ' .. prefixString( '#   ', node.msg ):sub(4, nil) )
            -- print('# ' .. node.stackTrace)
        elseif node:isError() then
            print( '#   Error: ' .. prefixString( '#   '  , node.msg ):sub(4, nil) )
            -- print('# ' .. node.stackTrace)
        end
    end

    function JUnitOutput:endSuite()
        print( '# '..M.LuaUnit.statusLine(self.result))

        -- XML file writing
        self.fd:write('<?xml version="1.0" encoding="UTF-8" ?>\n')
        self.fd:write('<testsuites>\n')
        self.fd:write(string.format(
            '    <testsuite name="LuaUnit" id="00001" package="" hostname="localhost" tests="%d" timestamp="%s" time="%0.3f" errors="%d" failures="%d">\n',
            self.result.runCount, self.result.startIsodate, self.result.duration, self.result.errorCount, self.result.failureCount ))
        self.fd:write("        <properties>\n")
        self.fd:write(string.format('            <property name="Lua Version" value="%s"/>\n', _VERSION ) )
        self.fd:write(string.format('            <property name="LuaUnit Version" value="%s"/>\n', M.VERSION) )
        -- XXX please include system name and version if possible
        self.fd:write("        </properties>\n")

        for i,node in ipairs(self.result.tests) do
            self.fd:write(string.format('        <testcase classname="%s" name="%s" time="%0.3f">\n',
                node.className, node.testName, node.duration ) )
            if node:isNotPassed() then
                self.fd:write(node:statusXML())
            end
            self.fd:write('        </testcase>\n')
        end

        -- Next two lines are needed to validate junit ANT xsd, but really not useful in general:
        self.fd:write('    <system-out/>\n')
        self.fd:write('    <system-err/>\n')

        self.fd:write('    </testsuite>\n')
        self.fd:write('</testsuites>\n')
        self.fd:close()
        return self.result.notPassedCount
    end


-- class TapOutput end

----------------------------------------------------------------
--                     class TextOutput
----------------------------------------------------------------

--[[

-- Python Non verbose:

For each test: . or F or E

If some failed tests:
    ==============
    ERROR / FAILURE: TestName (testfile.testclass)
    ---------
    Stack trace


then --------------
then "Ran x tests in 0.000s"
then OK or FAILED (failures=1, error=1)

-- Python Verbose:
testname (filename.classname) ... ok
testname (filename.classname) ... FAIL
testname (filename.classname) ... ERROR

then --------------
then "Ran x tests in 0.000s"
then OK or FAILED (failures=1, error=1)

-- Ruby:
Started
 .
 Finished in 0.002695 seconds.

 1 tests, 2 assertions, 0 failures, 0 errors

-- Ruby:
>> ruby tc_simple_number2.rb
Loaded suite tc_simple_number2
Started
F..
Finished in 0.038617 seconds.

  1) Failure:
test_failure(TestSimpleNumber) [tc_simple_number2.rb:16]:
Adding doesn't work.
<3> expected but was
<4>.

3 tests, 4 assertions, 1 failures, 0 errors

-- Java Junit
.......F.
Time: 0,003
There was 1 failure:
1) testCapacity(junit.samples.VectorTest)junit.framework.AssertionFailedError
    at junit.samples.VectorTest.testCapacity(VectorTest.java:87)
    at sun.reflect.NativeMethodAccessorImpl.invoke0(Native Method)
    at sun.reflect.NativeMethodAccessorImpl.invoke(NativeMethodAccessorImpl.java:62)
    at sun.reflect.DelegatingMethodAccessorImpl.invoke(DelegatingMethodAccessorImpl.java:43)

FAILURES!!!
Tests run: 8,  Failures: 1,  Errors: 0


-- Maven

# mvn test
-------------------------------------------------------
 T E S T S
-------------------------------------------------------
Running math.AdditionTest
Tests run: 2, Failures: 1, Errors: 0, Skipped: 0, Time elapsed:
0.03 sec <<< FAILURE!

Results :

Failed tests:
  testLireSymbole(math.AdditionTest)

Tests run: 2, Failures: 1, Errors: 0, Skipped: 0


-- LuaUnit
---- non verbose
* display . or F or E when running tests
---- verbose
* display test name + ok/fail
----
* blank line
* number) ERROR or FAILURE: TestName
   Stack trace
* blank line
* number) ERROR or FAILURE: TestName
   Stack trace

then --------------
then "Ran x tests in 0.000s (%d not selected, %d skipped)"
then OK or FAILED (failures=1, error=1)


]]

local TextOutput = genericOutput.new() -- derived class
local TextOutput_MT = { __index = TextOutput } -- metatable
TextOutput.__class__ = 'TextOutput'

    function TextOutput.new(runner)
        local t = genericOutput.new(runner, M.VERBOSITY_DEFAULT)
        t.errorList = {}
        return setmetatable( t, TextOutput_MT )
    end

    function TextOutput:startSuite()
        if self.verbosity > M.VERBOSITY_DEFAULT then
            print( 'Started on '.. self.result.startDate )
        end
    end

    function TextOutput:startTest(testName)
        if self.verbosity > M.VERBOSITY_DEFAULT then
            io.stdout:write( "    ", self.result.currentNode.testName, " ... " )
        end
    end

    function TextOutput:endTest( node )
        if node:isPassed() then
            if self.verbosity > M.VERBOSITY_DEFAULT then
                io.stdout:write("Ok\n")
            else
                io.stdout:write(".")
            end
        else
            if self.verbosity > M.VERBOSITY_DEFAULT then
                print( node.status )
                print( node.msg )
                --[[
                -- find out when to do this:
                if self.verbosity > M.VERBOSITY_DEFAULT then
                    print( node.stackTrace )
                end
                ]]
            else
                -- write only the first character of status
                io.stdout:write(string.sub(node.status, 1, 1))
            end
        end
    end

    function TextOutput:displayOneFailedTest( index, fail )
        print(index..") "..fail.testName )
        print( fail.msg )
        print( fail.stackTrace )
        print()
    end

    function TextOutput:displayFailedTests()
        if self.result.notPassedCount ~= 0 then
            print("Failed tests:")
            print("-------------")
            for i, v in ipairs(self.result.notPassed) do
                self:displayOneFailedTest(i, v)
            end
        end
    end

    function TextOutput:endSuite()
        if self.verbosity > M.VERBOSITY_DEFAULT then
            print("=========================================================")
        else
            print()
        end
        self:displayFailedTests()
        print( M.LuaUnit.statusLine( self.result ) )
        if self.result.notPassedCount == 0 then
            print('OK')
        end
    end

-- class TextOutput end


----------------------------------------------------------------
--                     class NilOutput
----------------------------------------------------------------

local function nopCallable()
    --print(42)
    return nopCallable
end

local NilOutput = { __class__ = 'NilOuptut' } -- class
local NilOutput_MT = { __index = nopCallable } -- metatable

function NilOutput.new(runner)
    return setmetatable( { __class__ = 'NilOutput' }, NilOutput_MT )
end

----------------------------------------------------------------
--
--                     class LuaUnit
--
----------------------------------------------------------------

M.LuaUnit = {
    outputType = TextOutput,
    verbosity = M.VERBOSITY_DEFAULT,
    __class__ = 'LuaUnit'
}
local LuaUnit_MT = { __index = M.LuaUnit }

if EXPORT_ASSERT_TO_GLOBALS then
    LuaUnit = M.LuaUnit
end

    function M.LuaUnit.new()
        return setmetatable( {}, LuaUnit_MT )
    end

    -----------------[[ Utility methods ]]---------------------

    function M.LuaUnit.asFunction(aObject)
        -- return "aObject" if it is a function, and nil otherwise
        if 'function' == type(aObject) then
            return aObject
        end
    end

    function M.LuaUnit.splitClassMethod(someName)
        --[[
        Return a pair of className, methodName strings for a name in the form
        "class.method". If no class part (or separator) is found, will return
        nil, someName instead (the latter being unchanged).

        This convention thus also replaces the older isClassMethod() test:
        You just have to check for a non-nil className (return) value.
        ]]
        local separator = string.find(someName, '.', 1, true)
        if separator then
            return someName:sub(1, separator - 1), someName:sub(separator + 1)
        end
        return nil, someName
    end

    function M.LuaUnit.isMethodTestName( s )
        -- return true is the name matches the name of a test method
        -- default rule is that is starts with 'Test' or with 'test'
        return string.sub(s, 1, 4):lower() == 'test'
    end

    function M.LuaUnit.isTestName( s )
        -- return true is the name matches the name of a test
        -- default rule is that is starts with 'Test' or with 'test'
        return string.sub(s, 1, 4):lower() == 'test'
    end

    function M.LuaUnit.collectTests()
        -- return a list of all test names in the global namespace
        -- that match LuaUnit.isTestName

        local testNames = {}
        for k, _ in pairs(_G) do
            if type(k) == "string" and M.LuaUnit.isTestName( k ) then
                table.insert( testNames , k )
            end
        end
        table.sort( testNames )
        return testNames
    end

    function M.LuaUnit.parseCmdLine( cmdLine )
        -- parse the command line
        -- Supported command line parameters:
        -- --verbose, -v: increase verbosity
        -- --quiet, -q: silence output
        -- --error, -e: treat errors as fatal (quit program)
        -- --output, -o, + name: select output type
        -- --pattern, -p, + pattern: run test matching pattern, may be repeated
        -- --exclude, -x, + pattern: run test not matching pattern, may be repeated
        -- --shuffle, -s, : shuffle tests before reunning them
        -- --name, -n, + fname: name of output file for junit, default to stdout
        -- --repeat, -r, + num: number of times to execute each test
        -- [testnames, ...]: run selected test names
        --
        -- Returns a table with the following fields:
        -- verbosity: nil, M.VERBOSITY_DEFAULT, M.VERBOSITY_QUIET, M.VERBOSITY_VERBOSE
        -- output: nil, 'tap', 'junit', 'text', 'nil'
        -- testNames: nil or a list of test names to run
        -- exeRepeat: num or 1
        -- pattern: nil or a list of patterns
        -- exclude: nil or a list of patterns

        local result, state = {}, nil
        local SET_OUTPUT = 1
        local SET_PATTERN = 2
        local SET_EXCLUDE = 3
        local SET_FNAME = 4
        local SET_REPEAT = 5

        if cmdLine == nil then
            return result
        end

        local function parseOption( option )
            if option == '--help' or option == '-h' then
                result['help'] = true
                return
            elseif option == '--version' then
                result['version'] = true
                return
            elseif option == '--verbose' or option == '-v' then
                result['verbosity'] = M.VERBOSITY_VERBOSE
                return
            elseif option == '--quiet' or option == '-q' then
                result['verbosity'] = M.VERBOSITY_QUIET
                return
            elseif option == '--error' or option == '-e' then
                result['quitOnError'] = true
                return
            elseif option == '--failure' or option == '-f' then
                result['quitOnFailure'] = true
                return
            elseif option == '--shuffle' or option == '-s' then
                result['shuffle'] = true
                return
            elseif option == '--output' or option == '-o' then
                state = SET_OUTPUT
                return state
            elseif option == '--name' or option == '-n' then
                state = SET_FNAME
                return state
            elseif option == '--repeat' or option == '-r' then
                state = SET_REPEAT
                return state
            elseif option == '--pattern' or option == '-p' then
                state = SET_PATTERN
                return state
            elseif option == '--exclude' or option == '-x' then
                state = SET_EXCLUDE
                return state
            end
            error('Unknown option: '..option,3)
        end

        local function setArg( cmdArg, state )
            if state == SET_OUTPUT then
                result['output'] = cmdArg
                return
            elseif state == SET_FNAME then
                result['fname'] = cmdArg
                return
            elseif state == SET_REPEAT then
                result['exeRepeat'] = tonumber(cmdArg)
                                     or error('Malformed -r argument: '..cmdArg)
                return
            elseif state == SET_PATTERN then
                if result['pattern'] then
                    table.insert( result['pattern'], cmdArg )
                else
                    result['pattern'] = { cmdArg }
                end
                return
            elseif state == SET_EXCLUDE then
                local notArg = '!'..cmdArg
                if result['pattern'] then
                    table.insert( result['pattern'],  notArg )
                else
                    result['pattern'] = { notArg }
                end
                return
            end
            error('Unknown parse state: '.. state)
        end


        for i, cmdArg in ipairs(cmdLine) do
            if state ~= nil then
                setArg( cmdArg, state, result )
                state = nil
            else
                if cmdArg:sub(1,1) == '-' then
                    state = parseOption( cmdArg )
                else
                    if result['testNames'] then
                        table.insert( result['testNames'], cmdArg )
                    else
                        result['testNames'] = { cmdArg }
                    end
                end
            end
        end

        if result['help'] then
            M.LuaUnit.help()
        end

        if result['version'] then
            M.LuaUnit.version()
        end

        if state ~= nil then
            error('Missing argument after '..cmdLine[ #cmdLine ],2 )
        end

        return result
    end

    function M.LuaUnit.help()
        print(M.USAGE)
        os.exit(0)
    end

    function M.LuaUnit.version()
        print('LuaUnit v'..M.VERSION..' by Philippe Fremy <phil@freehackers.org>')
        os.exit(0)
    end

----------------------------------------------------------------
--                     class NodeStatus
----------------------------------------------------------------

    local NodeStatus = { __class__ = 'NodeStatus' } -- class
    local NodeStatus_MT = { __index = NodeStatus } -- metatable
    M.NodeStatus = NodeStatus

    -- values of status
    NodeStatus.PASS  = 'PASS'
    NodeStatus.FAIL  = 'FAIL'
    NodeStatus.ERROR = 'ERROR'

    function NodeStatus.new( number, testName, className )
        local t = { number = number, testName = testName, className = className }
        setmetatable( t, NodeStatus_MT )
        t:pass()
        return t
    end

    function NodeStatus:pass()
        self.status = self.PASS
        -- useless but we know it's the field we want to use
        self.msg = nil
        self.stackTrace = nil
    end

    function NodeStatus:fail(msg, stackTrace)
        self.status = self.FAIL
        self.msg = msg
        self.stackTrace = stackTrace
    end

    function NodeStatus:error(msg, stackTrace)
        self.status = self.ERROR
        self.msg = msg
        self.stackTrace = stackTrace
    end

    function NodeStatus:isPassed()
        return self.status == NodeStatus.PASS
    end

    function NodeStatus:isNotPassed()
        -- print('hasFailure: '..prettystr(self))
        return self.status ~= NodeStatus.PASS
    end

    function NodeStatus:isFailure()
        return self.status == NodeStatus.FAIL
    end

    function NodeStatus:isError()
        return self.status == NodeStatus.ERROR
    end

    function NodeStatus:statusXML()
        if self:isError() then
            return table.concat(
                {'            <error type="', xmlEscape(self.msg), '">\n',
                 '                <![CDATA[', xmlCDataEscape(self.stackTrace),
                 ']]></error>\n'})
        elseif self:isFailure() then
            return table.concat(
                {'            <failure type="', xmlEscape(self.msg), '">\n',
                 '                <![CDATA[', xmlCDataEscape(self.stackTrace),
                 ']]></failure>\n'})
        end
        return '            <passed/>\n' -- (not XSD-compliant! normally shouldn't get here)
    end

    --------------[[ Output methods ]]-------------------------

    local function conditional_plural(number, singular)
        -- returns a grammatically well-formed string "%d <singular/plural>"
        local suffix = ''
        if number ~= 1 then -- use plural
            suffix = (singular:sub(-2) == 'ss') and 'es' or 's'
        end
        return string.format('%d %s%s', number, singular, suffix)
    end

    function M.LuaUnit.statusLine(result)
        -- return status line string according to results
        local s = {
            string.format('Ran %d tests in %0.3f seconds',
                          result.runCount, result.duration),
            conditional_plural(result.passedCount, 'success'),
        }
        if result.notPassedCount > 0 then
            if result.failureCount > 0 then
                table.insert(s, conditional_plural(result.failureCount, 'failure'))
            end
            if result.errorCount > 0 then
                table.insert(s, conditional_plural(result.errorCount, 'error'))
            end
        else
            table.insert(s, '0 failures')
        end
        if result.nonSelectedCount > 0 then
            table.insert(s, string.format("%d non-selected", result.nonSelectedCount))
        end
        return table.concat(s, ', ')
    end

    function M.LuaUnit:startSuite(testCount, nonSelectedCount)
        self.result = {
            testCount = testCount,
            nonSelectedCount = nonSelectedCount,
            passedCount = 0,
            runCount = 0,
            currentTestNumber = 0,
            currentClassName = "",
            currentNode = nil,
            suiteStarted = true,
            startTime = os.clock(),
            startDate = os.date(os.getenv('LUAUNIT_DATEFMT')),
            startIsodate = os.date('%Y-%m-%dT%H:%M:%S'),
            patternIncludeFilter = self.patternIncludeFilter,
            tests = {},
            failures = {},
            errors = {},
            notPassed = {},
        }

        self.outputType = self.outputType or TextOutput
        self.output = self.outputType.new(self)
        self.output:startSuite()
    end

    function M.LuaUnit:startClass( className )
        self.result.currentClassName = className
        self.output:startClass( className )
    end

    function M.LuaUnit:startTest( testName  )
        self.result.currentTestNumber = self.result.currentTestNumber + 1
        self.result.runCount = self.result.runCount + 1
        self.result.currentNode = NodeStatus.new(
            self.result.currentTestNumber,
            testName,
            self.result.currentClassName
        )
        self.result.currentNode.startTime = os.clock()
        table.insert( self.result.tests, self.result.currentNode )
        self.output:startTest( testName )
    end

    function M.LuaUnit:addStatus( err )
        -- "err" is expected to be a table / result from protectedCall()
        if err.status == NodeStatus.PASS then
            return
        end

        local node = self.result.currentNode

        --[[ As a first approach, we will report only one error or one failure for one test.

        However, we can have the case where the test is in failure, and the teardown is in error.
        In such case, it's a good idea to report both a failure and an error in the test suite. This is
        what Python unittest does for example. However, it mixes up counts so need to be handled carefully: for
        example, there could be more (failures + errors) count that tests. What happens to the current node ?

        We will do this more intelligent version later.
        ]]

        -- if the node is already in failure/error, just don't report the new error (see above)
        if node.status ~= NodeStatus.PASS then
            return
        end

        if err.status == NodeStatus.FAIL then
            node:fail( err.msg, err.trace )
            table.insert( self.result.failures, node )
        elseif err.status == NodeStatus.ERROR then
            node:error( err.msg, err.trace )
            table.insert( self.result.errors, node )
        end

        if node:isFailure() or node:isError() then
            -- add to the list of failed tests (gets printed separately)
            table.insert( self.result.notPassed, node )
        end
        self.output:addStatus( node )
    end

    function M.LuaUnit:endTest()
        local node = self.result.currentNode
        -- print( 'endTest() '..prettystr(node))
        -- print( 'endTest() '..prettystr(node:isNotPassed()))
        node.duration = os.clock() - node.startTime
        node.startTime = nil
        self.output:endTest( node )

        if node:isPassed() then
            self.result.passedCount = self.result.passedCount + 1
        elseif node:isError() then
            if self.quitOnError or self.quitOnFailure then
                -- Runtime error - abort test execution as requested by
                -- "--error" option. This is done by setting a special
                -- flag that gets handled in runSuiteByInstances().
                print("\nERROR during LuaUnit test execution:\n" .. node.msg)
                self.result.aborted = true
            end
        elseif node:isFailure() then
            if self.quitOnFailure then
                -- Failure - abort test execution as requested by
                -- "--failure" option. This is done by setting a special
                -- flag that gets handled in runSuiteByInstances().
                print("\nFailure during LuaUnit test execution:\n" .. node.msg)
                self.result.aborted = true
            end
        end
        self.result.currentNode = nil
    end

    function M.LuaUnit:endClass()
        self.output:endClass()
    end

    function M.LuaUnit:endSuite()
        if self.result.suiteStarted == false then
            error('LuaUnit:endSuite() -- suite was already ended' )
        end
        self.result.duration = os.clock()-self.result.startTime
        self.result.suiteStarted = false

        -- Expose test counts for outputter's endSuite(). This could be managed
        -- internally instead, but unit tests (and existing use cases) might
        -- rely on these fields being present.
        self.result.notPassedCount = #self.result.notPassed
        self.result.failureCount = #self.result.failures
        self.result.errorCount = #self.result.errors

        self.output:endSuite()
    end

    function M.LuaUnit:setOutputType(outputType)
        -- default to text
        -- tap produces results according to TAP format
        if outputType:upper() == "NIL" then
            self.outputType = NilOutput
            return
        end
        if outputType:upper() == "TAP" then
            self.outputType = TapOutput
            return
        end
        if outputType:upper() == "JUNIT" then
            self.outputType = JUnitOutput
            return
        end
        if outputType:upper() == "TEXT" then
            self.outputType = TextOutput
            return
        end
        error( 'No such format: '..outputType,2)
    end

    --------------[[ Runner ]]-----------------

    function M.LuaUnit:protectedCall(classInstance, methodInstance, prettyFuncName)
        -- if classInstance is nil, this is just a function call
        -- else, it's method of a class being called.

        local function err_handler(e)
            -- transform error into a table, adding the traceback information
            return {
                status = NodeStatus.ERROR,
                msg = e,
                trace = string.sub(debug.traceback("", 3), 2)
            }
        end

        local ok, err
        if classInstance then
            -- stupid Lua < 5.2 does not allow xpcall with arguments so let's use a workaround
            ok, err = xpcall( function () methodInstance(classInstance) end, err_handler )
        else
            ok, err = xpcall( function () methodInstance() end, err_handler )
        end
        if ok then
            return {status = NodeStatus.PASS}
        end

        -- Failure message usually looks like:
        -- "./test\\test_luaunit.lua:2241: LuaUnit test FAILURE: expected: 2, actual: 1"

        -- If failure prefix is present, we assume this is a failure
        -- we strip the prefix, and insert the iteration number along the way if relevant
        -- we only strip one failure prefix of course.

        local failed, iter_msg
        iter_msg = self.exeRepeat and 'iteration: '..self.currentCount..', '
        err.msg, failed = err.msg:gsub(M.FAILURE_PREFIX, iter_msg or '', 1)
        if failed > 0 then
            err.status = NodeStatus.FAIL
        end

        -- reformat / improve the stack trace
        if prettyFuncName then -- we do have the real method name
            err.trace = err.trace:gsub("in (%a+) 'methodInstance'", "in %1 '"..prettyFuncName.."'")
        end
        if STRIP_LUAUNIT_FROM_STACKTRACE then
            err.trace = stripLuaunitTrace(err.trace)
        end

        return err -- return the error "object" (table)
    end


    function M.LuaUnit:execOneFunction(className, methodName, classInstance, methodInstance)
        -- When executing a test function, className and classInstance must be nil
        -- When executing a class method, all parameters must be set

        if type(methodInstance) ~= 'function' then
            error( tostring(methodName)..' must be a function, not '..type(methodInstance))
        end

        local prettyFuncName
        if className == nil then
            className = '[TestFunctions]'
            prettyFuncName = methodName
        else
            prettyFuncName = className..'.'..methodName
        end

        if self.lastClassName ~= className then
            if self.lastClassName ~= nil then
                self:endClass()
            end
            self:startClass( className )
            self.lastClassName = className
        end

        self:startTest(prettyFuncName)

        local node = self.result.currentNode
        for iter_n = 1, self.exeRepeat or 1 do
            if node:isNotPassed() then
                break
            end
            self.currentCount = iter_n

            -- run setUp first (if any)
            if classInstance then
                local func = self.asFunction( classInstance.setUp ) or
                             self.asFunction( classInstance.Setup ) or
                             self.asFunction( classInstance.setup ) or
                             self.asFunction( classInstance.SetUp )
                if func then
                    self:addStatus(self:protectedCall(classInstance, func, className..'.setUp'))
                end
            end

            -- run testMethod()
            if node:isPassed() then
                self:addStatus(self:protectedCall(classInstance, methodInstance, prettyFuncName))
            end

            -- lastly, run tearDown (if any)
            if classInstance then
                local func = self.asFunction( classInstance.tearDown ) or
                             self.asFunction( classInstance.TearDown ) or
                             self.asFunction( classInstance.teardown ) or
                             self.asFunction( classInstance.Teardown )
                if func then
                    self:addStatus(self:protectedCall(classInstance, func, className..'.tearDown'))
                end
            end
        end

        self:endTest()
    end

    function M.LuaUnit.expandOneClass( result, className, classInstance )
        --[[
        Input: a list of { name, instance }, a class name, a class instance
        Ouptut: modify result to add all test method instance in the form:
        { className.methodName, classInstance }
        ]]
        for methodName, methodInstance in sortedPairs(classInstance) do
            if M.LuaUnit.asFunction(methodInstance) and M.LuaUnit.isMethodTestName( methodName ) then
                table.insert( result, { className..'.'..methodName, classInstance } )
            end
        end
    end

    function M.LuaUnit.expandClasses( listOfNameAndInst )
        --[[
        -- expand all classes (provided as {className, classInstance}) to a list of {className.methodName, classInstance}
        -- functions and methods remain untouched

        Input: a list of { name, instance }

        Output:
        * { function name, function instance } : do nothing
        * { class.method name, class instance }: do nothing
        * { class name, class instance } : add all method names in the form of (className.methodName, classInstance)
        ]]
        local result = {}

        for i,v in ipairs( listOfNameAndInst ) do
            local name, instance = v[1], v[2]
            if M.LuaUnit.asFunction(instance) then
                table.insert( result, { name, instance } )
            else
                if type(instance) ~= 'table' then
                    error( 'Instance must be a table or a function, not a '..type(instance)..' with value '..prettystr(instance))
                end
                local className, methodName = M.LuaUnit.splitClassMethod( name )
                if className then
                    local methodInstance = instance[methodName]
                    if methodInstance == nil then
                        error( "Could not find method in class "..tostring(className).." for method "..tostring(methodName) )
                    end
                    table.insert( result, { name, instance } )
                else
                    M.LuaUnit.expandOneClass( result, name, instance )
                end
            end
        end

        return result
    end

    function M.LuaUnit.applyPatternFilter( patternIncFilter, listOfNameAndInst )
        local included, excluded = {}, {}
        for i, v in ipairs( listOfNameAndInst ) do
            -- local name, instance = v[1], v[2]
            if  patternFilter( patternIncFilter, v[1] ) then
                table.insert( included, v )
            else
                table.insert( excluded, v )
            end
        end
        return included, excluded
    end

    function M.LuaUnit:runSuiteByInstances( listOfNameAndInst )
        --[[ Run an explicit list of tests. Each item of the list must be one of:
        * { function name, function instance }
        * { class name, class instance }
        * { class.method name, class instance }
        ]]

        local expandedList = self.expandClasses( listOfNameAndInst )
        if self.shuffle then
            randomizeTable( expandedList )
        end
        local filteredList, filteredOutList = self.applyPatternFilter(
            self.patternIncludeFilter, expandedList )

        self:startSuite( #filteredList, #filteredOutList )

        for i,v in ipairs( filteredList ) do
            local name, instance = v[1], v[2]
            if M.LuaUnit.asFunction(instance) then
                self:execOneFunction( nil, name, nil, instance )
            else
                -- expandClasses() should have already taken care of sanitizing the input
                assert( type(instance) == 'table' )
                local className, methodName = M.LuaUnit.splitClassMethod( name )
                assert( className ~= nil )
                local methodInstance = instance[methodName]
                assert(methodInstance ~= nil)
                self:execOneFunction( className, methodName, instance, methodInstance )
            end
            if self.result.aborted then
                break -- "--error" or "--failure" option triggered
            end
        end

        if self.lastClassName ~= nil then
            self:endClass()
        end

        self:endSuite()

        if self.result.aborted then
            print("LuaUnit ABORTED (as requested by --error or --failure option)")
            os.exit(-2)
        end
    end

    function M.LuaUnit:runSuiteByNames( listOfName )
        --[[ Run LuaUnit with a list of generic names, coming either from command-line or from global
            namespace analysis. Convert the list into a list of (name, valid instances (table or function))
            and calls runSuiteByInstances.
        ]]

        local instanceName, instance
        local listOfNameAndInst = {}

        for i,name in ipairs( listOfName ) do
            local className, methodName = M.LuaUnit.splitClassMethod( name )
            if className then
                instanceName = className
                instance = _G[instanceName]

                if instance == nil then
                    error( "No such name in global space: "..instanceName )
                end

                if type(instance) ~= 'table' then
                    error( 'Instance of '..instanceName..' must be a table, not '..type(instance))
                end

                local methodInstance = instance[methodName]
                if methodInstance == nil then
                    error( "Could not find method in class "..tostring(className).." for method "..tostring(methodName) )
                end

            else
                -- for functions and classes
                instanceName = name
                instance = _G[instanceName]
            end

            if instance == nil then
                error( "No such name in global space: "..instanceName )
            end

            if (type(instance) ~= 'table' and type(instance) ~= 'function') then
                error( 'Name must match a function or a table: '..instanceName )
            end

            table.insert( listOfNameAndInst, { name, instance } )
        end

        self:runSuiteByInstances( listOfNameAndInst )
    end

    function M.LuaUnit.run(...)
        -- Run some specific test classes.
        -- If no arguments are passed, run the class names specified on the
        -- command line. If no class name is specified on the command line
        -- run all classes whose name starts with 'Test'
        --
        -- If arguments are passed, they must be strings of the class names
        -- that you want to run or generic command line arguments (-o, -p, -v, ...)

        local runner = M.LuaUnit.new()
        return runner:runSuite(...)
    end

    function M.LuaUnit:runSuite( ... )

        local args = {...}
        if type(args[1]) == 'table' and args[1].__class__ == 'LuaUnit' then
            -- run was called with the syntax M.LuaUnit:runSuite()
            -- we support both M.LuaUnit.run() and M.LuaUnit:run()
            -- strip out the first argument
            table.remove(args,1)
        end

        if #args == 0 then
            args = cmdline_argv
        end

        local options = pcall_or_abort( M.LuaUnit.parseCmdLine, args )

        -- We expect these option fields to be either `nil` or contain
        -- valid values, so it's safe to always copy them directly.
        self.verbosity     = options.verbosity
        self.quitOnError   = options.quitOnError
        self.quitOnFailure = options.quitOnFailure
        self.fname         = options.fname

        self.exeRepeat            = options.exeRepeat
        self.patternIncludeFilter = options.pattern
        self.shuffle              = options.shuffle

        if options.output then
            if options.output:lower() == 'junit' and options.fname == nil then
                print('With junit output, a filename must be supplied with -n or --name')
                os.exit(-1)
            end
            pcall_or_abort(self.setOutputType, self, options.output)
        end

        self:runSuiteByNames( options.testNames or M.LuaUnit.collectTests() )

        return self.result.notPassedCount
    end
-- class LuaUnit

-- For compatbility with LuaUnit v2
M.run = M.LuaUnit.run
M.Run = M.LuaUnit.run

function M:setVerbosity( verbosity )
    M.LuaUnit.verbosity = verbosity
end
M.set_verbosity = M.setVerbosity
M.SetVerbosity = M.setVerbosity


return M
