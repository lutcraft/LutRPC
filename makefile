PATH_BIN = bin
PATH_LIB = lib
PATH_OBJ = obj

PATH_3RD = 3rd
PATH_CODE = code
PATH_COMM = $(PATH_CODE)/comm
PATH_SERVER = $(PATH_CODE)/server

PATH_TESTCASES = test

# will install lib to /usr/lib/libsocket.a
PATH_INSTALL_LIB_ROOT = /usr/lib

# will install all header file to /usr/include/socket
PATH_INSTALL_INC_ROOT = /usr/include

PATH_INSTALL_INC_COMM = $(PATH_INSTALL_INC_ROOT)/$(PATH_COMM)
PATH_INSTALL_INC_NET = $(PATH_INSTALL_INC_ROOT)/$(PATH_SERVER)


# PATH_PROTOBUF = /usr/include/google
# PATH_TINYXML = /usr/include/tinyxml

CXX := g++

CXXFLAGS += -g -O0 -std=c++11 -Wall -Wno-deprecated -Wno-unused-but-set-variable

CXXFLAGS += -I./ -I$(PATH_3RD) -I$(PATH_CODE)	-I$(PATH_COMM) -I$(PATH_SERVER)

# LIBS += /usr/lib/libprotobuf.a	/usr/lib/libtinyxml.a


COMM_OBJ := $(patsubst $(PATH_COMM)/%.cpp, $(PATH_OBJ)/%.o, $(wildcard $(PATH_COMM)/*.cpp))
SERVER_OBJ := $(patsubst $(PATH_SERVER)/%.cpp, $(PATH_OBJ)/%.o, $(wildcard $(PATH_SERVER)/*.cpp))

ALL_TESTS : $(PATH_BIN)/test_log $(PATH_BIN)/test_config $(PATH_BIN)/test_reactor

TEST_CASE_OUT := $(PATH_BIN)/test_log $(PATH_BIN)/test_config $(PATH_BIN)/test_reactor

LIB_OUT := $(PATH_LIB)/liblutrpc.a


$(PATH_BIN)/test_reactor: $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_reactor.cpp -o $@ $(LIB_OUT) $(LIBS) -ldl

$(PATH_BIN)/test_config: $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_config.cpp -o $@ $(LIB_OUT) $(LIBS) -ldl


$(PATH_BIN)/test_log: $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_log.cpp -o $@ $(LIB_OUT) $(LIBS) -ldl -pthread


$(LIB_OUT): $(COMM_OBJ) $(SERVER_OBJ)
	cd $(PATH_OBJ) && ar rcv liblutrpc.a *.o && cp liblutrpc.a ../lib/

$(PATH_OBJ)/%.o : $(PATH_COMM)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(PATH_OBJ)/%.o : $(PATH_SERVER)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# print something test
# like this: make PRINT-PATH_BIN, and then will print variable PATH_BIN
PRINT-% : ; @echo $* = $($*)


# to clean 
clean :
	rm -f $(COMM_OBJ) $(SERVER_OBJ) $(NET_OBJ) $(TEST_CASE_OUT) $(PATH_LIB)/liblutrpc.a $(PATH_OBJ)/liblutrpc.a

# install
install:
	mkdir -p $(PATH_INSTALL_INC_COMM) $(PATH_INSTALL_INC_NET) \
		&& cp $(PATH_COMM)/*.h $(PATH_INSTALL_INC_COMM) \
		&& cp $(PATH_SERVER)/*.h $(PATH_INSTALL_INC_NET) \
		&& cp $(LIB_OUT) $(PATH_INSTALL_LIB_ROOT)/


# uninstall
uninstall:
	rm -rf $(PATH_INSTALL_INC_ROOT)/lutrpc && rm -f $(PATH_INSTALL_LIB_ROOT)/liblutrpc.a