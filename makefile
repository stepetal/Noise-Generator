PROG_NAME = noise_gen_gui

main_compile:
	g++ -g -c main.cpp -lX11
alsa_compile:
	g++ -g -c alsa_classes.cpp -lasound -lX11 -lrt -lm -pthread -lfftw -lrfftw
$(PROG_NAME)_build: main_compile 
	g++ -g -o $(PROG_NAME) x11_classes.cpp main.o alsa_classes.o -lX11 -lasound -lrt -lm -pthread -lfftw -lrfftw 2>err.txt
$(PROG_NAME)_clear:
	rm $(PROG_NAME)

	
