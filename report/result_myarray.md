# Результаты тестирвоания

Компилятор MSVC 2019

Сборка Release

Платформа Windows x64, Intel(R) Core(TM) i7-7500U CPU, 2.70GHz, RAM 2133 MGz.

## SingleArray

Used cpp class ```class myarray::ExpandArray<class B,struct myarray::ExpandConstSize<1> >```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 1 M |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 1 M |  0 ms <br> 0.28 M |  0 ms <br> 1 M | 0.00 MB |
| 10'000 |  2 ms <br> 100.02 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  25 ms <br> 50.02 M |  2 ms <br> 100.02 M |  0 ms <br> 27.11 M |  2 ms <br> 100.02 M | 0.01 MB |
| 100'000 |  46 ms <br> 10'000.20 M |  3 ms <br> 0.20 M |  0 ms <br> 0.10 M |  2'882 ms <br> 5'000.25 M |  51 ms <br> 10'000.20 M |  5 ms <br> 2'744.23 M |  53 ms <br> 10'000.20 M | 0.10 MB |
| 1'000'000 |  485 ms <br> 1'000'002 M |  47 ms <br> 2 M |  38 ms <br> 1 M |  238'579 ms <br> 500'002.50 M |  288 ms <br> 1'000'002 M |  28 ms <br> 274'384.11 M |  338 ms <br> 1'000'002 M | 0.95 MB |
| 10'000'000 |  194'576 ms <br> 100'000'020 M |  213 ms <br> 20 M |  54 ms <br> 10 M |  skipped | skipped | skipped | skipped |9.54 MB |

Done

## VectorArray (100)

Used cpp class ```class myarray::ExpandArray<class B,struct myarray::ExpandConstSize<100> >```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 0.01 M |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.51 M |  0 ms <br> 0.28 M |  0 ms <br> 0.28 M | 0.00 MB |
| 10'000 |  0 ms <br> 1.02 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  22 ms <br> 50.02 M |  19 ms <br> 50.52 M |  0 ms <br> 27.41 M |  14 ms <br> 28.04 M | 0.01 MB |
| 100'000 |  1 ms <br> 100.20 M |  7 ms <br> 0.20 M |  0 ms <br> 0.10 M |  2'749 ms <br> 5'000.25 M |  2'531 ms <br> 5'050.20 M |  2 ms <br> 2'733.53 M |  1'177 ms <br> 2'814.18 M | 0.10 MB |
| 1'000'000 |  26 ms <br> 10'002 M |  22 ms <br> 2 M |  7 ms <br> 1 M |  227'433 ms <br> 500'002.50 M |  235'647 ms <br> 505'002 M |  28 ms <br> 274'069.32 M |  131'247 ms <br> 281'443.12 M | 0.95 MB |
| 10'000'000 |  2'392 ms <br> 1'000'020 M |  300 ms <br> 20 M |  61 ms <br> 10 M |  skipped | skipped | skipped | skipped |9.54 MB |

Done

## MatrixArray (100)

Used cpp class ```class myarray::MatrixArray<class B,100>```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 3'000 |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  10 ms <br> 0.50 M |  9 ms <br> 0.50 M |  8 ms <br> 0.28 M |  5 ms <br> 0.27 M | 0.00 MB |
| 10'000 |  0 ms <br> 0.03 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  1'464 ms <br> 50.02 M |  1'035 ms <br> 50.02 M |  798 ms <br> 27.68 M |  594 ms <br> 27.36 M | 0.01 MB |
| 100'000 |  3 ms <br> 0.30 M |  2 ms <br> 0.20 M |  1 ms <br> 0.10 M |  113'508 ms <br> 5'000.25 M |  115'423 ms <br> 5'000.25 M |  77'818 ms <br> 2'734.33 M |  56'078 ms <br> 2'744.28 M | 0.13 MB |
| 1'000'000  |  skipped             | skipped          | skipped           | skipped  |  skipped | skipped | skipped | skipped | 
| 10'000'000 |  8'555 ms <br> 30 M |  752 ms <br> 20 M |  123 ms <br> 10 M |  skipped | skipped | skipped | skipped |13.35 MB |

Done

## FactorArray (1.5)

Used cpp class ```class myarray::ExpandArray<class B,struct myarray::ExpandFactorSize<3,2> >```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 7'274 |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.50 M |  0 ms <br> 0.27 M |  0 ms <br> 0.28 M | 0.00 MB |
| 10'000 |  0 ms <br> 0.08 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  19 ms <br> 50.02 M |  19 ms <br> 50.05 M |  0 ms <br> 27.43 M |  11 ms <br> 27.43 M | 0.01 MB |
| 100'000 |  0 ms <br> 0.85 M |  2 ms <br> 0.20 M |  0 ms <br> 0.10 M |  1'934 ms <br> 5'000.25 M |  1'935 ms <br> 5'000.53 M |  2 ms <br> 2'741.39 M |  1'068 ms <br> 2'737.76 M | 0.13 MB |
| 1'000'000 |  7 ms <br> 7.20 M |  20 ms <br> 2 M |  5 ms <br> 1 M |  226'633 ms <br> 500'002.50 M |  212'001 ms <br> 500'004.60 M |  29 ms <br> 274'238.75 M |  126'530 ms <br> 274'319.31 M | 1.00 MB |
| 10'000'000 |  118 ms <br> 77.83 M |  379 ms <br> 20 M |  255 ms <br> 10 M |  skipped | skipped | skipped | skipped |11.40 MB |

Done

##  std::vector

Used cpp class ```class myarray::STDVectorArray<class B>```

| N | add(T) | get(rand) | removeLast | insert(T,0) pr | insert(T,0) new | remove(T, rand)  | insert(T, rand) new  | mem usage |
|---|:-------|:--------|:---------|:---------------|:--------------- |:---------------- |:-----------------|:------|
| 1'000 |  0 ms <br> 7'274 |  0 ms <br> 2'000 |  0 ms <br> 1'000 |  0 ms <br> 0.50 M |  0 ms <br> 0.51 M |  0 ms <br> 0.27 M |  0 ms <br> 0.29 M | 0.00 MB |
| 10'000 |  0 ms <br> 0.08 M |  0 ms <br> 0.02 M |  0 ms <br> 10'000 |  63 ms <br> 50.04 M |  51 ms <br> 50.07 M |  0 ms <br> 27.63 M |  15 ms <br> 27.38 M | 0.01 MB |
| 100'000 |  0 ms <br> 0.85 M |  2 ms <br> 0.20 M |  0 ms <br> 0.10 M |  2'773 ms <br> 5'000.45 M |  2'362 ms <br> 5'000.73 M |  2 ms <br> 2'743.22 M |  1'311 ms <br> 2'746.48 M | 0.13 MB |
| 1'000'000 |  1 ms <br> 7.20 M |  53 ms <br> 2 M |  0 ms <br> 1 M |  203'783 ms <br> 500'004.50 M |  196'183 ms <br> 500'006.60 M |  29 ms <br> 274'042.90 M |  117'166 ms <br> 274'374.32 M | 1.00 MB |
| 10'000'000 |  15 ms <br> 77.83 M |  171 ms <br> 20 M |  0 ms <br> 10 M |  skipped | skipped | skipped | skipped |11.40 MB |

Done
