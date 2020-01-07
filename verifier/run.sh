#!/bin/sh
g++ ../ReplacementPolicy/ReplacementPolicy/main.cpp -o project
declare -i i = 0
cmd = "./project $org $lst index_$i.rpt"
for org in t1/config/*.org; do
     for lst in t1/bench/*.lst; do
          echo "Org is $org"
          echo "Lst is $lst"
          eval "$cmd"
#          ./verify $org $lst index$i.rpt
     done
done
for org in t2/config/*.org; do
     for lst in t2/bench/*.lst; do
          echo "Org is $org"
          echo "Lst is $lst"
          ./project $org $lst index_($i).rpt
          i = $((i+1))
#          ./verify $org $lst index.rpt
     done
done
