# ==========================================
# Makefile para Firmware (SimulIDE / AVR)
# ==========================================

# Configurações do Microcontrolador (O padrão do SimulIDE para Uno é o atmega328p)
MCU = atmega328p
F_CPU = 16000000UL

# Ferramentas do Linux (certifique-se de ter instalado: avr-gcc)
CC = avr-gcc
OBJCOPY = avr-objcopy

# Flags de Compilação
# -Os: Otimiza para tamanho (padrão para embedded)
# -Wall: Mostra avisos importantes
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall

# Arquivos
TARGET = firmware
SRC = firmware.c

# --- Regras ---

# A regra padrão apenas compila, não tenta enviar (pois não há Arduino físico)
all: hex

# 1. Compila o .c para .elf (binário executável do AVR)
elf:
	@echo "Compilando firmware para $(MCU)..."
	$(CC) $(CFLAGS) -c -o $(TARGET).o $(SRC)
	$(CC) -mmcu=$(MCU) $(TARGET).o -o $(TARGET).elf

# 2. Converte o .elf para .hex (formato que o SimulIDE lê)
hex: elf
	@echo "Gerando arquivo HEX para o SimulIDE..."
	$(OBJCOPY) -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	@echo "Sucesso! O arquivo '$(TARGET).hex' está pronto para ser carregado no SimulIDE."

# Limpeza
clean:
	rm -f *.o *.elf *.hex

.PHONY: all elf hex clean
