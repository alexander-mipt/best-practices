# Bash Notes

### Antipov Alexander

# Making printing faster

## Key Bindings

**Ctrl+A** - go to the start of bash text line

**Ctrl+E** - to the end of line

**Ctrl+K** - clear line after cursor

**Ctrl+U** - clear line before cursor

**Ctrl+L** - clear screen

**Ctrl+R** + <some text> - recursive search of previous commands (tap **Ctrl+R** for switching results, **ESC** - for exit search)

### copy / past

**Ctrl + C/V** - windows shell

**[ Ctrl + Shift + C/V ] or [ right click / right click ] or [ mouse wheel click / mouse wheel click ]** - linux shell

**Simple text highlighting** / **right click** - supported in MobaXTerm

### inline commands & directory navigation

```bash
# run the last executed command
!!
# run the n-th line in history
!n
# run the last executed command with <keyword>
!<keyword>
# run the last executed command which contains <keyword>
!?<keyword>?

# switch to prev directory
cd -
# switch to home
cd
```

# Making search faster

## Manual (`man man`)

Manual consists of **9 parts**. It is important to know the description of called command:

1. -- shell commands (`man 1 <command>`)
2. . -- system calls (`man 2 <command>`)
3. -- libc functions (`man 3 <command>`)

## Man pages synopsys

![image-20220430154518232](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430154518232.png)

![image-20220430155558560](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430155558560.png)

* `<elem>`  -- elem is mandatory
* `{}` - are used in conjunction with `-a|-b` and <u>not</u> optional

## Man navigation

Short command info

````bash
man -f <command>
whatis <command>
````

Search keyword in a short info

````bash
man -k <keyword>
````

**Arrows, Page Up/Down** - page strolling

**u / d** - half page strolling

`/<key1>|<key2>|<key3>` + **Enter** - regex search of set of keys; **n / N** - switching occurrences (next / previous)

`-N` / `-n` - numeric lines ON / OFF

## Simple commands

`which`, `whereis`, `whatis (equvivalent to man -f)`

```bash
# short info
alexander@alexander-VirtualBox:~/tmp2$ man -f which whereis
which (1)            - locate a command
whereis (1)          - locate the binary, source, and manual page files for a command

# usage
alexander@alexander-VirtualBox:~/tmp2$ which gcc
/usr/bin/gcc
alexander@alexander-VirtualBox:~/tmp2$ whereis gcc
gcc: /usr/bin/gcc /usr/lib/gcc /usr/share/gcc /usr/share/man/man1/gcc.1.gz
alexander@alexander-VirtualBox:~/tmp2$ which gucc
alexander@alexander-VirtualBox:~/tmp2$ 
```

> Instead of using `man -f` type `whatis` command - the same output.

## Grep, find

````bash
find $where_to_find -name $filename_or_dir
grep -r "$match_string" $where_to_find
````

### useful find flags

```bash
-maxdepth
-type
-newer
-name / -wholname
```

### useful grep flags

```bash
rgrep --exclude-dir=[cmake,build] --binary-files=without-match
-E # Extended regex
-e # find pattern
-o # only matching
-n # line number
...
```

# About logging

## History (`man history`)

Bash command history. Contains last `HISTSIZE` commands (see `~/.bashrc` file) of terminal session/

Append <= `HISTSIZE` lines to the file `~/.bash_history`:

```bash
history -a
```

Clear local history:

```bash
history -c
```

Ignore duplicates:

```bash
# in ~/.bashrc:
HISTCONTROL=ignoreboth:erasedups
```

**Ctrl+R** + <some text> - recursive search of previous commands (tap **Cntrl+R** for switching results, **ESC** - for exit search)

## Script (`man script`) 

Creates **child session** and store **all commands and output of terminal session** into file:

```bash
script terminal.log
```

Use `less -r` to see it. But there can be some conflicts with **control characters**. That's why you can use vim too.

<img src="C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220625200844027.png" alt="image-20220625200844027" style="zoom:67%;" />

```bash
cat term2.log | less -r
```

<img src="C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220625201008804.png" alt="image-20220625201008804" style="zoom:67%;" />

```bash
vim term2.log
```

<img src="C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220625201413959.png" alt="image-20220625201413959" style="zoom:67%;" />

# Bash Environment (`man bash`)

## Local variables

Local variables -- variables that are present within the current instance of the shell (or function). <u>Not available for programs started in a child shell session.</u>

```bash
# print all local variables
set | less
# set new one in a shell or shell script
SOMEVAR="some value string"
# print it
echo $SOMEVAR
# delete declarated var
unset SOMEVAR
```

Local variables are used for transfering variables to the process only, for example:

```bash
CC=/opt/riscv32/bin/riscv32-unknown-elf-gcc ../configure --host=/opt/riscv32
```

Environment data in C sources can be accessed via `envp` (`argc`, `argv`, `evnp` - 3 arguments of `main` function)

## Environment variables

Unlike local variables <u>these variables can be accessed via child shell sessions.</u>

```bash
# print available environment
env | less
# declaration of new envrionment var
export ENVAR="3"
# delete it
export -n ENVAR
```

> In bash manual there is a definition of shell variables - the 3rd group of variables used for shell runtime. They can be either local or global.

## Making your own bash environment

It is better to use private shell script (`my_env.sh` for instance) for setting up environment on the start of the shell. For this purpose make `chmod +x my_env.sh`, add its path to `~/.bashrc`.

>  `~/.bashrc` executes on the start of running new local shell session
>
> `~/.profile`, `~/.bash_profile` execute on the start of SSH session and include `~/.bashrc`.

## Special variables

Many of them are used in shell scripts, but ones can be used in shell command prompt.

```bash
# Exit status or status of last executed command (most popular)
echo $?
```

```bash
# Filename of current shell script
echo $0
# n = 1, 2, ... - positional argumnts of shell script
echo ${1} ${2}
# The number of arguments supplied to a script
echo $#
# PID of current shell
echo $$
# PID of last background command
echo $!
# string of all arguments: "arg1 arg2 arg3"
echo $*
# list of all arguments: "arg1" "arg2" "arg3"
echo $@
```

## Aliases

Allows to rename frequently used bash commands to short prints. Default aliases are located in `~/.bashrc`. To add new one it is recommended way to store it into `~/.bash_aliases`.

Print all available aliases:

```bash
alias
```

Creating & deleting alias:

```bash
# create alias named word for <command>
alias word=`<command>`
# delete word alias
unalias word
```

Example:

```
# filtered text search
alias grep=`grep -r --exclude-dir=[cmake,build]` --binary-files=without-match`
```

> `alias rm=rm -i` will work incorrectly w/ `rm -rf <filedir>`. Why?

# Command pipeline

Redirect stdout of the 1st command to the stdin of the next command:

```bash
cat trace_log.txd | grep "addi" | less -S
```

Pipelined execution:

```bash
# <cmd2> will run only if <cmd1> has executed correctly
<cmd1> && <cmd2>
# <cmd2> will run after <cmd1>, exit status of <cmd1> is ignored
<cmd1> ; <cmd2>

# what does it mean?
<cmd1> || <cmd2>
```

Example:

```bash
traceGen.py > log.txt && process.py log.txt > result.csv && echo $?
```

# File stream redirections

```bash
# redirect stdout of <cmd> to the <file>
<cmd> > <file>
# redirect stdout of <cmd> to the file and APPEND data
<cmd> >> <file>
# redirect stderr
<cmd> 2> <file>
# redirect both stdout, stderr to the <file>
<cmd> &> <file>

# redirect both input and output from/to file
program.out < input.log > output.log
```

# Process interaction

![img](https://habrastorage.org/webt/dw/ru/5n/dwru5nbeoag7imroc_ki1qa9gba.png)

![image-20220707140043780](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707140043780.png)

[process states and livecycle](https://habr.com/ru/post/423049/)

```bash
# process states
R  Running
S  Sleeping in an interruptible wait
D  Waiting in uninterruptible disk sleep
Z  Zombie
T  Stopped (on a signal) or (before Linux 2.6.33) trace stopped
t  Tracing stop (Linux 2.6.33 onward)
W  Paging (only before Linux 2.6.0)
X  Dead (from Linux 2.6.0 onward)
x  Dead (Linux 2.6.33 to 3.13 only)
K  Wakekill (Linux 2.6.33 to 3.13 only)
W  Waking (Linux 2.6.33 to 3.13 only)
P  Parked (Linux 3.9 to 3.13 only)
```



## Foreground vs background processes

`sudo apt install htop` for installing simple and pretty inline task manager <u>where all commands and keys bindings are subscribed.</u>

But in many cases the number of background processes run by your own do not exceed 10 and can be simply managed as jobs:

```bash
jobs
```

![image-20220430173444361](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430173444361.png)

Return background process to the foreground:

```bash
# return process [n] to the foreground
fg %n
# %+ / %- are last / previous jobs
# return process [n] to the background
bg %n
```

![image-20220430174235207](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430174235207.png)

> Describe the log.

## Professional killer

![image-20220706192112559](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706192112559.png)

![image-20220706193142108](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706193142108.png)

![image-20220706193410883](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706193410883.png)

![image-20220706193507307](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706193507307.png)

![image-20220706193621524](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706193621524.png)

# Search engines

## Regular expressions (`man grep` + `/BRE` + **Enter**)

Google regex101.com for training and testing regular expressions.

Regexepr support BRE (basic), ERE (extended) and PCRE (Perl) syntax. BRE syntax is generally the same in different utilities, but ERE and PCRE can be a little bit different in some utilities as `awk`, `sed`, `grep`, `python3`, etc.

### Idea (step-by-step thinking)

1. Target characters:

   ```
   a b .. z A .. Z
   0 1 2 .. 9
   \( \" \t \n \' \{
   ```

   Additional special symbols: line start, end, word boundary:

   ```
   ^ $ \b
   ```

   or ANY SYMBOL:

   ```
   .
   ```

2. Grouping and groups (unique for every utility or engine):

   ```
   # general sequence of characters group
   abc
   # "one of us" group
   [abc]
   # "all digits" group - see grep manual
   [::digit::]
   ```

3. Group repetition symbols (applies to groups):

   Zero or more / one or more / zero or one / non-matched group / repetition count

   ```
   * + ? [^abc] {2,4}
   ```

   

4. Back references (not everywhere) - refer to previous matched groups:

   ```
   \1 \2 \3
   ```

   > Number means position number of matched group, group should surrounded by braces `( )`.

4. Flags (global text search, or line by line search, or smth else)

   ```
   \g \w ...
   ```

> Advice: use `rgrep`, not `grep`. It works faster!

### Examples

![image-20220430184854280](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430184854280.png)

## Wildcards

##### Do not mix wildcard and regex patterns! They use some same symbols as `?`, `*`, `[-]` but have DIFFERENT INTERPRETATION!

### Wildcard meaning

`*` - any sequence of symbols (<u>not applied as group repetition symbol</u>)

> Note: 
>
> Wildcard `*` **does not match hidden files**.

`?` - zero or one symbol ((<u>not applied as group repetition symbol</u>))

`[abc]`,`[a-c]` - group of ONE character that can be `a`, `b` or `c`.

### Wildcards usage examples

![image-20220430191014294](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430191014294.png)

# Inline array syntax

```bash
list=(sasha masha sasa a h)
# print num of elems
echo ${#list[*]}

# print elem indexes
for i in ${!list[*]}; do echo $i; done

# print strlen() of one's elem
echo ${#list[0]}

# print elems in idx range [2:5]
echo ${list[*]:2:5}

# print all w/ exclude
for i in ${list[@]}; do if ! [[ $i =~ ^(sasha|masha) ]]; then echo $i; fi; done
for i in ${list[@]}; do if ! [[ $i =~ ^(s) ]]; then echo $i; fi; done
for i in ${list[@]}; do if ! [[ $i =~ ^(s$) ]]; then echo $i; fi; done
```



# VIM (`sudo apt-get install vim`)

## Vim Basics

**Insert / ESC** - set insert / visual mode for enable / disable editing

`:q` - exit vim

<u>When visual mode enabled</u>, you can do the following:

***** - find a match pointed by cursor

`:n` go to line n

> `:1` - go to start ( like **g** in `less`)
>
> :1000000000.... - go to end (like **Shift+g** in `less`)
>
> :$
>
> **Ctrl+g** - view name of file

`/<keyword>` - search pattern <keyword>

![image-20220430192322179](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430192322179.png)

> Enter command `:set hlsearch` for yallow highlight of all patterns. Pay attention to regex pipe: `\|`. In less the command is differnt a bit.

![image-20220430192604102](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220430192604102.png)

You can configure Vim editor by adding new file `~/.vimrc` (or it has existed already) and add:

```vim
set mouse=a				"" mouse activation
set number				"" numeric lines
syntax on				"" highlight syntax
set listchars=space:. 	"" type :set list / :set nolist for marking/unmarking spaces in a code

vmap <C-c> hyi			"" 
vmap <C-v> hdi
imap <C-u> <ESC>ui
```



# System info

## OS basics (if you got "*knocked out!*")

```bash
alexander@alexander-VirtualBox:~/tmp2$ whatis --section=1 whoami uname date
whoami (1)           - print effective userid
uname (1)            - print system information
date (1)             - print or set the system date and time
```

```bash
# get pid of current terminal session
# can be used for learing linux process (ls /procs/$$/*)
echo $$
```

![image-20220625213336795](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220625213336795.png)

## Memory usage

### RAM

```bash
# info
alexander@alexander-VirtualBox:~/tmp2$ whatis --section=1 free
free (1)             - Display amount of free and used memory in the system
alexander@alexander-VirtualBox:~/tmp2$ free
              total        used        free      shared  buff/cache   available
Mem:        8143196      665640     5104104       28156     2373452     7198696
Swap:        459208           0      459208
```

### Disks & filesystems (see [src](https://laptrinhx.com/how-to-view-free-disk-space-and-disk-usage-from-the-linux-terminal-3636177735/))

#### Global

![image-20220625215622106](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220625215622106.png)

#### Current directory

```bash
# info
alexander@alexander-VirtualBox:/home$ whatis du
du (1)               - estimate file space usage

# example 1
alexander@alexander-VirtualBox:/home$ du -h --max-depth=1 ~
3,0M	/home/alexander/snap
236K	/home/alexander/Pictures
30M	/home/alexander/.mozilla
4,0K	/home/alexander/Music
4,0K	/home/alexander/Templates
4,0K	/home/alexander/tmp2
4,0K	/home/alexander/Public
196K	/home/alexander/.config
2,4M	/home/alexander/github
16K	/home/alexander/.ssh
4,0K	/home/alexander/Documents
4,0K	/home/alexander/Desktop
104M	/home/alexander/.cache
4,0K	/home/alexander/Downloads
792K	/home/alexander/.local
4,0K	/home/alexander/Videos
12K	/home/alexander/.dbus
141M	/home/alexander

# example 2
alexander@alexander-VirtualBox:/home$ du -sh ~
141M	/home/alexander

# example 3
alexander@alexander-VirtualBox:/home$ du -sb ~
143074275	/home/alexander

# example 4
alexander@alexander-VirtualBox:/home$ du -sm ~
141	/home/alexander
```

### CPU

> CPU - state machine with **combinational** and **sequential** logic.

![image-20220707140541083](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707140541083.png)

![image-20220707140636015](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707140636015.png)

![image-20220707140738850](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707140738850.png)

![image-20220707140826771](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707140826771.png)

![image-20220707140924225](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707140924225.png)



#### Types of parallelism

<img src="C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707000138347.png" alt="image-20220707000138347" style="zoom:80%;" />

#### Processing hierarchy (architecture topology) 

![image-20220706231242424](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706231242424.png)
$$
N_{\text{Logic Cores}} = S \cdot P \cdot C \cdot T = (P, C, T)
$$

> S - number of sockets; P - number of processors (CPUs) per socket; C - number of cores per processor; T - number of threads per core.

![image-20220706232726321](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706232726321.png)

### Statistics

![image-20220706233111452](C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220706233111452.png)

## Kernel configuration

```bash
alexander@alexander-VirtualBox:~$ whatis sysctl
sysctl (8)           - configure kernel parameters at runtime
```

> `sysctl` configuration changes are **active before reboot.**

```bash
# show kernel configuration
sysctl -a | less -S
# edit kernel configuration
alexander@alexander-VirtualBox:~$ sudo sysctl kernel.hostname=alectroid
# or
sudo echo alectroid > /proc/sys/kernel/hostname

# Ctrl + Alt + T
alexander@alectroid:~$

# load settings
sudo sysctl --load # load from /etc/sysctl.conf
sudo sysctl --load <filname>.conf # load from user's file w/ .conf suffix
sudo sysctl --system # load from all kernel config file in the following priority

```

## Process information

```bash
proc (5)             - process information pseudo-filesystem
```

Contains files & directories belonged to all system of processes. Directories `/proc/<pid>/` contain the whole information about process with current `<pid>`. For exapmle:

```bash
# display mappings of current process
cat /proc/$$/maps
```

<img src="C:\Users\alexa\AppData\Roaming\Typora\typora-user-images\image-20220707130729399.png" alt="image-20220707130729399" style="zoom:80%;" />

[process](https://habr.com/ru/post/423049/)

[proc](https://losst.ru/fajlovaya-sistema-proc-v-linux)

`man proc, man mmap, man smap`
