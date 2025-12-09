#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define MAX_USUARIOS 10
#define BUZZER_PIN 10
#define SERVO_PIN A0

#define SS_PIN 10
#define RST_PIN A1

struct Usuario {
  String nome;
  String senha;
  String nivel;
  String uid; 
};

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

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

String senhaDigitada = "";
Servo meuServo;

MFRC522 rfid(SS_PIN, RST_PIN);

void abrirPorta() {
  meuServo.write(90);
  delay(3000);
  meuServo.write(0);
}

String lerUID() {
  if (!rfid.PICC_IsNewCardPresent()) return "";
  if (!rfid.PICC_ReadCardSerial()) return "";

  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
  }

  rfid.PICC_HaltA();
  return uid;
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Sistema pronto");

  pinMode(BUZZER_PIN, OUTPUT);
  meuServo.attach(SERVO_PIN);   
  meuServo.write(0);
}

void loop() {

  String uid = lerUID();
  if (uid != "") {
    for (int i = 0; i < totalUsuarios; i++) {
      if (usuarios[i].uid == uid) {
        lcd.clear();
        lcd.print("RFID liberado");
        Serial.println("LOGIN:" + usuarios[i].nome + ";RFID_OK");
        abrirPorta();
        lcd.clear();
        lcd.print("Sistema pronto");
        return;
      }
    }

    Serial.println("RFID_DESCONHECIDO:" + uid);
  }

  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      validarSenha(senhaDigitada);
      senhaDigitada = "";
    }
    else if (key == '*') {
      senhaDigitada = "";
      lcd.clear();
      lcd.print("Senha limpa");
    }
    else {
      senhaDigitada += key;
      lcd.clear();
      lcd.print("Senha: ");
      for (int i = 0; i < senhaDigitada.length(); i++) lcd.print("*");
    }
  }

  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.replace("\r", "");
    comando.trim();

    if (comando.startsWith("CADASTRO:")) {
      cadastrarUsuario(comando);
      Serial.println("OK");
    }

    if (comando == "LER_RFID") {
      lcd.clear();
      lcd.print("Aprox. cartao...");
      String u = "";
      while (u == "") {
        u = lerUID();
      }
      Serial.println("RFID:" + u);
      lcd.clear();
      lcd.print("Sistema pronto");
    }

    if (comando.startsWith("CADASTRO_RFID:")) {
      cadastrarUsuarioRFID(comando);
      Serial.println("OK");
    }
  }
}

void validarSenha(String senha) {
  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].senha == senha) {
      lcd.clear();
      lcd.print("Acesso liberado");
      Serial.println("LOGIN:" + usuarios[i].nome + ";SUCESSO");
      abrirPorta();
      lcd.clear();
      lcd.print("Sistema pronto");
      return;
    }
  }

  lcd.clear();
  lcd.print("Senha incorreta");
  Serial.println("LOGIN:DESCONHECIDO;FALHA");
}

void cadastrarUsuario(String comando) {
  comando.replace("CADASTRO:", "");
  int p1 = comando.indexOf(';');
  int p2 = comando.indexOf(';', p1+1);

  String nome = comando.substring(0, p1);
  String senha = comando.substring(p1+1, p2);
  String nivel = comando.substring(p2+1);

  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].nome == nome) {
      usuarios[i].senha = senha;
      usuarios[i].nivel = nivel;
      lcd.clear();
      lcd.print("Senha alterada");
      return;
    }
  }

  usuarios[totalUsuarios].nome = nome;
  usuarios[totalUsuarios].senha = senha;
  usuarios[totalUsuarios].nivel = nivel;
  usuarios[totalUsuarios].uid   = "";
  totalUsuarios++;

  lcd.clear();
  lcd.print("Usuario criado");
}

void cadastrarUsuarioRFID(String comando) {
  comando.replace("CADASTRO_RFID:", "");
  int p1 = comando.indexOf(';');
  int p2 = comando.indexOf(';', p1+1);
  int p3 = comando.indexOf(';', p2+1);

  String nome = comando.substring(0, p1);
  String senha = comando.substring(p1+1, p2);
  String nivel = comando.substring(p2+1, p3);
  String uid   = comando.substring(p3+1);

  usuarios[totalUsuarios].nome = nome;
  usuarios[totalUsuarios].senha = senha;
  usuarios[totalUsuarios].nivel = nivel;
  usuarios[totalUsuarios].uid   = uid;
  totalUsuarios++;

  lcd.clear();
  lcd.print("User+RFID criado");
}
