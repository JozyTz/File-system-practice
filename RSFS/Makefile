CC = gcc 
LDLIBS = -lpthread -g -ggdb

objects = api.o application.o data_block.o dir.o inode.o open_file_table.o
App = app

all: $(App)

$(App): $(objects)
	$(CC) -o $(App) $(objects) $(LDLIBS)

$(objects): %.o: %.c 

clean:
	rm -f *.o app 
