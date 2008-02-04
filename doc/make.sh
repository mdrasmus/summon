#!/bin/sh

#mkdir -p html
#epydoc -v --html ../lib/summon

PYDOC=pydoc2.4
 
$PYDOC -w summon
$PYDOC -w summon.matrix
$PYDOC -w summon.sumtree
$PYDOC -w summon.util
$PYDOC -w summon.options
$PYDOC -w summon.core
$PYDOC -w summon.treelib
$PYDOC -w summon.simple
$PYDOC -w summon.plot
$PYDOC -w summon.svg
$PYDOC -w summon.colors
$PYDOC -w summon.multiwindow
$PYDOC -w summon.shapes
$PYDOC -w summon.timer
$PYDOC -w summon.vector
