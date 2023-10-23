buildWin:
	make -C ./asmblr buildWin
	make -C ./gui buildWin

buildLinux:
	make -C ./asmblr buildLinux
	make -C ./gui buildLinux

buildTest:
	./bin/trash ./ASMCODE/*.asm ./bin/systemCode/test.trash

clean:
	rm ./bin/systemCode/*.*trash
	rm ./ASMCODE/*.asm