# vshell - simple POSIX shell

## Builtin commands

`export [name=[word]] ...` - Add environment variables to local storage or do nothing if no arguments passed.

`unset [name ...]` - For each name, remove the corresponding variable.

`printenv` - Print all environment variables.

`exit` - Exit with EXIT_SUCCESS code.



## Building

```
$ cmake .
$ make
```

Requires C++17 compiler