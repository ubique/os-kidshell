CXX=g++
NAME=kidshell
OUTPUT=out

.PHONY: all clean

all:
	@rm -rf ${OUTPUT}
	@mkdir ${OUTPUT}
	@${CXX} argparser/*.cpp *.cpp -o ${OUTPUT}/${NAME}

clean:
	@rm -rf ${OUTPUT}
