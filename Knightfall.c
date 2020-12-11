kk
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string.h>

//Esse é o que conta!!!

//gcc testesdl.c -lSDL2 -lSDL2_image -lSDL2_mixer -o sdl

const int BAIXO = 85;
const int CIMA = 175;
const int DIREITA = 180;
const int ESQUERDA = 90;
const int HORIZONTAL = 240;
const int VERTICAL = 0;

typedef struct
{
    int vida;
    SDL_Rect posicao_tamanho;
    SDL_Rect sprite_atual;
    SDL_Rect idle;
    SDL_Texture *textura;
    int velocidade;
    int sprite_gap;
    int direcao;
    int sentido;
    char *tipo;
    int x;
    int y;
    int pontuacao;
}PERSONAGEM;

typedef struct
{
    SDL_Rect cima;
    SDL_Rect baixo;
    SDL_Rect direita;
    SDL_Rect esquerda;
    SDL_Rect posicao_tamanho;
    int x;
    int y;
}FLECHA;

typedef struct//pra automatizar o processo de mudança do mapa
{
    int xinicial,yinicial,w,h,altura,largura,identificador,tamanhopasso;
    SDL_Texture* textureMapa;
} Mapa;

typedef struct //pra automatizar o processo de pontuação no final
{
int pontos;
char nome[20];
} PontuacaoJogador;

//funcao de carregamento de imagens
SDL_Texture* loadTexture(char *arquivo, SDL_Renderer *renderer);

//funcao de movimentacao da camera
void movimentarTela(SDL_Rect *mapa_frame, int valor, char coordenada);

//funcao de movimentacao do player
void movimentarPlayer(SDL_Rect *jogador_frame, int valor, char coordenada);

//funcao de colisao com mapa (matriz)
int movimentarMatriz(PERSONAGEM *LINK,char tipo,int valor, char coordenada, int alturamapa, int larguramapa, char mapa[alturamapa][larguramapa], SDL_Rect *player_frame, SDL_Rect *mapa_frame, Mapa *mapaatual, int tamanhopasso, int altura, int largura);

//funcao de leitura de pontos do arquivo e escrita
void lerpontos(SDL_Renderer* renderer)
{
    TTF_Font* Sans = TTF_OpenFont("fontes/The Wild Breath of Zelda.otf", 64); //this opens a font style and sets a size
    SDL_Color White = {255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
    SDL_Surface* surfaceNome;
    SDL_Surface* surfacePontos;
    SDL_Texture* textureNome;
    SDL_Texture* texturePontos;
    SDL_Rect Nome_Rect = {370, 130, 100, 50};
    SDL_Rect Pontos_Rect = {550, 130, 36, 36};

    int contlinhas = 0, contvetor = 0, lixoint;
    char lixo[35], pontosstring[4];
    PontuacaoJogador p1, aux;
    FILE *arquivo = fopen("pontuacao.txt","r+");
    while(!feof(arquivo))
    {
        fscanf(arquivo, "%[^;];%d\n", lixo, &lixoint);
        contlinhas++;
    }
    PontuacaoJogador vetor[contlinhas];
    rewind(arquivo);
    while(contvetor < contlinhas)
    {
        fscanf(arquivo, "%[^;];%d\n", p1.nome, &p1.pontos);
        vetor[contvetor] = p1;
        for(int x = 0; x < 35; x++)
        {
            p1.nome[x] = '\0';
        }
        contvetor++;
    }
    for(int x=0; x<contlinhas; x++)
    {
        for(int y = x+1; y < contlinhas; y++)
        {
            if(vetor[x].pontos < vetor[y].pontos)
            {
                aux = vetor[x];
                vetor[x] = vetor[y];
                vetor[y] = aux;
            }
        }
    }
    int x = 0;
    while(x < 5 && x < contlinhas)
    {
        surfaceNome = TTF_RenderText_Solid(Sans, vetor[x].nome, White);
        textureNome = SDL_CreateTextureFromSurface(renderer, surfaceNome);
        SDL_RenderCopy(renderer, textureNome, NULL, &Nome_Rect);
        Nome_Rect.y += 30;
        sprintf(pontosstring, "%i", vetor[x].pontos);
        surfacePontos = TTF_RenderText_Solid(Sans, pontosstring, White);
        texturePontos = SDL_CreateTextureFromSurface(renderer, surfacePontos);
        SDL_RenderCopy(renderer, texturePontos, NULL, &Pontos_Rect);
        Pontos_Rect.y += 30;
        SDL_FreeSurface(surfaceNome);
        SDL_DestroyTexture(textureNome);
        SDL_FreeSurface(surfacePontos);
        SDL_DestroyTexture(texturePontos);
        x++;
    }
    fclose(arquivo);
    TTF_CloseFont(Sans);
}

//funcao que salva pontos no arquivo de pontuacao
void salvar(char nome[30], int pontos)
{
    FILE* arquivo = fopen("pontuacao.txt", "a+");
    fprintf(arquivo, "%s;%d\n",nome,pontos);
    fclose(arquivo);
}



int movimentarInimigo(PERSONAGEM *p, int largura, int altura, char matriz[altura][largura], int direcao, int sentido)
{
    int l, c;
    for(int i = 0; i < altura; i++)
    {
        for(int j = 0; j < largura; j++)
        {
            if(i == p->y && j == p->x)
            {
                if(direcao == VERTICAL)
                {
                    if(sentido == BAIXO)
                    {
                        l = i+1;
                        c = j;
                    }else if(sentido == CIMA)
                    {
                        l = i-1;
                        c = j;
                    }
                }else if(direcao == HORIZONTAL)
                {
                    if(sentido == DIREITA)
                    {
                        l = i;
                        c = j+1;
                    }else if(sentido == ESQUERDA)
                    {
                        l = i;
                        c = j+1;
                    }
                }

                if(matriz[l][c] == '1')
                {
                    //move
                    matriz[i][j] = '1';
                    matriz[l][c] = 'i';
                    if(direcao == HORIZONTAL)
                        movimentarPlayer(&p->posicao_tamanho, c-j, 'x');
                    else
                        movimentarPlayer(&p->posicao_tamanho, l-j, 'y');
                    return 1;
                }else if(matriz[l][c] == 'b')
                {
                    //bloqueio
                    return 0;
                }
            }
        }
    }
}

//Cria um Rect baseado nos parametros
SDL_Rect desenhar(int w, int h, int x, int y)
{
    SDL_Rect rect;
    rect.h = h;
    rect.w = w;
    rect.x = x;
    rect.y = y;
    return rect;
}

//É o construtor da struct personagem
PERSONAGEM * criar(int vida, SDL_Rect posicao_tamanho, SDL_Rect sprite_atual, int velocidade, int gap, SDL_Texture *textura, char *tipo, int pontuacao)
{
    PERSONAGEM *p = (PERSONAGEM *) malloc(sizeof(PERSONAGEM));
    p->textura = textura;
    p->vida = vida;
    p->posicao_tamanho = posicao_tamanho;
    p->sprite_atual = sprite_atual;
    p->idle = sprite_atual;
    p->velocidade = velocidade;
    p->sprite_gap = gap;
    p->direcao = VERTICAL;
    p->sentido = BAIXO;
    p->tipo = tipo;
    p->pontuacao = pontuacao;
    return p;
}

//É o destruidor da struct.
void destruir(PERSONAGEM *p)
{
    //free(p->textura);
    free(p);
}

//Reposiciona o começo de onde o sprite tem que começar a ser lido. Por exemplo, o primeiro é em (120, 0), os próximos seguem uma função afim para os lados.
int posicionarSprite(int w, int gap, int direcao, int frame)
{
    int v = direcao + (gap+w)*frame;
    return v;
}

PERSONAGEM * golpear(PERSONAGEM *p, int direcao, int *cont, int sentido, int *golpe)
{
    int limite = 5;
    if(p->direcao == BAIXO)
        limite = 6;

    if(*cont == limite)
    {
        *cont = 0;
        *golpe = 0;
        p->sprite_atual = p->idle;
    }else
    {
        p->sprite_atual.x = posicionarSprite(p->sprite_atual.w, p->sprite_gap, direcao, *cont);
    }

    return p;
}

//Função que movimenta o LINK, recebe o sentido (horizontal ou vertical) e a direção (direita, esquerda, cima, baixo etc) além do ponteiro do contador de frames
PERSONAGEM * animarPersonagem(PERSONAGEM *p, int sentido, int direcao, int *x)
{
    int limite;

    if(p->tipo == "jogador")
    {
        if(direcao == HORIZONTAL)
            limite = 6;
        else if(direcao == VERTICAL)
            limite = 8;
    }else if(p->tipo == "elemento")
    {
        limite = 3;
    }

    if(*x == limite)
        *x = 0;

    p->sprite_atual.x = posicionarSprite(p->sprite_atual.w, p->sprite_gap, direcao, *x);

    return p;
}

PERSONAGEM * animarNPC(PERSONAGEM *p, int sentido, int direcao, int *x)
{
    int limite = 4;

    if(sentido == ESQUERDA)
        limite = 3;

    if(*x == limite)
        *x = 0;
    p->sprite_atual.y = posicionarSprite(p->sprite_atual.h, p->sprite_gap, direcao, *x);

    return p;
}


PERSONAGEM * atirar(PERSONAGEM *p, int *contador, int *tiro)
{
    int limite = 3;

    if(*contador == 3)
    {
        *contador = 0;
        *tiro = 0;
        p->sprite_atual = p->idle;
    }else
        p->sprite_atual.x = posicionarSprite(p->sprite_atual.w, p->sprite_gap, p->direcao, *contador);
    return p;
}

PERSONAGEM * posicaoJogador(PERSONAGEM *p, int largura, int altura, char matriz[altura][largura])
{
    int a, b;
    for(int i = 0; i < altura; i++)
    {
        for(int j = 0; j < largura; j++)
        {
            if(matriz[i][j] == 'p')
            {
                a = i;
                b = j;
            }
        }
    }

    p->y = a;
    p->x = b;

    return p;
}

//Move o ponto (x,y) para o começo dos frames do sprite e zera a contagem de frames.
void parar(PERSONAGEM *p, SDL_Rect comeco, int *cont)
{
    *cont = 0;
    p->sprite_atual = comeco;
    p->idle = comeco;
}

void deletarJogador(PERSONAGEM *p, int largura, int altura, char matriz[altura][largura])
{
    matriz[p->y][p->x] = '1';
}

void posicionarInimigo(int i, int j, int largura, int altura, char matriz[altura][largura])
{
    matriz[i][j] = 'X';
}

void golpearMatriz(PERSONAGEM *p, int sentido, int largura, int altura, char matriz[largura][altura], PERSONAGEM *inimigos[3])
{
    int i, j;
    if(sentido == BAIXO)
    {
        i = p->y+1;
        j = p->x;
    }else if(sentido == CIMA)
    {
        i = p->y-1;
        j = p->x;
    }else if(sentido == DIREITA)
    {
        i = p->y;
        j = p->x+1;
    }else if(sentido == ESQUERDA)
    {
        i = p->y;
        j = p->x-1;
    }

    //printf("\n NOVA MATRIZ: \n");
    /*for(int x = 0; x < altura; x++)
    {
        for(int y = 0; y < largura; y++)
            printf("%c ", matriz[x][y]);
        printf("\n");
    }

    //printf("%d %d = %c\n", i, j, matriz[i][j]);*/

    if(matriz[i][j] == 'X')
    {
        for(int x = 0; x < 3; x++)
        {
            if(inimigos[x]->x == j && inimigos[x]->y == i)
            {
                inimigos[x]->vida -= 20;
                if(inimigos[x]->vida <= 0)
                {
                    matriz[i][j]='1';
                    p->pontuacao += inimigos[x]->pontuacao;
                    //printf("PONTUAÇÂO GERAL: %d", p->pontuacao);
                }

            }
        }
    }
}


FLECHA * criar_flecha(PERSONAGEM *p)
{
    FLECHA *f = (FLECHA *) malloc(sizeof(FLECHA));
    f->baixo = desenhar(16, 48, 0, 48);
    f->cima = desenhar(16, 48, 0, 0);
    f->direita = desenhar(48,16, 50, 16);
    f->esquerda = desenhar(48,16, 50, 66);
    f->posicao_tamanho = desenhar(32, 32, p->posicao_tamanho.x, p->posicao_tamanho.y);
    f->x = p->x;
    f->y = p->y;
    return f;
}

void destruir_flecha(FLECHA *f)
{
    free(f);
}



int main(int argc, char**argv)
{
    int jogando = 1, largura = 640, altura = 480, movimentacao, nmapa = 1, pontos = 0;
    char tela = 'i';
    char text[30] = "";
    char *composition;
    Sint32 cursor;
    Sint32 selection_len;

    SDL_Init(SDL_INIT_EVERYTHING);
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
    TTF_Init();
    SDL_bool done = SDL_FALSE;
    SDL_Window* janela = SDL_CreateWindow("Jogo", 200, 200, largura, altura, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(janela, -1, 0);
    SDL_Surface* textoponto;
    SDL_Texture* textureponto;
    SDL_Texture* textureFundo1 = loadTexture("mapas/mapa_mundo_gelo.jpg", renderer);
    SDL_Texture* textureFundo2 = loadTexture("mapas/mapa_mundo_fogo.jpg", renderer);
    SDL_Texture* textureFundo3 = loadTexture("mapas/mapa_mundo_floresta.jpg", renderer);
    SDL_Texture* textureCastelo = loadTexture("mapas/mapa_castelo_interior.jpg", renderer);
    SDL_Texture* texturePlayer = loadTexture("sprites/link3.png",renderer);
    SDL_Texture* textureInicio = loadTexture("menus/menu_jogo_sdl.jpg",renderer);
    SDL_Texture* textureRanking = loadTexture("menus/New_RANKING_jogo_sdl.jpg", renderer);
    SDL_Texture* textureSobre = loadTexture("menus/SOBRE_jogo_sdl.jpg", renderer);
    SDL_Texture* textureComoJogar = loadTexture("menus/comojogar_jogo_sdl.jpg", renderer);
    SDL_Texture* textureFim = loadTexture("menus/winner_jogo_sdl.jpeg", renderer);
    SDL_Texture* textureCastelo1 = loadTexture("mapas/mapa_castelo_porta1.png", renderer);
    SDL_Texture* textureCastelo2 = loadTexture("mapas/mapa_castelo_porta2.png", renderer);
    SDL_Texture* textureCastelo3 = loadTexture("mapas/mapa_castelo_porta3.png", renderer);
    SDL_Texture* textureLore1 = loadTexture("menus/cena1_jogo_sdl.jpg", renderer);
    SDL_Texture* textureLore2 = loadTexture("menus/cena2_jogo_sdl.jpg", renderer);
    SDL_Texture* textureLore3 = loadTexture("menus/cena3_jogo_sdl.jpg", renderer);
    SDL_Texture* textureLore4 = loadTexture("menus/cena4_jogo_sdl.jpg", renderer);
    SDL_Texture* textureOver = loadTexture("menus/gameover_jogo_sdl.jpg", renderer);
    Mix_Music* musicacastelo = Mix_LoadMUS( "musicas/Musica_Mundo_Castelo.mp3" );
    Mix_Music* musicafogo = Mix_LoadMUS( "musicas/Musica_Mundo_Fogo.mp3" );
    Mix_Music* musicagelo = Mix_LoadMUS( "musicas/Musica_Mundo_Gelo.mp3" );
    Mix_Music* musicafloresta = Mix_LoadMUS( "musicas/Musica_Mundo_Floresta.mp3" );
    Mix_Music* musicavila = Mix_LoadMUS( "musicas/Musica_Mundo_Vila_Nova.mp3" );
    Mix_Music* musicamenus = Mix_LoadMUS("musicas/Musica_Menus.mp3");
    Mix_Music* musicaatual;
    TTF_Font* Sans = TTF_OpenFont("fontes/sans.ttf", 72); //this opens a font style and sets a size
    SDL_Color White = {255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color


   char matmapa1[30][30] = {
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','c','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','b','1','b','1','1','b','b','1','1','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','b','1','b','1','b','b','b','b','b','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','b','1','b','1','b','b','b','b','b','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','b','1','1','1','1','1','b','1','1','1','1','b','b','1','b','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','b','b','b','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','b','b','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b'},
{'b','1','1','b','b','1','1','1','b','b','b','b','b','b','1','1','b','b','1','1','b','1','1','1','1','1','1','1','1','b'},
{'b','b','b','1','b','b','1','1','b','1','1','1','1','b','1','1','1','1','1','1','1','1','1','b','b','1','1','1','1','b'},
{'b','1','1','b','1','b','b','1','b','1','1','1','1','b','1','1','1','1','1','1','1','1','b','b','b','b','1','1','1','b'},
{'b','1','1','b','b','b','b','1','b','1','1','1','1','b','1','1','1','1','1','b','1','1','b','b','b','b','1','1','1','b'},
{'b','b','b','1','b','b','1','1','b','1','1','1','1','b','1','1','1','1','1','1','1','1','1','b','b','1','1','1','1','b'},
{'b','b','b','1','1','1','1','1','b','b','b','b','b','b','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','b','b','b','b','b','b','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','b','b','1','1','1','b','b','b'},
{'b','1','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','b','b','b','b','b','b','b','b'},
{'b','1','1','1','1','b','b','1','1','1','1','1','1','b','1','1','1','1','b','1','1','1','b','b','b','b','b','b','b','b'},
{'b','1','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','b','b','b','b','b','b','b','b'},
{'b','1','1','1','1','b','b','1','1','b','1','1','1','1','1','1','b','1','1','1','1','1','b','b','b','b','b','b','b','b'},
{'b','1','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b','b','b','b'},
{'b','1','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b'},
{'b','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','b','b','1','b','b','1','b','b','1','b','b','1','X','X','b','b','1','1','b','b','1','b','b','1','b','b','1','1','b'},
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','b','1','b','b','b','b','b','b','b','b','b','b','1','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','b','b','1','X','b','b','1','1','1','1','1','1','1','1','1','b','1','b'},
{'b','1','1','1','1','1','1','1','1','1','1','b','1','1','p','1','b','b','1','1','1','1','1','1','1','1','b','1','1','b'},
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','1','1','1','1','1','1','1','1','1','b','b','b'}};

char matmapa2[30][30] = {
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','c','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'},
{'b','1','1','1','1','1','1','1','b','b','b','b','1','b','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','b','1','1','1','1','1','b','b','b','b','b','b','b','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','1','1','1','1','b'},
{'b','1','1','1','1','1','1','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b'},
{'b','1','1','1','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','1','1','1','b'},
{'b','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','1','b'},
{'b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','b','b','b','1','1','1','b','b','b','b','1','1','1','b','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','1','1','b','1','1','1','b','1','1','b','1','1','1','b','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','1','1','1','b','1','b','1','1','1','b','1','1','1','b','1','1','1','1','1','b'},
{'b','b','1','1','1','1','1','1','1','b','b','b','b','b','1','b','b','b','b','b','1','1','1','b','b','b','b','b','b','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','1','1','1','1','1','1','1','1','1','b','b','b','b','1','b','b','b','b','1','1','1','1','b','1','1','1','1','1','b'},
{'b','b','b','1','1','1','1','1','1','1','b','b','b','b','1','b','b','b','b','1','1','1','1','b','1','1','1','1','1','b'},
{'b','1','b','1','1','1','1','1','1','1','b','1','1','b','1','b','1','1','b','1','1','1','1','b','1','1','1','1','1','b'},
{'b','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b','b','b'},
{'b','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','b'},
{'b','1','1','b','b','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b'},
{'b','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b'},
{'b','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b'},
{'b','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','1','X','1','1','X','1','1','1','1','1','1','1','1','b','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','b','1','1','1','1','1','X','1','1','1','1','1','b','b','b','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','1','b','1','1','b','1','b','1','1','b','b','b','b','1','1','1','1','1','1','1','b'},
{'b','1','1','1','1','1','1','1','1','1','b','1','1','b','p','b','1','1','b','b','b','1','1','1','1','1','1','1','1','b'},
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'}};

char matmapa3[30][30] = {
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','c','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','1','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','1','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','b','b','b','b','b','b'},
{'b','1','1','b','1','b','1','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','b','b','b','b','b','b'},
{'b','1','1','1','1','1','1','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','b','b','b','b','b','b'},
{'b','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','1','1','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','1','b','b','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','1','b','1','b','b','1','b','1','b','1','b','1','b','b','b','b','b','b','b','b','1','1','1','1','b','1','1','1','b'},
{'b','b','b','1','b','1','b','1','1','b','1','b','1','b','b','b','b','b','b','b','b','b','1','1','1','b','1','1','1','b'},
{'b','b','b','1','1','1','1','1','1','1','1','1','1','b','1','1','1','1','1','1','1','b','1','1','1','1','1','1','b','b'},
{'b','1','1','1','1','1','1','1','1','1','b','1','1','b','b','b','b','b','b','b','b','1','1','1','1','1','1','1','1','b'},
{'b','1','1','b','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','1','1','1','b','b','b'},
{'b','1','1','b','1','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b'},
{'b','b','1','1','b','b','b','b','b','1','1','b','1','1','1','b','b','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','1','1','b','1','b','b','1','b','1','b','1','1','b','b','b','b','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','b','1','1','1','b','b','b','1','b','b','1','1','1','1','b','b','1','1','1','1','1','1','1','b','b','1','b','b','b'},
{'b','b','1','1','1','b','b','b','b','1','1','b','1','1','b','1','1','b','1','1','1','b','b','1','b','b','b','b','b','b'},
{'b','1','1','1','1','1','b','b','b','1','1','b','1','1','b','1','b','b','1','b','1','b','b','1','1','1','b','b','b','b'},
{'b','1','1','1','1','1','b','b','b','b','b','1','1','1','1','1','1','1','1','b','1','b','1','1','1','1','1','b','b','b'},
{'b','1','1','1','1','1','b','b','b','b','b','b','1','1','1','1','1','1','1','1','1','b','b','1','1','1','1','1','1','b'},
{'b','b','1','1','1','1','b','b','b','b','b','b','1','1','1','b','b','1','1','1','1','b','b','1','1','1','1','1','1','b'},
{'b','b','1','1','1','1','b','b','1','b','b','b','1','1','b','b','b','b','1','1','1','1','1','1','1','1','1','1','1','b'},
{'b','b','1','1','1','b','1','1','1','1','b','b','1','1','b','b','b','b','1','1','1','1','b','1','b','b','b','1','1','b'},
{'b','b','b','b','X','1','1','X','1','1','X','1','1','1','b','b','b','b','1','b','1','1','1','1','b','b','b','b','b','b'},
{'b','b','b','b','b','1','1','1','1','1','1','b','1','1','b','b','b','b','b','b','b','b','1','1','1','1','1','b','b','b'},
{'b','b','b','b','b','b','b','1','b','1','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','1','b','b','b'},
{'b','b','b','b','b','b','b','p','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','1','b','b','b'},
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'}};

char matcastelo[20][20] = {
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','b','b','f','f','b','b','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','b','1','p','1','1','b','b','b','b','b','b','b','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','b','b','b','b','b','b','1','1','1','1','1','1','b','b','b','b','b','b','b'},
{'b','b','1','1','1','1','b','1','1','1','1','1','1','b','1','1','1','1','b','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','1','b','b','1','1','1','1','1','1','1','1','1','1','b','b','1','b','b'},
{'b','b','1','b','b','1','1','1','1','1','1','1','1','1','1','b','b','1','b','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','b','b'},
{'b','b','1','1','b','b','b','1','1','1','1','1','1','b','b','b','1','1','b','b'},
{'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b'}};



    Mapa mapa1 = {450, 420, 960, 960, 30,30, 1,32,textureFundo1}; //mapagelo (posicaoinicialjogadorx, //y, altura, largura, tamanho matriz, tamanhoquadrado, mapa)
    Mapa mapa2 = {510, 410, 1082, 1082, 30, 30, 2, 36, textureFundo2}; //mapafogo
    Mapa mapa3 = {230, 420, 960, 960, 30,30, 3, 32, textureFundo3}; //mapafloresta
    Mapa mapacastelo = {330, 75, 720, 720, 20, 20, 4, 36, textureCastelo1};


    Mapa mapaatual = mapacastelo; //struct que vai mudar conforme o mapa mudar no jogo
    SDL_Rect mapa1_frame = {0, 480, 640, 480}; //(posicaorecortadax, posicaorecortaday, tamanhorecortadox, tamanhorecortadoy)
    SDL_Rect mapa2_frame = {0, 602, 640, 480};
    SDL_Rect mapa3_frame = {0, 480, 640, 480};
    SDL_Rect mapacastelo_frame = {0, 0, 640, 480};
    SDL_Rect sprite_frame = desenhar(20, 25, 30, 0); //(posicaorecortadax, posicaorecortaday, tamanhorecortadox, tamanhorecortadoy)
    SDL_Rect player_frame = {mapaatual.xinicial, mapaatual.yinicial, 32, 32}; //(posicaoxnatela,posicaoynatela, tamanhox, tamanhoy)
    SDL_Rect texto_rect = {250, 140, 200, 100};
    SDL_Rect nome_rect = {250, 230, 0, 50};
    SDL_Event event;
    musicaatual = musicamenus;
    Mix_PlayMusic(musicaatual, -1);


    SDL_Texture* textureInimigo = loadTexture("sprites/enemies.png", renderer);

    PERSONAGEM *LINK = criar(100, player_frame, sprite_frame, 5, 10, texturePlayer, "jogador", 0);

    PERSONAGEM *inimigos[3];

    SDL_Rect inimigo_frame = {mapa1.xinicial,mapa1.yinicial - 320, 32,32};
    SDL_Rect inimigo_frame2 = {mapa1.xinicial-50,mapa1.yinicial - 300, 32,32};
    SDL_Rect inimigo_frame3 = {mapa1.xinicial+50,mapa1.yinicial - 200, 32,32};

    inimigos[0] = criar(60, inimigo_frame, desenhar(23, 35, 120, 155), 5, 5, textureInimigo, "inimigo", 100);
    inimigos[1] = criar(60, inimigo_frame2, desenhar(23, 35, 120, 155), 5, 5, textureInimigo, "inimigo", 100);
    inimigos[2] = criar(60, inimigo_frame3, desenhar(23, 35, 120, 155), 5, 5, textureInimigo, "inimigo", 100);




    int cont = 0, flecha = 0, golpe = 0, cont_flecha = 0, contgolpes = 1, continimigo = 0, mov = 0;


    while(jogando == 1)
    {
        int cima = 0, baixo = 0, esq = 0, dir = 0;
        SDL_RenderClear(renderer);

        if(LINK->vida <= 0)
        {

            deletarJogador(LINK, mapacastelo.largura, mapacastelo.altura, matcastelo);
            deletarJogador(LINK, mapa1.largura, mapa1.altura, matmapa1);
            deletarJogador(LINK, mapa2.largura, mapa2.altura, matmapa2);
            deletarJogador(LINK, mapa3.largura, mapa3.altura, matmapa3);

            posicionarInimigo(25,14,mapa1.largura, mapa1.altura, matmapa1);
            posicionarInimigo(25,13,mapa1.largura, mapa1.altura, matmapa1);
            posicionarInimigo(27,15,mapa1.largura, mapa1.altura, matmapa1);

            posicionarInimigo(25,14,mapa2.largura, mapa2.altura, matmapa2);
            posicionarInimigo(26,15,mapa2.largura, mapa2.altura, matmapa2);
            posicionarInimigo(25,11,mapa2.largura, mapa2.altura, matmapa2);

            posicionarInimigo(25, 7,mapa3.largura, mapa3.altura, matmapa3);
            posicionarInimigo(25,4,mapa3.largura, mapa3.altura, matmapa3);
            posicionarInimigo(25,10,mapa3.largura, mapa3.altura, matmapa3);

            Mix_HaltMusic();
            if(nmapa == 1)
            {
                mapa1_frame.x = 0;
                mapa1_frame.y = 480;
                matmapa1[0][14] = 'c';
                matmapa1[28][14] = 'p';
            }
            else if(nmapa == 2)
            {
                mapa2_frame.x = 0;
                mapa2_frame.y = 602;
                matmapa2[0][14] = 'c';
                matmapa2[28][14] = 'p';
            }
            else if(nmapa ==3)
            {
                mapa3_frame.x = 0;
                mapa3_frame.y = 480;
                matmapa3[0][22] = 'c';
                matmapa3[28][7] = 'p';
            }
            matcastelo[2][9] = 'p';
            matcastelo[10][15] = 'b';
            matcastelo[10][16] = 'b';
            matcastelo[1][9] = 'f';
            matcastelo[1][10] = 'f';
            matcastelo[10][3] = 'b';
            matcastelo[10][4] = 'b';
            mapacastelo.textureMapa = textureCastelo1;
            mapacastelo_frame.x = 0;
            mapacastelo_frame.y = 0;
            mapacastelo.xinicial = 330;
            mapacastelo.yinicial = 75;
            mapaatual = mapacastelo;
            nmapa = 1;
            Mix_FreeMusic(musicamenus);
            musicamenus = Mix_LoadMUS("musicas/Musica_Menus.mp3");
            musicaatual = musicamenus;
            Mix_PlayMusic(musicaatual, -1);
            player_frame.x = mapaatual.xinicial;
            player_frame.y = mapaatual.yinicial;
            tela = 'g';
            LINK->vida = 100;


        }

        if(tela == 'p')
        {
            SDL_StartTextInput();
            char cabecalho[20] = "Insira seu nome";
            while (done != SDL_TRUE)
            {
                if (SDL_PollEvent(&event))
                {
                    textoponto = TTF_RenderText_Solid(Sans, cabecalho, White);
                    textureponto = SDL_CreateTextureFromSurface(renderer, textoponto);
                    SDL_RenderCopy(renderer, textureponto, NULL, &texto_rect);
                    textoponto = TTF_RenderText_Solid(Sans, text, White);
                    textureponto = SDL_CreateTextureFromSurface(renderer, textoponto);
                    SDL_RenderCopy(renderer, textureponto, NULL, &nome_rect);
                    SDL_RenderPresent(renderer);
                    SDL_RenderClear(renderer);
                    switch (event.type)
                    {
                        case SDL_KEYDOWN:
                            if(event.key.keysym.sym == SDLK_RETURN)
                            {
                                done = SDL_TRUE;
                                salvar(text, LINK->pontuacao);
                                tela = 'i';
                                Mix_HaltMusic();
                                Mix_FreeMusic(musicamenus);
                                musicamenus = Mix_LoadMUS("musicas/Musica_Menus.mp3");
                                musicaatual = musicamenus;
                                Mix_PlayMusic(musicaatual, -1);
                            }
                        break;
                        case SDL_TEXTINPUT:
                            strcat(text, event.text.text);
                            nome_rect.w += 20;
                        break;
                        case SDL_TEXTEDITING:
                            composition = event.edit.text;
                            cursor = event.edit.start;
                            selection_len = event.edit.length;
                        break;
                    }
                }
            }
        }

        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                jogando = 0;

            if(event.type == SDL_KEYDOWN)
            {

                if(event.key.keysym.sym == SDLK_1)
                {
                    if(tela == 'i')
                        tela = '1';
                    else if(tela == 'r')
                        tela = 'i';
                    else if(tela == 'c')
                        tela = 'i';
                    else if(tela == 's')
                        tela = 'i';
                    else if(tela == 'f')
                        tela = 'p';
                    else if(tela == '1' || tela == '2' || tela == '3' || tela == '4')
                    {
                        tela = 'j';
                        Mix_HaltMusic();
                        Mix_FreeMusic(musicacastelo);
                        musicacastelo = Mix_LoadMUS( "musicas/Musica_Mundo_Castelo.mp3" );
                        musicaatual = musicacastelo;
                        Mix_PlayMusic(musicaatual, -1);
                    }else if(tela == 'g')
                        tela = 'p';

                }
                else if(event.key.keysym.sym == SDLK_2)
                {
                    if(tela == 'i')
                        tela = 'r';
                }
                else if(event.key.keysym.sym == SDLK_3)
                {
                    if(tela == 'i')
                        tela = 'c';
                }
                else if(event.key.keysym.sym == SDLK_4)
                {
                    if(tela == 'i')
                        tela = 's';
                }
                else if(event.key.keysym.sym == SDLK_5)
                {
                    if(tela == 'i')
                        jogando = 0;
                }

                else if(event.key.keysym.sym == SDLK_LEFT)
                {
                    esq = 1;
                    if(tela == 'j') //se a tela for a do jogo
                    {
                        if(mapaatual.identificador == 1)
                        {
                            movimentacao = movimentarMatriz(LINK, 'p',-1,'x',mapaatual.altura,mapaatual.largura,matmapa1,&player_frame,&mapa1_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 2)
                        {
                             movimentacao = movimentarMatriz(LINK, 'p',-1,'x',mapaatual.altura,mapaatual.largura,matmapa2,&player_frame,&mapa2_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 3)
                        {
                             movimentacao = movimentarMatriz(LINK, 'p',-1,'x',mapaatual.altura,mapaatual.largura,matmapa3,&player_frame,&mapa3_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 4)
                        {
                             movimentacao = movimentarMatriz(LINK, 'p',-1,'x',mapaatual.altura,mapaatual.largura,matcastelo,&player_frame,&mapacastelo_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }

                    }
                    else //senão (menu, outras coisas)
                    {
                        // outras telas
                    }

                }
                else if(event.key.keysym.sym == SDLK_RIGHT)
                {
                    dir = 1;
                    if(tela == 'j')
                    {
                        if(mapaatual.identificador == 1)
                        {
                            movimentacao = movimentarMatriz(LINK,'p',1,'x',mapaatual.altura,mapaatual.largura,matmapa1,&player_frame,&mapa1_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 2)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',1,'x',mapaatual.altura,mapaatual.largura,matmapa2,&player_frame,&mapa2_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 3)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',1,'x',mapaatual.altura,mapaatual.largura,matmapa3,&player_frame,&mapa3_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 4)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',1,'x',mapaatual.altura,mapaatual.largura,matcastelo,&player_frame,&mapacastelo_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                    }else if(tela == '1')
                    {
                        tela = '2';
                    }
                    else if(tela == '2')
                    {
                        tela = '3';
                    }
                    else if(tela == '3')
                    {
                        tela = '4';
                    }
                    else if(tela == '4')
                    {
                        tela = 'j';
                        Mix_HaltMusic();
                        Mix_FreeMusic(musicacastelo);
                        musicacastelo = Mix_LoadMUS( "musicas/Musica_Mundo_Castelo.mp3" );
                        musicaatual = musicacastelo;
                        Mix_PlayMusic(musicaatual, -1);
                    }



                }
                else if(event.key.keysym.sym == SDLK_UP)
                {
                    cima = 1;
                    if(tela == 'j')
                    {
                        if(mapaatual.identificador == 1)
                        {
                            movimentacao = movimentarMatriz(LINK,'p',-1,'y',mapaatual.altura,mapaatual.largura,matmapa1,&player_frame,&mapa1_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 2)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',-1,'y',mapaatual.altura,mapaatual.largura,matmapa2,&player_frame,&mapa2_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 3)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',-1,'y',mapaatual.altura,mapaatual.largura,matmapa3,&player_frame,&mapa3_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 4)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',-1,'y',mapaatual.altura,mapaatual.largura,matcastelo,&player_frame,&mapacastelo_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        if(movimentacao >= 1 && movimentacao <= 4)
                        {
                            Mix_HaltMusic();
                            if(movimentacao == 1)
                            {
                                if(nmapa == 1)
                                {
                                    mapa1_frame.x = 0;
                                    mapa1_frame.y = 480;
                                    mapacastelo.textureMapa = textureCastelo2;
                                    mapaatual = mapacastelo;
                                    Mix_FreeMusic(musicacastelo);
                                    musicacastelo = Mix_LoadMUS( "musicas/Musica_Mundo_Castelo.mp3" );
                                    musicaatual = musicacastelo;
                                }
                                else if(nmapa == 2)
                                {
                                    mapa2_frame.x = 0;
                                    mapa2_frame.y = 602;
                                    mapacastelo_frame.x = 0;
                                    mapacastelo_frame.y = 240;
                                    mapacastelo.textureMapa = textureCastelo3;
                                    mapacastelo.xinicial = 115;
                                    mapacastelo.yinicial = 160;
                                    mapaatual = mapacastelo;
                                    Mix_FreeMusic(musicacastelo);
                                    musicacastelo = Mix_LoadMUS( "musicas/Musica_Mundo_Castelo.mp3" );
                                    musicaatual = musicacastelo;
                                }
                                else if(nmapa == 3)
                                {
                                    //final (decidir o que fazer)
                                    LINK->pontuacao += 250;
                                    mapa3_frame.x = 0;
                                    mapa3_frame.y = 480;
                                    mapacastelo_frame.x = 0;
                                    mapacastelo_frame.y = 0;
                                    mapacastelo.textureMapa = textureCastelo1;
                                    mapacastelo.xinicial = 330;
                                    mapacastelo.yinicial = 75;
                                    mapaatual = mapacastelo;
                                    Mix_FreeMusic(musicamenus);
                                    musicamenus = Mix_LoadMUS("musicas/Musica_Menus.mp3");
                                    musicaatual = musicamenus;
                                    tela = 'f';
                                    nmapa = 0;

                                }
                                nmapa++;
                            }
                            else if(movimentacao == 2)
                            {
                                for(int i = 0; i < 3; i++)
                                    inimigos[i]->vida = 60;
                                mapaatual = mapa1;
                                musicaatual = musicagelo;
                            }
                            else if(movimentacao == 3)
                            {
                                for(int i = 0; i < 3; i++)
                                    inimigos[i]->vida = 60;
                                mapaatual = mapa2;
                                musicaatual = musicafogo;
                            }
                            else if(movimentacao == 4)
                            {
                                for(int i = 0; i < 3; i++)
                                    inimigos[i]->vida = 60;
                                mapaatual = mapa3;
                                musicaatual = musicafloresta;
                            }
                            else if(movimentacao == 5)
                            {
                                musicaatual = musicamenus;
                            }
                            Mix_PlayMusic(musicaatual, -1);
                            player_frame.x = mapaatual.xinicial;
                            player_frame.y = mapaatual.yinicial;
                        }
                    }
                    else if (tela == 'f')
                    {
                        tela = 'p';
                    }

                }
                else if(event.key.keysym.sym == SDLK_DOWN)
                {
                    baixo = 1;
                    if(tela == 'j')
                    {
                        if(mapaatual.identificador == 1)
                        {
                            movimentacao = movimentarMatriz(LINK,'p',1,'y',mapaatual.altura,mapaatual.largura,matmapa1,&player_frame,&mapa1_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 2)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',1,'y',mapaatual.altura,mapaatual.largura,matmapa2,&player_frame,&mapa2_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 3)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',1,'y',mapaatual.altura,mapaatual.largura,matmapa3,&player_frame,&mapa3_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                        else if(mapaatual.identificador == 4)
                        {
                             movimentacao = movimentarMatriz(LINK,'p',1,'y',mapaatual.altura,mapaatual.largura,matcastelo,&player_frame,&mapacastelo_frame,&mapaatual,mapaatual.tamanhopasso,altura,largura);
                        }
                    }

                }else if(event.key.keysym.sym == SDLK_x)
                    golpe = 1;
                else if(event.key.keysym.sym == SDLK_z)
                    flecha = 1;
            }else if(event.type == SDL_KEYUP)
            {
                if(event.key.keysym.sym == SDLK_DOWN)
                    baixo = 2;
                else if(event.key.keysym.sym == SDLK_UP)
                    cima = 2;
                else if(event.key.keysym.sym == SDLK_RIGHT)
                    dir = 2;
                else if(event.key.keysym.sym == SDLK_LEFT)
                    esq = 2;
            }
        }

        if(tela == 'j')
        {
            //Andar para a esquerda
                /*INIMIGO->idle = desenhar(35, 35, 155, 160);

                if(continimigo == 0 || continimigo == 1)
                    INIMIGO->sprite_atual = INIMIGO->idle;

                animarNPC(INIMIGO, ESQUERDA, INIMIGO->idle.y, &continimigo);
                continimigo++;*/
            //Andar para cima



            /*INIMIGO->idle = desenhar(35, 35, 200, 160);

            if(continimigo == 0 || continimigo == 1)
                INIMIGO->sprite_atual = INIMIGO->idle;
            INIMIGO = animarNPC(INIMIGO, CIMA, INIMIGO->idle.y, &continimigo);
            continimigo++;*/


           /* //Andar para baixo
                INIMIGO->idle = desenhar(35, 35, 120, 155);

                if(continimigo == 0 || continimigo == 1)
                    INIMIGO->sprite_atual = INIMIGO->idle;
                INIMIGO = animarNPC(INIMIGO, BAIXO, INIMIGO->idle.y, &continimigo);
                continimigo++;*/


            if(cima == 1)
            {
                if(cont == 0)
                    LINK->sprite_atual = desenhar(20, 25, 0, 120);
                animarPersonagem(LINK, CIMA, VERTICAL, &cont);
                cont++;
            }else if (cima == 2)
            {
                parar(LINK, desenhar(20, 25, 90, 120), &cont);
                LINK->sentido = CIMA;
                LINK->direcao = VERTICAL;
            }

            if(baixo == 1)
            {
                if(cont == 0)
                    LINK->sprite_atual = desenhar(20, 25, 0, 30);
                animarPersonagem(LINK, BAIXO, VERTICAL, &cont);
                cont++;
            }else if(baixo == 2)
            {
                parar(LINK, desenhar(20, 25, 30, 0), &cont);
                cont = 0;
                LINK->sentido = BAIXO;
                LINK->direcao = VERTICAL;
            }

            if(dir == 1)
            {
                if(cont == 0)
                    LINK->sprite_atual = desenhar(20, 25, 240, 120);
                animarPersonagem(LINK, DIREITA, HORIZONTAL, &cont);
                cont++;
            }else if(dir == 2)
            {
                parar(LINK,desenhar(20, 25, 330, 120), &cont);
                cont = 0;
                LINK->sentido = DIREITA;
                LINK->direcao = HORIZONTAL;
            }

            if(esq == 1)
            {
                if(cont == 0)
                    LINK->sprite_atual = desenhar(20, 25, 240, 30);

                LINK = animarPersonagem(LINK, ESQUERDA, HORIZONTAL, &cont);
                cont++;
            }else if(esq == 2)
            {
                parar(LINK, desenhar(20, 25, 330, 30), &cont);
                LINK->sentido = ESQUERDA;
                LINK->direcao = HORIZONTAL;
            }

            if(flecha == 1)
            {
                //Empunhar arco
                LINK->sprite_atual = desenhar(LINK->idle.w, LINK->idle.h, LINK->direcao, LINK->sentido-30);
                LINK = atirar(LINK, &cont_flecha, &flecha);
                cont_flecha++;
            }

            if(golpe == 1)
            {
                LINK->sprite_atual = desenhar(20, 30, LINK->direcao, LINK->sentido);
                LINK = golpear(LINK, LINK->direcao, &contgolpes, LINK->sentido, &golpe);
                if(golpe == 0)
                {
                    if(mapaatual.identificador == 1)
                        golpearMatriz(LINK, LINK->sentido, mapaatual.largura, mapaatual.altura, matmapa1, inimigos);
                    else if (mapaatual.identificador == 2)
                        golpearMatriz(LINK, LINK->sentido, mapaatual.largura, mapaatual.altura, matmapa2, inimigos);
                    else if(mapaatual.identificador == 3)
                        golpearMatriz(LINK, LINK->sentido, mapaatual.largura, mapaatual.altura, matmapa3, inimigos);
                    else if(mapaatual.identificador == 4)
                        golpearMatriz(LINK, LINK->sentido, mapaatual.largura, mapaatual.altura, matcastelo, inimigos);
                }
                contgolpes++;
            }

            LINK->posicao_tamanho = player_frame;

            inimigos[0]->posicao_tamanho = desenhar(32,32,mapaatual.xinicial,mapaatual.yinicial - 90);

            inimigos[1]->posicao_tamanho = desenhar(32,32,mapaatual.xinicial-32,mapaatual.yinicial - 100);

            inimigos[2]->posicao_tamanho = desenhar(32,32,mapaatual.xinicial+32,mapaatual.yinicial - 32);

            if(mapaatual.identificador == 1)
            {

                inimigos[0]->x = 14;
                inimigos[0]->y = 25;

                inimigos[1]->x = 13;
                inimigos[1]->y = 25;

                inimigos[2]->x = 15;
                inimigos[2]->y = 27;

                SDL_RenderCopy(renderer,mapaatual.textureMapa, &mapa1_frame, NULL);
                for(int k = 0; k < 3; k++)
                {
                    if(inimigos[k]->vida > 0)
                        SDL_RenderCopy(renderer, inimigos[k]->textura, &inimigos[k]->sprite_atual, &inimigos[k]->posicao_tamanho);
                }
            }else if(mapaatual.identificador == 2)
            {
                inimigos[0]->posicao_tamanho = desenhar(32, 32, mapa2.xinicial,mapa2.yinicial - 96);
                inimigos[1]->posicao_tamanho = desenhar(32, 32, mapa2.xinicial+32,mapa2.yinicial - 64);
                inimigos[2]->posicao_tamanho = desenhar(32, 32, mapa2.xinicial-106,mapa2.yinicial - 106);

                inimigos[0]->x = 14;
                inimigos[0]->y = 25;
                inimigos[1]->x = 15;
                inimigos[1]->y = 26;
                inimigos[2]->x = 11;
                inimigos[2]->y = 25;


                SDL_RenderCopy(renderer,mapaatual.textureMapa, &mapa2_frame, NULL);
                for(int k = 0; k < 3; k++)
                {
                    if(inimigos[k]->vida > 0)
                        SDL_RenderCopy(renderer, inimigos[k]->textura, &inimigos[k]->sprite_atual, &inimigos[k]->posicao_tamanho);
                }
                //SDL_RenderCopy(renderer, INIMIGO->textura, &INIMIGO->sprite_atual, &INIMIGO->posicao_tamanho);
            }
            else if(mapaatual.identificador == 3){

                /*inimigos[0]->posicao_tamanho = ;
                inimigos[1]->posicao_tamanho = ;
                inimigos[2]->posicao_tamanho = ;*/

                inimigos[0]->posicao_tamanho = desenhar(32, 32, mapa3.xinicial,mapa2.yinicial - 96);
                inimigos[2]->posicao_tamanho = desenhar(32, 32, mapa3.xinicial+96,mapa2.yinicial - 96);
                inimigos[1]->posicao_tamanho = desenhar(32, 32, mapa3.xinicial-96,mapa2.yinicial - 96);



                SDL_RenderCopy(renderer,mapaatual.textureMapa, &mapa3_frame, NULL);
                inimigos[0]->x = 7;
                inimigos[0]->y = 25;

                inimigos[1]->x = 4;
                inimigos[1]->y = 25;

                inimigos[2]->x = 10;
                inimigos[2]->y = 25;

                for(int k = 0; k < 3; k++)
                {
                    if(inimigos[k]->vida > 0)
                        SDL_RenderCopy(renderer, inimigos[k]->textura, &inimigos[k]->sprite_atual, &inimigos[k]->posicao_tamanho);
                }
                //SDL_RenderCopy(renderer, INIMIGO->textura, &INIMIGO->sprite_atual, &INIMIGO->posicao_tamanho);
            }else if(mapaatual.identificador == 4){
                SDL_RenderCopy(renderer,mapaatual.textureMapa, &mapacastelo_frame, NULL);
                //SDL_RenderCopy(renderer, INIMIGO->textura, &INIMIGO->sprite_atual, &INIMIGO->posicao_tamanho);
            }

            SDL_RenderCopy(renderer, LINK->textura, &LINK->sprite_atual, &LINK->posicao_tamanho);

        }
        else if(tela == 'g')
            SDL_RenderCopy(renderer, textureOver, NULL, NULL);
        else if(tela == 'i')
            SDL_RenderCopy(renderer, textureInicio, NULL, NULL);
        else if(tela == 'r')
        {
            SDL_RenderCopy(renderer, textureRanking, NULL, NULL);
            lerpontos(renderer);
        }
        else if(tela == 's')
            SDL_RenderCopy(renderer, textureSobre, NULL, NULL);
        else if(tela == 'c')
            SDL_RenderCopy(renderer, textureComoJogar, NULL, NULL);
        else if(tela == 'f')
            SDL_RenderCopy(renderer, textureFim, NULL, NULL);
        else if(tela == '1')
            SDL_RenderCopy(renderer, textureLore1, NULL, NULL);
        else if(tela == '2')
            SDL_RenderCopy(renderer, textureLore2, NULL, NULL);
        else if(tela == '3')
            SDL_RenderCopy(renderer, textureLore3, NULL, NULL);
        else if(tela == '4')
            SDL_RenderCopy(renderer, textureLore4, NULL, NULL);

        if(mapaatual.identificador == 1)
            LINK = posicaoJogador(LINK, mapaatual.largura, mapaatual.altura, matmapa1);
        if(mapaatual.identificador == 2)
            LINK = posicaoJogador(LINK, mapaatual.largura, mapaatual.altura, matmapa2);
        if(mapaatual.identificador == 3)
            LINK = posicaoJogador(LINK, mapaatual.largura, mapaatual.altura, matmapa3);
        if(mapaatual.identificador == 4)
            LINK = posicaoJogador(LINK, mapaatual.largura, mapaatual.altura, matcastelo);
        SDL_RenderPresent(renderer);
        SDL_Delay(60);
    }
    SDL_DestroyWindow(janela);
    SDL_Quit();
    return 0;
}

SDL_Texture* loadTexture(char *arquivo, SDL_Renderer *renderer)
{
    SDL_Surface* surfaceload = IMG_Load(arquivo);
    SDL_Texture* textureFundo = SDL_CreateTextureFromSurface(renderer, surfaceload);
    SDL_FreeSurface(surfaceload);
    return textureFundo;
}

void movimentarTela(SDL_Rect *mapa_frame, int valor, char coordenada)
{
    if(coordenada == 'x')
    {
        (*mapa_frame).x += valor; //move o frame do mapa pra direita
    }
    else
    {
        (*mapa_frame).y += valor; //move o frame do mapa pra esquerda
    }
}

void movimentarPlayer(SDL_Rect *jogador_frame, int valor, char coordenada)
{
    if(coordenada == 'x')
    {
        (*jogador_frame).x += valor; //move o frame do jogador pra direita
    }
    else
    {
        (*jogador_frame).y += valor; //move o frame do jogador pra esquerda
    }
}

int movimentarMatriz(PERSONAGEM *LINK, char tipo,int valor, char coordenada, int alturamapa, int larguramapa, char mapa[alturamapa][larguramapa], SDL_Rect *player_frame, SDL_Rect *mapa_frame, Mapa *mapaatual, int tamanhopasso, int altura, int largura)
{
    char casa;
    for (int i = 0; i < alturamapa; i++)
    {
        for(int j = 0; j < larguramapa; j++)
        {
            if(mapa[i][j] == tipo) //encontra posicao do jogador na matriz
            {
                if(coordenada == 'x') //se a movimentacao for horizontal
                {
                    if(mapa[i][j+valor] != 'b' && mapa[i][j+valor] != 'X' && mapa[i][j+valor] != 'p') //se a posicao destino nao for obstaculo
                    {
                        mapa[i][j+valor] = tipo; //move jogador pra posicao destino
                        mapa[i][j] = '1'; //posicao origem vira chao
                        if(valor == 1)
                        {
                            i++;
                            if(player_frame->x +tamanhopasso <= largura/2) //se o jogador nao estiver no meio da tela
                            {
                                movimentarPlayer(player_frame, tamanhopasso, 'x'); //move jogador
                            }
                            else
                            {
                                if(mapa_frame->x + mapa_frame->w + tamanhopasso <= mapaatual->w) // se nao estiver no canto da tela
                                {
                                    movimentarTela(mapa_frame, tamanhopasso, 'x');
                                }
                                else // se estiver, move o player
                                {
                                    movimentarPlayer(player_frame, tamanhopasso, 'x');
                                }
                            }
                        }
                        else
                        {
                            if(player_frame->x -tamanhopasso >= largura/2)
                            {
                                movimentarPlayer(player_frame, -tamanhopasso, 'x');
                            }
                            else
                            {
                                if(mapa_frame->x - tamanhopasso >= 0)
                                {
                                    movimentarTela(mapa_frame, -tamanhopasso, 'x');
                                }
                                else
                                {
                                    movimentarPlayer(player_frame, -tamanhopasso, 'x');
                                }

                            }


                        }
                        return 0;
                    }
                    else
                    {


                        if(mapa[i][j+valor] == 'X')
                        {
                            //perde vida

                            LINK->vida -= 20;
                            //printf("PERDI VIDA: %d\n", LINK->vida);
                        }


                        return -1;
                    }
                }
                else if(coordenada == 'y')
                {
                    if(mapa[i+valor][j] != 'b' && mapa[i+valor][j] != 'X' && mapa[i+valor][j] != 'p')
                    {
                        casa = mapa[i+valor][j];
                        mapa[i+valor][j] = tipo;
                        mapa[i][j] = '1';
                        if(valor == 1)
                        {
                            i++;
                            j++;
                            if(player_frame->y +tamanhopasso <= altura/2)
                            {
                                movimentarPlayer(player_frame, tamanhopasso, 'y');
                            }
                            else
                            {
                                if(mapa_frame->y + mapa_frame->h + tamanhopasso <= mapaatual->h)
                                {
                                    movimentarTela(mapa_frame, tamanhopasso, 'y');
                                }
                                else
                                {
                                    movimentarPlayer(player_frame, tamanhopasso, 'y');
                                }
                            }
                        }
                        else
                        {
                            if(player_frame->y -tamanhopasso >= altura/2)
                            {
                                movimentarPlayer(player_frame, -tamanhopasso, 'y');
                            }
                            else
                            {
                                if(mapa_frame->y - tamanhopasso >= 0)
                                {
                                    movimentarTela(mapa_frame, -tamanhopasso, 'y');
                                }
                                else
                                {
                                    movimentarPlayer(player_frame, -tamanhopasso, 'y');
                                }
                            }
                        }
                        if(casa == 'c' && mapa[28][14] == '1')
                        {
                            mapa[i+valor][j] = 'c';
                            mapa[28][14] = 'p';
                            return 1;
                        }
                        else if(casa == 'c' && mapa[28][14] == 'b')
                        {
                            mapa[i+valor][j] = 'c';
                            mapa[28][7] = 'p';
                            return 1;
                        }
                        else if(casa == 'f')
                        {
                            mapa[2][9] = 'p';
                            mapa[1][9] = 'b';
                            mapa[1][10] = 'b';
                            mapa[10][3] = 'g';
                            mapa[10][4] = 'g';
                            return 2;
                        }
                        else if(casa == 'g')
                        {
                            mapa[11][3] = 'p';
                            mapa[10][3] = 'b';
                            mapa[10][4] = 'b';
                            mapa[10][15] = 'h';
                            mapa[10][16] = 'h';
                            return 3;
                        }
                        else if(casa == 'h')
                        {
                            mapa[2][9] = 'p';
                            mapa[10][15] = 'b';
                            mapa[10][16] = 'b';
                            mapa[1][9] = 'f';
                            mapa[1][10] = 'f';
                            return 4;
                        }
                        else
                            return 0;
                    }
                    else
                    {
                        if(mapa[i+valor][j] == 'X')
                        {
                            //perde vida
                            LINK->vida -= 20;
                            //printf("PERDI VIDA: %d\n", LINK->vida);


                        }
                        return -1;
                    }

                }
            }
        }
    }

}
