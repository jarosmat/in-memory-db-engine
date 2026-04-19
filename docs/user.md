# User documentation

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

## Operations

- `INSERT [KEY] [VALUE]` - inserts key/value pair into database
- `GET [KEY]` - prints value associated with specified key, if such entry exists
- `DELETE [KEY]` - deletes entry associated with specified key, if such entry exists
- `TERMINATE` - stops the database
- `<OPERATION> [TARGET] [OPERANDS]...` 
  - `<OPERATION>` is one of operations on db entries that are defined below
  - `[TARGET]` is where to store the result, by default target is key under which to store the value, for storing the
    result into file add `-f` flag

`INSERT` and `<OPERATION>` have also option `--ttl` that specifies the time to live for the value being stored. The option
is used in similar fashion as is done at database startup, when this option is omitted, default value is used.

If operand is prefixed by `$`, instead of using the literal value of operand, value stored in database under that value,
is used.

If operand is prefixed by `@`, instead of using the literal value of operand, byte stream of file under specified value
is used as a key to get value from database to use as an operand.

This notation is used also when specifying key under which to store a value.

## Operations on numbers

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

## Operations on String values 

- `CONCAT [LEFT] [RIGHT]`
  - Result is concatenation of two Operands
  - returns string value
- `SUFFIX [VALUE] [N]`
  - First operand is string 
  - Second operand is number
  - result is first `[N]` characters of `[VALUE]` 
  - returns string value
- `PREFIX [VALUE] [N]`
  - First operand is string 
  - Second operand is number
  - result is last `[N]` characters of `[VALUE]`
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
The value specified is treated as key by default (without specifying `$`, if the values begins with `$`, it is omitted).
If the value is prepended by `@` file under that value is used as a key.

- `APPEND [VECT] [WHAT]`
  - appends value `[WHAT]` to end of `[VECT]` 
  - operation does not return anything, but modifies an existing value
- `POP [VECT]`
  - removes last value from `[VECT]`
  - operation does not return anything, but modifies an existing value
- `VINSERT [VECT] [I] [VALUE]`
  - inserts `[VALUE]` to position `[I]` in `[VECT]`
  - operation does not return anything, but modifies an existing value
- `REMOVE [VECT] [I]`
  - removes value from position `[I]` in `[VECT]`
  - operation does not return anything, but modifies an existing value`
- `SET [VECT] [I] [VALUE]`
  - sets value at position `[I]` to `[VALUE]` in `[VECT]`
  - operation does not return anything, but modifies an existing value`
- `VGET [VECT] [I]`
  - returns value at position `[I]` in `[VECT]`
  - operation returns value corresponding to type of vector
- `SIZE [VECT]`
  - returns number of values in `[VECT]`
   