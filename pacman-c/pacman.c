#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pacman.h"
#include "mapa.h"
#include "ui.h"

MAPA m;
POSICAO heroi;
int temPilula = 0;

int direcaoFantasmas(int xAtual, int yAtual, int* xDestino, int* yDestino){
    int opcoes[4][2] = {
        {xAtual, yAtual-1},
        {xAtual, yAtual+1},
        {xAtual-1, yAtual},
        {xAtual+1, yAtual}
    };

    srand(time(0));

   for (int i = 0; i < 10; i++){
       int posicao = rand() % 4;

       if (podeAndar(&m, FANTASMA, opcoes[posicao][0], opcoes[posicao][1])){ 
            *xDestino = opcoes[posicao][0];
            *yDestino = opcoes[posicao][1];
            return 1;
       }
   } 
   return 0;
}

void fantasmas(){
    MAPA copia;
    
    copiaMapa(&copia, &m);

    for (int i = 0; i < m.linhas; i++){
        for (int j = 0; j < m.colunas; j++){
            if (copia.matriz[i][j] == FANTASMA){
                int xDestino, yDestino;
                int encontrou = direcaoFantasmas(i, j, &xDestino, &yDestino);

                if (encontrou)
                    andaNoMapa(&m, i, j, xDestino, yDestino);
            }
        }
    }

    liberaMapa(&copia);
}

int acabou(){
    POSICAO posi;
    int fogefogeNoMapa = encontraMapa(&m, &posi, HEROI);
    return !fogefogeNoMapa;
}

int ehDirecao(char direcao){
    if (direcao == ESQUERDA 
    || direcao == CIMA
    || direcao == BAIXO
    || direcao == DIREITA)
        return 0;
    return 1;
}

void move(char direcao){

    if (ehDirecao(direcao))
        return;

    int proximoX = heroi.x;
    int proximoY = heroi.y;

    switch(direcao){
        case ESQUERDA:
            proximoY--;
            break;
        case CIMA:
            proximoX--;
            break;
        case BAIXO:
            proximoX++;
            break;
        case DIREITA:
            proximoY++;
            break;
    }

    if (!podeAndar(&m, HEROI, proximoX, proximoY))
        return;

    if (ehPersonagem(&m, PILULA, proximoX, proximoY))
        temPilula = 1;

    andaNoMapa(&m, heroi.x, heroi.y, proximoX, proximoY);
    heroi.x = proximoX;
    heroi.y = proximoY;

}

void explodePilula(){
    if (!temPilula)
        return;

    explodePilula2(heroi.x, heroi.y, 0, 1, 3);
    explodePilula2(heroi.x, heroi.y, 0, -1, 3);
    explodePilula2(heroi.x, heroi.y, 1, 0, 3);
    explodePilula2(heroi.x, heroi.y, -1, 0, 3);

    temPilula = 0;
}

void explodePilula2(int x, int y, int somaX, int somaY, int qtd){
    if (qtd == 0)
       return;

    int novoX = x + somaX;
    int novoY = y + somaY;

    if (!ehValida(&m, novoX, novoY))
        return;

    if (ehParede(&m, novoX, novoY))
        return;

    m.matriz[novoX][novoY] = VAZIO;
    explodePilula2(novoX, novoY, somaX, somaY, qtd-1);
}

int main(){

    leMapa(&m);
    encontraMapa(&m, &heroi, HEROI);
    
    do{
        imprimeMapa(&m);
        char comando;
        scanf(" %c", &comando);
        move(comando);
        if (comando == BOMBA) 
            explodePilula();
        fantasmas();
    } while(!acabou());

    liberaMapa(&m);
}
