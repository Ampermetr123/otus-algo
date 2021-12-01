# Результаты тестирования

Компилятор MSVC 2019

Сборка Release

Платформа Windows x64, Intel(R) Core(TM) i7-7500U CPU, 2.70GHz, RAM 2133 MGz.

Размер элемента: 1 байт.

## SingleArray

Used cpp class ```class myarray::ExpandArray<class myarray::B,struct myarray::ExpandConstSize<1> >```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 1 M |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 1 M |  0 ms <br> 0.27 M |  0 ms <br> 1 M | 0.00 MB |
| 10'000 |  3 ms <br> 100.02 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  29 ms <br> 50.02 M |  2 ms <br> 100.02 M |  0 ms <br> 27.52 M |  2 ms <br> 100.02 M | 0.01 MB |
| 100'000 |  64 ms <br> 10'000.20 M |  2 ms <br> 0.20 M |  1 ms <br> 0.10 M |  3'088 ms <br> 5'000.25 M |  33 ms <br> 10'000.20 M |  3 ms <br> 2'734.67 M |  36 ms <br> 10'000.20 M | 0.10 MB |
| 1'000'000 |  434 ms <br> 1'000'002 M |  25 ms <br> 2 M |  6 ms <br> 1 M |  311'451 ms <br> 500'002.50 M |  546 ms <br> 1'000'002 M |  44 ms <br> 273'906.26 M |  658 ms <br> 1'000'002 M | 0.95 MB |
| 10'000'000 |  194'357 ms <br> 100'000'020 M |  176 ms <br> 20 M |  60 ms <br> 10 M |  skipped | skipped | skipped | skipped |9.54 MB |

Done

## VectorArray (100)

Used cpp class ```class myarray::ExpandArray<class myarray::B,struct myarray::ExpandConstSize<100> >```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 0.01 M |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.51 M |  0 ms <br> 0.27 M |  0 ms <br> 0.29 M | 0.00 MB |
| 10'000 |  0 ms <br> 1.02 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  21 ms <br> 50.02 M |  21 ms <br> 50.52 M |  0 ms <br> 27.36 M |  11 ms <br> 28.28 M | 0.01 MB |
| 100'000 |  1 ms <br> 100.20 M |  1 ms <br> 0.20 M |  0 ms <br> 0.10 M |  2'035 ms <br> 5'000.25 M |  1'993 ms <br> 5'050.20 M |  2 ms <br> 2'738.37 M |  1'110 ms <br> 2'812.76 M | 0.10 MB |
| 1'000'000 |  21 ms <br> 10'002 M |  16 ms <br> 2 M |  4 ms <br> 1 M |  213'168 ms <br> 500'002.50 M |  204'344 ms <br> 505'002 M |  27 ms <br> 273'734.05 M |  111'063 ms <br> 281'364.80 M | 0.95 MB |
| 10'000'000 |  1'980 ms <br> 1'000'020 M |  173 ms <br> 20 M |  57 ms <br> 10 M |  skipped | skipped | skipped | skipped |9.54 MB |

Done

## MatrixArray (100)

Used cpp class ```class myarray::MatrixArray<class myarray::B,100>```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 3'000 |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.50 M |  0 ms <br> 0.28 M |  0 ms <br> 0.28 M | 0.00 MB |
| 10'000 |  0 ms <br> 0.03 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  0 ms <br> 50.02 M |  0 ms <br> 50.02 M |  0 ms <br> 27.49 M |  0 ms <br> 27.22 M | 0.01 MB |
| 100'000 |  2 ms <br> 0.30 M |  2 ms <br> 0.20 M |  0 ms <br> 0.10 M |  2 ms <br> 5'000.25 M |  7 ms <br> 5'000.25 M |  3 ms <br> 2'739.59 M |  5 ms <br> 2'742.02 M | 0.13 MB |
| 10'000'000 |  8'229 ms <br> 30 M |  612 ms <br> 20 M |  71 ms <br> 10 M |  skipped | skipped | skipped | skipped |13.35 MB |

Done

## FactorArray (1.5)

Used cpp class ```class myarray::ExpandArray<class myarray::B,struct myarray::ExpandFactorSize<3,2> >```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 7'274 |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.50 M |  0 ms <br> 0.27 M |  0 ms <br> 0.28 M | 0.00 MB |
| 10'000 |  0 ms <br> 0.08 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  19 ms <br> 50.02 M |  21 ms <br> 50.05 M |  0 ms <br> 27.73 M |  12 ms <br> 27.57 M | 0.01 MB |
| 100'000 |  0 ms <br> 0.85 M |  1 ms <br> 0.20 M |  1 ms <br> 0.10 M |  2'083 ms <br> 5'000.25 M |  2'037 ms <br> 5'000.53 M |  2 ms <br> 2'741.89 M |  1'098 ms <br> 2'751.67 M | 0.13 MB |
| 1'000'000 |  7 ms <br> 7.20 M |  16 ms <br> 2 M |  6 ms <br> 1 M |  200'373 ms <br> 500'002.50 M |  198'223 ms <br> 500'004.60 M |  27 ms <br> 274'188.52 M |  112'411 ms <br> 274'357.46 M | 1.00 MB |
| 10'000'000 |  65 ms <br> 77.83 M |  174 ms <br> 20 M |  57 ms <br> 10 M |  skipped | skipped | skipped | skipped |11.40 MB |

Done

##  std::vector

Used cpp class ```class myarray::STDVectorArray<class myarray::B>```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 7'274 |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.51 M |  0 ms <br> 0.26 M |  0 ms <br> 0.29 M | 0.00 MB |
| 10'000 |  0 ms <br> 0.08 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  19 ms <br> 50.04 M |  19 ms <br> 50.07 M |  0 ms <br> 27.29 M |  11 ms <br> 27.59 M | 0.01 MB |
| 100'000 |  0 ms <br> 0.85 M |  1 ms <br> 0.20 M |  0 ms <br> 0.10 M |  1'975 ms <br> 5'000.45 M |  1'966 ms <br> 5'000.73 M |  2 ms <br> 2'740.62 M |  1'072 ms <br> 2'744.77 M | 0.13 MB |
| 1'000'000 |  1 ms <br> 7.20 M |  16 ms <br> 2 M |  0 ms <br> 1 M |  205'357 ms <br> 500'004.50 M |  198'421 ms <br> 500'006.60 M |  27 ms <br> 274'159.97 M |  109'469 ms <br> 273'960.84 M | 1.00 MB |
| 10'000'000 |  16 ms <br> 77.83 M |  172 ms <br> 20 M |  0 ms <br> 10 M |  skipped | skipped | skipped | skipped |11.40 MB |

Done
