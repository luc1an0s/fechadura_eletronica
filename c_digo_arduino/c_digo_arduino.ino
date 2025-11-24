#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define MAX_USUARIOS 10
#define BUZZER_PIN 10 

struct Usuario {
  String nome;
  String senha;
  String nivel;
};

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Teclado
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

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Sistema pronto");

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {

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
    else if (key == 'A') {   
      tone(BUZZER_PIN, 1000);
      delay(300);
      noTone(BUZZER_PIN);   
      lcd.clear();
      lcd.print("Buzzer acionado");
    } 
    else {
      senhaDigitada += key;

      lcd.clear();
      lcd.print("Senha: ");

      // mostrar * no lugar dos números
      for (int i = 0; i < senhaDigitada.length(); i++) {
        lcd.print("*");
      }
    }
  }

  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando.startsWith("CADASTRO:")) {
      cadastrarUsuario(comando);
    }
  }
}

void validarSenha(String senha) {
  bool encontrado = false;
  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].senha == senha) {
      encontrado = true;
      lcd.clear();
      lcd.print("Acesso liberado");
      Serial.println("LOGIN:" + usuarios[i].nome + ";SUCESSO");
      break;
    }
  }
  if (!encontrado) {
    lcd.clear();
    lcd.print("Senha incorreta");
    Serial.println("LOGIN:DESCONHECIDO;FALHA");
  }
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
      Serial.println("USUARIO_ATUALIZADO:" + nome);
      return;
    }
  }

  if (totalUsuarios < MAX_USUARIOS) {
    usuarios[totalUsuarios].nome = nome;
    usuarios[totalUsuarios].senha = senha;
    usuarios[totalUsuarios].nivel = nivel;
    totalUsuarios++;
    lcd.clear();
    lcd.print("Usuario cadastrado");
    Serial.println("USUARIO_CADASTRADO:" + nome);
  } else {
    lcd.clear();
    lcd.print("Limite atingido");
    Serial.println("CADASTRO_FALHA:limite");
  }
}
