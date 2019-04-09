A simple unix shell.
Supports environment variables, double quotes and their escaping.
To start a process with specified environment, either describe it before your command, or use **export** and **unset**.
Quoting a string containing spaces will make it a single argument for the program.

Here is an example session:


```
0 -> /bin/echo A=2
A=2
0 -> A=2 /bin/echo

0 -> A=2 /bin/env
A=2
0 -> /bin/ls
0 -> /bin/touch "a b.txt"
0 -> /bin/touch "a\"b.txt"
0 -> /bin/touch "a\\b.txt"
0 -> /bin/ls
'a b.txt'  'a"b.txt'  'a\b.txt'
0 -> "A=Hello, world!" /bin/env
A=Hello, world!
0 -> "A=Hello\"world\"" /bin/env
A=Hello"world"
0 -> /bin/ls "a b.txt" "a\"b.txt" "a\\b.txt"
'a b.txt'  'a"b.txt'  'a\b.txt'
0 -> /bin/rm "a b.txt" "a\"b.txt" "a\\b.txt"
0 -> /bin/ls
0 -> ^C
```
