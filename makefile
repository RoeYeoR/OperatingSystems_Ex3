# Define the subdirectories
SUBDIRS := prog1 prog2 prog3 prog4 prog5 prog6 prog7 prog8 prog9 prog10

# Define the phony targets
.PHONY: all clean $(SUBDIRS)

# Default target
all: $(SUBDIRS)

# Target to build all subdirectories
$(SUBDIRS):
	$(MAKE) -C $@

# Clean all subdirectories
clean:
	for dir in $(SUBDIRS); do \
    	$(MAKE) -C $$dir clean; \
	done