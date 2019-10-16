reset
unset key
set view map
set pm3d map
set autoscale fix
set xrange[0:x]
set yrange[0:y]
set palette rgbformulae 22,13,-31
set term png
set output outfile
splot filename
