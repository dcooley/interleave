
[![R build
status](https://github.com/dcooley/interleave/workflows/R-CMD-check/badge.svg)](https://github.com/dcooley/interleave/actions)

<!-- README.md is generated from README.Rmd. Please edit that file -->

# interleave

I’m building this library to be the workhorse for converting matrices
(and lists of matrices) into single, interleaved vectors, ready for
WebGL applications.

``` r

( mat1 <- matrix(1:20, ncol = 2, byrow = T) )
#       [,1] [,2]
#  [1,]    1    2
#  [2,]    3    4
#  [3,]    5    6
#  [4,]    7    8
#  [5,]    9   10
#  [6,]   11   12
#  [7,]   13   14
#  [8,]   15   16
#  [9,]   17   18
# [10,]   19   20

interleave( mat1 )
#  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20

( mat2 <- matrix(20:1, ncol = 5, byrow = T) )
#      [,1] [,2] [,3] [,4] [,5]
# [1,]   20   19   18   17   16
# [2,]   15   14   13   12   11
# [3,]   10    9    8    7    6
# [4,]    5    4    3    2    1

interleave( mat2 )
#  [1] 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1

lst <- list( mat1, mat2 )

interleave( lst )
#  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 20 19 18 17 16
# [26] 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1
```

## Ear-cutting / geometries / polygon structures

Isn’t this what `decido` is for?

Yes and no. I’m using this repo to experiment. Dunno if it’s going to
stay here or not. I’ve deviated away from `mapbox::earcut` slightly by
returning interleaved coordinates directly (rather than indicies), and I
accept any dimension (XY(Z(M(…n))), and so I can’t make it nicely fit in
`decido` (yet).

``` r
x <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
y <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
mat <- cbind(x, y)
lst <- list( mat )
interleave:::rcpp_earcut( lst )
# $coordinates
#  [1] 0.00 1.00 0.00 0.00 0.69 0.00 0.69 0.00 0.80 0.60 0.50 0.70 0.50 0.70 1.00
# [16] 0.80 0.75 1.00 0.00 1.00 0.69 0.00 0.50 0.70 0.50 0.70 0.75 1.00 0.00 1.00
# 
# $start_indices
# [1]  0  6 12 18 24
# 
# $n_coordinates
# [1] 3 3 3 3 3
# 
# $total_coordinates
# [1] 15
# 
# $stride
# [1] 2
```

With a hole

``` r

x1 <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
x2 <- c(0.2, 0.5, 0.5, 0.3, 0.2)
y1 <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
y2 <- c(0.2, 0.2, 0.4, 0.6, 0.4)

mat1 <- cbind(x1, y1)
mat2 <- cbind(x2, y2)

lst <- list( mat1, mat2 )

interleave:::rcpp_earcut( lst )
# $coordinates
#  [1] 0.00 0.00 0.20 0.20 0.20 0.40 0.50 0.20 0.20 0.20 0.00 0.00 0.69 0.00 0.80
# [16] 0.60 0.50 0.70 0.50 0.70 1.00 0.80 0.75 1.00 0.00 1.00 0.00 0.00 0.20 0.40
# [31] 0.50 0.20 0.00 0.00 0.69 0.00 0.50 0.70 0.75 1.00 0.00 1.00 0.00 1.00 0.20
# [46] 0.40 0.30 0.60 0.50 0.40 0.50 0.20 0.69 0.00 0.50 0.70 0.00 1.00 0.30 0.60
# [61] 0.50 0.40 0.69 0.00 0.50 0.70 0.50 0.70 0.30 0.60 0.50 0.40
# 
# $start_indices
#  [1]  0  6 12 18 24 30 36 42 48 54 60 66
# 
# $n_coordinates
#  [1] 3 3 3 3 3 3 3 3 3 3 3 3
# 
# $total_coordinates
# [1] 36
# 
# $stride
# [1] 2
```

**But Dave, why not just transpose a matrix and remove the dimension?**

Yeah, of course you can do that if you want. But my goal is to make
these functions callable through C++ directly, by other packages. And it
benchmarks pretty well.

``` r

mat <- matrix(rnorm(5e6), ncol = 5, byrow = T)

microbenchmark::microbenchmark(
  leave = { interleave( mat ) },
  baset = {
    mat2 <- t( mat )
    dim( mat2 ) <- NULL
    },
  times = 25
)
# Unit: milliseconds
#   expr       min       lq     mean   median       uq      max neval
#  leave 10.693548 12.40403 25.04083 15.97341 48.25999 63.53208    25
#  baset  9.804281 10.73121 17.59349 14.18340 15.50976 50.16852    25



mat <- matrix(1e6, ncol = 2)
lst <- list( mat, mat, mat, mat, mat )

microbenchmark::microbenchmark(
  leave = { interleave( lst ) },
  baset = {
    unlist(
      lapply(lst, function(x) {
        y <- t(x)
        dim( y ) <- NULL
        y
      })
    )
  },
  times = 25
)
# Unit: microseconds
#   expr    min     lq      mean median     uq       max neval
#  leave  4.515  5.026   6.45844  5.265  6.359    18.539    25
#  baset 14.173 15.625 651.73228 16.323 17.611 15892.314    25
```

## Interleaved object

  - **coordinates** - the interleaved coordinates
  - **stride** - the number of values in each coordinate.
  - **start\_indices** - the index of **coordinates** which denotes the
    start of each geometry (using 0-based indexing)
  - **n\_coordinates** - the number of coordinates in each geometry
  - **total\_coordinates** - the total number of coordinates in
    **coordinates**
