
PACKAGES =
MD5 = md5sum

all: loopdub

loopdub: $(PACKAGES)
	@echo
	@echo
	@echo "Dependancies are all compiled. LoopDub should now be able to compile and link. Type './configure' to proceed."
	@echo
	@echo
