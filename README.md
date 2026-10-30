This is persistent key-value store project coded in C. It stores key value pairs in memory and on a binary file. Multiple stores can be created and managed. The binary log file consists of "put" and "delete" commands. When the store is opened every command is executed into the memory store. This recreates the store that was made during the last session. The log file is constantly updated to match the content of the store. The store is crash proof (deletes half written records at the end of the file) and corruption proof (CRC code used for detection of corruption, the program refuses to open the store). The store does not guarantee storing the information if power ran out.  

The record format (in bytes) inside the file is the following:

| (4) CRC | (1) OPERATION (put or delete) | (4) key length | (4) value length | (KL) key | (VL) value | 

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

For this project Claude made every design decision and presented them to me in the form of tasks. I completed the tasks manually typing.