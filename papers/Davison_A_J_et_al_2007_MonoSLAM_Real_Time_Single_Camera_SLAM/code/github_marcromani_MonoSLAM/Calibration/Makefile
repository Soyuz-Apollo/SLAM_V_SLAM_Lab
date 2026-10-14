# Компилятор
CC = g++

# Исходные файлы
FILES_C = calibration.cpp

# Исполняемый файл
TARGET = main

# Параметры компиляции
CFLAGS = -Wall -g -std=c++14 -march=native -O3 -funroll-loops `pkg-config --cflags opencv4`

# Параметры линковщика
LFLAGS = `pkg-config --libs opencv4`

FILES_O = $(subst .cpp,.o,$(FILES_C))

$(TARGET): $(FILES_O) Makefile
	$(CC) $(FILES_O) -o $(TARGET) $(LFLAGS)

%.o: %.cpp Makefile
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

clean:
	rm $(FILES_O) $(TARGET)
