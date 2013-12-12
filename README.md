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

A note on using the parallel search:
	- realloc() will be executed in a parallel section of code, so the program
	  must be linked against a threadsafe version of the stdc library.
		- This is the default for windows.
		
Base compile-time flags:
    - -std=c11 -pedantic -pedantic-errors -Wall -Wextra -Werror -m64

Some references I'm using:
	- http://stackoverflow.com/questions/494721/what-are-some-good-resources-for-writing-a-chess-engine
	- http://www.gamedev.net/page/resources/_/technical/artificial-intelligence/chess-programming-part-i-getting-started-r1014
