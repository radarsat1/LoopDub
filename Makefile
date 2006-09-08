
.PHONY: loopdub clean debug
loopdub:
	scons

debug:
	scons debug=1

clean:
	scons -c
