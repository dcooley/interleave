
## List Rows

expect_error( interleave:::.test_list_rows( 1:5 ), "interleave - expecting a list input")
expect_error( interleave:::.test_list_rows( matrix(1:4, ncol = 2) ), "interleave - expecting a list input")
expect_error( interleave:::.test_list_rows( data.frame(x = 1:3, y = 1:3 ) ), "interleave - expecting a list input")
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

