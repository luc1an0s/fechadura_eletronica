#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VAL ((F_CPU/16/BAUD)-1)

// Mapa das teclas
const char TECLAS[4][3] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

void uart_init(void) {
    UBRR0H = (unsigned char)(UBRR_VAL >> 8);
    UBRR0L = (unsigned char)UBRR_VAL;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Espera buffer limpar
    UDR0 = data;
}

unsigned char uart_receive_noblock(void) {
    if (UCSR0A & (1 << RXC0)) return UDR0;
    return 0;
}

// Função que escaneia o teclado
char lerTeclado(void) {
    for (int r = 0; r < 4; r++) {
        // 1. Coloca todas as linhas em HIGH
        PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);
        
        // 2. Coloca a linha atual (r) em LOW
        PORTD &= ~(1 << (r + 2)); 

        // 3. Verifica as colunas
        // Coluna 1 e 2 estão no Port D (PD6, PD7)
        // Coluna 3 agora está no Port B (PB1) - Pino 9 do Arduino
        if (!(PIND & (1 << PD6))) { _delay_ms(200); return TECLAS[r][0]; }
        if (!(PIND & (1 << PD7))) { _delay_ms(200); return TECLAS[r][1]; }
        if (!(PINB & (1 << PB1))) { _delay_ms(200); return TECLAS[r][2]; } 
    }
    return 0;
}

int main(void) {
    // --- CONFIGURAÇÃO DE SAÍDAS (LEDs/Relés) ---
    // PB5 (LED Builtin/Pino 13), PB4 (Pino 12), PB0 (Pino 8)
    DDRB |= (1 << PB5) | (1 << PB4) | (1 << PB0); 
    
    // --- CONFIGURAÇÃO DO TECLADO ---
    // Linhas: PD2, PD3, PD4, PD5 (Pinos 2, 3, 4, 5) como SAÍDA
    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);
    
    // Colunas: PD6, PD7 (Pinos 6, 7) e PB1 (Pino 9) como ENTRADA
    DDRD &= ~((1 << PD6) | (1 << PD7)); // Entradas Port D
    DDRB &= ~(1 << PB1);                // Entrada Port B
    
    // Ativa Pull-up nas colunas (para não flutuar)
    PORTD |= (1 << PD6) | (1 << PD7);   // Pull-up Port D
    PORTB |= (1 << PB1);                // Pull-up Port B

    uart_init();

    while (1) {
        // 1. Verifica se tem tecla apertada
        char tecla = lerTeclado();
        if (tecla != 0) {
            uart_send(tecla); // Manda pro PC!
        }

        // 2. Verifica se o PC mandou comando (A ou N)
        char cmd = uart_receive_noblock();
        if (cmd == 'A') {
            PORTB |= (1 << PB5) | (1 << PB0); // Liga LED e Relé
            _delay_ms(3000);
            PORTB &= ~((1 << PB5) | (1 << PB0)); // Desliga
        }
        else if (cmd == 'N') {
            for(int i=0; i<3; i++) {
                PORTB |= (1 << PB4); _delay_ms(200); // Pisca LED de erro
                PORTB &= ~(1 << PB4); _delay_ms(200);
            }
        }
    }
}