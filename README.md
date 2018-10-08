# Walk Engine in a nutshell
Walk engine is a text to text tool that is highly flexible.
It takes a *grammar*, *input text* and *program(GDL)* to generate *output text(s)*.

![graph](./doc/WalkEngine.png)

## Key features

* Walk can ingest any  *input text*, as a *grammar* explains how it should be interpreted.
* Walk can generate any *output text*, as stated in the *GDL program*.
* Walk is written is C++ and therefore incredibly fast.

## How to start?
0. Installation
1. Input text
2. Why do I need a grammar?
3. How do I generate output?


## 0. Installation
Walk engine was written in C++ more than 20 years ago on SGI workstations. The current implementation runs on MacOSX and should run on any UNIX machines. First, download this git repository. Then build the executable from C++ Sources:
````
cd src
make
````
This should results in a WalkEngine in the /bin directory.

## 1. Input text
Walk engine was designed to take any text file in input. Some examples are provided in the /test directory. Have a look at [The Tragedy of Hamlet, Prince of Denmark](./test/theater/HamletActI.tht)
Don't be worried by the .tht extension, it simply there to tell you this text file follow the Theater grammar.  

## 2. Grammar
A grammar describe in *simple rules* how you input text file is structured, eg theater.meta start with these four lines:
````
theather        grammar $play $skip
skip            manyOf $comment #null #null #null
comment         sequence '//' $rol $eol
play            sequence name=$rol $eol $acts
````
The first one tell you that *theater* is a **grammar**, $play is the grammar's main rule and $skip explain what part of the text should be ignored (one or more comments such as ''//this'). The last line informs you that the input text is a *play* including a name and some *acts*.

## 3. Generation
Generation Description Language (or GDL) is a descriptive programming language that helps you to customize the output(s) to generate. 
### 3.1 Hello World!
````
Hello World!
````
This (simple) GDL program will output the string 'Hello World!'

### 3.2 Play's Title
````
$VAR[top.name]
````
This will produce 'The Tragedy of Hamlet, Prince of Denmark' the *name* of the *play*.

### 3.2 Play Table of content
````
$FOR[top.acts,a]
  Act $VAR[a.id]$CR
  $FOR[a.scene,s]
    Scene $VAR[s.id]$CR
  $ENDFOR[]
$ENDFOR[]
````
This will produce a table of contents for Hamlet's play something like this:
````
Act I
    Scene 1
    Scene 2
    Scene 3
    Scene 4
    Scene 5
  Act II
    Scene 1
    Scene 2
  Act III
    Scene 1
    Scene 2
    Scene 3
    Scene 4
  Act IV
    Scene 1
    Scene 2
    Scene 3
    Scene 4
    Scene 5
    Scene 6
    Scene 7
  Act V
    Scene 1
    Scene 2
````

## History
I developed this code generator in 1996 while working on *IVS* (an early VoD system built on top of *Aperios OS*) at Sony Europe.
On Aperios OS, read [SONY'S PLAN FOR WORLD RECREATION](https://www.wired.com/1999/11/sony-3/) by David Sheff, 1999.
