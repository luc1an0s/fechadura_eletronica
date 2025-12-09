#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define MAX_USUARIOS 10 // maximo de usuarios (memoria do arduino é limitada né)
#define SERVO_PIN A0 // servo motor da fechadura

#define SS_PIN 10 // pinos do modulo RFID
#define RST_PIN A1

// struct pra guardar info do usuario
struct Usuario {
  String nome;
  String senha;
  String nivel;
  String uid; // UID do cartao rfid (vazio se nao tem)
};

Usuario usuarios[MAX_USUARIOS]; // array de usuarios
int totalUsuarios = 0; // contador

LiquidCrystal_I2C lcd(0x27, 16, 2); // display LCD I2C

// configuracao do teclado matricial 4x4
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; // pinos das linhas
byte colPins[COLS] = {5, 4, 3, 2}; // pinos das colunas

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String senhaDigitada = ""; // vai acumulando os numeros digitados
Servo meuServo; // servo motor

MFRC522 rfid(SS_PIN, RST_PIN); // modulo RFID

// abre a porta girando o servo 90 graus por 3 segundos
void abrirPorta() {
  meuServo.write(90); // gira
  delay(3000); // espera 3s
  meuServo.write(0); // volta
}

// le o UID do cartao RFID (se tiver algum proximo)
String lerUID() {
  if (!rfid.PICC_IsNewCardPresent()) return ""; // sem cartao = retorna vazio
  if (!rfid.PICC_ReadCardSerial()) return ""; // nao conseguiu ler = retorna vazio

  String uid = "";
  // converte os bytes do UID pra hexadecimal
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toLowerCase(); // normaliza pra minusculas
  rfid.PICC_HaltA(); // para a comunicacao com o cartao
  return uid;
}

// setup inicial do arduino
void setup() {
  Serial.begin(9600); // serial pra conversar com o PC
  SPI.begin(); // SPI pro modulo RFID
  rfid.PCD_Init(); // inicializa o leitor RFID

  lcd.init(); // inicializa o LCD
  lcd.backlight(); // liga a luz de fundo
  lcd.setCursor(0,0);
  lcd.print("Sistema pronto"); // mensagem inicial

  meuServo.attach(SERVO_PIN); // conecta o servo no pino  
  meuServo.write(0); // posicao inicial (fechado)
}

// loop principal do arduino (roda infinitamente)
void loop() {

  // primeiro verifica se tem algum cartao RFID proximo
  String uid = lerUID();
  if (uid != "") { // achou um cartao!
      bool encontrado = false;

      // procura se esse UID ta cadastrado
      for (int i = 0; i < totalUsuarios; i++) {
        // Compara sempre em minusculas
        String uidArmazenado = usuarios[i].uid;
        uidArmazenado.toLowerCase();
        if (uidArmazenado == uid) { // achou o usuario!
          encontrado = true;

          lcd.clear();
          lcd.print("Chave liberada"); // mostra no LCD
          
          // manda log pro PC com timestamp (millis/1000 = segundos desde que ligou)
          String timestamp = String(millis() / 1000);
          Serial.println("LOG:ENTRADA;" + timestamp + ";" + usuarios[i].nome + ";RFID;" + usuarios[i].nivel);
          
          abrirPorta(); // ABRE A PORTA!!
          lcd.clear();
          lcd.print("Sistema pronto");
          return; // sai do loop pra nao processar mais nada
        }
      }

      // chegou aqui = cartao nao cadastrado
      if (!encontrado) {
          lcd.clear();
          lcd.print("Chave nao");
          lcd.setCursor(0, 1);
          lcd.print("reconhecida"); // cartao desconhecido
          
          // manda log de erro pro PC
          String timestamp = String(millis() / 1000);
          Serial.println("LOG:ERRO;" + timestamp + ";DESCONHECIDO;RFID_INVALIDO;" + uid);
          
          delay(2000); // espera 2s
          lcd.clear();
          lcd.print("Sistema pronto");
      }
  }

  // le uma tecla do teclado (se apertou alguma)
  char key = keypad.getKey();
  if (key) {
    if (key == '#') { // # = confirma a senha
      validarSenha(senhaDigitada); // valida
      senhaDigitada = ""; // limpa
    }
    else if (key == '*') { // * = limpa a senha
      senhaDigitada = "";
      lcd.clear();
      lcd.print("Senha limpa");
    }
    else { // qualquer outro numero = adiciona na senha
      senhaDigitada += key;
      lcd.clear();
      lcd.print("Senha: ");
      // mostra asteriscos ao inves dos numeros (obvio)
      for (int i = 0; i < senhaDigitada.length(); i++) lcd.print("*");
    }
  }

  // verifica se o PC mandou algum comando pela serial
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.replace("\r", ""); // remove \r se tiver
    comando.trim(); // remove espacos

    // comando pra cadastrar usuario SEM rfid
    if (comando.startsWith("CADASTRO:")) {
      cadastrarUsuario(comando);
      Serial.println("OK"); // responde OK
    }

    // comando pra ler um cartao RFID (usado no cadastro)
    if (comando == "LER_RFID") {
      lcd.clear();
      lcd.print("Aprox. cartao..."); // pede pro usuario aproximar o cartao
      String u = "";
      while (u == "") { // fica esperando ate ler algum cartao
        u = lerUID();
      }
      Serial.println("RFID:" + u); // manda o UID pro PC
      lcd.clear();
      lcd.print("Sistema pronto");
    }

    // comando pra cadastrar usuario COM rfid
    if (comando.startsWith("CADASTRO_RFID:")) {
      cadastrarUsuarioRFID(comando);
      Serial.println("OK"); // responde OK
    }
  }
}

// valida se a senha digitada ta certa
void validarSenha(String senha) {
  // procura a senha no array de usuarios
  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].senha == senha) { // ACERTOU!!
      lcd.clear();
      lcd.print("Acesso liberado");
      
      // manda log de entrada pro PC
      String timestamp = String(millis() / 1000);
      Serial.println("LOG:ENTRADA;" + timestamp + ";" + usuarios[i].nome + ";SENHA;" + usuarios[i].nivel);
      
      abrirPorta(); // ABRE A PORTA
      lcd.clear();
      lcd.print("Sistema pronto");
      return; // sai da funcao
    }
  }

  // chegou aqui = senha errada
  lcd.clear();
  lcd.print("Senha incorreta");
  
  // manda log de erro pro PC
  String timestamp = String(millis() / 1000);
  Serial.println("LOG:ERRO;" + timestamp + ";DESCONHECIDO;SENHA_INCORRETA;" + senha);
  
  delay(2000); // espera 2s
  lcd.clear();
  lcd.print("Sistema pronto");
}

// cadastra usuario sem RFID (so senha mesmo)
void cadastrarUsuario(String comando) {
  comando.replace("CADASTRO:", ""); // remove o prefixo
  // parse do comando (formato: nome;senha;nivel)
  int p1 = comando.indexOf(';');
  int p2 = comando.indexOf(';', p1+1);

  String nome = comando.substring(0, p1);
  String senha = comando.substring(p1+1, p2);
  String nivel = comando.substring(p2+1);

  // verifica se ja existe (pra alterar a senha)
  for (int i = 0; i < totalUsuarios; i++) {
    if (usuarios[i].nome == nome) { // ja existe = so altera
      usuarios[i].senha = senha;
      usuarios[i].nivel = nivel;
      lcd.clear();
      lcd.print("Senha alterada");
      return;
    }
  }

  // nao existe = cadastra novo
  usuarios[totalUsuarios].nome = nome;
  usuarios[totalUsuarios].senha = senha;
  usuarios[totalUsuarios].nivel = nivel;
  usuarios[totalUsuarios].uid   = ""; // sem rfid
  totalUsuarios++;

  lcd.clear();
  lcd.print("Usuario criado");
}

// cadastra usuario COM rfid
void cadastrarUsuarioRFID(String comando) {
  comando.replace("CADASTRO_RFID:", ""); // remove prefixo
  // parse do comando (formato: nome;senha;nivel;uid)
  int p1 = comando.indexOf(';');
  int p2 = comando.indexOf(';', p1+1);
  int p3 = comando.indexOf(';', p2+1);

  String nome = comando.substring(0, p1);
  String senha = comando.substring(p1+1, p2);
  String nivel = comando.substring(p2+1, p3);
  String uid   = comando.substring(p3+1);
  uid.toLowerCase(); // normaliza pra minusculas

  // cadastra direto (nao verifica se ja existe pq a GUI ja faz isso)
  usuarios[totalUsuarios].nome = nome;
  usuarios[totalUsuarios].senha = senha;
  usuarios[totalUsuarios].nivel = nivel;
  usuarios[totalUsuarios].uid   = uid;
  totalUsuarios++;

  lcd.clear();
  lcd.print("Usuário com chave criado"); // mensagem de sucesso
}
