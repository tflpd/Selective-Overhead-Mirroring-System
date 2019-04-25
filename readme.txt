A Selective Overhead Mirroring System that is making use a set of client-apps MirrorInitiator to communicate with the help of a 
MirrorServer with one or more ContentServers in order to acquire one ore more files from those. There have been used threads for multiple 
parallel connections through sockets and mutexes in order to keep everything concurrent and TCP protocol for the communication. For an
explanatory figure see Project3-S17-K24.pdf. This is pretty much actually an implementation of a p2p app in C.

SysPro - Assignment 3
Thanasis Filippidis - 1115201400215 - sdi1400215 
sdi1400215@di.uoa.gr

Makefile provided with seperate compilation

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
