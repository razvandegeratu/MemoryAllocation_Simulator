# Numele executabilului
TARGET = program

# Compilatorul
CC = gcc

# Opțiuni de compilare
CFLAGS = -Wall -Wextra -g

# Fișierele sursă
SRC = main.c dll.c heap.c

# Fișierele obiect
OBJ = $(SRC:.c=.o)

# Regula implicită: cum să creezi programul final
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regula pentru compilarea fiecărui fișier .c în .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Curățare: șterge fișierele obiect și executabilul
clean:
	rm -f $(OBJ) $(TARGET)

# Regula de "phony" (nu sunt fișiere reale)
.PHONY: clean
