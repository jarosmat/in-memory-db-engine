# Programmer documentation

## `src/DBCore`

This folder contains the core components of the engine. It is structured into two folders:

- `BDEntry`
- `SwissTable`

### `DBEntry`

This folder contains classes used to represent specific entries to database. Only String entries are supported in current
version.

- `class String`
  - uses `std::string` for internal value representation
  - `class PrefixAut` is class used for searching for occurrences of substring using the Knuth-Morris-Pratt algorithm
  - constructor accepts either R-value reference or const L-value reference to `std::string`, default constructor is also
    available
  - static function `Concatenate` takes two arguments: `String valLeft` and `String valRight` and returns new `String` 
    instance, whose value is the concatenation of `valLeft + valRight`
    - this function accepts eith both L-value reference arguments or both R-value arguments
  - `operator[]` - operator for accessing i-th character in String
  - `Find` method finds all occurrences of its argument `valToFind` in `this`
  - `operator==` compares internal values of `std::string`
  - `Size` returns number of characters in String
  - `operator<<` dumps the internal `std::string` into specified stream


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

