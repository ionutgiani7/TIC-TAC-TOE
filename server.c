#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
typedef struct Game
{
    char board[3][3];
    char currentPlayer;
    char player1;
    char player2;
    int connfd1;
    int connfd2;
    struct sockaddr_in client;
} Game;
void showBoard(Game *g)
{
    char buffer[10] = {0};
    strcpy(buffer, "|-----|\n\0");
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            sprintf(buffer, "|%c", g->board[i][j]);
            write(g->connfd1, buffer, strlen(buffer));
            write(g->connfd2, buffer, strlen(buffer));
        }
        strcpy(buffer, "|-----|\n\0");
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
    }
    strcpy(buffer, "|-----|\n\0");
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));
}
void updateBoard(Game *g, int cell, char player)
{
    int row = (cell - 1) / 3;
    int col = (cell - 1) % 3;
    char buffer[256] = {"a"};
    if (g->board[row][col] == 'O' || g->board[row][col] == 'X')
    {
        if (g->currentPlayer == 'X')
        {
            strcpy(buffer, "Cell already ocupied select another one\n\0");
            write(g->connfd1, buffer, strlen(buffer));
        }
        else
        {
            strcpy(buffer, "Cell already ocupied select another one\n\0");
            write(g->connfd2, buffer, strlen(buffer));
        }
    }
    else
    {
        g->board[row][col] = player;
        if (g->currentPlayer == 'X')
        {
            g->currentPlayer = 'O';
        }
        else
            g->currentPlayer = 'X';
        showBoard(g);
    }
}
void makeMove(Game *g)
{
    char buffer[256] = {0};
    char input[256] = {0};
    int cell = 0;

    sprintf(buffer, "%c's turn\n", g->currentPlayer);
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));
    memset(buffer,0,sizeof(buffer));
    if (g->currentPlayer == 'X')
    {
        strcpy(buffer, "Waiting for Player X to make a move...\n");
        write(g->connfd2, buffer, strlen(buffer));
        memset(buffer,0,sizeof(buffer));
        while (1)
        {
            memset(input, 0, sizeof(input));
            read(g->connfd1, input, sizeof(input) - 1);
            cell = atoi(input);
            if (cell >= 1 && cell <= 9 && g->board[(cell - 1) / 3][(cell - 1) % 3] != 'X' && g->board[(cell - 1) / 3][(cell - 1) % 3] != 'O')
                break;
            strcpy(buffer, "Invalid input. Please select a valid cell (1-9):\n");
            write(g->connfd1, buffer, strlen(buffer));
            memset(buffer,0,sizeof(buffer));
        }
    }
    else
    {
        strcpy(buffer, "Waiting for Player O to make a move...\n");
        write(g->connfd1, buffer, strlen(buffer));
        memset(buffer,0,sizeof(buffer));
        while (1)   
        {
            memset(input, 0, sizeof(input));
            read(g->connfd2, input, sizeof(input) - 1);
            cell = atoi(input);
            if (cell >= 1 && cell <= 9 && g->board[(cell - 1) / 3][(cell - 1) % 3] == ' ')
                break;
            strcpy(buffer, "Invalid input. Please select a valid cell (1-9):\n");
            write(g->connfd2, buffer, strlen(buffer));
            memset(buffer,0,sizeof(buffer));
        }
    }

    // Update the board with the player's move
    updateBoard(g, cell, g->currentPlayer);
}

void resetboard(Game *g){
    char cellNumber = '1';
    int i;
    int j;
    for(i = 0 ; i < 3 ; i++){
        for(j = 0 ; j < 3 ; j++){
            g->board[i][j] = cellNumber;
            cellNumber++;
        }
    }
}

int checkDraw(Game *g){
    int i;
    int j;
    char buffer[1024] = {"a"};
    for(i = 0 ; i < 3 ; i++){
        for(j = 0 ; j < 3 ; j++){
            if(g->board[i][j] != 'X' && g->board[i][j] != 'O'){
                return 0;//mai sunt celule libere
            }
        }
    }
    sprintf(buffer,"Draw\n");
    write(g->connfd1,buffer,strlen(buffer));
    write(g->connfd2,buffer,strlen(buffer));
    return 1;//nu mai sunt celule libere
}

int checkWin(Game *g)
{
    char buffer[1024] = {"a"};
    for (int i = 0; i < 3; i++)
    {
        if (g->board[i][0] == g->board[i][1] && g->board[i][1] == g->board[i][2] && g->board[i][0] != ' ')
        {
            sprintf(buffer, "%c Won\n", g->board[i][0]);
            write(g->connfd1, buffer, strlen(buffer));
            write(g->connfd2, buffer, strlen(buffer));
            return 1;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (g->board[0][i] == g->board[1][i] && g->board[1][i] == g->board[2][i] && g->board[0][i] != ' ')
        {
            sprintf(buffer, "%c Won\n", g->board[i][0]);
            write(g->connfd1, buffer, strlen(buffer));
            write(g->connfd2, buffer, strlen(buffer));
            return 1;
        }
    }

    if (g->board[0][0] == g->board[1][1] && g->board[1][1] == g->board[2][2] && g->board[0][0] != ' ')
    {
        sprintf(buffer, "%c Won", g->board[0][0]);
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
        return 1;
    }

    if (g->board[0][2] == g->board[1][1] && g->board[1][1] == g->board[2][0] && g->board[0][2] != ' ')
    {
        sprintf(buffer, "%c Won", g->board[0][2]);
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
        return 1;
    }

    return 0;
}

void *initializeBoard(void *arg);//prototipul functiei, deoarece imi dadea eroare la compilarea functiei playAgain, functiile folosindu-se reciproc

void playAgain(Game *g){ //functia care permite ca jucatorii sa joace din nou
    char buffer[1024] = {"a"};
    while(0 == 0){
        strcpy(buffer,"Do you want to play again?\n");
        write(g->connfd1,buffer,strlen(buffer));
        write(g->connfd2,buffer,strlen(buffer));
        char ans1[10] = {0};
        char ans2[10] = {0};
        read(g->connfd1,ans1,sizeof(ans1) - 1);
        read(g->connfd2,ans2,sizeof(ans2) - 1);
        if(strncmp(ans1,"yes",3) == 0 && strncmp(ans2,"yes",3) == 0){
            resetboard(g);
            if(rand() % 2 == 0){
                g->currentPlayer = 'X';
            }
            else
                g->currentPlayer = 'O';
            initializeBoard((void *)g);
            return;
        }
        else{
            strcpy(buffer,"Game over!\n");
            write(g->connfd1,buffer,strlen(buffer));
            write(g->connfd2,buffer,strlen(buffer));
            close(g->connfd1);
            close(g->connfd2);
            free(g);
            pthread_exit(NULL);
        }
    }
}

void *initializeBoard(void *arg)
{
    Game *g = (Game *)arg;
    char cellNumber = '1';
    char buffer[1024] = {'a'};
    char buffer1[1024] = {"a"};
    srand(time(NULL));
    g->currentPlayer = rand() % 2 == 0 ? 'X' : 'O';
    strcpy(buffer, "You are X\n");
    write(g->connfd1, buffer, strlen(buffer));
    memset(buffer,0,sizeof(buffer));
    strcpy(buffer, "You are O\n");
    write(g->connfd2, buffer, strlen(buffer));
    if (g->currentPlayer == 'X')
    {
        strcpy(buffer, "X begins\n");
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
    }
    else
    {
        strcpy(buffer, "O begins\n");
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
    }
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer1, "|-----|\n");
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            g->board[i][j] = cellNumber;
            cellNumber++;
            sprintf(buffer, "|%c", g->board[i][j]);
            strcat(buffer1, buffer);
        }
        strcat(buffer1, "|\n");
    }
    strcat(buffer1, "|-----|\n\0");
    write(g->connfd1, buffer1, strlen(buffer1));
    write(g->connfd2, buffer1, strlen(buffer1));
    while (checkWin(g) != 1 && checkDraw(g) != 1)
    {
        makeMove(g);
    }
    playAgain(g);
    return NULL;
}

int main(int argc, char *argv[])
{
    int sockfd;
    int connfd1;
    int connfd2;
    struct sockaddr_in serveraddr;
    if (argc != 3)
    {
        printf("Usage:./server IP PORT\n");
        exit(0);
    }
    int port = atoi(argv[2]);
    char *ip = argv[1];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed\n");
        exit(0);
    }
    else
        printf("Socket creation succesfull\n");
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    serveraddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) != 0)
    {
        printf("socket bind failed\n");
        exit(1);
    }
    else
        printf("Socket successfully binded\n");
    if ((listen(sockfd, 10)) != 0)
    {
        printf("listen failed \n");
        exit(2);
    }
    printf("Server listening\n");
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        if ((connfd1 = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Error accepting client1");
            exit(3);
        }
        printf("Client1 connected\n");
        if ((connfd2 = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Error accepting client2");
            exit(3);
        }
        printf("Client2 connected\n");
        Game *g = malloc(sizeof(Game));
        g->client = client_addr;
        g->connfd1 = connfd1;
        g->connfd2 = connfd2;
        pthread_t thread;
        pthread_create(&thread, NULL, initializeBoard, (void *)g);
        pthread_detach(thread);
    }

    close(sockfd);
    return 0;
}