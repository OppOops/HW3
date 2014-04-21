JCC=javac
TARGET=src/*.java
BIN=bin

all: src/Benchmark.java
	mkdir -p bin
	${JCC} ${TARGET} -d ${BIN}
	cd bin && dx --dex --output=../Benchmark.dex *.class
clean:
	rm *.class 
