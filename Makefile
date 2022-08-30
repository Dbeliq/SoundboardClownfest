build_main:
	g++ -c src/*.cpp -I include && g++ *.o -o main -l winmm
build-run_main:
	g++ -c src/*.cpp -I include && g++ *.o -o main -l winmm && main.exe
