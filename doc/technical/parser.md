\page technicalParser Parser
\tableofcontents

## Cases
### Preamble
Grammar:
```
preamble       : statementlist "=="
               ;

statementlist  :
               | statement statementlist
               ;

statement      : "import" IDENT
               | "doctype" IDENT dictionary
               ;

dictionary     : "{" dictentrylist "}"
               ;

dictentrylist  :
               | dictentry "," dictentrylist
               ;

dictentry      : IDENT ":" value
               ;

value          : text
               | number
               | array
               | dictionary
               ;

text           : "(\\"|\\\\|[^"])*"
               | [^,]*
               ;

number         : digitstr
               | digitstr "." digitstr
               ;

digitstr       : digit
               | digit digistr
               ;

digit          : 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
               ;

array          : "[" arrayentrylist "]"
               ;

arrayentrylist :
               | value "," arrayentrylist
               ;
```

### Commands
```
\command{arg}{arg}{no-arg}
```

`command` `arg` `arg` `{` `n` `o` `-` `a` `r` `g` `}`

### Environments
Content **must** be indented by at least one space (the exact amount is taken from the first line of content)
```
[environment>]
    Content
[<environment]
```

Environment with arguments:
```
[environment>]{arg1}{arg2}{arg3}
    Content
[<environment]
```

### Math
Inline math:
```
Text text text $a^2 + b^2 = c^2$ text text text
```

Block math:
```
Text text text
$$a^2 + b^2 = c^2$$
text text text
```


### Subscripts, Superscripts and Underscores
In Textmode the following are not identical:
```
hello_world
hello_{world}
```
The former produces `hello_world` and the latter <code>hello<sub>world</sub></code>.

*Maybe, in mathmode, both should mean the subscript*

In math- and textmode the following similarly produce `hello^world` and <code>hello<sup>world</sup></code> respectively:
```
hello^world
hello^{world}
```

### Active Charactersequences
An active charactersequence can be used to overload characters with special meaning, e.g.:
1. `# Section` as a shortcut for `\section{Section}`
2. `--` for an endash
3. `---` for an emdash

or even declare delimited active character sequences, e.g.:
1. `**text**` for `\bf{text}`
2. `//text//` for `\it{text}`
3. `|a|` for `\abs{a}`



## Procedure
