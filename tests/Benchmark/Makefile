JCC=javac
TARGET=src/*.java
BIN=bin

all: src/dhry.java
	${JCC} ${TARGET} -d ${BIN}
	cd bin && dx --dex --output=../dhry.dex *.class
clean:
	rm bin/*.class 
