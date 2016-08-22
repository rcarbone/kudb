# The list of directories to compile
SUBDIRS  = src

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
DRIVER   = src/runit/runit
PROGRAMS = $(shell ls -1 src/*/test)
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
