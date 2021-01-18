CXX = clang++
SRC_DIR := source
OBJ_DIR := obj
INC_DIR := include
BIN_DIR := .

EXE := $(BIN_DIR)/compiler
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXXFLAGS := -I$(INC_DIR) -Wall -std=c++11

.PHONY: all 

all: $(EXE)

$(EXE): $(OBJ) 
	$(CXX) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	@$(RM) -rv $(OBJ_DIR)

