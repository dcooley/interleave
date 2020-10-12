
## List Rows
err <- "interleave - expecting a list input"
expect_error( interleave:::.test_list_rows( 1:5 ), err)
expect_error( interleave:::.test_list_rows( matrix(1:4, ncol = 2) ), err )
expect_error( interleave:::.test_list_rows( data.frame(x = 1:3, y = 1:3 ) ), err )
expect_equal( interleave:::.test_list_rows( list( data.frame(x = 1:3, y = 1:3 ) ) ), list( 3 ) )

l <- list(
  1:10
  , matrix(1:16, ncol = 2)
  , list(
    list( data.frame(x = 1:10, y = 10:1 ) )
  )
)

expect_equal( interleave:::.test_list_rows( l ) , list(1,8,list(list(10))) )
expect_equal( unlist( interleave:::.test_list_rows( l ) ), c(1,8,10) )

## List Size
l <- list(
  1:10
  , matrix(1:16, ncol = 2)
  , list(
      1:4
      , matrix(1:10, ncol = 5)
    )
  , list(
    list( data.frame(x = 1:10, y = 10:1 ) )
  )
)

expect_equal( interleave:::.test_list_element_count( l ), list(10, 16, list(4, 10), list( list( 20 ) ) ) )

## Unlist list
expect_error( interleave:::.test_unlist_list( 1:4 ), err )
expect_error( interleave:::.test_unlist_list( matrix(1:4, ncol = 2) ), err)
expect_error( interleave:::.test_unlist_list( data.frame( x = 1:4 ) ), err )

l <- list(
  1:10
  , list(
    list(
      matrix(1:20, ncol = 2)
    )
  )
)

expect_equal( interleave:::.test_unlist_list( l ), c(1:10, 1:20)  )

l <- list(
  c(TRUE, TRUE, FALSE)
  , matrix(1:16, ncol = 2)
  , list(
    letters[1:4]
    , matrix(rep(T, 10), ncol = 5)
  )
  , list(
      list( data.frame(x = 1:10, y = 10:1 ) )
  )
)

list_sizes <- interleave:::.test_list_element_count( l)
expect_equal( interleave:::.test_unlist_list( list_sizes ),  unlist( list_sizes) )
expect_error( interleave:::.test_unlist_list( l ), err ) ## errors because there are df's and matrices in list


