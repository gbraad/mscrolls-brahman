################################################################################
# Stefan Bylund 2020
#
# Makefile for compiling the TrimProg Java tool.
#
# Example:
# make clean all
################################################################################

JAVAHOME := C:/Java/jdk-11.0.5

JAVAC := $(JAVAHOME)/bin/javac

JAR := $(JAVAHOME)/bin/jar

MKDIR := mkdir -p

RM := rm -rf

.PHONY: all
all:
	$(MKDIR) bin
	$(JAVAC) -sourcepath src -d bin src/magnetic/*.java
	$(JAR) cvfe trim_prog.jar magnetic.TrimProg -C bin .

.PHONY: clean
clean:
	$(RM) bin
