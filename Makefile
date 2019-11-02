.PHONY: switchcrypt clean check

switchcrypt:
	$(MAKE) all -C build

clean:
	$(MAKE) clean -C build

check:
	echo "WARNING: Be sure to run 'make clean' if you try to build SwitchCrypt after this!"
	$(MAKE) pre -C build
	$(MAKE) check -C build -B
