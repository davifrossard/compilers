#!/bin/bash

DATA=/home/zambon/Teaching/2016-2/CC/laboratorio/tests/
IN=$DATA/in
OUT=$DATA/out11

EXE=./runner

for infile in `ls $IN/*.tny`; do
    base=$(basename $infile)
    dotfile=$OUT/${base/.tny/.dot}
    pdffile=$OUT/${base/.tny/.pdf}
    $EXE < $infile > $dotfile
    dot -Tpdf $dotfile -o $pdffile
done
