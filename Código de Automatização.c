#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---- Struct ---- //
typedef struct {
    int id;
    char nome[50];
    int tipo;      // 1 = funcionário | 2 = admin
    float salario;
} Usuario;

typedef struct {
    int idUsuario;
    int entrada;
    int saida;
    float horasExtras;
    float valorPago;
} RegistroHoras;

// ---- FUNÇÕES ---- //
void menuPrincipal();
void registrarUsuario();
void listarUsuarios();
Usuario login();
Usuario buscarUsuarioPorID(int id);

void menuFuncionario(Usuario user);
void registrarHoras(Usuario user);

void menuAdmin(Usuario admin);
void listarPendentes();
void aprovarHoras();
void gerarRelatorio();

// ---- MAIN ---- //
int main() {
    menuPrincipal();
    return 0;
}

// ---- MENU PRINCIPAL ---- //
void menuPrincipal() {
    int opcao;

    while (1) {
        printf("\n=== SISTEMA DE HORAS EXTRAS ===\n");
        printf("1. Login\n");
        printf("2. Registrar Usuario\n");
        printf("3. Lista de Usuarios Registrados\n");
        printf("4. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
               // procura o id e redireciona se é adm ou funcionario para seu respectivo menu
                Usuario u = login(); 
                if (u.id != -1) {
                    if (u.tipo == 1) {
                        menuFuncionario(u);
                    } else if (u.tipo == 2) {
                        menuAdmin(u);
                    } else {
                        printf("Tipo de usuario invalido.\n");
                    }
                }
                break;
            }
            case 2:
                registrarUsuario();
                break;

            case 3:
                listarUsuarios();
                break;

            case 4:
                printf("Saindo...\n");
                exit(0);

            default:
                printf("Opcao invalida!\n");
        }
    }
}

// ---- REGISTRAR USUÁRIO ---- //
void registrarUsuario() {
    Usuario u;

    printf("\n--- Registro de Usuário ---\n");
    printf("Digite o ID: ");
    scanf("%d", &u.id);

    printf("Digite o nome: ");
    scanf(" %[^\n]", u.nome);

    printf("Tipo (1=Funcionario / 2=Admin): ");
    scanf("%d", &u.tipo);

    printf("Salario (valor da hora): ");
    scanf("%f", &u.salario);

    FILE *f = fopen("usuarios.txt", "a");
    if (f == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    fprintf(f, "%d %s %d %.2f\n", u.id, u.nome, u.tipo, u.salario);
    fclose(f);

    printf("Usuario registrado com sucesso!\n");
}

// ---- LISTAR USUÁRIOS ---- //
void listarUsuarios() {
    FILE *f = fopen("usuarios.txt", "r");
    if (f == NULL) {
        printf("Nenhum usuario registrado.\n");
        return;
    }

    Usuario u;
    printf("\n--- Lista de Usuarios ---\n");

    while (fscanf(f, "%d %49s %d %f", &u.id, u.nome, &u.tipo, &u.salario) == 4) {
        printf("ID: %d | Nome: %s | Tipo: %d | Salario por hora: %.2f\n",
               u.id, u.nome, u.tipo, u.salario);
    }

    fclose(f);
}

// ---- LOGIN ---- //
Usuario login() {
    int id;
    printf("\nDigite seu ID: ");
    scanf("%d", &id);

    return buscarUsuarioPorID(id);
}

// ---- BUSCAR USUÁRIO NO ARQUIVO ---- //
Usuario buscarUsuarioPorID(int id) {
    FILE *f = fopen("usuarios.txt", "r");
    Usuario u;
    u.id = -1;

    if (!f) {
        printf("Erro ao abrir o arquivo de usuarios.\n");
        return u;
    }

    while (fscanf(f, "%d %49s %d %f", &u.id, u.nome, &u.tipo, &u.salario) == 4) {
        if (u.id == id) {
            fclose(f);
            return u;
        }
    }

    fclose(f);
    printf("Usuario nao encontrado!\n");
    u.id = -1;
    return u;
}

// ---- MENU DO FUNCIONÁRIO ---- //
void menuFuncionario(Usuario user) {
    int opc;

    while (1) {
        printf("\n=== MENU FUNCIONARIO (%s) ===\n", user.nome);
        printf("1. Registrar Horas Extras\n");
        printf("2. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);

        switch (opc) {
            case 1:
                registrarHoras(user);
                break;

            case 2:
                return;

            default:
                printf("Opcao invalida!\n");
        }
    }
}

// ---- REGISTRAR HORAS ---- //
void registrarHoras(Usuario user) {
    RegistroHoras r;

    printf("\n--- Registrar Horas Extras ---\n");

    printf("Digite horario de entrada (somente hora inteira): ");
    scanf("%d", &r.entrada);

    printf("Digite horario de saida (somente hora inteira): ");
    scanf("%d", &r.saida);

    int tempo = r.saida - r.entrada;

    // Proibido jornadas > 10h
    if (tempo > 10) {
        printf("\nERRO: A jornada nao pode ultrapassar 10 horas.\n");
        return;
    }
    // Calculo das horas extras
    r.horasExtras = tempo - 8;


    if (r.horasExtras < 0) {
        printf("\nERRO: Horario invalido.\n");
        return;
    }

    // Calculo do valor das horas (jornada padrao de 200h e pagamento de 50% de bônus obrigatório)
    r.valorPago = r.horasExtras * (user.salario * 1.5 / 200);
    r.idUsuario = user.id;

    // Registro do pedido pendente
    FILE *fp = fopen("horas_pendentes.txt", "a");
    if (!fp) {
        printf("Erro ao abrir arquivo de pendencias!\n");
        return;
    }

    fprintf(fp, "%d %d %d %.2f %.2f\n",
            r.idUsuario, r.entrada, r.saida, r.horasExtras, r.valorPago);

    fclose(fp);

    printf("\nPedido enviado para aprovacao do administrador.\n");
    printf("Horas extras: %.2f\n", r.horasExtras);
}

// ---- MENU DO ADMINISTRADOR ---- //
void menuAdmin(Usuario admin) {
    int opc;

    while (1) {
        printf("\n=== MENU ADMIN (%s) ===\n", admin.nome);
        printf("1. Ver pedidos pendentes\n");
        printf("2. Aprovar / Rejeitar horas\n");
        printf("3. Gerar relatorio de pagamentos\n");
        printf("4. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);

        switch (opc) {
            case 1:
                listarPendentes();
                break;
            case 2:
                aprovarHoras();
                break;
            case 3:
                gerarRelatorio();
                break;
            case 4:
                return;
            default:
                printf("Opcao invalida!\n");
        }
    }
}

// ---- LISTAR HORAS PENDENTES ---- //
void listarPendentes() {
    FILE *f = fopen("horas_pendentes.txt", "r");
    if (!f) {
        printf("Nenhuma hora pendente encontrada.\n");
        return;
    }

    RegistroHoras r;
    printf("\n--- Pedidos Pendentes ---\n");

    while (fscanf(f, "%d %d %d %f %f",
                  &r.idUsuario, &r.entrada, &r.saida,
                  &r.horasExtras, &r.valorPago) == 5) {

        printf("Usuario %d | Entrada: %d | Saida: %d | Extras: %.2f | Valor: %.2f\n",
               r.idUsuario, r.entrada, r.saida, r.horasExtras, r.valorPago);
    }

    fclose(f);
}

// ---- APROVAR OU REJEITAR HORAS ---- //
void aprovarHoras() {
    int id;
    printf("\nDigite o ID do usuario para aprovar/rejeitar: ");
    scanf("%d", &id);

    FILE *f = fopen("horas_pendentes.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!f || !temp) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    int escolha;
    RegistroHoras r;
    int encontrado = 0;

    while (fscanf(f, "%d %d %d %f %f",
                  &r.idUsuario, &r.entrada, &r.saida,
                  &r.horasExtras, &r.valorPago) == 5) {

        if (r.idUsuario == id) {
            encontrado = 1;

            printf("\nPedido encontrado!\n");
            printf("Entrada: %d | Saida: %d | Extras: %.2f | Valor: %.2f\n",
                   r.entrada, r.saida, r.horasExtras, r.valorPago);

            printf("\n1 = Aprovar\n2 = Rejeitar\nEscolha: ");
            scanf("%d", &escolha);

            if (escolha == 1) {
                FILE *ap = fopen("horas.txt", "a");
                fprintf(ap, "%d %d %d %.2f %.2f\n",
                        r.idUsuario, r.entrada, r.saida,
                        r.horasExtras, r.valorPago);
                fclose(ap);
                printf("Hora extra APROVADA.\n");
            } else {
                printf("Hora extra REJEITADA.\n");
            }

        } else {
            fprintf(temp, "%d %d %d %.2f %.2f\n",
                    r.idUsuario, r.entrada, r.saida,
                    r.horasExtras, r.valorPago);
        }
    }

    fclose(f);
    fclose(temp);

    remove("horas_pendentes.txt");
    rename("temp.txt", "horas_pendentes.txt");

    if (!encontrado)
        printf("Nenhum pedido encontrado para esse usuario.\n");
}
// ---- RELATORIO DE HORAS APROVADAS ---- //
void gerarRelatorio() {
    FILE *f = fopen("horas.txt", "r");
    if (!f) {
        printf("\nNenhuma hora aprovada encontrada.\n");
        return;
    }

    typedef struct {
        int id;
        float totalHoras;
        float totalValor;
    } Relatorio;

    Relatorio lista[200];
    int count = 0;

    RegistroHoras r;
    int existe;

    while (fscanf(f, "%d %d %d %f %f",
                  &r.idUsuario, &r.entrada, &r.saida,
                  &r.horasExtras, &r.valorPago) == 5) {

        existe = 0;

        for (int i = 0; i < count; i++) {
            if (lista[i].id == r.idUsuario) {
                lista[i].totalHoras += r.horasExtras;
                lista[i].totalValor += r.valorPago;
                existe = 1;
                break;
            }
        }

        if (!existe) {
            lista[count].id = r.idUsuario;
            lista[count].totalHoras = r.horasExtras;
            lista[count].totalValor = r.valorPago;
            count++;
        }
    }

    fclose(f);

    printf("\n=== RELATORIO FINAL ===\n");

    for (int i = 0; i < count; i++) {
        printf("\nID: %d\n", lista[i].id);
        printf("Total de horas extras: %.2f\n", lista[i].totalHoras);
        printf("Total a pagar: R$ %.2f\n", lista[i].totalValor);
        printf("---------------------------\n");
    }
}