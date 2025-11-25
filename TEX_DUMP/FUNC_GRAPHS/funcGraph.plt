set terminal png size 800,600
set output 'TEX_DUMP/FUNC_GRAPHS/funcGraph3.png'
set xlabel 'x'
set ylabel 'y'
set grid
f(x) = sin(x)
tangent(x) = 0.841471 + (0.540302)*(x - 1)
plot f(x) with lines title "Исходная функция", \
	tangent(x) with lines title "Касательная в точке 1"
