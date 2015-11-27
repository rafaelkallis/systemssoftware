The crucial part of the program is defined in the <memory> class. In there we have the alloc and release functions which use mutexes.
During the creation of each thread, a pointer to the shared memory is being passed to each thread.
When the user inputs  <e> from standard input, a global variable (which is read-only for the child "threads") gets set to false and all child threads exit their execution loop as soon as they wake up.
