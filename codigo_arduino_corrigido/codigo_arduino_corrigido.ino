#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define MAX_USUARIOS 10
#define BUZZER_PIN 10

// Função auxiliar para espelhar a saída do LCD para a Serial
void lcd_print_mirror(LiquidCrystal_I2C& lcd, const String& linha1, const String& linha2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  
  // Espelha para a Serial
  Serial.print("DISPLAY: ");
  Serial.print(linha1);

  if (linha2.length() > 0) {
    lcd.setCursor(0, 1);
    lcd.print(linha2);
    Serial.print("|"); // Separador de linha
    Serial.print(linha2);
  }
  Serial.println(); // Nova linha para finalizar a mensagem serial
}

struct Usuario {
  String nome;
  String senha;
  String nivel;
};

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Teclado matricial
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
  
  // Saída inicial espelhada
  lcd_print_mirror(lcd, "Sistema pronto");
  
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {

  // -------------------------------
  // Leitura do teclado
  // -------------------------------
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      validarSenha(senhaDigitada);
      senhaDigitada = "";
    }
    else if (key == '*') {
      senhaDigitada = "";
      // Saída espelhada
      lcd_print_mirror(lcd, "Senha limpa");
    }
    else if (key == 'A') {
      tone(BUZZER_PIN, 1000);
      delay(300);
      noTone(BUZZER_PIN);
      // Saída espelhada
      lcd_print_mirror(lcd, "Buzzer acionado");
    }
    else {
      senhaDigitada += key;

      // Saída para o LCD (apenas a primeira linha é espelhada de forma simples)
      lcd.clear();
      lcd.print("Senha: ");
      
      // Mostra **** no LCD, em vez da senha real
      String asteriscos = "";
      for (int i = 0; i < senhaDigitada.length(); i++) {
        lcd.print("*");
        asteriscos += "*";
      }
      
      // Espelha a senha digitada (em asteriscos) para a Serial
      Serial.print("DISPLAY: Senha: ");
      Serial.println(asteriscos);
    }
  }

  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');

    comando.replace("\r", "");  // remove CR (Windows)
    comando.trim();             // remove espaços ou lixo

    if (comando.startsWith("CADASTRO: ")) {
      cadastrarUsuario(comando);
      Serial.println("OK");     // ACK para o PC
    }
  }
}

void validarSenha(String senha) {
  bool encontrado = false;

  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].senha == senha) {
      encontrado = true;
      // Saída espelhada
      lcd_print_mirror(lcd, "Acesso liberado");

      Serial.println("[LOGIN]:" + usuarios[i].nome + "; SUCESSO");
      return;
    }
  }

  if (!encontrado) {
    // Saída espelhada
    lcd_print_mirror(lcd, "Senha incorreta");
    Serial.println("[LOGIN]: DESCONHECIDO ; FALHA");
  }
}


void cadastrarUsuario(String comando) {

  comando.replace("CADASTRO:", "");

  int p1 = comando.indexOf(';');
  int p2 = comando.indexOf(';', p1+1);

  String nome = comando.substring(0, p1);
  String senha = comando.substring(p1+1, p2);
  String nivel = comando.substring(p2+1);

  // Atualizar usuário existente?
  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].nome == nome) {
      usuarios[i].senha = senha;
      usuarios[i].nivel = nivel;

      Serial.println("USUARIO_ATUALIZADO:" + nome);
      // Saída espelhada
      lcd_print_mirror(lcd, "Senha alterada");
      return;
    }
  }

  if (totalUsuarios < MAX_USUARIOS) {
    usuarios[totalUsuarios].nome = nome;
    usuarios[totalUsuarios].senha = senha;
    usuarios[totalUsuarios].nivel = nivel;
    totalUsuarios++;

    Serial.println("USUARIO_CADASTRADO:" + nome);
    // Saída espelhada
    lcd_print_mirror(lcd, "Usuario criado");
  }
  else {
    Serial.println("CADASTRO_FALHA:limite");
    // Saída espelhada
    lcd_print_mirror(lcd, "Limite atingido");
  }
}
