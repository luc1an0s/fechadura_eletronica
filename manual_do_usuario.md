# Manual do Usuario: Sistema de Controle de Acesso


## 1. Visão Geral e Estrutura do Projeto

Este manual destina-se a desenvolvedores que desejam configurar, compilar e modificar o **Sistema de Controle de Acesso**. O projeto é um sistema híbrido que combina uma aplicação de desktop em C com uma lógica de controle em um microcontrolador Arduino.

### 1.1. Requisitos de Desenvolvimento

Para trabalhar no projeto, o desenvolvedor precisará de um ambiente que suporte a compilação de código C e a programação de microcontroladores.

| Componente | Finalidade | Requisito Mínimo |
| :--- | :--- | :--- |
| **Compilador C** | Compilar a aplicação desktop. | GCC (GNU Compiler Collection) |
| **Sistema de Build** | Gerenciar o processo de compilação. | GNU Make |
| **Biblioteca Gráfica** | Renderizar a Interface Gráfica do Usuário (GUI). | Raylib |
| **IDE/Editor** | Escrever e editar o código. | VS Code, Vim, ou IDE C/C++ |
| **Programação Arduino** | Compilar e carregar o firmware. | Arduino IDE |

### 1.2. Estrutura de Diretórios

O projeto segue uma estrutura modular para separar o código-fonte, os cabeçalhos e o firmware do Arduino.

```
.
├── firmware/
│   └── arduino/
│       └── arduino.ino  # Código-fonte do Arduino
├── include/             # Arquivos de cabeçalho (.h) da aplicação C
├── src/                 # Arquivos de código-fonte (.c) da aplicação C
├── Makefile             # Script de build para compilação da aplicação C
└── README.md            # Informações gerais e de instalação
```

---

## 2. Configuração do Ambiente de Desenvolvimento

O processo de configuração varia significativamente entre sistemas operacionais. As instruções a seguir pressupõem que o desenvolvedor está começando com um sistema operacional limpo, sem as dependências de desenvolvimento instaladas.

### 2.1. Configuração no Linux (Debian/Ubuntu)

O ambiente Linux é o mais direto para a compilação do projeto, pois o `Makefile` está configurado para usar o `gcc` e as bibliotecas padrão do sistema.

#### Passo 1: Instalar Ferramentas Essenciais

Instale o compilador C (`gcc`), o gerenciador de build (`make`) e o sistema de controle de versão (`git`).

```bash
sudo apt update
sudo apt install build-essential git
```

#### Passo 2: Instalar Dependências da Raylib

A aplicação desktop depende da Raylib e de suas bibliotecas de sistema (X11, OpenGL, etc.).

```bash
# Instalação das bibliotecas de desenvolvimento necessárias para a Raylib
sudo apt install libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev

# Instalação da Raylib (opção 1: via pacote do sistema)
# Esta é a forma mais fácil, mas a versão pode não ser a mais recente.
sudo apt install libraylib-dev

# OU (opção 2: compilar do código-fonte - recomendado para a versão mais recente)
# git clone https://github.com/raysan5/raylib.git
# cd raylib/src/
# make PLATFORM=PLATFORM_DESKTOP
# sudo make install
```

#### Passo 3: Configurar Comunicação Serial

O projeto utiliza funções de comunicação serial que, no Linux, são tratadas por bibliotecas padrão e pelo acesso ao dispositivo `/dev/ttyACM0` (ou similar). Nenhuma biblioteca adicional é necessária, mas o usuário pode precisar de permissão para acessar a porta serial.

```bash
# Adicionar o usuário atual ao grupo 'dialout' para acesso à porta serial
sudo usermod -a -G dialout $USER
# Você precisará fazer logout e login novamente para que a mudança tenha efeito.
```

### 2.2. Configuração no Windows

No Windows, a abordagem mais robusta para projetos C que utilizam `make` e `gcc` é através do ambiente **MSYS2** ou **MinGW-w64**.

#### Passo 1: Instalar o MSYS2

1.  Baixe e instale o **MSYS2** no [site oficial](https://www.msys2.org/).
2.  Siga as instruções para atualizar o sistema de pacotes:
    ```bash
    pacman -Syu
    ```

#### Passo 2: Instalar o Toolchain MinGW-w64

Abra o terminal **MSYS2 MinGW 64-bit** e instale o conjunto de ferramentas de desenvolvimento.

```bash
# Instalação do compilador, make e git
pacman -S --needed base-devel mingw-w64-x86_64-toolchain git
```

#### Passo 3: Instalar a Raylib

Use o gerenciador de pacotes `pacman` do MSYS2 para instalar a versão pré-compilada da Raylib para o ambiente MinGW-w64.

```bash
pacman -S mingw-w64-x86_64-raylib
```

#### Passo 4: Configurar a IDE (VS Code)

Se estiver usando o VS Code, você precisará configurar o `tasks.json` e o `c_cpp_properties.json` para que o VS Code utilize o `make` e o `gcc` instalados no caminho do MSYS2.

1.  **Adicionar o MinGW ao PATH:** Certifique-se de que o caminho para o executável `mingw64/bin` dentro da sua instalação do MSYS2 esteja no PATH do sistema, ou execute o VS Code a partir do terminal **MSYS2 MinGW 64-bit**.

---

## 3. Compilação e Execução do Projeto

Com o ambiente configurado, a compilação da aplicação desktop é simplificada pelo `Makefile` fornecido.

### 3.1. Compilando a Aplicação Desktop

1.  Navegue até o diretório raiz do projeto (`/caminho/para/o/projeto/arduino`).
2.  Execute o comando `make`.

```bash
cd /caminho/para/o/projeto/arduino
make
```

O `Makefile` irá:
*   Criar o diretório `obj/`.
*   Compilar cada arquivo `.c` em `src/` para um arquivo objeto `.o` em `obj/`.
*   Linkar todos os arquivos `.o` com as bibliotecas necessárias (`-lraylib -lm -lpthread -ldl -lrt -lX11`) para criar o executável `sistema_acesso` (ou `sistema_acesso.exe` no Windows).

### 3.2. Executando o Projeto

Após a compilação bem-sucedida, execute o binário:

```bash
# No Linux
./sistema_acesso

# No Windows (dentro do terminal MSYS2 MinGW 64-bit)
./sistema_acesso.exe
```

### 3.3. Limpeza

Para remover os arquivos de build (`obj/` e o executável), use o comando `clean`:

```bash
make clean
```

---

## 4. Desenvolvimento do Firmware (Arduino)

O desenvolvimento do firmware requer a **Arduino IDE** e a instalação de bibliotecas específicas.

#### Passo 1: Instalar a Arduino IDE

Baixe e instale a [Arduino IDE](https://www.arduino.cc/en/software).

#### Passo 2: Instalar Bibliotecas do Arduino

O firmware (`firmware/arduino/arduino.ino`) depende das seguintes bibliotecas. Elas devem ser instaladas através do **Gerenciador de Bibliotecas** da Arduino IDE (`Sketch > Incluir Biblioteca > Gerenciar Bibliotecas...`).

| Biblioteca | Autor | Finalidade |
| :--- | :--- | :--- |
| `LiquidCrystal I2C` | Frank de Brabander | Controle do Display LCD |
| `Keypad` | Mark Stanley, Alexander Brevig | Leitura do Teclado Matricial |
| `MFRC522` | GithubCommunity | Comunicação com o Leitor RFID |
| `Servo` | (Padrão) | Controle do Servo Motor |
| `Wire` e `SPI` | (Padrão) | Comunicação I2C e SPI |

#### Passo 3: Carregar o Firmware

1.  Abra o arquivo `firmware/arduino/arduino.ino` na Arduino IDE.
2.  Conecte o Arduino ao computador.
3.  Selecione a Placa e a Porta Serial corretas em `Ferramentas`.
4.  Clique em **Upload** para gravar o código no microcontrolador.

---

## 5. Dicas de Desenvolvimento

### 5.1. Debugging da Comunicação Serial

A comunicação entre o desktop e o Arduino é um ponto crítico.

*   **No Desktop:** Use ferramentas de log (como `printf` no C) para verificar o que está sendo enviado e recebido pela porta serial.
*   **No Arduino:** Use `Serial.println()` para enviar mensagens de depuração para o **Monitor Serial** da Arduino IDE. **Atenção:** O Monitor Serial não deve estar aberto ao mesmo tempo que a aplicação desktop, pois ambos competem pelo acesso à porta.

### 5.2. Alteração da Porta Serial

A aplicação desktop tenta se conectar a portas padrão. Se o seu Arduino estiver em uma porta diferente, você deve modificar o arquivo `src/main.c`:

```c
// Linhas 24-28 de src/main.c (exemplo)
#ifdef _WIN32
    statusSerial = setupSerial("COM3"); // Mude "COM3" para a porta correta
#else 
    statusSerial = setupSerial("/dev/ttyACM0"); // Mude "/dev/ttyACM0" para a porta correta
#endif 
```

Após a alteração, **recompile** o projeto com `make`.

### 5.3. Raylib e Raygui

A interface é construída usando o paradigma de **Modo Imediato** da Raygui.

*   **Estilos:** O arquivo `include/estiloTela.h` e `src/estiloTela.c` definem as cores e a aparência da GUI.
*   **Telas:** A lógica de cada tela (Login, Admin, Gestão) está separada em seus respectivos arquivos `.c` e `.h` (ex: `tela_login.c`, `tela_admin.c`).

---

## 6. Referências

- **[1] MSYS2:** [https://www.msys2.org/](https://www.msys2.org/) - Ambiente de desenvolvimento para Windows.
- **[2] Raylib:** [https://www.raylib.com/](https://www.raylib.com/) - Biblioteca para programação de jogos e aplicações gráficas.
- **[3] Arduino IDE:** [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software) - Ambiente de desenvolvimento para microcontroladores.
- **[4] GNU Make:** [https://www.gnu.org/software/make/manual/](https://www.gnu.org/software/make/manual/) - Documentação oficial do sistema de build.
