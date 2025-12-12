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
### 1.1. Componentes do Sistema

O ecossistema é formado por dois pilares que trabalham em conjunto:

1.  **Firmware (Arduino):** O núcleo operacional que controla os periféricos de hardware. Ele é responsável por:
    * Ler dados do teclado matricial e do leitor RFID.
    * Exibir informações no display LCD.
    * Acionar o mecanismo de trava (servo motor).
    * Validar credenciais de acesso de forma autônoma.
    * Comunicar-se com a aplicação desktop via porta serial para sincronização e registro de eventos.

2.  **Aplicação Desktop (C com Raylib):** A interface de gerenciamento central, que oferece uma experiência visual para o administrador do sistema. Suas funções incluem:
    * **Gerenciamento de Usuários:** Cadastrar, editar e remover usuários, definir senhas, associar cartões RFID e atribuir níveis de acesso.
    * **Visualização de Logs:** Exibir um registro detalhado de todos os eventos de acesso (autorizados e negados), com informações de usuário, método e horário.
    * **Sincronização:** Enviar a base de dados de usuários atualizada para o Arduino, garantindo que o hardware sempre tenha as informações mais recentes.
    * **Recuperação de Senha:** Um mecanismo seguro para que usuários redefinam suas senhas através de uma pergunta de segurança.
      
# Sistema de Controle de Acesso (Fechadura Eletrônica)

### Níveis de Acesso

O sistema opera com dois níveis de permissão para usuários:

| Nível | Descrição |
| :--- | :--- |
| **1 - Comum** | Usuário padrão com permissão para destravar a porta usando suas credenciais (senha ou RFID). Não tem acesso à tela de administração. |
| **2 - Administrador** | Usuário com privilégios elevados. Além de poder destravar a porta, tem acesso total à aplicação de gerenciamento para administrar outros usuários e visualizar logs. |

---

## Guia de Uso da Aplicação Desktop

Esta seção detalha o funcionamento de cada tela da aplicação de gerenciamento.

### Tela de Login

A porta de entrada para o sistema. A autenticação pode ser realizada tanto pelo teclado do computador quanto pelo teclado matricial conectado ao Arduino.

**Campos e Botões da Interface:**

| Elemento | Função |
| :--- | :--- |
| **Campo ID** | Insira o número de identificação do usuário. O foco alterna automaticamente entre ID e Senha. |
| **Campo Senha** | Insira a senha numérica. Por segurança, os dígitos são mascarados. |
| **Mensagem de Status** | Exibe informações contextuais, como "Sistema Pronto", "Acesso Permitido" ou "Acesso Negado". |
| **Botão Recuperar Senha** | Inicia o fluxo de recuperação de senha para o caso de esquecimento. |

**Interação via Teclado do Arduino:**

* **Números (0-9):** Usados para preencher o campo de ID ou Senha que estiver em foco.
* **Tecla `#` (Enter/Avançar):** Pressionada uma vez, move o foco do campo ID para o campo Senha. Pressionada novamente, submete os dados para tentativa de login.
* **Tecla `*` (Cancelar/Limpar):** Limpa os campos de ID e Senha, reiniciando o processo de login.

Após um login bem-sucedido, se o usuário for um **Administrador (Nível 2)**, o sistema redireciona para a Tela de Administração. Se for um usuário **Comum (Nível 1)**, uma mensagem de "Acesso Permitido" é exibida e o comando de abertura é enviado ao Arduino.

### Tela de Administração

O centro de controle do sistema, acessível apenas para usuários com nível de administrador. Esta tela é dividida logicamente em três seções principais: **Gestão de Usuários**, **Logs do Sistema** e **Lista de Usuários**.

#### Painel de Gestão de Usuários

Este painel é usado para adicionar novos usuários ou editar os existentes.

| Campo/Botão | Descrição |
| :--- | :--- |
| **ID** | Identificador numérico único para o usuário. O sistema sugere o próximo ID disponível. |
| **Nome** | Nome completo do usuário para fácil identificação nos logs. |
| **Senha / Confirmar Senha** | Campos para definir a senha numérica do usuário. A confirmação é necessária para evitar erros. |
| **Pergunta / Resposta Secreta** | Credenciais para o mecanismo de recuperação de senha. |
| **Nível de Acesso** | Dropdown para selecionar o nível de permissão: `1 - Comum` ou `2 - Admin`. |
| **Botão Ler RFID** | Coloca o sistema em modo de escuta. Aproxime um cartão RFID do leitor conectado ao Arduino para associá-lo a este usuário. O UID do cartão será exibido ao lado. |
| **Botão Salvar** | Cria um novo usuário (se o ID for novo) ou atualiza um usuário existente com as informações fornecidas. |
| **Botão Limpar** | Limpa todos os campos do formulário. |


####  Painel de Logs do Sistema

Exibe um registro em tempo real de todos os eventos importantes. Cada entrada de log contém a data, a hora e a descrição do evento (ex: "Acesso LIBERADO ID 101", "Acesso NEGADO ID 102"). Uma barra de rolagem permite navegar por todo o histórico de logs.

#### Painel de Lista de Usuários

Mostra todos os usuários cadastrados no sistema em uma lista rolável. A partir daqui, é possível buscar, editar ou deletar usuários.

| Elemento | Descrição |
| :--- | :--- |
| **Campo de Busca** | Filtra a lista de usuários pelo nome ou ID. |
| **Botão Editar** | Ao clicar neste botão ao lado de um usuário, seus dados são carregados no painel de **Gestão de Usuários** para modificação. |
| **Botão Deletar** | Remove permanentemente o usuário do sistema. Uma confirmação pode ser necessária. Lembre-se de **sincronizar** após a exclusão. |

### Tela de Recuperação de Senha

Esta tela guia o usuário por um processo de três etapas para redefinir uma senha esquecida.

1.  **Passo 1: Inserir ID:** O usuário deve fornecer o ID da conta que deseja recuperar.
2.  **Passo 2: Responder Pergunta Secreta:** O sistema exibe a pergunta de segurança associada àquele ID. O usuário deve inserir a resposta correta.
3.  **Passo 3: Definir Nova Senha:** Se a resposta estiver correta, o usuário poderá definir uma nova senha para a conta.

---

##  Solução de Problemas Comuns 

| Problema | Causa Provável | Solução Sugerida |
| :--- | :--- | :--- |
| **Aplicação não inicia ou fecha inesperadamente.** | Falha na instalação da biblioteca Raylib ou drivers gráficos desatualizados. | Reinstale a Raylib seguindo o guia do `README.md` e verifique se os drivers de vídeo do seu computador estão atualizados. |
| **"Erro ao abrir serial. O Arduino está conectado?"** | O Arduino não está conectado, a porta serial está incorreta ou o driver USB não foi instalado. | 1. Verifique se o cabo USB está firmemente conectado.<br>2. Confirme a porta serial correta na Arduino IDE e ajuste no arquivo `src/main.c` se necessário.<br>3. Instale os drivers CH340/CP210x se estiver usando uma placa Arduino não oficial. |
| **Teclado ou RFID não respondem na aplicação.** | A comunicação serial está funcionando, mas o Arduino não está processando os dados corretamente. | Verifique as conexões (fiação) do teclado e do módulo RFID no circuito do Arduino. Abra o Serial Monitor na Arduino IDE para depurar as leituras dos periféricos. |
| **Acesso negado mesmo com a senha correta.** | A senha digitada no teclado matricial pode estar incorreta ou o sistema está com foco no campo errado. | Use a tecla `*` para limpar os campos e comece de novo. Digite o ID, pressione `#`, digite a senha e pressione `#` novamente. Verifique se a senha cadastrada no sistema está correta. |
