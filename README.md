This is persistent key-value storage project coded in C.
It allows storing key-value pairs, and later recalling them,
the persistent part is that closing the program doesn't lose the store.
When the program is opened it asks for the name of the store
you want to open. When you give it a name it opens that store 
and reconstructs it for faster lookup. If the name doesn't exists 
it makes a new store. The program stores the key-value pairs inside
of the log file. The log file consists of "put" and "delete" commands
When the store is opened every command is executed into the fast 
lookup no-persistence store which the program uses. This recreates 
the store that was made during the last session. The log file is
constantly updated to match the content of the store. 
The following commands are available inside of the store:
- put [key] [value] -  enter a key-value pair
- get [key]         -  get value paired with specified key 
- delete [key]      -  delete specified key-value pair
- pstore            - print store, prints all key-value pairs
                        sorted into buckets according to
                        their hash first, their time of addition
                        later (time of addition unusable and 
                        unimportant)
- optlog            - optimizes the log file, deleting redundant
                      lines (only put commands remain)
- close             - closes the store and the log file