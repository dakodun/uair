SUBDIRS=$(wildcard tst/*/)

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ clean
	
.PHONY: all $(SUBDIRS)
