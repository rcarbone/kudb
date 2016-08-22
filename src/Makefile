# The list of directories to compile
SUBDIRS  = benchmark   # library
SUBDIRS += runit       # test driver

# C Macro implementations under test (header files included)
SUBDIRS += khash
SUBDIRS += kbtree
SUBDIRS += htable
SUBDIRS += stb_hash
SUBDIRS += uthash
SUBDIRS += sglib_rbtree
SUBDIRS += NP_rbtree
SUBDIRS += NP_splaytree
SUBDIRS += JG_btree
SUBDIRS += JE_rb_new
SUBDIRS += JE_rb_old
SUBDIRS += JE_trp_hash
SUBDIRS += JE_trp_prng
SUBDIRS += TN_rbtree

# C implementations under test (library files included)
SUBDIRS += libavl_avl
SUBDIRS += libavl_rb
SUBDIRS += libavl_prb
SUBDIRS += libavl_bst

# C implementations under test (require packages installed)
SUBDIRS += _glib_hash
SUBDIRS += _glib_tree
# SUBDIRS += _gdsl_rb  # only in my development environment

# C++ implementations under test (library files included)
SUBDIRS += rdestl
SUBDIRS += tr1_unordered_map
SUBDIRS += sgi_map
SUBDIRS += sgi_hash_map
SUBDIRS += stx_btree
SUBDIRS += google_dense
SUBDIRS += google_sparse
SUBDIRS += libavl_avl_cpp
SUBDIRS += libavl_rb_cpp
SUBDIRS += libavl_rb_cpp2

# C++ implementations under test (require packages installed)
SUBDIRS += _boost_hash
SUBDIRS += _qt_qhash
SUBDIRS += _qt_qmap

# failed
# SUBDIRS += GM_avl
# SUBDIRS += stlavlmap
# SUBDIRS += WK_avl

# The main target is responsible to compile all
all:
	@for dir in ${SUBDIRS} ; do \
           if [ -d $$dir ] ; then \
             echo "Making sub-directory $$dir ..." ; \
             (cd $$dir && make -s) ; \
           fi \
         done

# Cleanup rules
clean distclean:
	@rm -f *~
	@for dir in ${SUBDIRS} ; do \
           if [ -d $$dir ] ; then \
             (cd $$dir && make -s clean) ; \
           fi \
         done

# Run tests
DRIVER   = runit/runit
PROGRAMS = $(shell ls -1 */test)
TYPES    = i s
run:
	@for t in ${TYPES}; do \
           for p in ${PROGRAMS} ; do \
             if [ -x $$p ] ; then \
               echo "Running test $$p (-$$t). Please wait ..." ; \
               ${DRIVER} $$p -$$t ; \
             fi \
           done \
         done
