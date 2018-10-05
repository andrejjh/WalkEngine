                proc Init {} {  
global argv psfile verbose origin  
set psfile " "  
set origin " "  
set verbose 0  
set version [info tclversion]  
for { set c 0} {$c < [llength $argv]} {incr c} {  
set arg [lindex $argv $c]  
set type [string range $arg 0 1]  
if { $type == "-p"} {  
	if { $version == "8.5" } { set psfile [string range $arg 2 [string length $arg]] } 
}  
if { $type == "-o"} {  
	set origin [string range $arg 2 [string length $arg]]  
	if { [string index $origin 0] != "/"} {  
		set path [pwd] append path "/" append path $origin  
		set origin $path 
	} 
}  
if { $arg == "-notes"} {  
	set verbose 1 
} 
}  
global nof_notes notesT notesC  
set nof_notes 0  
global lbEntryCount  
set lbEntryCount 0  
global commentCount  
set commentCount 0  
global nof_objects  
set nof_objects 0  
global next_object  
set next_object 0  
global nof_messages  
set nof_messages 0  
global startY  
set startY 0  
global smallFont  
set smallFont "-adobe-times-bold-r-normal--12-120-75-75-p-67-iso8859-1"  
global largeFont  
set largeFont "-adobe-times-bold-r-normal--14-140-75-75-p-77-iso8859-1" 
listbox .lb 
canvas .c -bg bisque -width 800 -height 800 
# image create photo .c.logo_image -file "ivs_logo.gif" 
}  
proc SCALE {scaleX scaleY} {  
global xgrid ygrid  
set xgrid $scaleX  
set ygrid $scaleY  
global currX currY  
set currX [expr $xgrid / 2]  
set currY [expr $ygrid * 5]  
global currObj objects  
set currObj -1  
for { set o 0} {$o < 10} { incr o 1} {  set objects($o) 0 } 
}  


proc Exit {} { 
	global psfile verbose 
	global lbEntryCount smallFont 
	global nof_objects objects 
	global currX currY xgrid ygrid 
	global nof_notes notesT notesC 
	if {$lbEntryCount != 0} { pack .lb -side bottom return } 
	.c create line [expr $currX - 10 ] $currY [expr $currX + 10] $currY -fill blue -width 3 
	set maxY 0 
	for {set o 0} {$o < 10} { incr o 1} { 
		if {$objects($o) > $maxY} { set maxY $objects($o) } 
	} 
	incr maxY $ygrid 
	for {set o 0} {$o < 10} { incr o 1} { 
		if {$objects($o) > 0} { 
			set x [expr [expr $o * $xgrid] + $xgrid] 
			.c create line $x $objects($o) $x $maxY -fill gray 
			.c create line [expr $x - 20] $maxY [expr $x + 20] $maxY -width 2 
		} 
	} 
	incr maxY $ygrid 
	set endDrawY $maxY 
	.c create line 0 $maxY 800 $maxY -fill gray -width 3 
	if { $verbose } { 
		for {set n 0} {$n < $nof_notes} {incr n} { 
			set text $notesT($n) 
			set t [.c create text [expr $xgrid / 4] $maxY -text $text -font $smallFont -fill $notesC($n)] 
			set coord [.c bbox $t] 
			set dy [expr [lindex $coord 3] - [lindex $coord 1]]
			.c move $t [expr [expr [lindex $coord 2] - [lindex $coord 0]] / 2] [expr $dy /2] 
			incr maxY $dy 
			if { [expr $maxY % 800] > [expr 800 - $ygrid] } { incr maxY $ygrid } 
			incr maxY [expr $ygrid /4] 
		} 
	} 
	if { $maxY > 800 } { 
		set pageTop 800 
		set pageNb 1 
		set pageCount [expr 1 + [expr [expr $maxY - 1] / 800 ]] 
		while { $pageNb <= $pageCount } { 
			set t "page " 
			append t $pageNb 
			append t " of " 
			append t $pageCount 
			drawText 700 $pageTop $t $smallFont red biscue 0 -1 
			.c create line 0 $pageTop 800 $pageTop -fill red -width 1 
			incr pageTop 800 
			incr pageNb 
		} 
		set maxY [expr $pageCount * 800] 
	}
	.c configure -scrollregion [list 0 0 800 $maxY] 
	.c configure -yscrollcommand ".scrolly set" 
	scrollbar .scrollx -command ".c xview" -orient horizontal 
	scrollbar .scrolly -command ".c yview" 
	pack .scrolly -side right -fill y  
	pack .scrollx -side bottom -fill x 
	set text "none" 
	pack .c -side top 
	pack .lb -side bottom 
	if { $psfile != "" } { # generate a postscript file update 
		incr maxY $ygrid 
		set pageTop 0 
		set pageCount 0 
		while { $pageTop < $maxY } { 
			set file $psfile 
			append file $pageCount 
			.c postscript -height 800 -width 800 -y $pageTop -file $file -colormode gray 
			incr pageTop 800 
			incr pageCount 1 
		}
	}
} 


proc drawText {x y t f fc bc b w} {  
set t [ .c create text $x $y -text $t -font $f -fill $fc]  
set coord [ .c bbox $t]  
set dy [expr [expr [lindex $coord 3] - [lindex $coord 1]] /2]  
if {$w != 0} {  
	.c move $t 0 [expr $w * $dy] 
}  
set coord [ .c bbox $t]  
set left [expr [lindex $coord 0] - $b]  
set top [expr [lindex $coord 1] - $b]  
set right [expr [lindex $coord 2] + $b]  
set bottom [expr [lindex $coord 3] + $b]  
if {$b !=0} {  
	.c create rectangle $left $top $right $bottom -fill $bc -outline $fc  
	.c raise $t 
} 
return $t 
}  
proc drawOvalText {x y text f fc bc b w} {  
set t [ .c create text $x $y -text $text -font $f -fill $fc]  
set coord [ .c bbox $t]  
set dy [expr [expr [lindex $coord 3] - [lindex $coord 1]] /2]  
if {$w != 0} { .c move $t 0 [expr $w * $dy] }  
set coord [ .c bbox $t]  
set left [expr [lindex $coord 0] - $b]  
set top [expr [lindex $coord 1] - $b]  
set right [expr [lindex $coord 2] + $b]  
set bottom [expr [lindex $coord 3] + $b]  
if {$b !=0} {  
	set oval [ .c create oval $left $top $right $bottom -fill $bc -outline $fc]  
	.c raise $t 
} 
return $t }  

proc BindText {x y text fc b} {  
global largeFont  
set bindText [ 
.c create text $x $y -text $text -font $largeFont -fill $fc]  
set coord [ 
.c bbox $bindText]  
set left [lindex $coord 0]  
set top [lindex $coord 1]  
set right [lindex $coord 2]  
set bottom [lindex $coord 3]  
set width 800  
set height 800 #move text  
if it exceeds canvas borders  
if {$left < 0} {  
.c move $bindText [expr 0 - $left] 0  
set left 0 }  
if {$right > $width} {  
.c move $bindText [expr $width - $right] 0  
set right $width }  
if {$top < 0} {  
.c move $bindText 0 [expr 0 - $top]  
set top 0 }  
if {$bottom > $height} {  
.c move $bindText 0 [expr $height - $bottom]  
set bottom $height } return $bindText }  

proc BindMsgPress {class msg params x y fc} {  
global bindText bindRect  
set text $msg append text "(" append text $params append text ")"  
set b 5  
set bindText [BindText $x $y $text $fc $b]  
set coord [ 
.c bbox $bindText]  
set left [expr [lindex $coord 0] - $b]  
set top [expr [lindex $coord 1] - $b]  
set right [expr [lindex $coord 2] + $b]  
set bottom [expr [lindex $coord 3] + $b]  
if {$b !=0} {  
set bindRect [ 
.c create rectangle $left $top $right $bottom -fill lightgray ]  
.c raise $bindText } }  

proc BindMSCPress {msc comment x y fc} {  
global bindText bindRect  
set text $msc append text $comment  
set b 5  
set bindText [BindText $x $y $text $fc $b]  
set coord [ 
.c bbox $bindText]  
set left [expr [lindex $coord 0] - $b]  
set top [expr [lindex $coord 1] - $b]  
set right [expr [lindex $coord 2] + $b]  
set bottom [expr [lindex $coord 3] + $b]  
if {$b !=0} {  
set bindRect [ 
.c create rectangle $left $top $right $bottom -fill lightgray ]  
.c raise $bindText } 

}  
proc BindDoPress { name comment x y fc} {  
global bindText bindRect largeFont  
set text $name append text " : " append text $comment  
set b 5  
set bindText [BindText $x $y $text $fc $b]  
set coord [ 
.c bbox $bindText]  
set left [expr [lindex $coord 0] - $b]  
set top [expr [lindex $coord 1] - $b]  
set right [expr [lindex $coord 2] + $b]  
set bottom [expr [lindex $coord 3] + $b]  
if {$b !=0} {  
set bindRect [ 
.c create oval $left $top $right $bottom -fill lightgray ]  
.c raise $bindText } }  

proc BindRelease {} {  
global bindText bindRect  
.c delete $bindText  
.c delete $bindRect }  

proc drawObject {x y nm cl} {  
	global class objects nof_objects next_object  
	global smallFont xgrid ygrid  
	set xcenter [expr [expr $x + 1] * $xgrid]  
	drawText $xcenter $y $cl $smallFont red bisque 0 -2  
	drawText $xcenter $y $nm $smallFont blue lightgray 2 0  
	set objects($x) $y  
	set class($x) $cl  
	if { $x == $next_object } { incr next_object } 
	incr nof_objects 
}  

proc addMessage {fromX toX toY toObj fromY} {  
global nof_messages messages currX  
	set msg_index $nof_messages  
	set messages($msg_index,0) $fromX  
	set messages($msg_index,1) $toX  
	if {$currX < $toX} {  
		set messages($msg_index,2) 20 
		} else {  
			set messages($msg_index,2) -20 
		}  
	set messages($msg_index,3) $toY  
	set messages($msg_index,4) $toObj  
	set messages($msg_index,5) $fromY 
	incr nof_messages 
	return $msg_index 
}  
proc ADDNOTE {text color} {  
global nof_notes notesT notesC  
set notesT($nof_notes) "* " 
append notesT($nof_notes) $text  
set notesC($nof_notes) $color 
incr nof_notes 
}  
proc TITLE {nm comment date} {  
global xgrid ygrid  
global largeFont smallFont origin  
set bot [expr $ygrid * 2]  
set right 800 
 
drawText 100 $ygrid $nm $largeFont black black 0 0 
 
drawText 400 [expr $ygrid / 2] $comment $largeFont black black 0 0 
 
drawText 400 [expr [expr $ygrid * 3] / 2] $origin $smallFont indianred indianred 0 0 
 
drawText 700 $ygrid $date $smallFont black black 0 0  
.c create line 0 $bot $right $bot -fill gray -width 5  
set tx 200  
.c create line $tx 0 $tx $bot -fill gray -width 3  
set tx 600  
.c create line $tx 0 $tx $bot -fill gray -width 3 }  

proc OBJECT {nm cl} {  
global xgrid ygrid next_object 
set index $next_object 
drawObject $index [expr 4 * $ygrid] $nm $cl 
return $index 
}  
proc drawMSC {x y nm cl} {  
global class objects nof_objects next_object smallFont  
global xgrid ygrid xMSC yMSC MSCText  
set xcenter [expr [expr $x + 1] * $xgrid]  
set y [expr $y + $ygrid]  
set xMSC $xcenter  
set yMSC $y  
set MSCText [ 
drawText $xcenter $y $nm $smallFont black lightgray 2 -1]  
.c bind $MSCText <Any-ButtonPress> "BindMSCPress \"$mn\" \"$cl\" $xMSC $yMSC black"  
.c bind $MSCText <Any-ButtonRelease> "BindRelease "  
set objects($x) $y  
set class($x) $nm  
if { $x == $next_object } { incr next_object } incr nof_objects }  

proc BEGIN_MSC {nm cl x} {  
global xgrid ygrid next_object currY  
if {$x == 0} {  
set index $next_object } else {  
set index $x }  
set last [string last "/" $nm ]  
if {$last != -1} { incr last  
set len [string length $nm]  
set nm [string range $nm $last $len ] } drawMSC $index $currY $nm $cl return $index }  

proc END_MSC { } {  
global xMSC yMSC xgrid ygrid currY MSCText  
set left [expr $xMSC - [expr $xgrid / 2] ]  
set right [expr $xMSC + [expr $xgrid / 2] ]  
set top $yMSC  
set bottom $currY  
set border [ 
.c create rectangle $left $top $right $bottom -outline black -width 6]  
.c create rectangle $left $top $right $bottom -outline gray -width 5  
.c raise $MSCText $border }  

proc NEW {nm cl x} {  
global xgrid ygrid currX currY next_object  
if {$x == 0} {  
set x $next_object } drawObject $x $currY $nm $cl SKIP 1  
set toX [expr [expr $x + 1] * $xgrid]  
.c create line $currX $currY $toX $currY -arrow last  
set objects($x) $currY return $x }  

proc SEND {obj name params delta reserve} {  
global currX currY ygrid messages nof_messages startY  
global class objects smallFont xgrid ygrid 
SKIP 1  
set toX [expr [expr $obj + 1] * $xgrid]  
set toY $currY  
set X $toX  
	if {$currY <= $objects($obj)} {  
		set toY $objects($obj)  
		if {$delta == 0} { set delta 1 } 
	}  
	if {$delta != 0} { incr toY [expr $reserve * $ygrid] }  
	set msg [addMessage $currX $toX $toY $obj $currY]  
	if {$delta == 0} {  
		.c create line $currX $currY $toX $currY -fill blue -arrow last -width 2 
	} else {  
		set X [expr $toX - [expr $delta * $messages($msg.2) ] ]  
		.c create line $currX $currY $X $currY -fill blue -width 2  
		.c create line $X $currY $X $toY -fill blue -width 2  
		.c create line $toX $toY $X $toY -fill blue -arrow first - width 2 
	}  
	set textX [expr [expr $currX + $X] / 2]  
	set t [ drawText $textX $currY $name $smallFont blue blue 0 -1]  
	.c bind $t <Any-ButtonPress> "BindMsgPress $class($obj) \"$name\" \"$params\" $textX $currY blue"  
	.c bind $t <Any-ButtonRelease> "BindRelease "  
	set note "Send: " 
	append note $name 
	append note "(" append note $params append note ")" 
	ADDNOTE $note blue 
	return $msg 
}  

proc CALL {obj name params delta reserve} {  
global currX currY ygrid messages nof_messages startY  
global class objects smallFont xgrid ygrid SKIP 1  
set toX [expr [expr $obj + 1] * $xgrid]  
set toY $currY  
set X $toX  
if {$currY <= $objects($obj)} {  
set toY $objects($obj)  
if {$delta == 0} {  
set delta 1 } }  
if {$delta != 0} { incr toY [expr $reserve * $ygrid] }  
set msg [addMessage $currX $toX $toY $obj $currY]  
if {$delta == 0} {  
.c create line $currX $currY $toX $currY -fill blue -arrow last -width 2 } else {  
set X [expr $toX - [expr $delta * $messages($msg,2) ] ]  
.c create line $currX $currY $X $currY -fill blue -width 2  
.c create line $X $currY $X $toY -fill blue -width 2  
.c create line $toX $toY $X $toY -fill blue -arrow first - width 2 }  
set textX [expr [expr $currX + $X] / 2]  
set t [ 
drawText $textX $currY $name $smallFont blue blue 0 -1]  
.c bind $t <Any-ButtonPress> "BindMsgPress $class($obj) \"$name\" \"$params\" $textX $currY blue"  
.c bind $t <Any-ButtonRelease> "BindRelease "  
set note "Call: " append note $name append note "(" append note $params append note ")" ADDNOTE $note blue return $msg }  

proc atObject {msg name color w a} {  
global currX currY messages startY  
global objects currObj smallFont xgrid ygrid  
if {$currObj != -1} {  .c create line [expr $currX - 10 ] $currY [expr $currX + 10] $currY  -fill blue -width 3 }  
set currX $messages($msg,1)  
set currY $messages($msg,3)  
set currObj $messages($msg,4)  
.c create line $currX $objects($currObj) $currX $currY -fill $color  -width $w -arrow $a  
drawText $currX $currY $name $smallFont $color black 0 -1  
set startY $currY  
if {$objects($currObj) != 0} {  
set objects($currObj) $currY } }  

proc AT {msg name} { atObject $msg $name gray 1 none }  

proc BACK {msg name} { atObject $msg $name blue 1 none }  

proc IN {obj name} {  
global currX currY messages startY objects currObj smallFont xgrid ygrid  
if {$currObj == -1} {  
	set currObj $obj  
	set currX [expr [expr $obj + 1] * $xgrid]  
	set startY [expr $currY + $ygrid]  
	.c create line $currX $currY $currX $startY -fill blue -width 1 -arrow last  
	set currY $startY 
}  
if {$objects($currObj) != 0} {  
	set objects($currObj) $currY 
} 
}  
proc DO {name text} {  
global currY currX smallFont startY objects currObj xgrid ygrid 
SKIP 1  
set t [drawOvalText $currX $currY $name $smallFont darkgreen lightgray 5 1]  
set coord [ .c bbox $t] 
incr currY [expr [lindex $coord 3] - [lindex $coord 1]] 
incr currY 5  
.c bind $t <Any-ButtonPress> "BindDoPress \"$name\" \"$text\" $currX $currY darkgreen"  
.c bind $t <Any-ButtonRelease> "BindRelease "  
set startY $currY  
if {$objects($currObj) != 0} { set objects($currObj) $currY }  
set note "Do: " 
append note $name 
append note "(" 
append note $text 
append note ")" 
ADDNOTE $note darkgreen 
}  
proc SKIP {delta} {  
global currX currY startY currObj objects xgrid ygrid 
	incr currY [expr $ygrid * $delta]  
	if {$currObj != -1} {  
	.c create line $currX $startY $currX $currY -width 3 -fill blue  
	if {$objects($currObj) != 0} {  set objects($currObj) $currY } 
	} 
}  
proc REPLY {obj msg reply replyPar cont contPar delta reserve} {  
global currX currY messages nof_messages  
global startY class objects currObj smallFont xgrid ygrid 
SKIP 1  
set t [drawText $currX $currY $reply $smallFont red gray 2 1]  
.c bind $t <Any-ButtonPress> "BindMsgPress $class($obj) \"$reply\" \"$replyPar\" $currX $currY red"  
.c bind $t <Any-ButtonRelease> "BindRelease "  
set coord [ .c bbox $t] 
incr currY [expr [lindex $coord 3] - [lindex $coord 1]] 
incr currY 2  
set startY $currY  
set text "Reply: " 
append text $reply 
append text "(" 
append text $replyPar 
append text ")" 
ADDNOTE $text red 
return [ALSO $obj $msg $cont $contPar $delta $reserve 0] 
}  

proc RETURN {obj msg reply replyPar delta reserve} {  
global currX currY messages nof_messages  
global startY class objects currObj smallFont xgrjd ygrid SKIP 1  
set t [ 
drawText $currX $currY $reply $smallFont red gray 2 1]  
.c bind $t <Any-ButtonPress> "BindMsgPress $class($obj) \"$reply\" \"$replyPar\" $currX $currY red"  
.c bind $t <Any-ButtonRelease> "BindRelease "  
set coord [ 
.c bbox $t] incr currY [expr [lindex $coord 3] - [lindex $coord 1]] incr currY 2  
set startY $currY  
set text "Return: " append text $reply append text "(" append text $replyPar append text ")" ADDNOTE $text red  
set toX [expr [expr $obj + 1] * $xgrid]  
set X [expr $messages($msg,0) + [expr $delta * $messages($msg,2) ] ]  
.c create line $currX $currY $X $currY -fill blue -arrow last -width 2  
set msg_index [addMessage $currX $X $currY $obj $currY] return $msg_index }  

proc ALSO {obj msg cont contPar delta reserve skip} {  
global currX currY messages nof_messages  
global startY class objects currObj smallFont xgrid ygrid 
SKIP $skip  
set toX [expr [expr $obj + 1] * $xgrid]  
set X [expr $messages($msg,0) + [expr $delta * $messages($msg,2) ] ]  
.c create line $currX $currY $X $currY -fill red -arrow last -width 2  
if {$currY <= $objects($obj)} {  
set toY $objects($obj) } else {  
set toY $currY }  
if {$delta != 0} { incr toY [expr $reserve * $ygrid] }  
.c create oval [expr $X - 2] [expr $messages($msg,5) - 2] \ [expr $X + 2] [expr $messages($msg,5) + 2] -fill purple  
.c create line $X $messages($msg,5) $X $toY -fill purple -width 2  
.c create line $toX $toY $X $toY -arrow first -fill purple -width 2  
set textY $toY  
set textX $X  
set t [ 
drawText $textX $textY $cont $smallFont purple purple 0 1]  
.c bind $t <Any-ButtonPress> "BindMsgPress $class($obj) \"$cont\" \"$contPar\" $textX $textY purple"  
.c bind $t <Any-ButtonRelease> "BindRelease "  
set msg_index [addMessage $currX $toX $toY $obj $currY]  
if {$objects($currObj) != 0} { incr objects($currObj) $ygrid }  
set note "Cont: " 
append note $cont 
append note "(" 
append note $contPar 
append note ")" 
ADDNOTE $note purple 
return $msg_index 
}  

proc COMMENT {text} {  
global currX currY smallFont commentCount  
set t [ 
drawText [expr $currX - 10] $currY $commentCount $smallFont gold gray 1 -1] SKIP 1  
set note "Note " append note $commentCount append note ": " append note $text ADDNOTE $note black incr commentCount }  

proc ERROR {text} {  
global lbEntryCount incr lbEntryCount .lb insert end $text } 
 
 Init
 SCALE  250 15
 TITLE "FS" "Opening a file" "Wed Oct  3 17:31:17 2018 "
      set o_p_client [OBJECT "p_client" "CLIENT"]  
     set o_fs [OBJECT "fs" "FS"]  
    IN $o_p_client "???" 
   set m_p_entry [SEND $o_fs "OpenFile" "fileId, cid"  0  0 ] 
    AT $m_p_entry "p_entry" 
   DO FileUsageCreation "fileID -> UsageID" 
   set m_p_exit1 [REPLY $o_p_client $m_p_entry "FileOpened" "UsageID" "p_continue" "<contParams>"  1  1 ]  
    AT $m_p_exit1 "p_exit1" 
   set m_p_entry [SEND $o_fs "CloseFile" "fileId, cid"  0  0 ] 
    AT $m_p_entry "p_entry" 
   DO FileUsageDeletion "fileID -> UsageID" 
   set m_p_exit2 [REPLY $o_p_client $m_p_entry "FileClosed" "UsageID" "p_continue" "<contParams>"  1  1 ]  
    AT $m_p_exit2 "p_exit2" 
  Exit 