# leakcount
This program tracks any memory leaks from another program it executes with.
A shim library is used to intercept any calls to the malloc and free functions.
When malloc is called, a new node will be added to a linked list containing the size of the allocated memory and a pointer pointing to the next node. After which the built-in malloc function is used normally.
When free is called, the associated node is removed from the list and the built-in free function is used.
When the tracked program exits, the tracker will print the remaining linked list indicating any memory leaks that exist in the tracked program.
