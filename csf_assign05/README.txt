Sample README.txt

Eventually your report about how you implemented thread synchronization
in the server should go here

We implemented thread synchronization by using mutexes and semaphores 
when entering sections of the code where threads accessed shared data 
structures (critical sections). A good example of this synchronization 
is our implementation of enqueue in message_queue.cpp, where when attempting
to enqueue a message, the program will semwait until there is a slot 
available to enqueue a message in. Once a slot is available, the program 
than uses a mutex to make sure that while modifying the shared m_messages 
data structure, no other thread attempts to modify it, as this could lead 
to errors resulting from a thread seeing the data structure in an incorrect state.

An example of using only a mutex for a critical section can be found in our
implementation of adding a user to a room in room.cpp. This needs to be a
critical section, since otherwise a thread may attempt to add a user at the same time
as another thread, leading to data races and potentially compromising the data
structures integrity.

Also, mutexes are implemented using guard objects in our code. This is to help prevent
deadlocks, because instead of manually unlocking a mutex after leaving a critical section,
the guard object destructor does it for you.