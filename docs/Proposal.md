# In memory Database engine

The semestral project will be a in memory database engine. The engine will be key/value system supporting CRUD operations.

All the most important data structures will be self implemented. This includes the hash table along with the hash function.
The hash function will support byte sequences for possible usage with different types of keys.

## Database entries

Database entries are key/value pairs

### Keys

Keys will be byte sequences, so basically anything can be used as a key.

### Values

Values will be instances of multiple types. Each database instance will contain only objects of one type. Each value type
will have interface for operations typical for said value type. Each operation will have operands and target where result
should be saved - either key under which the value should be saved or file where to write the result.

#### Numbers
One of primitive types will be numerical types, numerical types shall be of two types:

- Floating point numbers
- Integral numbers

Operations supported for numerical values are:
- Addition
- Subtraction
- Division
  - For Floating point values result is also floating point value
  - For Integral values result will be integral value
- Multiplication

#### Stings
Next primitive type will be Strings, sequences of 8-bit characters.

Supported operations on strings will be:
- Concatenation
- Removing/Adding prefixes/suffixes
- Accessing character at specified position
- Finding occurrences of other string in a value

#### Vectors 
Vectors as a type will be supported, Vectors may store only one type of values.

Supported operations on Vectors will be:
- Appending value/removing from end in $O(1)$ time
- Inserting/removing values from random positions in vector in $O(n)$ time
- Getting/Setting value at a random position of a vector in $O(1)$ time
- Returning Vector size

#### Objects

Objects consist of multiple fields of primitive types. Every database entry must have the same structure. The structure
is defined at database startup. Fields will have the option to be optional, in the case when optional values won't be set
its value will be `null`

Supported operations on objects will be:
- Get/Set on Fields

## Time to Live

The database will have the option for specifying time to live for its entries. Every entry will have it's time to live
specified when adding it, when the time runs out the database entry will be invalidated and removed from database.

There will be option to set new timer on each database entry. At database startup the default time to live will be
specified and then if time to live for new entry won't be specified the default will be used.

## CLI

Every database operation will be made using the database CLI. The CLI will have commands and options for all the operations
specified above

## External libraries

Project will use following external libraries:

- Catch2 as a library for writing tests.