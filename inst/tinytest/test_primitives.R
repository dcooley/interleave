

expect_error( interleave:::.test_interleave_primitive( list(), 1 ), "interleave - empty list" )
expect_error( interleave:::.test_interleave_primitive( 1:4, 2), "interleave - expecting a list" )

expect_error( interleave:::.test_interleave_primitive( list(1:4), 3 ), "interleave - unknown primitive type" )


expect_error( interleave:::.test_subset_vector( 1:4, -1 ), "index error" )
expect_error( interleave:::.test_subset_vector( 1:4, 4 ), "index error" )

expect_equal( interleave:::.test_subset_vector( 1:4, 2 ), 3 )
expect_equal( interleave:::.test_subset_vector( c(T,T,F), 0 ), 1 ) ## logial is coerced to integer
expect_equal( interleave:::.test_subset_vector( c(T,T,F), 2 ), 0 ) ## logial is coerced to integer

expect_equal( interleave:::.test_subset_vector( c(1.2, 1.3, 1.4), 0 ), 1.2 )
expect_error( interleave:::.test_subset_vector( complex(1), 0 ), "interleave - unknown vector type")
