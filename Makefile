
.PHONY: all pkg clean

all:
	
	$(MAKE) install -C lib
	$(MAKE) -C samples

pkg:
	$(MAKE) install -C lib
	$(MAKE) pkg -C samples

clean:
	$(MAKE) clean -C lib
	$(MAKE) clean -C samples

