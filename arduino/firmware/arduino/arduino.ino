#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define MAX_USUARIOS 10 
#define SERVO_PIN A0 
#define SS_PIN 10 
#define RST_PIN A1

// Estados do Sistema
enum Estado {
    ESPERA,
    DIGITANDO_ID,
    DIGITANDO_SENHA
};

Estado estadoAtual = ESPERA;

struct Usuario {
    String id;    
    String nome;
    String senha;
    String nivel;
    String uid; 
};

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

// Configuração do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuração do Teclado
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variáveis Globais
String entradaID = "";
String entradaSenha = "";
Servo meuServo;
MFRC522 rfid(SS_PIN, RST_PIN);

// --- Funções Auxiliares ---

void abrirPorta() {
    meuServo.write(90);
    delay(3000); 
    meuServo.write(0); 
}

String lerUID() {
    // Se não tiver cartão novo, retorna vazio
    if (!rfid.PICC_IsNewCardPresent()) {
        return "";
    }
    
    // Se não conseguir ler o serial, retorna vazio
    if (!rfid.PICC_ReadCardSerial()) {
        return "";
    }

    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        uid += String(rfid.uid.uidByte[i], HEX);
    }
    
    uid.toLowerCase();
    rfid.PICC_HaltA();
    return uid;
}

void resetarEstado() {
    estadoAtual = ESPERA;
    entradaID = "";
    entradaSenha = "";
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sistema pronto");
}

// --- Setup e Loop ---

void setup() {
    Serial.begin(9600);
    SPI.begin();
    
    rfid.PCD_Init(); 
    lcd.init(); 
    lcd.backlight();
    
    meuServo.attach(SERVO_PIN);  
    meuServo.write(0);
    
    resetarEstado();
}

void loop() {
    char key = keypad.getKey();

    // --- MÁQUINA DE ESTADOS ---
    switch (estadoAtual) {
        
        case ESPERA:
            if (key) {
                // Se for número, inicia o processo de ID
                if (isDigit(key)) {
                    Serial.print(key); 
                    entradaID += key;
                } 
                else if (key == '*') {
                    Serial.print(key); // Envia comando de limpar pro PC
                }
                
                estadoAtual = DIGITANDO_ID;
                
                lcd.clear();
                lcd.print("ID:");
                lcd.setCursor(0, 1);
                
                if (entradaID.length() > 0) {
                    lcd.print(entradaID);
                }
            }
            
            // Verifica RFID apenas no estado de espera
            {
                String uid = lerUID();
                if (uid != "") {
                    processarRFID(uid);
                }
            }
            break;

        case DIGITANDO_ID:
            if (key) {
                Serial.print(key); // Envia tecla para o PC
                
                if (key == '#') { // Confirmar ID
                    if (entradaID.length() > 0) {
                        estadoAtual = DIGITANDO_SENHA;
                        
                        lcd.clear();
                        lcd.print("Senha:");
                        lcd.setCursor(0, 1);
                    }
                }
                else if (key == '*') { // Cancelar
                    resetarEstado();
                }
                else if (isDigit(key)) { // Digitando números
                    entradaID += key;
                    lcd.print(key);
                }
            }
            break;

        case DIGITANDO_SENHA:
            if (key) {
                Serial.print(key); // Envia tecla para o PC
                
                if (key == '#') { // Confirmar Senha
                    validarLogin(entradaID, entradaSenha);
                    resetarEstado();
                }
                else if (key == '*') { // Cancelar
                    resetarEstado();
                }
                else if (isDigit(key)) { // Digitando números da senha
                    entradaSenha += key;
                    lcd.print('*'); // Mostra asterisco para esconder senha
                }
            }
            break;
    }

    // --- COMUNICAÇÃO SERIAL (PC) ---
    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n');
        comando.replace("\r", "");
        comando.trim();

        if (comando.startsWith("CADASTRO:")) {
            cadastrarUsuario(comando);
        }
        
        if (comando == "LER_RFID") {
            modoLeituraRFID();
        }
        
        if (comando.startsWith("CADASTRO_RFID:")) {
            cadastrarUsuarioRFID(comando);
        }
    }
}

// --- Funções de Lógica do Sistema ---

void processarRFID(String uid) {
    for (int i = 0; i < totalUsuarios; i++) {
        String uidArmazenado = usuarios[i].uid;
        uidArmazenado.toLowerCase();
        
        if (uidArmazenado == uid) {
            lcd.clear();
            lcd.print("Chave liberada");
            
            String timestamp = String(millis() / 1000);
            Serial.println("LOG:ENTRADA;" + timestamp + ";" + usuarios[i].nome + ";RFID;" + usuarios[i].nivel);
            
            abrirPorta();
            resetarEstado();
            return;
        }
    }
    
    // Se saiu do loop, não encontrou
    lcd.clear();
    lcd.print("Chave nao");
    lcd.setCursor(0, 1);
    lcd.print("reconhecida");
    
    String timestamp = String(millis() / 1000);
    Serial.println("LOG:ERRO;" + timestamp + ";DESCONHECIDO;RFID_INVALIDO;" + uid);
    
    delay(2000);
    resetarEstado();
}

void validarLogin(String id, String senha) {
    for (int i = 0; i < totalUsuarios; i++) {
        if (usuarios[i].id == id && usuarios[i].senha == senha) {
            lcd.clear();
            lcd.print("Acesso liberado");
            
            String timestamp = String(millis() / 1000);
            Serial.println("LOG:ENTRADA;" + timestamp + ";" + usuarios[i].nome + ";SENHA;" + usuarios[i].nivel);
            
            abrirPorta();
            return;
        }
    }

    // Login falhou
    lcd.clear();
    lcd.print("Dados incorretos");
    
    String timestamp = String(millis() / 1000);
    Serial.println("LOG:ERRO;" + timestamp + ";ID_" + id + ";SENHA_INCORRETA;" + senha);
    
    delay(2000);
}

void modoLeituraRFID() {
    lcd.clear();
    lcd.print("Aprox. cartao...");
    
    String u = "";
    long timeout = millis() + 10000; // 10s timeout
    
    while (u == "" && millis() < timeout) {
        u = lerUID();
    }
    
    if (u != "") {
        Serial.println("RFID:" + u);
    } else {
        Serial.println("ERRO:Timeout");
    }
    
    resetarEstado();
}

// --- Funções de Cadastro (ID Único) ---

void cadastrarUsuario(String comando) {
    comando.replace("CADASTRO:", "");
    
    int p1 = comando.indexOf(';');
    int p2 = comando.indexOf(';', p1+1);
    int p3 = comando.indexOf(';', p2+1);

    String id = comando.substring(0, p1);
    String nome = comando.substring(p1+1, p2);
    String senha = comando.substring(p2+1, p3);
    String nivel = comando.substring(p3+1);

    // 1. Procura se o ID já existe para ATUALIZAR
    for (int i = 0; i < totalUsuarios; i++) {
        if (usuarios[i].id == id) {
            usuarios[i].nome = nome;
            usuarios[i].senha = senha;
            usuarios[i].nivel = nivel;
            
            lcd.clear(); 
            lcd.print("Usuario Atualiz.");
            
            delay(1000); 
            resetarEstado();
            return;
        }
    }

    // 2. Se não achou e tem espaço, CRIA NOVO
    if (totalUsuarios < MAX_USUARIOS) {
        usuarios[totalUsuarios].id = id;
        usuarios[totalUsuarios].nome = nome;
        usuarios[totalUsuarios].senha = senha;
        usuarios[totalUsuarios].nivel = nivel;
        usuarios[totalUsuarios].uid = "";
        
        totalUsuarios++;
        
        lcd.clear(); 
        lcd.print("Usuario Criado");
    } else {
        lcd.clear(); 
        lcd.print("Memoria Cheia!");
    }
    
    delay(1000); 
    resetarEstado();
}

void cadastrarUsuarioRFID(String comando) {
    comando.replace("CADASTRO_RFID:", "");
    
    int p1 = comando.indexOf(';');
    int p2 = comando.indexOf(';', p1+1);
    int p3 = comando.indexOf(';', p2+1);
    int p4 = comando.indexOf(';', p3+1);

    String id = comando.substring(0, p1);
    String nome = comando.substring(p1+1, p2);
    String senha = comando.substring(p2+1, p3);
    String nivel = comando.substring(p3+1, p4);
    String uid = comando.substring(p4+1);
    
    uid.toLowerCase();

    // 1. Procura se o ID já existe para ATUALIZAR
    for (int i = 0; i < totalUsuarios; i++) {
        if (usuarios[i].id == id) {
            usuarios[i].nome = nome;
            usuarios[i].senha = senha;
            usuarios[i].nivel = nivel;
            usuarios[i].uid = uid;
            
            lcd.clear(); 
            lcd.print("User Atualiz.");
            
            delay(1000); 
            resetarEstado();
            return;
        }
    }

    // 2. Cria novo se não existir
    if (totalUsuarios < MAX_USUARIOS) {
        usuarios[totalUsuarios].id = id;
        usuarios[totalUsuarios].nome = nome;
        usuarios[totalUsuarios].senha = senha;
        usuarios[totalUsuarios].nivel = nivel;
        usuarios[totalUsuarios].uid = uid;
        
        totalUsuarios++;
        
        lcd.clear(); 
        lcd.print("User Criado");
    } else {
        lcd.clear(); 
        lcd.print("Memoria Cheia!");
    }
    
    delay(1000); 
    resetarEstado();
}