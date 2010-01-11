#ifndef __BASIC_INC__
#define __BASIC_INC__

#define  C      :play c
#define  D      :play d
#define  E      :play e
#define  F      :play f
#define  G      :play g
#define  A      :play a
#define  B      :play b

#define L1      :time d1
#define L2      :time d2
#define L4      :time d4
#define L8      :time d8
#define L16     :time d16
#define L32     :time d32
#define L64     :time d64

#define O0      :oct 0
#define O1      :oct 1
#define O2      :oct 2
#define O3      :oct 3
#define O4      :oct 4
#define O5      :oct 5
#define O6      :oct 6
#define O7      :oct 7


#define V0      :vol 0
#define V1      :vol 1
#define V2      :vol 2
#define V3      :vol 3
#define V4      :vol 4
#define V5      :vol 5
#define V6      :vol 6
#define V7      :vol 7

#define R       :rest?
#define I       :inst?

#define T    :tempo=

#define V(x) :vol x?
#define O(x) :oct x?
#define L(x) :times?Hz*60*4/tempo*1/x

#endif
