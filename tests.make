SUBDIRS=$(wildcard tst/*/)

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@
	
.PHONY: all $(SUBDIRS)
