
.PHONY: all pkg clean

all:
	
	$(MAKE) -C spheres3D
	$(MAKE) -C sprites2D
	$(MAKE) -C fonts


pkg:
	$(MAKE) pkg -C spheres3D
	$(MAKE) pkg -C sprites2D
	$(MAKE) pkg -C fonts

clean:
	$(MAKE) clean -C spheres3D
	$(MAKE) clean -C sprites2D
	$(MAKE) clean -C fonts

