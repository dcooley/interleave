
[![R build
status](https://github.com/dcooley/interleave/workflows/R-CMD-check/badge.svg)](https://github.com/dcooley/interleave/actions)

<!-- README.md is generated from README.Rmd. Please edit that file -->

# interleave

I’m building this library to be the workhorse for converting matrices
(and lists of matrices) into single, interleaved vectors, ready for
WebGL applications.

``` r

( mat1 <- matrix(1:20, ncol = 2, byrow = T) )
>       [,1] [,2]
>  [1,]    1    2
>  [2,]    3    4
>  [3,]    5    6
>  [4,]    7    8
>  [5,]    9   10
>  [6,]   11   12
>  [7,]   13   14
>  [8,]   15   16
>  [9,]   17   18
> [10,]   19   20

interleave:::rcpp_interleave( mat1 )
>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20

( mat2 <- matrix(20:1, ncol = 5, byrow = T) )
>      [,1] [,2] [,3] [,4] [,5]
> [1,]   20   19   18   17   16
> [2,]   15   14   13   12   11
> [3,]   10    9    8    7    6
> [4,]    5    4    3    2    1

interleave:::rcpp_interleave( mat1 )
>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20

lst <- list( mat1, mat2 )

interleave:::rcpp_interleave( lst )
>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 20 19 18 17 16
> [26] 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1
```

## Ear-cutting / geometries / polygon structures

dunno if this is going to stay here or not. I’ve deviated away from
`mapbox::earcut` slightly by returning coordinates directly, and
accepting any dimension (XY(Z(M(…n))), and so I can’t make it nicely fit
in `decido`

``` r

x <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
y <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
mat <- cbind(x, y)
lst <- list( mat )
interleave:::rcpp_earcut_sfg( lst )
>  [1] 0.00 1.00 0.00 0.00 0.69 0.00 0.69 0.00 0.80 0.60 0.50 0.70 0.50 0.70 1.00
> [16] 0.80 0.75 1.00 0.00 1.00 0.69 0.00 0.50 0.70 0.50 0.70 0.75 1.00 0.00 1.00
```

## Don’t run this yet, it will crash your system.

``` r

# x1 <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
# x2 <- c(0.2, 0.5, 0.5, 0.3, 0.2)
# y1 <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
# y2 <- c(0.2, 0.2, 0.4, 0.6, 0.4)
# 
# mat1 <- cbind(x1, y1)
# mat2 <- cbind(x2, y2)
# 
# lst <- list( mat1, mat2 )
# 
# interleave:::rcpp_earcut_sfg()
```

``` r

mat <- matrix(rnorm(5e6), ncol = 5, byrow = T)

microbenchmark::microbenchmark(
  leave = { interleave:::rcpp_interleave( mat ) },
  asvec = { as.vector( t( mat ) ) },
  baset = { 
    mat2 <- t( mat )
    dim( mat2 ) <- NULL
    },
  times = 25
)
> Unit: milliseconds
>   expr       min       lq     mean   median       uq      max neval
>  leave 11.010127 11.54357 17.92472 15.68550 19.71998 50.59579    25
>  asvec 16.916456 20.57808 32.65317 24.06367 54.45727 67.18989    25
>  baset  9.592249 10.32024 16.85107 13.93715 16.58001 60.99526    25
```
