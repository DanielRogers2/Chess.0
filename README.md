Chess.0
=======

A chess playing AI hobby project. Originally for an AI class at Marist College.  

Dependencies:
	- curl64 - for connecting to a server
		- linked against
	- jansson - for parsing json responses from the server
		- built from source

Compile time defines specific to the project:
	- To enable parallel negamax 
		- -DPARALLEL_NEGAMAX
	- To attempt to use all available cores
		- -DUSE_MAX_THREADS
