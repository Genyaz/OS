all:
	cd ./lib && $(MAKE)
	cd ./cat && $(MAKE)

clean:
	cd ./lib && $(MAKE) clean
	cd ./cat && $(MAKE) clean