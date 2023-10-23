buildWin:
	make -C ./trash buildWin
	make -C ./gui buildWin

buildLinux:
	make -C ./trash buildLinux
	make -C ./gui buildLinux

#would like a windows and linux bash way to allow it to run trash on all in the ASMCODE folder giving them appropriate names in the systemCode folder
buildTest:
	./bin/trash ./ASMCODE/*.asm ./bin/systemCode/test.trash

clean:
	rm ./bin/systemCode/*.trash
	rm ./ASMCODE/*.asm