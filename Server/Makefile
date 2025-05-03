
FTP.H := ./../Shared/ftp.h 
FTP.C :=./../Shared/ftp.c

MSC.H := ./../Shared/msg.h 
MSG.C := ./../Shared/msg.c

myftpd: ftpd.c ftp.o msg.o
	gcc -Wall ftpd.c ftp.o msg.o -o ftpd

ftp.o: $(FTP.H) $(FTP.C)
	gcc -c $(FTP.C)

msg.o: $(MSG.H) $(MSG.C)
	gcc -c $(MSG.C)

clean:
	rm *.o
