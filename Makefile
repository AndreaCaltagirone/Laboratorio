CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c11
LDFLAGS = -pthread

# File sorgenti
SRCS = server.c handle_client.c method.c utility.c auth.c
OBJS = $(SRCS:.c=.o)
TARGET = server

# Target principale
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilazione oggetti
%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -f $(OBJS) $(TARGET)

# Esecuzione
run: $(TARGET)
	./$(TARGET)

# Test con curl
test: $(TARGET)
	@echo "=== Avvio server in background ==="
	./$(TARGET) &
	@sleep 1
	@echo ""
	@echo "=== Test GET pubblico ==="
	curl -v http://localhost:8080/index.html
	@echo ""
	@echo "=== Test GET protetto senza auth (atteso 401) ==="
	curl -v http://localhost:8080/admin/secret.txt
	@echo ""
	@echo "=== Test GET protetto con auth ==="
	curl -v -u admin:123 http://localhost:8080/admin/secret.txt
	@echo ""
	@echo "=== Test POST ==="
	curl -v -X POST -d "test data" http://localhost:8080/test.txt
	@echo ""
	@echo "=== Test PUT ==="
	curl -v -X PUT -d "updated data" http://localhost:8080/test.txt
	@echo ""
	@echo "=== Test DELETE ==="
	curl -v -X DELETE http://localhost:8080/test.txt
	@killall server

.PHONY: all clean run test