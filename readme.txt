SysPro - Assignment 3
Thanasis Filippidis - 1115201400215 - sdi1400215 
sdi1400215@di.uoa.gr

Makefile provided with seperate compilation

Everything is implemented except from 
1) the option to choose the available path at ContentServer
2) the statistics requested

Files:
> Makefile
> ContentServer.c
> MirrorServer.c
> MirrorInitiator.c

Implementation notes:

-> ContentServer
	for termination please use SIGSTOP (ctrl + C)
	after instruction of Mr. Delis that we can use it.

-> MirrorServer
	for the parallel process of the various requests I have used
	threading

-> Mutexes
	in order to avoid busy waiting I have used 2 condition variables
	1) non full, 2) non empty and one mutex where according to the protocol
	of many consumers/producers provided by the instructors, the threads
	are cooperating smoothly 
