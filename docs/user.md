# User documentation

Database supports three 4 commands:

- `INSERT [KEY] [VALUE]` - inserts key/value pair into database
- `GET [KEY]` - prints value associated with specified key, if such entry exists
- `DELETE [KEY]` - deletes entry associated with specified key, if such entry exists
- `TERMINATE` - stops the database

## String values

- Operations returning `String` are defined in a following way
    - Constant value
        - constant values must be enclosed in curly brackets
        - for example, to use constant value "result1", one must write `{result1}`
    - Database key
        - database key must start with dollar sign
        - for example, to use value under key "value", one must write `$value`
    - String operation
        - String operations are described below
        - Operands must be other Operations returning String value

- `SET [KEY] [VALUE]`
  - `[VALUE]` is result of Operation returning `String` as defined w, 
  - Each n-ary operation is written in following way: `OPERATION [OPERAND1] [OPERAND2] ... [OPERANDn]` 

- `GET [VALUE]`
  - `[VALUE]` is a result of Operation returning `String` 

- String operations that do not return `String` values must be written into result file or dumped into standard output

### Operations retuning String

- `CONCAT [LEFT] [RIGHt]`
  - Result is concatenation of two Operations returning `String` 
- `SUFFIX [VALUE] [N]`
  - First operand is Operation returning `String`
  - Second operand is number
  - result is first `[N]` characters of `[VALUE]` 
- `PREFIX [VALUE] [N]`
  - First operand is Operation returning `String`
  - Second operand is number
  - result is last `[N]` characters of `[VALUE]`
- `AT [VALUE] [I]`
  - First operand is operation returning `String`
  - Second operation is index
  - result is `[I]`-th character of `[VALUE]`

### Operations returning non-String
- `FIND [VALUE] [TOFIND]`
  - both operands are `String` values
  - result is all positions of `[VALUE]`, where the string `[TOFIND]` starts as a substring of `[VALUE]`
   