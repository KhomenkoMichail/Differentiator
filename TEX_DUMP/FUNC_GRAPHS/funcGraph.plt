set terminal png size 800,600
set output 'TEX_DUMP/FUNC_GRAPHS/funcGraph3.png'
set xlabel 'x'
set ylabel 'y'
set grid
f(x) = sin((1 / x)) * cos((2 * x + 1)) ** 3 - log((x ** 2)) / exp(x) + tan((3.14 * x))
tangent(x) = 0.0260297 + (2.43822)*(x - 3)
plot f(x) with lines title "Исходная функция", \
	tangent(x) with lines title "Касательная в точке 3"
