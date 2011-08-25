DIRS=$(shell find . -type d -d 1)

all:
	@for d in $(DIRS); do \
		echo "        Building $$d payload..."; \
		make -C $$d; \
	done
	
clean:
	@for d in $(DIRS); do \
		make -C $$d clean; \
	done
