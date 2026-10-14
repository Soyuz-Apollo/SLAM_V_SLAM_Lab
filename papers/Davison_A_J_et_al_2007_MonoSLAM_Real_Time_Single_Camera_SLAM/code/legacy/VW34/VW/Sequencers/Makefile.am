## Sequencers
##
## include this file in parent directory's Makefile.am

Sequencersdir = $(includedir)/VW/Sequencers

sequencers.h: $(Sequencers_HEADERS)
	$(BUILDRULE)

Sequencers_HEADERS = 	Sequencers/thread.h \
			Sequencers/sequencerbase.h \
			Sequencers/sequencerfile.h \
			Sequencers/sequencermovie.h

libVW_a_SOURCES += 	Sequencers/thread.cpp \
			Sequencers/sequencerbase.cpp \
			Sequencers/sequencerfile.cpp \
			Sequencers/sequencermovie.cpp



