
.PHONY: all pkg clean

all:
	
	$(MAKE) -C spheres3D
	$(MAKE) -C sprites2D
	$(MAKE) -C fonts
	$(MAKE) -C surfaces

pkg:
	$(MAKE) pkg -C spheres3D
	$(MAKE) pkg -C sprites2D
	$(MAKE) pkg -C fonts
	$(MAKE) pkg -C surfaces

clean:
	$(MAKE) clean -C spheres3D
	$(MAKE) clean -C sprites2D
	$(MAKE) clean -C fonts
	$(MAKE) clean -C surfaces

