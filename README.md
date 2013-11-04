Chess.0
=======
A chess project for CMSC-404-AI

Build options:
  - Compile time defines:
    - -DHASHKEY=uint16_t
  - Initial compile time flags:
    - -std=c11 -pedantic -pedantic-errors -Wall -Wextra
  - Flags after base functionality is complete:
    - -std=c11 -pedantic -pedantic-errors -Wall -Wextra -Werror
      
References:
  - http://stackoverflow.com/questions/494721/what-are-some-good-resources-for-writing-a-chess-engine
  - http://www.gamedev.net/page/resources/_/technical/artificial-intelligence/chess-programming-part-i-getting-started-r1014
  
Identifying owners:
  - When adopting a function to work on, push a change to the repository that
    adds an @owner field to the function documentation prior to starting. This
    will indicate that someone is already working on that functionality and
    keep us from wasting time doing duplicate work.

Philosophy on #includes
  - Separate into two groups
    - System <includes>
    - Project "includes"
  - A file's header file should contain all the #include statements needed to
    compile that file.
    
Philosophy on global variables
  - Global variables should only be used if absolutely necessary
  - They should in general only contain data that is either constant or 
    initialized once during program runtime.
  - Global variables should only be declared in header files
  - Header files that declare global variables should have an @modifiers field
    that indicate what files in the project will modify the global variables
  - Before each global variable, there should be a block comment specifying
    several things:
    - What it is for
    - How it is used
    - An @users field indicating files that use it
    - An @modifiers field indicating files that modify it
    - An @initializer field indicating the file/function that initializes it
      for each individual function that does so
      - If there is more than one initializer function, this documentation
        should also indicate when a particular initializer is to be used
  - Header files that define functions that reference or edit a global
    variable should include an @uses or an @modifies field for those globals
    - These fields should indicate the name of the global and the header file
      in which it is declared.
    - @uses indicates that the global is referenced, but not modified
    - @modifies indicates that the global is both referenced and modified
    - Each global must be declared on a separate line with the appropriate tag
  - Functions that use global variables should also have the appropriate @uses
    or @modifies field. 
    - These declarations can be combined onto a single line if desired
    
Documentation Philosophy
  - All header files need documentation for each function with the following
    - A description of what the function does
    - A description of the function's side effects, if applicable
    - An @param field for each parameter, with a description of the purpose of
      the parameter
      - If the function has a side-effect on a parameter that will have scope
        external to the function, this should be documented as well
    - An @return field for non-void functions indicating what is returned, and
      any information that a caller might need to know in order to use the
      returned data.
  - .c files can duplicate the documentation if desired
    