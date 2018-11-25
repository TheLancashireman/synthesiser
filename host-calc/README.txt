This subdirectory contains some programs I wrote for Linux while playing at tone generation.

On a Linux system:
* cc wave.c -o wave -I .. -I ../../davros/davros-3
* something like "./wave 2 5 > square-5.csv" creates a CSV file that you can read with libreoffice or similar
	(Hint: create a barchart of columns 1 and 2 and you'll see the waveform)
