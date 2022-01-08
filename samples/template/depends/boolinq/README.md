# boolinq 3.0

[![CI Status](https://travis-ci.org/k06a/boolinq.svg?branch=master)](https://travis-ci.org/k06a/boolinq)
[![Coverage Status](https://coveralls.io/repos/github/k06a/boolinq/badge.svg?branch=master)](https://coveralls.io/github/k06a/boolinq?branch=master)

Super tiny C++11 single-file header-only LINQ template library

Just imagine .NET Framework LINQ support for STL/Qt collections :)

Get source code here: **[boolinq.h](/include/boolinq/boolinq.h)**

## Wanna demo?

#### Example with integers

```C++
int src[] = {1,2,3,4,5,6,7,8};
auto dst = from(src).where( [](int a) { return a % 2 == 1; })      // 1,3,5,7
                    .select([](int a) { return a * 2; })           // 2,6,10,14
                    .where( [](int a) { return a > 2 && a < 12; }) // 6,10
                    .toStdVector();

// dst type: std::vector<int>
// dst items: 6,10
```

#### Example with structs

```C++
struct Man {
    std::string name;
    int age;
};

Man src[] = {
    {"Kevin",14},
    {"Anton",18},
    {"Agata",17},
    {"Terra",20},
    {"Layer",15},
};

auto dst = from(src).where(  [](const Man & man) { return man.age < 18; })
                    .orderBy([](const Man & man) { return man.age; })
                    .select( [](const Man & man) { return man.name; })
                    .toStdVector();

// dst type: std::vector<std::string>
// dst items: "Kevin", "Layer", "Agata"
```

#### Interesting example

```C++
struct Message {
    std::string PhoneA;
    std::string PhoneB;
    std::string Text;
};

Message messages[] = {
    {"Anton","Troll","Hello, friend!"},
    {"Denis","Wride","OLOLO"},
    {"Anton","Papay","WTF?"},
    {"Denis","Maloy","How r u?"},
    {"Denis","Wride","Param-pareram!"},
};

int DenisUniqueContactCount =
    from(messages).where(   [](const Message & msg) { return msg.PhoneA == "Denis"; })
                  .distinct([](const Message & msg) { return msg.PhoneB; })
                  .count();

// DenisUniqueContactCount == 2    
```

## Containers supported?

- C++: Native arrays, pairs of pointers
- STL: list, stack, queue, vector, deque, set, map, any compatible ....
- Qt: QList, QVector, QSet, QMap.

## Operators supported?

#### Filters and reorders:

- `where(predicate)`, `where_i(predicate)`
- `take(count)`, `takeWhile(predicate)`, `takeWhile_i(predicate)`
- `skip(count)`, `skipWhile(predicate)`, `skipWhile_i(predicate)`
- `orderBy()`, `orderBy(transform)`
- `distinct()`, `distinct(transform)`
- `append(items)`, `prepend(items)`
- `concat(linq)`
- `reverse()`
- `cast<T>()`

#### Transformers:

- `select(transform)`, `select_i(transform)`
- `groupBy(transform)`
- `selectMany(transfom)`

#### Aggregators:

- `all()`, `all(predicate)`
- `any()`, `any(lambda)`
- `sum()`, `sum<T>()`, `sum(lambda)`
- `avg()`, `avg<T>()`, `avg(lambda)`
- `min()`, `min(lambda)`
- `max()`, `max(lambda)`
- `count()`, `count(value)`, `count(predicate)`
- `contains(value)`
- `elementAt(index)`
- `first()`, `first(filter)`, `firstOrDefault()`, `firstOrDefault(filter)`
- `last()`, `last(filter)`, `lastOrDefault()`, `lastOrDefault(filter)`
- `toStdSet()`, `toStdList()`, `toStdDeque()`, `toStdVector()`

#### Bits and Bytes:

- `bytes<T>(ByteDirection?)`
- `unbytes<T>(ByteDirection?)`
- `bits(BitsDirection?, BytesDirection?)`
- `unbits<T = unsigned char>(BitsDirection?, BytesDirection?)`

#### Coming soon:

- gz()
- ungz()
- leftJoin
- rightJoin
- crossJoin
- fullJoin
