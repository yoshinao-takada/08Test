DEBUG_CC_FLAGS = -c -g -I../Lib
RELEASE_CC_FLAGS = -c -O2 -I../Lib
DEBUG_BIN_DIR = ../Debug/Bin
RELEASE_BIN_DIR = ../Release/Bin
DEBUG_OBJ_DIR = ../Debug/O
RELEASE_OBJ_DIR = ../Release/O
DEBUG_TARGET = $(DEBUG_BIN_DIR)/Test.exe
RELEASE_TARGET = $(RELEASE_BIN_DIR)/Test.exe
DEBUG_OBJECTS = $(DEBUG_OBJ_DIR)/main.o $(DEBUG_OBJ_DIR)/BMQBaseUT.o \
	$(DEBUG_OBJ_DIR)/BMPoolBaseUT.o $(DEBUG_OBJ_DIR)/BMEvUT.o \
	$(DEBUG_OBJ_DIR)/BMEvQUT.o $(DEBUG_OBJ_DIR)/BMDefsUT.o \
	$(DEBUG_OBJ_DIR)/BMTickUT.o $(DEBUG_OBJ_DIR)/BMRingBufUT.o

.PHONY	:	debug
.PHONY	:	clean_debug
debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS) $(DEBUG_BIN_DIR)/libBMLIB.a
	$(CC) -o $@ $^ -L$(DEBUG_BIN_DIR) -lBMLIB -lpthread

$(DEBUG_OBJ_DIR)/main.o	:	main.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMDefsUT.o	:	BMDefsUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMQBaseUT.o	:	BMQBaseUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMPoolBaseUT.o	:	BMPoolBaseUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMEvUT.o	:	BMEvUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMEvQUT.o	:	BMEvQUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMTickUT.o	:	BMTickUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMRingBufUT.o	:	BMRingBufUT.c
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

clean_debug	:
	rm -f $(DEBUG_TARGET) $(DEBUG_OBJECTS)
