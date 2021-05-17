server = server
client = client

main_obj = main.o
server_obj = server.o
server_main_obj = server_main.o
client_main_obj = client_main.o
client_obj = client.o
user_obj = user.o
repos_obj = repos.o
table_obj = table.o
fabric_obj = fabric.o
entry_obj = entry.o
file_obj = file.o
exception_obj = exception.o
scanner_obj = scanner.o
parser_obj = parser.o
inter_obj = inter.o

main_cpp = src/main.cpp
server_cpp = src/net/server/server.cpp
client_cpp = src/net/client/client.cpp
server_main_cpp = src/server.cpp
client_main_cpp = src/client.cpp
user_cpp = src/user/user.cpp
repos_cpp = src/db/repos.cpp
table_cpp = src/db/table.cpp
fabric_cpp = src/db/bdFabric.cpp
entry_cpp = src/db/entry.cpp
file_cpp = src/db/fileInOut.cpp
exception_cpp = src/exceptions.cpp
scanner_cpp = src/interpreter/scanner.cpp
parser_cpp = src/interpreter/parser.cpp
inter_cpp = src/interpreter/dbInterpreter.cpp

CFLAGS = -O2 -c -g -ftrapv  -Wall -Werror -Wformat-security -Wignored-qualifiers\
		 -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body\
		 -Wlogical-op -Wmissing-field-initializers -Wcast-qual -Wwrite-strings
COMPILER = g++

build_server: $(server)
$(server): $(server_main_obj)   $(exception_obj) $(repos_obj) $(table_obj) $(fabric_obj) $(entry_obj) $(file_obj) $(server_obj) $(client_obj) $(user_obj) $(scanner_obj) $(parser_obj) $(inter_obj)
	$(COMPILER)  -g $^ -o $@ -fsanitize=undefined -lm

build_client: $(client)
$(client): $(client_main_obj)  $(exception_obj) $(repos_obj) $(table_obj) $(fabric_obj) $(entry_obj) $(file_obj) $(server_obj) $(client_obj) $(user_obj) $(scanner_obj) $(parser_obj) $(inter_obj)
	$(COMPILER)  -g $^ -o $@ -fsanitize=undefined -lm

$(server_obj): $(server_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(main_obj): $(main_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(client_obj): $(client_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(repos_obj): $(repos_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(table_obj): $(table_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(fabric_obj): $(fabric_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(entry_obj): $(entry_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(file_obj): $(file_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@


$(net_obj): $(net_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@


$(exception_obj): $(exception_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(user_obj): $(user_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(server_main_obj): $(server_main_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(client_main_obj): $(client_main_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(scanner_obj): $(scanner_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(parser_obj): $(parser_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

$(inter_obj): $(inter_cpp)
	$(COMPILER) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o $(program_name)
	rm server
	rm client

clean_server:
	rm -rf *.o $(server)
clean_user:
	rm -rf *.o $(client)