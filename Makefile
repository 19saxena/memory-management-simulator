all:
	g++ -std=c++17 -Wall \
src/main.cpp \
src/allocator/memory_manager.cpp \
src/cache/cache.cpp \
src/cache/multilevel_cache.cpp \
src/buddy/buddy_allocator.cpp \
-o memsim.exe

run:
	memsim.exe

clean:
	rm -f memsim memsim.exe
