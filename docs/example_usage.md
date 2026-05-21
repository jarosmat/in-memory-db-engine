# Example usage

## Integers

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

## Floating point numbers

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

## Strings
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
## Vectors

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

## Objects

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

## Time to live

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




