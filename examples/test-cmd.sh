FN=test-points-5.txt
OFN=./test-output.txt
A=1
B=0.4
D=0.4

./cmd_line -c 'ID %i POS (%x,%y,%z) VERTS %w %p EDGES %g FACES %s %t' \
    -wp $A $B 0 $D \
    -wp $A -$B 0 $D \
    -wp -$A -$B 0 $D \
    -wp -$A $B 0 $D \
    -wp 0 $B $A $D \
    -wp 0 -$B $A $D \
    -wp 0 -$B -$A $D \
    -wp 0 $B -$A $D \
    -1 1 -1 1 -1 1 $FN

mv $FN.vol $OFN
