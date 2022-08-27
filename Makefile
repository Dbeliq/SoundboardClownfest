build_main:
	g++ -c src/*.cpp -I include && g++ *.o -o main
build-run_main:
	g++ -c src/*.cpp -I include && g++ *.o -o main && main.exe
