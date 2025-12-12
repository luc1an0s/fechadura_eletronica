# fechadura_eletronica
projeto de fechadura eletrônica inteligente desenvolvida em linguagem C, com interface embarcada via Arduino Uno
# Sistema de Controle de Acesso com Arduino e Raylib

## Introdução

Este projeto é um sistema de controle de acesso completo, composto por uma aplicação de gerenciamento para desktop e um firmware para um microcontrolador Arduino. A solução permite o controle de acesso a um ambiente físico através de senha numérica ou cartão RFID, oferecendo uma interface gráfica para administração de usuários, visualização de logs e configuração do sistema.

A comunicação entre o software de desktop e o hardware é realizada via porta serial, permitindo que a aplicação de gerenciamento envie atualizações de usuários (adicionar, remover, alterar permissões) em tempo real para o Arduino, que armazena e valida os dados de acesso de forma autônoma.

### Funcionalidades Principais

*   **Interface Gráfica (Desktop):** Aplicação desenvolvida em C com a biblioteca Raylib para uma experiência de usuário intuitiva.
*   **Múltiplos Métodos de Acesso:** Suporte para autenticação via teclado numérico (ID + Senha) e por proximidade com cartões RFID.
*   **Gerenciamento de Usuários:** Cadastro, edição e remoção de usuários com diferentes níveis de acesso (Comum e Administrador).
*   **Registro de Atividades:** Geração de logs para todos os eventos de acesso (liberado, negado, tentativas) com data e hora.
*   **Sincronização em Tempo Real:** Os dados dos usuários são sincronizados da aplicação desktop para o Arduino.

## Arquitetura do Sistema

O sistema é dividido em dois componentes principais:

1.  **Aplicação Desktop:** Uma interface gráfica construída em C com a biblioteca Raylib. É responsável pela gestão dos dados, como o cadastro de usuários e a visualização de logs. Ela se comunica com o Arduino através da porta serial para enviar comandos e sincronizar informações.
2.  **Firmware do Arduino:** O cérebro da operação física. Ele controla diretamente o leitor RFID, o teclado matricial, o display LCD e o servo motor (para a trava). O firmware é responsável por validar as credenciais de acesso (seja por RFID ou senha) e acionar a trava, funcionando de forma independente após a sincronização.

### Hardware Necessário (Circuito do Arduino)

*   Arduino Uno (ou compatível)
*   Módulo Leitor RFID MFRC522
*   Display LCD 16x2 com módulo I2C
*   Teclado Matricial 4x4
*   Servo Motor (ex: SG90)
*   Protoboard e jumpers para as conexões

---

## Guia de Instalação

O processo de instalação é dividido em duas etapas: a configuração do firmware do Arduino e a compilação da aplicação desktop.

### Etapa 1: Configurar o Firmware do Arduino (Windows e Linux)

Esta etapa é idêntica para ambos os sistemas operacionais.

1.  **Instale a Arduino IDE:** Baixe e instale a versão mais recente da [Arduino IDE](https://www.arduino.cc/en/software).

2.  **Instale as Bibliotecas:** A comunicação com os módulos de hardware exige bibliotecas específicas. Na Arduino IDE, vá em `Tools > Manage Libraries...` e instale as seguintes bibliotecas:
    *   `LiquidCrystal I2C` por Frank de Brabander
    *   `Keypad` por Mark Stanley, Alexander Brevig
    *   `MFRC522` por GithubCommunity
    *   `Servo` (geralmente já vem instalada com a IDE)

3.  **Carregue o Código:**
    *   Abra o arquivo `firmware/arduino/arduino.ino` na Arduino IDE.
    *   Conecte seu Arduino ao computador via USB.
    *   Selecione a placa correta em `Tools > Board` (ex: "Arduino Uno").
    *   Selecione a porta serial correta em `Tools > Port`.
    *   Clique no botão "Upload" (seta para a direita) para gravar o firmware no Arduino.

### Etapa 2: Compilar a Aplicação Desktop

As instruções variam dependendo do seu sistema operacional.

#### Para Linux (Debian/Ubuntu)

1.  **Instale as Dependências de Compilação:** Abra um terminal e execute o comando abaixo para instalar o compilador `gcc`, o `make` e as bibliotecas necessárias para a compilação da Raylib.

    ```bash
    sudo apt-get update && sudo apt-get install build-essential git libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
    ```

2.  **Instale a Biblioteca Raylib:** A forma recomendada é compilar a partir do código-fonte para garantir compatibilidade.

    ```bash
    # Clone o repositório oficial da Raylib
    git clone https://github.com/raysan5/raylib.git

    # Entre no diretório e compile
    cd raylib/src/
    make PLATFORM=PLATFORM_DESKTOP

    # Instale a biblioteca no sistema
    sudo make install
    ```

3.  **Compile o Projeto:** Navegue de volta para a pasta raiz do projeto `sistema_acesso` e execute o comando `make`.

    ```bash
    cd /caminho/para/o/projeto/arduino
    make
    ```

    Isso irá compilar o código-fonte e gerar um executável chamado `sistema_acesso`.

#### Para Windows

1.  **Instale o MSYS2:** O MSYS2 fornece um ambiente de desenvolvimento com ferramentas como `gcc` e `make`. Faça o download no [site oficial do MSYS2](https://www.msys2.org/) e siga as instruções de instalação.

2.  **Instale as Ferramentas de Compilação:** Após instalar o MSYS2, abra o terminal "MSYS2 MinGW 64-bit" e instale o toolchain do MinGW-w64.

    ```bash
    pacman -Syu
    pacman -S --needed base-devel mingw-w64-x86_64-toolchain git
    ```

3.  **Instale a Biblioteca Raylib:** Use o `pacman` para instalar a versão pré-compilada da Raylib.

    ```bash
    pacman -S mingw-w64-x86_64-raylib
    ```

4.  **Compile o Projeto:** Navegue até a pasta do projeto `sistema_acesso` dentro do terminal do MSYS2 e execute o comando `make`.

    ```bash
    # Navegue até o diretório do seu projeto
    # Exemplo: cd /c/Users/SeuUsuario/Downloads/arduino
    make
    ```

    Isso irá gerar o executável `sistema_acesso.exe`.

## Como Usar

1.  **Conecte o Hardware:** Certifique-se de que o Arduino com o circuito montado esteja conectado ao computador.

2.  **Execute a Aplicação:**
    *   No Linux: `./sistema_acesso`
    *   No Windows: `sistema_acesso.exe`

3.  **Verifique a Conexão Serial:** A aplicação tentará se conectar automaticamente à porta serial (`/dev/ttyACM0` no Linux, `COM3` no Windows). Se o seu Arduino estiver em uma porta diferente, você precisará alterar o código no arquivo `src/main.c` e recompilar.

4.  **Utilize o Sistema:** A interface gráfica permitirá que você adicione novos usuários, defina senhas, associe cartões RFID e monitore os logs de acesso.

