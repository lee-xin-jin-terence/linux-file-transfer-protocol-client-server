
FTP.H := ./../Shared/ftp.h 
FTP.C :=./../Shared/ftp.c

MSC.H := ./../Shared/msg.h 
MSG.C := ./../Shared/msg.c

myftp: myftp.c ftp.o msg.o token.o
	gcc -Wall myftp.c ftp.o msg.o token.o -o myftp

ftp.o: $(FTP.H) $(FTP.C)
	gcc -c $(FTP.C)

msg.o: $(MSG.H) $(MSG.C)
	gcc -c $(MSG.C)

token.o: token.h token.c
	gcc -c token.c

clean:
	rm *.o
