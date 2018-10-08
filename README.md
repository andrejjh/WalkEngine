# Walk Engine in a nutshell
Walk engine is a text to text tool that is highly flexible.
It takes a *grammar*, *input text* and *program(GDL)* to generate *output text(s)*.

![graph](./doc/WalkEngine.png)

## Key features

* Walk is highly **flexible** and can ingest any  *input text*, as a *grammar* explains how this input should be interpreted.
* Walk can generate any text , as you describe in a *GDL program* what will appear in the *output text*.
* Walk follows a **minimalist approach** and only depends on standard C++ Libraries such as *stdio, stdlib, iostream and regex*.
* Walk is completely written is C++ and thus **incredibly fast**.

## Where to begin?
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
This should normally results in a WalkEngine appearing in the /bin directory.

## 1. Input text
Walk engine was designed to take any text file in input. Some examples are provided in the /test directory. Have a look at [The Tragedy of Hamlet, Prince of Denmark](./test/theater/HamletActI.tht) as an example.
Don't be worried by the .tht extension, it is simply there to remind you this text follows the **Theater** grammar.(see below)

## 2. Grammar
A grammar describes in a set of *simple rules* how you input text is structured and should be interpreted, eg theater.meta starts like this:
````
theater        grammar $play $skip
skip            manyOf $comment #null #null #null
comment         sequence '//' $rol $eol
play            sequence name=$rol $eol $acts
...
rol             token '^.+'
eol             token '^\n'
````
The first line tell you that *'theater'* is a **grammar**, *'play'* is the grammar's main rule and *'skip'* explains what text should be ignored (in this case one or more comment(s) such as ''// this is a comment.'). The last line informs you that the input text is a *play* including a name (*'rol'* stands for rest of line, *'eol* for end of line') and *acts*.
Four types of rules are supported in a *grammar*: *sequence*, selection (=*oneOf*), repetition (=*manyOf*) and regular expressions (=*token*).

## 3. GDL
Generation Description Language (or GDL) is a programming language that helps you to describe the output(s) to generate.
### 3.1 Hello World!
````
Hello World!
````
This GDL program will output the text: 'Hello World!'.

### 3.2 Play's Title
````
$VAR[top.name]
````
This will produce 'The Tragedy of Hamlet, Prince of Denmark' the *name* of the *play*, the top object of a theater grammar.

### 3.2 Play Table of content
This GDL program:
````
$VAR[top.name]$CR
$FOR[top.acts,a]
  Act $VAR[a.id]$CR
  $FOR[a.scenes,s]
    Scene $VAR[s.id]:$VAR[s.location]$CR
  $ENDFOR[]
$ENDFOR[]
````
will produce a table of contents for Hamlet's play:
````
The Tragedy of Hamlet, Prince of Denmark
Act I
    Scene 1: Elsinore. A platform before the castle.
    Scene 2: A room of state in the castle.
    Scene 3: A room in Polonius' house.
    Scene 4: The platform.
    Scene 5: Another part of the platform.
  Act II
    Scene 1: A room in POLONIUS' house.
    Scene 2: A room in the castle.
  Act III
    Scene 1: A room in the castle.
    Scene 2: A hall in the castle.
    Scene 3: A room in the castle.
    Scene 4: The Queen's closet.
  Act IV
    Scene 1: A room in the castle.
    Scene 2: Another room in the castle.
    Scene 3: Another room in the castle.
    Scene 4: A plain in Denmark.
    Scene 5: Elsinore. A room in the castle.
    Scene 6: Another room in the castle.
    Scene 7: Another room in the castle.
  Act V
    Scene 1: A churchyard.
    Scene 2: A hall in the castle.
````
The rest is ignored.

### 3.3 Marked down formatting
You want to format the text in markdown, see this:
````
$CLASS[act]$SCRIPT[show]# Act $VAR[me.id]$CR$FOR[me.scenes,s]$ONDO[s,show]$ENDFOR[]$ENDSCRIPT$ENDCLASS
$CLASS[scene]$SCRIPT[show]## Scene $VAR[me.id]: $VAR[me.location]$CR$FOR[me.things,t]$ONDO[t,show]$ENDFOR[]$ENDSCRIPT$ENDCLASS
$CLASS[fact]$SCRIPT[show]*$VAR[me.com]*$CR$ENDSCRIPT$ENDCLASS
$CLASS[tirade]$SCRIPT[show]$VAR[me.speaker]:$CR$FOR[me.sentences,s]$ONDO[s,show]$ENDFOR[]$CR$ENDSCRIPT$ENDCLASS
$CLASS[sentence]$SCRIPT[show]> $VAR[me.rol]$CR$ENDSCRIPT$ENDCLASS
$FOR[top.acts,a]$ONDO[a,show]$ENDFOR[]````
````
Yes, GDL has some sort of object abstraction as you can invoke script on classes very similar to C++ virtual functions.
This code will generate [markdown](./test/theater/Hamlet.md) that can easily be turned into this nice [PDF](./test/theater/Hamlet.pdf).

## How could that be!
WalkEngine executes as follow:
1. Read the *grammar* and store it
2. Read/Parse the *input* file according to the grammar
3. Read/Parse the *generation program* (or gdl)
4. Run this GDL program and generate the *output*
5. Last but not least, delete all these (C++) objects. No Garbage collector here!

A typical run will look like this:
````script
$ ../../Bin/WalkEngine theater.meta HamletActI.tht title.gdl out
Meta parsing...
... end meta parsing
Source parsing...
... end source parsing
Parsing your WALK program...
... end of WALK program load/parse
Executing your WALK program...
... end of execution(1 statements).
Deleting program
Deleting source
Deleting parser
Deleting meta
````
### Parameters
WalkEngine normally takes four parameters:
* a grammar (.meta file)
* an input text file
* a Walk generation program (.gdl file)
* an output text file

## History
I developed this code generator in 1996 while working on *IVS* (an early VoD system built on top of *Aperios OS*) at Sony Europe.
At that time it was mostly used to convert [IDL](https://en.wikipedia.org/wiki/Interface_description_language) into C++ or HTML and to draw [Message Sequence Charts](https://en.wikipedia.org/wiki/Message_sequence_chart) from text.
*-To be completed-*

On Aperios OS, read [SONY'S PLAN FOR WORLD RECREATION](https://www.wired.com/1999/11/sony-3/) by David Sheff, 1999.
