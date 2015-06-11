scaler2_reader_bit.exe :	scaler2_reader_bit_bd4.c scaler2_reader_bit_bd5.c
	gcc scaler2_reader_bit_bd4.c -o scaler2_reader_bit_bd4.exe
	gcc scaler2_reader_bit_bd5.c -o scaler2_reader_bit_bd5.exe

clean:	
	/bin/rm -f scaler2_reader_bit*.exe
