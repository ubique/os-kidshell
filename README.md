# kidshell

Basic shell. Supports running commands, `export`, `unset`, `exit` and `cd`. Can
do basic tilde(`~`, `~user~`), variable(`$HOME`) and arithmetic(`$[1 + 3]`)
expansions as well as command substitution(`$(ls)`).

NOTE: hw version doesn't support running commands from $PATH. (I don't want to reimplement `execvp`)
