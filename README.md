## In-Memory Database Engine

My implementation of in-memory database key-value store database engine using the Swiss Table hash table and SipHash-2-4
as a hash function.

## Proposal

Proposal of this project can be seen [here](docs/Proposal.md)

## Documentation

- [Programmer documentation](docs/programer.md#programmer-documentation)
- [User documentation](docs/user.md#user-documentation)
- [Examples of usage](docs/user.md#example-usage)

## Build

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
