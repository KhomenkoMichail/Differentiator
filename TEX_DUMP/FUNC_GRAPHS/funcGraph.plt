set terminal png size 800,600
set output 'TEX_DUMP/FUNC_GRAPHS/funcGraph3.png'
set xlabel 'x'
set ylabel 'y'
set grid
f(x) = sin(x)
tangent(x) = 0.14112 + (-0.989992)*(x - 3)
plot f(x) with lines title "Исходная функция", \
	tangent(x) with lines title "Касательная в точке 3"
