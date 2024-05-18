CC = cc
DEBUG_CC_FLAGS = -c -g -I.
RELEASE_CC_FLAGS = -c -O2 -I.
DEBUG_BIN_DIR = ../Debug/Bin
RELEASE_BIN_DIR = ../Release/Bin
DEBUG_OBJ_DIR = ../Debug/O
RELEASE_OBJ_DIR = ../Release/O
DEBUG_TARGET = $(DEBUG_BIN_DIR)/libBMLIB.a
RELEASE_TARGET = $(RELEASE_BIN_DIR)/libBMLIB.a
DEBUG_OBJECTS = $(DEBUG_OBJ_DIR)/BMEv.o $(DEBUG_OBJ_DIR)/BMPoolBase.o \
	$(DEBUG_OBJ_DIR)/BMQBase.o $(DEBUG_OBJ_DIR)/BMEvQ.o \
	$(DEBUG_OBJ_DIR)/BMTick.o $(DEBUG_OBJ_DIR)/BMRingBuf.o
RELEASE_OBJECTS = $(RELEASE_OBJ_DIR)/BMEv.o $(RELEASE_OBJ_DIR)/BMPoolBase.o \
	$(RELEASE_OBJ_DIR)/BMQBase.o $(RELEASE_OBJ_DIR)/BMEvQ.o \
	$(RELEASE_OBJ_DIR)/BMTick.o $(RELEASE_OBJ_DIR)/BMRingBuf.o

debug	:	$(DEBUG_TARGET)
$(DEBUG_TARGET)	:	$(DEBUG_OBJECTS)
	mkdir -p $(DEBUG_BIN_DIR)
	$(AR) r $@ $^

$(DEBUG_OBJ_DIR)/BMEv.o	:	BMEv.c BMEv.h BMPoolBase.h BMDefs.h
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMPoolBase.o	:	BMPoolBase.c BMPoolBase.h BMDefs.h
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMQBase.o	:	BMQBase.c BMQBase.h BMDefs.h
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMEvQ.o	:	BMEvQ.c BMEvQ.h BMEv.h BMPoolBase.h BMQBase.h BMDefs.h
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMTick.o	:	BMTick.c BMTick.h BMEv.h BMPoolBase.h BMEvQ.h BMQBase.h BMDefs.h
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

$(DEBUG_OBJ_DIR)/BMRingBuf.o	:	BMRingBuf.c BMRingBuf.h BMPoolBase.h BMQBase.h BMDefs.h
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) $(DEBUG_CC_FLAGS) -o $@ $<

clean_debug	:
	rm -f $(DEBUG_OBJECTS) $(DEBUG_TARGET)