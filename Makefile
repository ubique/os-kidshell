CXX := c++
STANDART := c++14
DIR := out

all: $(DIR)/interpreter.out

$(DIR)/interpreter.out:
	mkdir -p $(DIR)
	c++ -std=$(STANDART) Interpreter.cpp -o $(DIR)/interpreter.out

run: $(DIR)/interpreter.out
	$(DIR)/interpreter.out

.PHONY: clean
clean:
	rm -rf $(DIR)