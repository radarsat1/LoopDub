
PACKAGES = scons
MD5 = md5sum

all: loopdub

loopdub: $(PACKAGES)
	@echo
	@echo
	@echo "Dependancies are all compiled. LoopDub should now be able to compile and link. Type 'libdeps/scons.py' to proceed."
	@echo
	@echo


# Scons - local version
scons: scons.py
	@if (scons -v || ./scons.py -v); then echo "Scons verified."; else echo "Error running Scons."; false; fi

scons.py: scons.verified
	@echo "Unpacking Scons..."
	tar -xzf $(PKG_SCONS_TAR)
	chmod +x scons.py
	if ! [ -h ../scons ]; then ln -s "$(shell pwd)/scons.py" ../scons; fi

scons.verified:
	@if (scons -v); then echo "System Scons found, no need to bootstrap Linux"; false; fi
	@echo "Getting Scons..."
	curl -L $(PKG_SCONS_URL) -o $(PKG_SCONS_TAR)
	if [ `$(MD5) $(PKG_SCONS_TAR) | cut -d" " -f1`x = $(PKG_SCONS_MD5)x ]; then touch scons.verified; else echo "MD5 error on $(PKG_SCONS_TAR)"; false; fi

