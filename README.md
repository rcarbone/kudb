# kudb - Benchmark of Dictionary Structures by Heng Li

# Results for the impatients

Data sorted by CPU Time (unsigned keys, int values)

Linux nuc 3.2.0-4-amd64 #1 SMP Debian 3.2.81-1 x86_64 GNU/Linux

| Rank | Implementation    | CPU (secs) | Memory (kB) |    #    | Notes     |
| ---- | ----------------- | ---------- | ----------- | ------- | --------- |
|    1 | rdestl            |      0.360 |     265.432 |  625792 |           |
|    2 | khash             |      0.530 |     228.424 |  625792 |           |
|    3 | google_dense      |      0.640 |     249.068 |  625792 |           |
|    4 | tr1_unordered_map |      1.020 |     244.628 |  625792 |           |
|    5 | stb_hash          |      1.170 |     232.076 |  625792 |           |
|    6 | sgi_hash_map      |      1.220 |     241.744 |  625792 |           |
|    7 | google_sparse     |      1.300 |     225.140 |  625792 |           |
|    8 | htable            |      1.390 |     297.124 |  625792 |           |
|    9 | qt_qhash          |      1.390 |     245.592 |  625792 | **        |
|   10 | glib_hash         |      1.540 |     256.348 |  625792 | **        |
|   11 | uthash            |      1.570 |     273.184 |  625792 |           |
|   12 | boost_hash        |      1.820 |     241.752 |  625792 | **        |
|   13 | stx_btree         |      2.130 |     225.128 |  625792 |           |
|   14 | kbtree            |      2.490 |     224.228 |  625792 |           |
|   15 | sgi_map           |      4.040 |     245.456 |  625792 |           |
|   16 | JE_rb_old         |      4.260 |     244.820 |  625792 |           |
|   17 | NP_rbtree         |      4.280 |     244.820 |  625792 |           |
|   18 | libavl_rb_cpp     |      4.600 |     245.456 |  625792 |           |
|   19 | TN_rbtree         |      4.700 |     245.456 |  625792 |           |
|   20 | libavl_rb_cpp2    |      4.760 |     235.688 |  625792 |           |
|   21 | libavl_avl_cpp    |      4.960 |     245.456 |  625792 |           |
|   22 | JE_rb_new         |      5.480 |     234.784 |  625792 |           |
|   23 | sglib_rbtree      |      5.500 |     244.820 |  625792 |           |
|   24 | libavl_avl        |      5.920 |     264.356 |  625792 |           |
|   25 | libavl_prb        |      5.960 |     264.356 |  625792 |           |
|   26 | libavl_rb         |      6.110 |     264.356 |  625792 |           |
|   27 | JG_btree          |      6.370 |     256.172 |  625792 |           |
|   28 | JE_trp_hash       |      6.580 |     234.784 |  625792 |           |
|   29 | JE_trp_prng       |      6.890 |     244.556 |  625792 |           |
|   30 | NP_splaytree      |      6.970 |     234.788 |  625792 |           |
|   31 | gdsl_rb           |      7.290 |     264.356 |  625792 | **        |
|   32 | glib_tree         |      7.470 |     270.052 |  625792 | **        |
|   33 | libavl_bst        |      7.910 |     254.588 |  625792 |           |
|   34 | qt_qmap           |      8.420 |     237.624 |  625792 | **        |

 ** Pre-installation of System Libraries is required
