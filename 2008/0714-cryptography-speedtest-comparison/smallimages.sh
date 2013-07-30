#!/bin/sh -x
 
for f in crypto-speedtest-0.1/results/*.png crypto-speedtest-0.1/results-flags/*.png; do
   convert -resize 400x -quality 90 $f 400/`basename ${f%%.png}.jpg`
   convert -resize 500x -quality 90 $f 500/`basename ${f%%.png}.jpg`
   convert -resize 800x -quality 90 $f 800/`basename ${f%%.png}.jpg`
done
