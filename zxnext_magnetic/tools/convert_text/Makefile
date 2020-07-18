################################################################################
# Stefan Bylund 2019
#
# Makefile for compiling the ConvertText Java tool.
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
	$(JAR) cvfe convert_text.jar magnetic.ConvertText -C bin .

.PHONY: clean
clean:
	$(RM) bin
