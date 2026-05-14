# Programmer documentation

At first the options from CLI are parsed (see [user documentation](user.md)), then if the option are correct an instantiation
of function `RunDB` found in `src/CLI/Runner.hpp` is called and the database is running. Database runs in two threads:
1. reader thread
   - this thread just reads input stream specified when calling `RunDB` (currently `std::cin`) and pushes
     lines that are read into `std::queue` shared with main thread
2. main thread
   - this thread does all the important work
   - thread works in cycles:
     1. It checks `std::priority_queue<QueueEntry>`, that holds information about expiration of database entries. On top
        of the priority queue is entry, that will expire first. If the entry on top of the queue is expired it pops it 
        from the queue and removes associated entry from database
     2. It checks if there is a new line (operation od database) in `std::queue` shared with reader thread, if there is,
        it pops it from the queue and executes the operation
     3. repeat

## `src/DBCore`

This folder contains the core components of the engine. It is structured into three folders:

- `BDEntry`
- `SwissTable`
- `Hashfunction`

### `DBEntry`

This folder contains classes used to represent specific entries to database.

- `class String`
  - uses `std::string` for internal value representation
  - `class PrefixAut` is class used for searching for occurrences of substring using the Knuth-Morris-Pratt algorithm
  - constructor accepts either R-value reference or const L-value reference to `std::string` or `char` value for
    construction of `String` consisting of one character, default constructor is also available:
    - `String(const std::string &val)`
    - `String(std::string &&val)`
    - `String(char ch)`
    - `String()`
      - constructs instance with empty value of internal `std::string`
  - static function `String Concatenate` takes two arguments: `String valLeft` and `String valRight` and returns new `String` 
    instance, whose value is the concatenation of `valLeft + valRight`
    - this function accepts eith both L-value reference arguments or both R-value arguments:
      - `String Concatenate(const std::string &valLeft, const std::string &valRight)`
      - `String Concatenate(std::string &&valLeft, std::string &&valRight)`
      - `String Concatenate(String &&valLeft, String &&valRight)`
  - `char operator[](std::size_t i)` - operator for accessing i-th character in String
  - `Find(const String &valToFind)` method finds all occurrences of its argument `valToFind` in `this`
  - `bool operator==` compares internal values of `std::string`
  - `std::size_t Size()` returns number of characters in String
  - `operator<<` dumps the internal `std::string` into specified stream
  - `void RemovePrefix(size_t n)` - removes first `n` characters of `this`
  - `void RemoveSuffix(size_t n)` - removes last `n` characters of `this`
- `class Object`
  - Is a class used to represent database value type `object`
  - For avoiding circular dependency, the declaration of class interface is in file `src/DBCore/DBEntry/ValueTypes.hpp`, 
    this file is then included in `Object.hpp`
  - In `src/DBCore/DBEntry/ValueTypes.hpp` an alias `PrimitiveValuePack` is defined, this alias holds all value types, that
    are available for fields of object to hold, in a `std::tuple`.
  - In `src/DBCore/DBEntry/ValueTypes.hpp`, there is a struct `ValueType`, that is used to infer type of actual type of
    field value representation, it takes a `std::tuple` template and defines struct alias `ValueType<Args ...>::type`,
    that is a type `std::variant<Args ...>`
  - For representation of fields of an object, instance of `std::unordered_map<Key, Value>` is used
    - `Key` is an alias for `std::string`
    - `Value = ValueType<PrimitoveValuePack>::type`, which is a `std::variant` with all value types, that are possible
  - class can be constructed with an iterator pair into `std::vector<std::pair<Key, Value>>`, this constructs an instance
    with fields, that are in the vector between specified iterators, where the pair specifies field name and the value of
    that field:
    - `Object(std::vector<std::pair<Key, Value>>::iterator first, std::vector<std::pair<Key, Value>>::iterator last)`
    - `Object()`
      - constructs object with empty instance of `std::unordered_map<Key, Value>`
  - `T get_member(const Key &name)` function template, that takes a key and returns a value, that is stored under only argument of type `Key`,
    argument of function template is the value type, that is expected to be stored under specified field name
  - `void set_value(const Key &name, const T &value)` function template, that stores `value` under `name` field name, template of the function is the value type
    of `value`, function has two overloads, for `value` being passed as an r-value of l-value reference
  - `bool has_member(const Key &name)` function, that returns `true`, if the instance contains a field under a `name`, otherwise returns `false`
  - `std::size_t MemberCount()` function, that return number of fields currently stored in the instance
  - `operator<<` overload, that dumps the object into specified stream in format: `{field1: value1, field2: value2, ...}`
- `class Timepoint`
  - class used to measure time elapsed from some timepoint
  - at construction uses `std::chrono::steady_clock` for getting a timepoint and later compares it to current timepoint,
    taken using the same clock
    - `std::chrono::steady_clock` is used, because timepoints of this clock cannot decrease as time goes on
  - `Timepoint()` constructs an instance, takes a timepoint
  - `bool Elapsed(T &&val)` function template, that takes value `val` of type `T`, function template `T` must be specialization
    of `std::chrono::duration` (it must be possible to cast duration values to this type). Function returns `true` if time
    specified as value `val` has elapsed since creation of this object, otherwise returns `false`.
  - `T RemainingTime(T &&val)` function template, that takes value `val` of type `T` with same traits as parameter of function
    `Elapsed`. This function returns remaining time before the time `val` elapses since the construction of this object.
    Return value if of type `T`, if time `val` has already elapsed the function returns `T(0)`.
- `class DBEntry_t<typename E, typename T>`
  - class template used to store entries into database, `E` specifies the type of entry that is to be stored in the
    database and `T` specifies a type used for specifying time to live, `T` must be specialization of `std::chrono::duration`
  - instance of instantiation of this class template holds: 
    - `E payload` - actual value stored in this entry
    - `Timepoint timepoint_` - used for measuring if entry is already expired
    - `T ttl` - time for the instance to live - used for comparing against `timepoint_`
    - `QueueEntry *entryPointer` - pointer to entry of priority queue used for tracking expired entries. This pointer is
      used to invalidate the entry in priority queue, so the entry won't be removed more than once
  - `DBEntry_t(EC &&payload, TC &&ttl, QueueEntry *pointer)` - only constructor, `payload` is the value to be stored,
    `ttl` is time to live of the entry, `pointer` is pointer to priority queue
  - `E GetRef()` - returns reference to payload
  - `SetVal(ES &&val)` - function template used for setting the value of entry, template uses forwarding reference
  - `bool Expired()` - function to evaluate if entry is expired
  - `T RemainingTime()` - function used for getting remaining time of entry
  - `void Invalidate()` - invalidates entry in priority queue used for time to live using `entryPointer`
  - class template has partial specialization `DBEntry_t<E, ImmortalSentinel>`
    - this specialization is used for entries, that do not have set a time to live, where `ImmortalSentinel` is empty class
    - function `Expired` always returns `false`
    - function `RemainingTime` always returns `std::chrono::milliseconds::max()`
- `class DBEntry<E>`
  - class template used for type erasure of `T` template parameter of `DBEntry_t`. This class exists so that entries
    with multiple different types of time to live can be stored in one database instance
- `class Null`
  - class used as a value of optional fields of `object` entries, that are not set
  - `bool operator==` - always returns `true`A
  - `operator<<` - dumps string `null` into stream
- `ValueTypes.hpp`
  - header used for including all possible database entries
    - `String`
    - `Integer`
    - `Float`
    - `Object`
    - `Vector<T>`
  - header also contains declaration of class `Object`


### `SwissTable`

Contains classes used for declaration of `class SwissTable`, which serves as a hash table. Swiss Tabe is a hash map design,
explained for example [here](https://go.dev/blog/swisstable).


- `class Vector`
  - class creating object similar to `std::vector`
  - used for representation of backing-array
- `class Group` 
  - used for representation of groups in Swiss Tables
  - all indexes in some inputs/outputs of this class, can be used to directly access data in
    backing-array - these indexes point to positions in backing array 
  - `std::vector<uint8_t>` is used internally to represent the control word of a group
  - `start` and `end` indicate where in backing array the group starts and where it ends, `end` points to value just after
    last value in this group
  - `LookUp` method tries to find occurrence of specified h2 hash, if it does not find any it tries to find empty space,
    - when hash is found in group, pair `<LookupRes::USED, std::vector<size_t>>` is returned, where the vector contains
      all occurrences of specified h2 in this group
    - when hash is not found and there is empty slot in this group - indicating, that value we are searching for is not
      in hash map yet, method returns pair `<LookupRes::EMPTY, std::vector<size_t>>`, where the vector contains all empty
      indexes in this group
    - when hash in found and the group is full, the pair `<LookupRes::FULL, std::vector<size_t>>` is returned, where the
      vector can be ignored since it does not contain any useful data
  - `SetVal` sets specified hash as a value stored at a specified position in group
  - `Delete` deletes value at specified position in group
  - `Used` method return `true` if specified position in a group is in use, returns `false` otherwise

- `class Entry`
  - instance of this class is used for altering position in a hash map, that is associated with some key
  - `GetResult` return result of look up of some key
  - `DeleteEntry` deletes entry associated with looked up key
  - `SetValue` sets value associated with looked up key to value specified as argument, accepts both L-value and R-value 
    references
  - `GetValue` returns reference to value associated with looked up key

- `class SwissTable`
  - contains `Vector` with instances of groups, `Vector` with keys and `Vector` with value, variable `dataCount` contains
    number of values actually inserted in hash map
  - class has only default constructor
  - `LookUp` returns instance of `Entry` that can be used to access/delete/update value associated with specified key
  - `Rehash` method expands the backing array and inserts all value in the hash map
  - `Delete` method deletes value at specified position in backing array
  - `SetValue` method sets the value at specified position in backing array to specified value, h2 hash of the key must 
    be specified, method accepts both L-value and R-value references to value that is to be inserted
  - `GetValue` method returns reference to value at specified position in backing array
  - `GetNextGroup` returns index of next group when sequentially iterating over groups
  - `CheckExistence` checks if key actually exists in certain group with specified lookup result from this group of the
    keys h2 hash, returns `std::pair<bool, size_t>` where `bool` specifies whether key was actually found and `size_t`
    specifies the index in backing array where the key actually is

### `HashFunction`

Contains implementation of a Sip hash hashing function. Information about SipHash can be found [here](https://eprint.iacr.org/2012/351.pdf).
Key is chosen randomly on database startup

## `src/CLI`

### `DBType`

Contains definitions of enums, functions associated with parsing of startup arguments and inferring type of database at
compile time.

- enum class `DBTypes`
  - enum, that is used to specify what is types of entries in a database
- struct `get_db_type`
  - `DBTypes` type tag used to infer type of database based on template argument
- enum class `StartupTokens`
  - used when parsing startup options to know what to parse
- enum class `TTLTypes`
  - used for specifying what unit of time is used for time to live
- enum class `ObjectFieldTypes`
  - used for specifying what type is field of object
- enum class `SplitFieldRes`
  - used for specifying result of splitting string representation of object into vector of strings, containing individual
    fields
- `using view_t = std::ranges::subrange<std::vector<std::string>::iterator>`
- enum class `ObjectParseRes`
  - used for specifying result of parsing of string representation of object
- `StartupTokens GetOptionToken(view_t &args)`
  - function used for parsing next token in startup arguments
- `std::pair<variant_t, TTLTypes> GetTTL`
  - used for parsing representation of time to live after parsing its option
- `DBTypes GetType(viev_t &args)`
  - used for parsing type of database entries after parsing its option
- `std::tuple<DBTypes, ttl_t, Object, object_reference> GetDBType(view_t &args)`
  - function used for parsing database entry type and time to live for entries
- `std::tuple<ObjectParseRes, Object, object_reference> ParseObjectStructure(view_t &viewIn)`
  - function used for parsing object structure on database startup, when type of entries is object

### `Error`

Contains functions, that return strings of errors, that may occur while executing database operations

- `err_t = std::vector<std::string>`
  - alias for setting tye of errors
  - it is a vector of strings, because it makes it easier for creation of parametrized errors
- `err_t CouldNotOpenFileErr(const std::string &file)`
  - returns error, when some file could not be opened
  - returns: `{"Could not open file: ", file}`
- `err_t CouldNotReadFileErr(const std::string &file)`
  - returns error, when some file could not be read
  - returns `{"Could not read file: ", file}`
- `err_t InvalidArgs()`
  - returns error, when invalid arguments were passed to database operation
  - returns `{"Invalid arguments"}`
- `err_t InvalidArgsCount(const std::size_t expected, const std::size_t actual)`
  - returns error, when invalid number of arguments was passed to database operation
  - return `{"Invalid number of arguments, expected: ", std::to_string(expected), ", got: ", std::to_string(actual)}`
- `err_t InvalidOperation()`
  - returns error, when invalid operation was run on database instance, f.e. object operation was run on integer database
  - returns `{"Invalid operation"}`
- `err_t InvalidField(const std::string &field)`
  - returns error, when field of an object is invalid 
  - returns `{"Invalid field: ", field}`

### `Nodes`
### `OperationType`
### `Parser`
### `QueueEntry`
### `Utils`

Contains utility functions.

- `void SkipSpaces(std::string_view &view)`
  - moves begin iterator forward until `view` is empty, or it starts on character, that does not satisfy `std::isspace`
- `void RemoveTrailingSpaces(std::string_view &view)` 
  - moves end iterator backwards until `view` is empty or last character does not satisfy `std::isspace`
- `enum class BytesFromFileRes` 
  - enum class representing result of reading bytes from a file
    - `VALID` - reading of bytes was successful
    - `COULD_NOT_OPEN` - file could not be opened
    - `COULD_NOT_READ` - file could not be read
- `std::pair<BytesFromFileRes, std::vector<uint8_t> > readFileBytes(const std::string &filePath)`
  - reads bytes of file under path `filePath`
  - first of returned pair is result of reading
  - second of returned pair is vector of bytes representing file under `filePath`
- `std::vector<uint8_t> stringBytes(std::string &&str)` 
  - returns vector of bytes of string `str`
- `std::pair<uint64_t, uint64_t> sampleKey(uint64_t seed)`
  - samples a pair of 64-bit numbers to be used as a key for hash function
- `std::pair<bool, std::vector<std::string> > parseLine(const std::string &line)`
  - parses string `line` into vector of strings, string values are delimited by characters, that satisfy `std::isspace`,
    only values enclosed in quotes are parsed as one value regardless if these values contains characters, that satisfy
    `std::isspace`
### `Runner.hpp`
