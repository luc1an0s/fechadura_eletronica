#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "serial.h"
#include "log.h"

// sei la cara, precisava dessas variaveis globais
HWND hwndLoginUser, hwndLoginPass;
HWND hwndAdminList, hwndNome, hwndSenha, hwndNivel, hwndRFIDCheck;

LRESULT CALLBACK LoginProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AdminProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void atualizarListaUsuarios() {
    SendMessage(hwndAdminList, LB_RESETCONTENT, 0, 0);
    wchar_t buffer[256];
    char rfidText[50];
    
    for (int i = 0; i < totalUsuarios; i++) {
        // tem rfid ou nao tem? essa é a questao
        if (strlen(usuarios[i].uid) > 0) {
            strcpy(rfidText, usuarios[i].uid);
        } else {
            strcpy(rfidText, "Nao cadastrado");
        }
        
        // formatando essa bagaca pq windows é chato com UTF-8
        _snwprintf(buffer, 256, L"%d. %hs | Nível: %hs | RFID: %hs", 
                i + 1,
                usuarios[i].nome, 
                usuarios[i].nivel,
                rfidText);
        SendMessageW(hwndAdminList, LB_ADDSTRING, 0, (LPARAM)buffer);
    }
    
    // contador de usuarios pq sim
    _snwprintf(buffer, 256, L"Total: %d/%d usuários", totalUsuarios, MAX_USUARIOS);
    SendMessageW(hwndAdminList, LB_ADDSTRING, 0, (LPARAM)L"");
    SendMessageW(hwndAdminList, LB_ADDSTRING, 0, (LPARAM)buffer);
}

void limparCampos() {
    // APAGA TUDOOOO
    SetWindowTextW(hwndNome, L"");
    SetWindowTextW(hwndSenha, L"");
    SetWindowTextW(hwndNivel, L"");
    SendMessage(hwndRFIDCheck, BM_SETCHECK, BST_UNCHECKED, 0);
}

void preencherCamposUsuario(int index) {
    // preenche automatico qdo clica duas vezes (genial demais)
    if (index >= 0 && index < totalUsuarios) {
        SetWindowText(hwndNome, usuarios[index].nome);
        SetWindowText(hwndSenha, usuarios[index].senha);
        SetWindowText(hwndNivel, usuarios[index].nivel);
        if (strlen(usuarios[index].uid) > 0) {
            SendMessage(hwndRFIDCheck, BM_SETCHECK, BST_CHECKED, 0);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    InitCommonControls();
    SetConsoleCP(65001);  // UTF-8 porra!!
    SetConsoleOutputCP(65001);

    // registrando as janelas (sério isso é necessário??)
    WNDCLASS wcLogin = {0}, wcAdmin = {0}, wcLog = {0};
    wcLogin.lpfnWndProc = LoginProc;
    wcLogin.hInstance = hInstance;
    wcLogin.lpszClassName = "LoginWindow";
    RegisterClass(&wcLogin);

    wcAdmin.lpfnWndProc = AdminProc;
    wcAdmin.hInstance = hInstance;
    wcAdmin.lpszClassName = "AdminWindow";
    RegisterClass(&wcAdmin);

    wcLog.lpfnWndProc = LogProc;
    wcLog.hInstance = hInstance;
    wcLog.lpszClassName = "LogWindow";
    RegisterClass(&wcLog);

    // cria a janela de login
    HWND hwndLogin = CreateWindowEx(0, "LoginWindow", "Login Admin",
                                    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                    300, 180, NULL, NULL, hInstance, NULL);
    ShowWindow(hwndLogin, nCmdShow);

    carregarUsuariosDoArquivo();
    abrirSerial("\\\\.\\COM5");  // TODO: tornar isso configuravel algum dia talvez

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    fecharSerial();
    return 0;
}

// ---------------------- LOGIN ----------------------
LRESULT CALLBACK LoginProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindowW(L"STATIC", L"Usuário:", WS_VISIBLE | WS_CHILD, 20, 20, 60, 20, hwnd, NULL, NULL, NULL);
            hwndLoginUser = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 90, 20, 150, 20, hwnd, NULL, NULL, NULL);
            
            CreateWindowW(L"STATIC", L"Senha:", WS_VISIBLE | WS_CHILD, 20, 50, 60, 20, hwnd, NULL, NULL, NULL);
            hwndLoginPass = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 90, 50, 150, 20, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"BUTTON", L"Entrar", WS_VISIBLE | WS_CHILD, 90, 90, 80, 30, hwnd, (HMENU)1, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                char user[50], pass[50];
                GetWindowText(hwndLoginUser, user, sizeof(user));
                GetWindowText(hwndLoginPass, pass, sizeof(pass));

                if (strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0) {
                    registrarLog("Login admin bem-sucedido");
                    ShowWindow(hwnd, SW_HIDE);

                    HWND hwndAdmin = CreateWindowExW(0, L"AdminWindow", L"Painel Administrativo",
                                                    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                                    620, 380, NULL, NULL, GetModuleHandle(NULL), NULL);
                    ShowWindow(hwndAdmin, SW_SHOW);
                } else {
                    registrarLog("Tentativa de login admin falhou");
                    MessageBoxW(hwnd, L"Usuário ou senha incorretos!", L"Erro", MB_ICONERROR);
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// ---------------------- VISUALIZADOR DE LOGS ----------------------
LRESULT CALLBACK LogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndLogList = NULL;

    switch (uMsg) {
        case WM_CREATE: {
            hwndLogList = CreateWindowW(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                       10, 10, 660, 430, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"BUTTON", L"Atualizar", WS_VISIBLE | WS_CHILD, 10, 450, 80, 30,
                         hwnd, (HMENU)1, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Limpar Logs", WS_VISIBLE | WS_CHILD, 100, 450, 80, 30,
                         hwnd, (HMENU)2, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Fechar", WS_VISIBLE | WS_CHILD, 600, 450, 80, 30,
                         hwnd, (HMENU)3, NULL, NULL);

            // Carregar logs na criação
            int totalLinhas = 0;
            char **logs = carregarLogs(&totalLinhas);
            if (logs) {
                for (int i = 0; i < totalLinhas; i++) {
                    SendMessage(hwndLogList, LB_ADDSTRING, 0, (LPARAM)logs[i]);
                }
                liberarLogs(logs, totalLinhas);
            }
            break;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Atualizar
                SendMessage(hwndLogList, LB_RESETCONTENT, 0, 0);
                int totalLinhas = 0;
                char **logs = carregarLogs(&totalLinhas);
                if (logs) {
                    for (int i = 0; i < totalLinhas; i++) {
                        SendMessage(hwndLogList, LB_ADDSTRING, 0, (LPARAM)logs[i]);
                    }
                    liberarLogs(logs, totalLinhas);
                }
            } else if (LOWORD(wParam) == 2) { // Limpar Logs
                if (MessageBoxW(hwnd, L"Tem certeza que deseja limpar todos os logs?", L"Confirmar", MB_YESNO) == IDYES) {
                    FILE *f = fopen("log.txt", "w");
                    if (f) fclose(f);
                    SendMessage(hwndLogList, LB_RESETCONTENT, 0, 0);
                    MessageBoxW(hwnd, L"Logs limpos com sucesso!", L"Info", MB_OK);
                }
            } else if (LOWORD(wParam) == 3) { // Fechar
                DestroyWindow(hwnd);
            }
            break;

        case WM_DESTROY:
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
// ---------------------- PAINEL ADMIN ----------------------
LRESULT CALLBACK AdminProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindowW(L"STATIC", L"Usuários Cadastrados:", WS_VISIBLE | WS_CHILD, 10, 10, 200, 20, hwnd, NULL, NULL, NULL);
            hwndAdminList = CreateWindowW(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                         10, 30, 560, 150, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"STATIC", L"Nome:", WS_VISIBLE | WS_CHILD, 10, 200, 50, 20, hwnd, NULL, NULL, NULL);
            hwndNome = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 70, 200, 150, 20, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"STATIC", L"Senha:", WS_VISIBLE | WS_CHILD, 240, 200, 50, 20, hwnd, NULL, NULL, NULL);
            hwndSenha = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 300, 200, 150, 20, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"STATIC", L"Nível:", WS_VISIBLE | WS_CHILD, 10, 230, 50, 20, hwnd, NULL, NULL, NULL);
            hwndNivel = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 70, 230, 150, 20, hwnd, NULL, NULL, NULL);

            hwndRFIDCheck = CreateWindowW(L"BUTTON", L"Possui CHAVE?", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                                         240, 230, 150, 20, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"BUTTON", L"Cadastrar", WS_VISIBLE | WS_CHILD, 10, 270, 100, 30,
                         hwnd, (HMENU)1, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Alterar Senha", WS_VISIBLE | WS_CHILD, 130, 270, 100, 30,
                         hwnd, (HMENU)2, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Remover", WS_VISIBLE | WS_CHILD, 260, 270, 100, 30,
                         hwnd, (HMENU)4, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Limpar", WS_VISIBLE | WS_CHILD, 380, 270, 80, 30,
                         hwnd, (HMENU)5, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Ver Logs", WS_VISIBLE | WS_CHILD, 480, 270, 90, 30,
                         hwnd, (HMENU)3, NULL, NULL);

            atualizarListaUsuarios();
            
            // Timer para processar logs a cada 500ms (2x por segundo - bem leve)
            SetTimer(hwnd, 1, 500, NULL);
            break;
        
        case WM_TIMER:
            if (wParam == 1) {
                processarLogsArduino();
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Cadastrar
                char nome[50], senha[50], nivel[20], uid[50] = "";
                GetWindowText(hwndNome, nome, sizeof(nome));
                GetWindowText(hwndSenha, senha, sizeof(senha));
                GetWindowText(hwndNivel, nivel, sizeof(nivel));

                // Validações
                if (strlen(nome) == 0 || strlen(senha) == 0 || strlen(nivel) == 0) {
                    MessageBoxW(hwnd, L"Preencha todos os campos!", L"Erro", MB_ICONERROR);
                    break;
                }

                if (totalUsuarios >= MAX_USUARIOS) {
                    MessageBoxW(hwnd, L"Limite de usuários atingido!", L"Erro", MB_ICONERROR);
                    break;
                }

                // Verificar se a senha já está em uso
                if (verificarSenhaEmUso(senha)) {
                    MessageBoxW(hwnd, L"Esta senha já está em uso por outro usuário!", L"Erro", MB_ICONERROR);
                    break;
                }

                if (SendMessage(hwndRFIDCheck, BM_GETCHECK, 0, 0)) {
                    // Envia comando pra ler RFID
                    enviarSerial("LER_RFID");
                    
                    // Aguarda até 10 segundos pelo RFID (de forma simples e síncrona)
                    char *rfid = lerRFIDSimples(10);
                    
                    if (rfid != NULL && strlen(rfid) > 0) {
                        strcpy(uid, rfid);
                        
                        // Verificar se o UID já está em uso
                        if (verificarUIDEmUso(uid)) {
                            MessageBoxW(hwnd, L"Este RFID já está cadastrado para outro usuário!", L"Erro", MB_ICONERROR);
                            break;
                        }
                        
                        enviarCadastroUsuarioRFID(nome, senha, nivel, uid);
                    } else {
                        MessageBoxW(hwnd, L"Timeout! RFID não lido. Tente novamente.", L"Erro", MB_ICONERROR);
                        break;
                    }
                } else {
                    enviarCadastroUsuario(nome, senha, nivel);
                }

                strcpy(usuarios[totalUsuarios].nome, nome);
                strcpy(usuarios[totalUsuarios].senha, senha);
                strcpy(usuarios[totalUsuarios].nivel, nivel);
                strcpy(usuarios[totalUsuarios].uid, uid);
                totalUsuarios++;
                salvarUsuariosEmArquivo();
                registrarLog("Usuário cadastrado");
                MessageBoxW(hwnd, L"Usuário cadastrado com sucesso!", L"Info", MB_OK);
                limparCampos();
                atualizarListaUsuarios();
            } else if (LOWORD(wParam) == 2) { // Alterar senha
                char nome[50], novaSenha[50];
                GetWindowText(hwndNome, nome, sizeof(nome));
                GetWindowText(hwndSenha, novaSenha, sizeof(novaSenha));
                
                if (strlen(nome) == 0 || strlen(novaSenha) == 0) {
                    MessageBoxW(hwnd, L"Preencha o nome e a nova senha!", L"Erro", MB_ICONERROR);
                    break;
                }
                
                // Verificar se a nova senha já está em uso por outro usuário
                if (verificarSenhaEmUsoExceto(novaSenha, nome)) {
                    MessageBoxW(hwnd, L"Esta senha já está em uso por outro usuário!", L"Erro", MB_ICONERROR);
                    break;
                }
                
                if (alterarSenha(nome, novaSenha)) {
                    salvarUsuariosEmArquivo();
                    registrarLog("Senha alterada via GUI");
                    MessageBoxW(hwnd, L"Senha alterada com sucesso!", L"Info", MB_OK);
                    limparCampos();
                    atualizarListaUsuarios();
                } else {
                    MessageBoxW(hwnd, L"Usuário não encontrado!", L"Erro", MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 3) { // Ver Logs
                HWND hwndLog = CreateWindowExW(0, L"LogWindow", L"Visualizador de Logs",
                                              WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                              700, 500, NULL, NULL, GetModuleHandle(NULL), NULL);
                ShowWindow(hwndLog, SW_SHOW);
            } else if (LOWORD(wParam) == 4) { // Remover usuário
                char nome[50];
                GetWindowText(hwndNome, nome, sizeof(nome));
                
                if (strlen(nome) == 0) {
                    MessageBoxW(hwnd, L"Digite o nome do usuário a ser removido!", L"Erro", MB_ICONERROR);
                    break;
                }
                
                wchar_t mensagem[200];
                MultiByteToWideChar(CP_ACP, 0, nome, -1, mensagem + 16, 100);
                wcscpy(mensagem, L"Remover usuário ");
                wcscat(mensagem, L"?");
                
                if (MessageBoxW(hwnd, mensagem, L"Confirmar", MB_YESNO) == IDYES) {
                    if (removerUsuario(nome)) {
                        MessageBoxW(hwnd, L"Usuário removido com sucesso!", L"Info", MB_OK);
                        limparCampos();
                        atualizarListaUsuarios();
                    } else {
                        MessageBoxW(hwnd, L"Usuário não encontrado!", L"Erro", MB_ICONERROR);
                    }
                }
            } else if (LOWORD(wParam) == 5) { // Limpar campos
                limparCampos();
            } else if (HIWORD(wParam) == LBN_DBLCLK && (HWND)lParam == hwndAdminList) {
                // Duplo clique na lista - preencher campos
                int index = SendMessage(hwndAdminList, LB_GETCURSEL, 0, 0);
                if (index >= 0 && index < totalUsuarios) {
                    preencherCamposUsuario(index);
                }
            }
            break;

        case WM_DESTROY:
            KillTimer(hwnd, 1); // Para o timer ao fechar
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
