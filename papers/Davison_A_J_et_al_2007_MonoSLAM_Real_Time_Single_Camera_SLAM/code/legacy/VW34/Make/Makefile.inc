## define BUILDRULE for constructing composite include files 
## include this file wherever $(BUILDRULE) is required

BUILDRULE = ( echo "\#ifndef _`echo $@ |  sed y/./_/ | sed y/\\\//m/`_"; \
	    echo "\#define _`echo $@ |  sed y/./_/ | sed y/\\\//m/`_"; \
	    echo; \
	    for i in $^; do (echo "\#include \"$$i\""); done; \
	    echo; \
	    echo "\#endif //_`echo $@ |  sed y/./_/ | sed y/\\\//m/`_" ) > $@



