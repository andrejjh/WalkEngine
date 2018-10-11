# An Introduction to GDL
Generation Description Language (GDL) is a programming language that helps you to describe the text output(s) you want to generate.
You will learn the basic concept of this language.
Material of this tutorial can be find in the /test/theater directory.
Before reading the tutorial, take close look at the input file([HamletActI.tht](../test/theater/HamletActI.tht)) and the associated grammar ([theater.meta](../test/theather/theater.meta)).

## A. Hello World!
Let's start with the classical 'Hello World!'. This GDL program:
````
Hello World!
````
will output the text: 'Hello World!'.
> As simple as that!

## B. Title
Now we want to display the title of the play. The GDL code:
````
$VAR[top.name]
````
will produce 'The Tragedy of Hamlet, Prince of Denmark' the *name* of the *play*, the top node of a theater grammar.
> Not too difficult!

## C. Table of content
As play are structured into *acts* and *scenes* it would be good to generate a summary, without all sentences. See this GDL code:
````
$VAR[top.name]$CR
$FOR[top.acts,a]
  Act $VAR[a.id]$CR
  $FOR[a.scenes,s]
    Scene $VAR[s.id]:$VAR[s.location]$CR
  $ENDFOR[]
$ENDFOR[]
````
> Wait a moment, I get lost!

OK, this one need a bit of explanations:
* *\$VAR[]* outputs a node attribute.
* *\$FOR...\$ENDFOR* iterates through node containers such as acts or scenes.
* *\$CR* produce an end-of-line.

The GDL program above will produce a table of contents for Hamlet's play:
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

## D. Formatting
Now, we'll do some markdown formatting:
````
$CLASS[act]$SCRIPT[show]# Act $VAR[me.id]$CR$FOR[me.scenes,s]$ONDO[s,show]$ENDFOR[]$ENDSCRIPT$ENDCLASS
$CLASS[scene]$SCRIPT[show]## Scene $VAR[me.id]: $VAR[me.location]$CR$FOR[me.things,t]$ONDO[t,show]$ENDFOR[]$ENDSCRIPT$ENDCLASS
$CLASS[fact]$SCRIPT[show]*$VAR[me.com]*$CR$ENDSCRIPT$ENDCLASS
$CLASS[tirade]$SCRIPT[show]$VAR[me.speaker]:$CR$FOR[me.sentences,s]$ONDO[s,show]$ENDFOR[]$CR$ENDSCRIPT$ENDCLASS
$CLASS[sentence]$SCRIPT[show]> $VAR[me.rol]$CR$ENDSCRIPT$ENDCLASS
$FOR[top.acts,a]$ONDO[a,show]$ENDFOR[]````
````
Euh..yes, as you can see GDL has some sort of object abstraction:
* *\$CLASS[]...\$ENDCLASS* declares a GDL class
* *\$SCRIPT[]..\$ENDSCRIPT* declares a class's script similar to C++ virtual functions.
* *\$ONDO[]* invokes a class script and of course, the script may vary from class to class.

So far so good. This code will generate [markdown](./test/theater/Hamlet.md) that can easily be turned into this nice [PDF](./test/theater/Hamlet.pdf).

## E. More than one output
Here we will split Hamlet's text into several files,so that each character get its own text.
````
$SCRIPT[redirect]
$SETSTRING["./tirades/",file]$APPEND[fileName,file]$APPENDSTRING[".md",file]$OFSTREAM[file]
$ENDSCRIPT
````
The *redirect* GDL script will open(in append mode) the character file to add the tirade's sentences.
````
$CLASS[tirade]
$SCRIPT[explore]
$SET[me.speaker,fileName]$DO[redirect]
$FOR[me.sentences,s]$ONDO[s,show]$ENDFOR[]$CR
$ENDSCRIPT
$ENDCLASS
````
The tirade.explore script invokes *redirect* to change the output file.

The file name is constructed as: './tirades/<character>.md'.
See [BERNARDO's text](../test/tirades/BERNARDO.md) and the others.
