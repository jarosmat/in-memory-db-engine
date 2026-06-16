# User documentation

## Minimum HW requirements
- CPU with 2 or more cores
- 4 GB of RAM
- less than 1 GB of storage


## Minimal SW requirements
- Windows 11
- Linux based operating system


## Installation

For building, the following packages must be installed:
- `cmake`
- `make`
- C++ compiler

To build the project, run

```./build.sh```

To build project for testing, run

```./build_test.sh```

to run the main executable, run

```./run.sh```

Any cli arguments to be passed to main executable, add to script `run.sh`

## Example usage

### Integers

```
./database --value int
```

input:

```
INSERT key 1
ADD keya 9 $key
SUBTRACT keys 12 $keya
MULTIPLY keym 8 $keys
DIVIDE keyd 16 $keys
GET key
GET keya
GET keys
GET keym
GET keyd
```

output:

```
Value is: 1
Value is: 10
Value is: 2
Value is: 16
Value is: 8
```

### Floating point numbers

```
./database --value float 
```

input:

```
INSERT key 1.5
FADD keya 9.9 $key
FSUBTRACT keys 12.0134 $keya
FMULTIPLY keym 8.98 $keys
FDIVIDE keyd 16.35 $keys
GET key
GET keya
GET keys
GET keym
GET keyd
```

output:

```
Value is: 1.5
Value is: 11.4
Value is: 0.6134
Value is: 5.50833
Value is: 26.6547
```

### Strings
```
./database --value string
```

input:

```
INSERT key left
CONCAT keyc $key " right"
SUFFIX keys $keyc 4
PREFIX keyp $keyc 4
AT keya $keyc 0
FIND @keyfind ababababab ab
GET key
GET keyc
GET keys
GET keyp
GET keya
```

output:

```
Value is: left
Value is: left right 
Value is: ight
Value is: left
Value is: l  
```

also file with name `keyfind` is created with content: `{0, 2, 4, 6, 8}`

### Vectors

```
./database --value vect-int
```

input:

```
INSERT key "{0,1,2,3,4}"
APPEND keya $key 5
POP keyp $key
VINSERT keyi $key 0 0
REMOVE keyr $key 0
SET keys $keyi 0 10
VGET @keyg $keys 0
GET key 
GET keya 
GET keyp 
GET keyi 
GET keyr 
GET keys
```

output:

```
Value is: {0, 1, 2, 3, 4}
Value is: {0, 1, 2, 3, 4, 5}
Value is: {0, 1, 2, 3}
Value is: {0, 0, 1, 2, 3, 4}
Value is: {1, 2, 3, 4}
Value is: {10, 0, 1, 2, 3, 4}
```

also file with name `keyg` is created with content: `10`

### Objects

```
./database --value object --struct "{Age: int, Name: string, College: string-O}"
```

input:

```
INSERT key "{Age: 1, Name: 'tom'}"
OSET keysa $key Age 24
OSET keysc $keysa College UK
OGET @keyget $keysa College
GET key
GET keysa
GET keysc
```

output:

```
Value is: {Name: tom, Age: 1, College: null}
Value is: {Name: tom, Age: 24, College: null} 
Value is: {Name: tom, Age: 24, College: UK}
```

also file with name `keyget` is created with content: `null`

### Time to live

```
./database --value int --ttl 2s
```

input:

```
INSERT key 2
```

wait for two seconds, then input:

```
GET key
```

output:
```
Invalid key
```


## Startup

Use command `database` for starting the database. Following options are available:
- `--value <type>`
  - possible values for `<type>` are:
    - `int` - entries of type: integral numbers
    - `float` - entries of type: floating point numbers
    - `string` - entries of type: strings
    - `object` - entries of type: objects
    - `vect-int` - entries of type: vector of integral numbers
    - `vect-float` - entries of type: vector of floating point numbers
    - `vect-string` - entries of type: vector of strings
  - is used for specifying type of database entries, when type `object` is specified, path to file specifying object
    structure must follow
- `--ttl <time>`
  - possible values are:
    - `<num>ms` - `<num>` milliseconds
    - `<num>s` - `<num>` seconds
    - `<num>m` - `<num>` minutes
  - `<num>` is a number
  - Is used to specify default time to live for entries, when the option is omitted, there won't be default value of time
    to live and entries that won't have time to live explicitly specified won't be deleted until user does so
- `--struct <value>`
  - value must be of format specified below, this flag may only be present when `--value` is set to `object`, if this
  option is present, it is implicitly assumed that `--value` will be set to `object`, so the option `--value` does not 
  have to be present.
  - this option is used to specify the database entry object structure
  - If there is option `--value` present with value other than `object` it is considered as wrong input.
  - specification of `<value>` is as follows:

`{[FIELD1]:[TYPE1], [FIELD2]: [TYPE2], [FIELD3]:[TYPE3]}`

Every entry in database must adhere to same structure, how to specify values on insertion is described below. Types may
only be one of three: `int`, `float`, `string`.

Field have also may be optional, these fields do not have to be set on insertion, to specify, that write then in this way
on startup: `[FIELD]:[TYPE]-O`

For example:
`{Age: int, Name: string, College: string-O }`
Defines that entry has 3 fields, one integer field Age, two string fields: Name, College, also the field College is optional.


## Operations

- `INSERT [KEY] [VALUE]` - inserts key/value pair into database
  - for getting key from file byte stream prefix the path to file with `@`
- `GET [KEY]` - prints value associated with specified key, if such entry exists
  - for getting key from file byte stream prefix the path to file with `@`
- `DELETE [KEY]` - deletes entry associated with specified key, if such entry exists
  - for getting key from file byte stream prefix the path to file with `@`
- `TERMINATE` - stops the database
- `<OPERATION> [TARGET] [OPERANDS]...` 
  - `<OPERATION>` is one of operations on db entries that are defined below
  - `[TARGET]` is where to store the result, by default target is key under which to store the value, for storing the
    result into file prefix it with `@`

`INSERT` and `<OPERATION>` have also option `--ttl` that specifies the time to live for the value being stored. The option
is used in similar fashion as is done at database startup, when this option is omitted, default value is used.

Time to live must be specified as a first thing after operation, after time to live comes target and after that come
operands of the operation.

If operand is prefixed by `$`, instead of using the literal value of operand, value stored in database under that value,
is used.

If operand is prefixed by `@`, instead of using the literal value of operand, byte stream of file under specified value
is used as a key to get value from database to use as an operand.

This notation is used also when specifying key under which to store a value.

If you want to use any of these characters at the start of the operand, prefix the value by `\`. This value will always be
ignored at the start of an operand (when the key starts with \, just put \\ at the start).

Example op operation is:

`ADD --ttl 20ms keyRes $key 20`

this performs `ADD` operation, the result is to be saved under key `keyRes` nad operands are: value stored under key `key`
and the number `20` 

## Operations on numbers

Numbers are integral or floating point. Integral numbers are specified in usual matter, without spaces between numerals,
e.g.: `0`,`41`,`-4`. Floating point numbers are also specified in usual matter, without spaces between numerals and with
whole part and decimal part divided by dot `.`, e.g.: `0.13`, `14,532`

- `ADD [SUMMAND1] [SUMMAND2]`
  - computes sum of `[SUMMAND1]` and `[SUMMAND2]`
  - resulting value type is based on type of operands, if at least one is floating point number, result is floating point
    number, else the result is integer
- `SUBTRACT [MINUEND] [SUBTRAHEND]`
  - computes the difference between `[MINUEND]` and `[SUBTRAHEND]`
  - resulting value type is based on type of operands, if at least one is floating point number, result is floating point
    number, else the result is integer
- `DIVIDE [DIVIDEND] [DIVISOR]`
  - divides `[DIVIDEND]` by `[DIVISOR]`
  - resulting value type is based on type of operands, if at least one is floating point number, result is floating point
    number, else the result is integer, for integers the resulting value is rounded
  - result of division on integer numbers rounds towards zero
- `MULTIPLY [FACTOR1] [FACTOR2]`
  - computes the product of the two factors
  - resulting value type is based on type of operands, if at least one is floating point number, result is floating point
    number, else the result is integer

For operations on floating point numbers, add F in front of operation: e.g.: `FADD`, `FSUBTRACT`.

## Operations on String values 

- `CONCAT [LEFT] [RIGHT]`
  - Result is concatenation of two Operands
  - returns string value
- `SUFFIX [VALUE] [N]`
  - First operand is string 
  - Second operand is number
  - result is last `[N]` characters of `[VALUE]` 
  - returns string value
- `PREFIX [VALUE] [N]`
  - First operand is string 
  - Second operand is number
  - result is first `[N]` characters of `[VALUE]`
  - returns string value
- `AT [VALUE] [I]`
  - First operand is string 
  - Second operation is number 
  - result is `[I]`-th character of `[VALUE]`
  - returns string value
- `FIND [VALUE] [TOFIND]`
  - both operands are `String` values
  - result is all positions of `[VALUE]`, where the string `[TOFIND]` starts as a substring of `[VALUE]`
  - result is vector of integers


## Operations on Vectors

In all operation operand called `[VECT]` appears, this operand specifies vector on which the operation is to take place.
If you want to write a constant vector, it is in a format: `{item1, item2, item3}`. For vector of strings, items have to 
be inside apostrophes '', all outside apostrophes are ignores. If you want the item to be `'something`, just write:
`{..., ''somehing', ...}`, this notation is only used, when Inserting vector into database. For all other operations on
vectors only keys are accepted.

- `APPEND [VECT] [WHAT]`
  - appends value `[WHAT]` to end of `[VECT]` 
  - operation returns new vector 
- `POP [VECT]`
  - removes last value from `[VECT]`
  - operation returns new vector
- `VINSERT [VECT] [I] [VALUE]`
  - inserts `[VALUE]` to position `[I]` in `[VECT]`
  - operation returns new vector
- `REMOVE [VECT] [I]`
  - removes value from position `[I]` in `[VECT]`
  - operation returns new vector
- `SET [VECT] [I] [VALUE]`
  - sets value at position `[I]` to `[VALUE]` in `[VECT]`
  - operation returns new vector
- `VGET [VECT] [I]`
  - returns value at position `[I]` in `[VECT]`
  - operation returns value corresponding to type of vector
- `SIZE [VECT]`
  - returns number of values in `[VECT]`
  

## Operations on objects

For writing a constant value of object write it in a format: `{[FIELD1]:val, [FIELD2]: val, [FIELD3]: val}`

Every string value, that is to be saved as a value of a field must be enclosed in apostrophes: '', all outside apostrophes
are ignores. If you want the item to be `'something`, just write: `''somehing'`

- `OGET [OBJECT] [FIELD]`
  - gets the value of field named `[FIELD]` from object `[OBJECT]`
- `OSET [OBJECT] [FIELD] [VALUE]`
  - sets the value of field `[FIELD]` in object `[OBJECT]` to value `[VALUE]`