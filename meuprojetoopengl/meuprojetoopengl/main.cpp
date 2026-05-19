#include <GL/freeglut.h>
#include <cmath>
#include <string>


// TAMANHO DA JANELA
int largura = 1000;
int altura  = 700;


// ESTADO DO JOGO
enum EstadoTela { MENU, JOGO, PAUSADO, GAME_OVER };
EstadoTela telaAtual = MENU;

// BOTÃO PLAY
float botaoX, botaoY;
float botaoLargura = 200;
float botaoAltura  = 60;

// PONTUAÇÃO
int pontos = 0;
int vidas = 5;
int recordePessoal = 0;

// BARRA DO JOGADOR
float barraX = largura / 2.0f;
float barraY = 70.0f;

float barraLargura = 160.0f;
float barraAltura  = 20.0f;

float velocidadeBarra = 6.0f;

// ANIMAÇÃO – tempo global (em segundos
float tempoTotal = 0.0f;

// TECLAS PRESSIONADAS
bool teclaEsquerda = false;
bool teclaDireita  = false;

// DESENHAR TEXTO BITMAP
void desenharTexto(float x, float y, const std::string& texto,
                   void* fonte = GLUT_BITMAP_TIMES_ROMAN_24)
{
    glRasterPos2f(x, y);
    for (char c : texto)
        glutBitmapCharacter(fonte, c);
}


// PRIMITIVAS
void desenharQuadrado(float x, float y, float tam, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x,       y);
    glVertex2f(x + tam, y);
    glVertex2f(x + tam, y + tam);
    glVertex2f(x,       y + tam);
    glEnd();
}

void desenharTriangulo(float x, float y, float tam,float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x,           y);
    glVertex2f(x + tam,     y);
    glVertex2f(x + tam / 2, y + tam);
    glEnd();
}

void desenharCirculo(float cx, float cy, float raio, float r, float g, float b, int seg = 64)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) {
        float a = 2.0f * 3.14159f * i / seg;
        float x = cx + raio * cosf(a);
        float y = cy + raio * sinf(a);
       glVertex2f(x, y);
    }
    glEnd();
}

void desenharPoligono(float cx, float cy, float raio, int lados,float r, float g, float b, float rotacao = 0.0f)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= lados; i++) {
        float a = 2.0f * 3.14159f * i / lados + rotacao;

        float x = cx + raio * cosf(a);
        float y = cy + raio * sinf(a);
        glVertex2f(x, y);
    }
    glEnd();
}

// Diamante (quadrado rotacionado 45°)
void desenharDiamante(float cx, float cy, float raio,float r, float g, float b)
{
    desenharPoligono(cx, cy,raio,4,r, g, b,3.14159f / 4.0f);
}

void desenharFundo()
{
    glBegin(GL_QUADS);
    glColor3f(0.66f, 0.86f, 1.0f);  // azul claro (baixo)
    glVertex2f(0,       0);
    glVertex2f(largura, 0);
    glColor3f(0.10f + 0.1f*sinf(tempoTotal), 0.50f, 0.95f); // azul forte (cima)
    glVertex2f(largura, altura);
    glVertex2f(0,       altura);
    glEnd();
}


// FORMA FLUTUANTE 
// Aplica translação de flutuação baseada no tempo e fase
void iniciarFlutuar(float cx, float cy, float amplitude, float fase,float& outX, float& outY)
{
    outX = cx;
    outY = cy + amplitude * sinf(tempoTotal * 1.8f + fase);
}


// FORMAS FLUTUANTES
void desenharFormasDecorativas()
{
    float x, y;

    //  ESQUERDA
    // Quadrado vermelho
    iniciarFlutuar(120, 520, 12, 0.0f, x, y);
    desenharQuadrado(x - 30, y - 30, 60, 1.0f, 0.1f, 0.1f);

    // Círculo azul royal
    iniciarFlutuar(190, 350, 14, 0.7f, x, y);
    desenharCirculo(x, y, 38, 0.1f, 0.4f, 1.0f);

    // Triângulo amarelo
    iniciarFlutuar(90, 160, 10, 1.4f, x, y);
    desenharTriangulo(x - 38, y - 38, 76, 1.0f, 0.85f, 0.0f);

    // Diamante rosa
    iniciarFlutuar(250, 430, 12, 2.1f, x, y);
    desenharDiamante(x, y, 36, 1.0f, 0.3f, 0.75f);

    // - DIREITA -
    // Hexágono roxo
    iniciarFlutuar(largura - 180, 520, 14, 0.5f, x, y);
    desenharPoligono(x, y, 42, 6, 0.6f, 0.2f, 0.95f, 0.0f);

    // Círculo dourado 
    iniciarFlutuar(largura - 110, 340, 12, 1.2f, x, y);
    desenharCirculo(x, y, 36, 1.0f, 0.85f, 0.0f);  // anel
    desenharCirculo(x, y, 26, 1.0f, 0.65f, 0.0f);  // centro

    // Pentágono verde
    iniciarFlutuar(largura - 250, 430, 10, 1.9f, x, y);
    desenharPoligono(x, y, 34, 5, 0.15f, 0.85f, 0.2f,-3.14159f / 2.0f);

    // Quadrado rosa
    iniciarFlutuar(largura - 90, 170, 10, 2.7f, x, y);
    desenharQuadrado(x - 26, y - 26, 52, 1.0f, 0.2f, 0.55f);

}


// BARRA DO JOGADOR (inicio)
void desenharBarraPreview()
{
    // A barra desliza
    float oscX  = (largura / 2.0f) + 180.0f * sinf(tempoTotal * 0.8f);
    float bW    = 140.0f;
    float bH    = 18.0f;
    float bY    = 80.0f;

    // Barra principal (azul)
    glColor3f(0.0f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(oscX - bW / 2, bY);
    glVertex2f(oscX + bW / 2, bY);
    glVertex2f(oscX + bW / 2, bY + bH);
    glVertex2f(oscX - bW / 2, bY + bH);
    glEnd();

    // interno
    glColor3f(0.6f, 0.97f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(oscX - bW / 2 + 6, bY + bH * 0.6f);
    glVertex2f(oscX + bW / 2 - 6, bY + bH * 0.6f);
    glVertex2f(oscX + bW / 2 - 6, bY + bH * 0.85f);
    glVertex2f(oscX - bW / 2 + 6, bY + bH * 0.85f);
    glEnd();
}


// BARRA DO JOGADOR
void desenharBarra()
{
    // sombra
    glColor3f(0.0f, 0.25f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(barraX - barraLargura / 2 + 4, barraY - 4);
    glVertex2f(barraX + barraLargura / 2 + 4, barraY - 4);
    glVertex2f(barraX + barraLargura / 2 + 4, barraY + barraAltura - 4);
    glVertex2f(barraX - barraLargura / 2 + 4, barraY + barraAltura - 4);
    glEnd();

    // barra principal
    glColor3f(0.0f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(barraX - barraLargura / 2, barraY);
    glVertex2f(barraX + barraLargura / 2, barraY);
    glVertex2f(barraX + barraLargura / 2, barraY + barraAltura);
    glVertex2f(barraX - barraLargura / 2, barraY + barraAltura);

    glEnd();

    // brilho
    glColor3f(0.7f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(barraX - barraLargura / 2 + 8, barraY + barraAltura * 0.6f);
    glVertex2f(barraX + barraLargura / 2 - 8, barraY + barraAltura * 0.6f);
    glVertex2f(barraX + barraLargura / 2 - 8, barraY + barraAltura * 0.85f);
    glVertex2f(barraX - barraLargura / 2 + 8,barraY + barraAltura * 0.85f);
    glEnd();
}

// TÍTULO 
void desenharTitulo()
{
    const std::string titulo = "CATCH GAME";
    float tituloX = largura / 2.0f - 260;
    float tituloY = altura / 2.0f + 160;
    float escala  = 0.9f;

    
    glPushMatrix();
    glTranslatef(tituloX + 5, tituloY - 5, 0);
    glScalef(escala, escala, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (char c : titulo) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    glPopMatrix();

    
    glPushMatrix();
    glTranslatef(tituloX, tituloY, 0);
    glScalef(escala, escala, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (char c : titulo) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    glPopMatrix();
}


// PAINEL DE PONTUCAO
void desenharHUD()
{
    glColor3f(0.2f, 0.0f, 0.3f); // roxo escuro
    glBegin(GL_QUADS);
    glVertex2f(0, altura - 40);
    glVertex2f(largura, altura - 40);
    glVertex2f(largura, altura);
    glVertex2f(0, altura);
    glEnd();

    glColor3f(1, 1, 1);
    desenharTexto(
        20,
        altura - 25,
        "PONTOS: " + std::to_string(pontos),
        GLUT_BITMAP_HELVETICA_18
    );

    desenharTexto(largura/2.0f - 60, altura-25,
                  "RECORDE: " + std::to_string(recordePessoal),
                  GLUT_BITMAP_HELVETICA_18);


    desenharTexto( largura - 140,altura - 25,
        "VIDAS: " + std::to_string(vidas),
        GLUT_BITMAP_HELVETICA_18
    );

}

// BOTÃO PLAY 
void desenharBotao()
{
    botaoX = largura / 2.0f - botaoLargura / 2.0f;
    botaoY = altura  / 2.0f - botaoAltura  / 2.0f - 60.0f;

    // EFEITO 
    float pulso = 1.0f + 0.05f * sinf(tempoTotal * 4.0f);
    float larguraPulso = botaoLargura * pulso;
    float alturaPulso = botaoAltura * pulso;
    float x = largura / 2.0f - larguraPulso / 2.0f;
    float y = botaoY - (alturaPulso - botaoAltura) / 2.0f;

    // BOTÃO   
    float g = 0.75f + 0.10f * sinf(tempoTotal * 4.0f);

    glColor3f(0.1f, g, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + larguraPulso, y);
    glVertex2f(x + larguraPulso, y + alturaPulso);
    glVertex2f(x, y + alturaPulso);
    glEnd();

    // TEXTO
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(x + 65,y + 22,
        "PLAY",GLUT_BITMAP_HELVETICA_18
    );
}


// OBJETOS CAINDO (tela inicial)
struct ObjetoDemo {
    float xNorm;   // 0..1 (posição horizontal relativa)
    float offset;  // offset de fase para variar a posição vertical
    float r, g, b;
    int tipo;      // 0=quad, 1=circ, 2=tri, 3=hex
};

static ObjetoDemo objsDemo[] = {
    {0.28f, 0.0f,  1.0f, 0.2f, 0.2f, 0},
    {0.50f, 1.5f,  1.0f, 0.85f,0.0f, 1},
    {0.68f, 0.8f,  0.6f, 0.2f, 1.0f, 2},
    {0.40f, 2.4f,  0.15f,0.85f,0.2f, 3},
};

void desenharObjetosDemo()
{
    float velDemo = 0.5f;  // velocidade de queda demo
    for (auto& o : objsDemo) {
        float cx = o.xNorm * largura;
        // posição y: começa em cima e vai para baixo, reset
        float fase  = fmodf(tempoTotal * velDemo + o.offset, 1.0f);
        float cy    = altura - fase * (altura + 60.0f) + 30.0f;
        float raio  = 20.0f;

        switch (o.tipo) {
            case 0: desenharQuadrado(cx - raio, cy - raio, raio * 2,o.r, o.g, o.b); 
                break;
            case 1: desenharCirculo (cx, cy, raio, o.r, o.g, o.b); 
                break;
            case 2: desenharTriangulo(cx - raio, cy - raio, raio * 2, o.r, o.g, o.b);
                break;
            case 3: desenharPoligono(cx, cy, raio, 6, o.r, o.g, o.b); 
                break;
        }
    }
}

// OBJETOS DO JOGO
struct Objeto
{
    float x;
    float y;
    float velocidade;
    int tipo;
    bool vermelho;
};

const int quantidadeObjetos = 12;

Objeto objetos[quantidadeObjetos];

// INICIAR OBJETOS
void iniciarObjetos()
{
    for (int i = 0; i < quantidadeObjetos; i++)
    {
        objetos[i].x = rand() % largura;
        objetos[i].y = altura + rand() % 600;
        objetos[i].velocidade = 2.0f + (rand() % 2);
        objetos[i].tipo = rand() % 4;
        objetos[i].vermelho = (rand() % 100 < 40);
    }
}


// DESENHAR OBJETOS CAINDO
void desenharObjetos()
{
    for (int i = 0; i < quantidadeObjetos; i++)
    {
        float r, g, b;
       
        if (objetos[i].vermelho)
        {
            r = 1.0f;
            g = 0.1f;
            b = 0.1f;
        }
        else
        {
            // cores aleatórias
            switch (i % 4)
            {
                case 0:
                    r = 0.2f;
                    g = 0.5f;
                    b = 1.0f;
                    break;
                case 1:
                    r = 1.0f;
                    g = 0.8f;
                    b = 0.0f;
                    break;
                case 2:
                    r = 0.7f;
                    g = 0.2f;
                    b = 1.0f;
                    break;
                default:
                    r = 0.0f;
                    g = 1.0f;
                    b = 0.5f;
            }
        } 
        switch (objetos[i].tipo)
        {
            case 0:
                desenharQuadrado(objetos[i].x,objetos[i].y,35,r, g, b);
                break;
            case 1:
                desenharCirculo(objetos[i].x,objetos[i].y,20,r, g, b);   
                break;
            case 2:
                desenharTriangulo(objetos[i].x,objetos[i].y,40,r, g, b);
                break;
            case 3:
                desenharPoligono(objetos[i].x,objetos[i].y,22,6,r, g, b);
                break;
        }
    }
}

// COLISÃO AABB
bool colidiu(float objX, float objY, float tamanho)
{
    float barraEsq = barraX - barraLargura / 2;
    float barraDir = barraX + barraLargura / 2;
    float barraTopo = barraY + barraAltura;
    float barraBase = barraY;
    float objEsq = objX;
    float objDir = objX + tamanho;
    float objBase = objY;
    float objTopo = objY + tamanho;

    return (
        objEsq < barraDir &&
        objDir > barraEsq &&
        objBase < barraTopo &&
        objTopo > barraBase
    );
}


// ATUALIZAR OBJETOS
void atualizarObjetos()
{
    for (int i = 0; i < quantidadeObjetos; i++)
    {
       float dificuldade = 1.0f;
        if (objetos[i].vermelho)
        {
            dificuldade += (pontos / 5) * 0.2f;
        }
        objetos[i].y -= objetos[i].velocidade * dificuldade;

        // COLISÃO COM A BARRA        
        if (colidiu(objetos[i].x,objetos[i].y,35))
        {
            if (objetos[i].vermelho)
            {
                pontos++;
                if (pontos > recordePessoal)
                     recordePessoal = pontos;
            }

            // reinicia objeto
            objetos[i].x =rand() % (largura - 50);
            objetos[i].y = altura + rand() % 300;
            
            if (objetos[i].vermelho)
            {
                objetos[i].velocidade = 2.0f + (rand() % 2);
            }
            else
            {
                objetos[i].velocidade =1.0f + (rand() % 2);
            }

            objetos[i].tipo = rand() % 4;

            
            objetos[i].vermelho =
                (rand() % 100 < 20);
        }

        // PASSOU DA TELA
        if (objetos[i].y < -50)
        {
            if (objetos[i].vermelho)
            {
                vidas--;
                if (vidas <= 0) {
                    telaAtual = GAME_OVER; 
                }
            }

            objetos[i].x =rand() % (largura - 50);
            objetos[i].y = altura + rand() % 300;
            objetos[i].tipo =rand() % 4;
            objetos[i].vermelho = (rand() % 100 < 20);
            if (objetos[i].vermelho)
            {
                objetos[i].velocidade = 2.0f + (rand() % 2);
            }
            else
            {
                objetos[i].velocidade = 1.0f + (rand() % 2);
            }
        }
    }
}


// MENU
void telaMenu()
{
    glClear(GL_COLOR_BUFFER_BIT);
    desenharFundo();
    desenharFormasDecorativas();
    desenharObjetosDemo();
    desenharBarraPreview();
    desenharHUD();
    desenharTitulo();
    desenharBotao();
    glutSwapBuffers();
}

// TELA DO JOGO 
void telaJogo()
{
    glClear(GL_COLOR_BUFFER_BIT);
    desenharFundo();
    desenharHUD();
    desenharObjetos();
    desenharBarra();

     if (tempoTotal < 20.0f) {
        glColor3f(1, 1, 1);
        desenharTexto(largura/2.0f - 130, 40,
             "SETAS para mover a barra | P = pausar | ESC = sair | M = menu ", GLUT_BITMAP_HELVETICA_12);
    }


    glutSwapBuffers();
}

// TELA PAUSE
void telaPause()
{
    // Renderiza o jogo em fundo
    glClear(GL_COLOR_BUFFER_BIT);
    desenharFundo();
    desenharHUD();
    desenharObjetos();
    desenharBarra();

    // Overlay semitransparente simulado com um quad escuro
    glColor3f(0.0f, 0.05f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(largura/2.0f-180, altura/2.0f-80);
    glVertex2f(largura/2.0f+180, altura/2.0f-80);
    glVertex2f(largura/2.0f+180, altura/2.0f+80);
    glVertex2f(largura/2.0f-180, altura/2.0f+80);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    desenharTexto(largura/2.0f - 55, altura/2.0f + 30,
                  "PAUSADO", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(largura/2.0f - 100, altura/2.0f - 10,
                  "P - Continuar   M - Menu",
                  GLUT_BITMAP_HELVETICA_18);

    glutSwapBuffers();
}

// TELA GAME_OVER
void telaGameOver()
{
    glClear(GL_COLOR_BUFFER_BIT);
    desenharFundo(); 

    // 1. O PAINEL CENTRALIZADO 
    glColor3f(0.2f, 0.0f, 0.3f); // Roxo escuro
    glBegin(GL_QUADS);
        glVertex2f(largura / 2.0f - 200, altura / 2.0f - 100);
        glVertex2f(largura / 2.0f + 200, altura / 2.0f - 100);
        glVertex2f(largura / 2.0f + 200, altura / 2.0f + 100);
        glVertex2f(largura / 2.0f - 200, altura / 2.0f + 100);
    glEnd();

    // 2. TEXTO DE STATUS (Vermelho)
    glColor3f(1.0f, 0.1f, 0.1f);
    desenharTexto(largura / 2.0f - 80, altura / 2.0f + 50, 
                  "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    // 3. PONTUAÇÃO FINAL DO JOGADOR (Branco)
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(largura / 2.0f - 60, altura / 2.0f - 10, 
                  "PONTOS: " + std::to_string(pontos), GLUT_BITMAP_HELVETICA_18);
                  
    desenharTexto(largura / 2.0f - 60, altura / 2.0f - 30, 
                  "RECORDE: " + std::to_string(recordePessoal), GLUT_BITMAP_HELVETICA_18);

    // 4. INSTRUÇÕES DE COMANDO (Azul Claro)
    glColor3f(0.7f, 0.9f, 1.0f);
    desenharTexto(largura / 2.0f - 160, altura / 2.0f - 60, 
                  "Pressione R para recomecar | M para menu | ESC para sair", GLUT_BITMAP_HELVETICA_12);

    glutSwapBuffers(); 


// DISPLAY
void display()
{
    switch (telaAtual) {
        case MENU:
            telaMenu();
            break;
        case JOGO:
            telaJogo();
            break;
        case PAUSADO:
            telaPause();
            break;
        case GAME_OVER:
            telaGameOver();
            break;
    }
}


// TIMER – dispara a cada ~16ms (~60fps)
void timer(int)
{
    tempoTotal += 0.016f;
    if (teclaEsquerda)
        barraX -= velocidadeBarra;
    if (teclaDireita)
        barraX += velocidadeBarra;
    // impedir sair da tela
    if (barraX - barraLargura / 2 < 0)
        barraX = barraLargura / 2;
    if (barraX + barraLargura / 2 > largura)
        barraX = largura - barraLargura / 2;

    if (telaAtual == JOGO)
    {
        atualizarObjetos();
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


// MOUSE
void mouse(int botao, int estado, int x, int y)
{
    if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN) {
        y = altura - y;
        if (x >= botaoX && x <= botaoX + botaoLargura &&
            y >= botaoY && y <= botaoY + botaoAltura)
        {
            telaAtual = JOGO;
            pontos     = 0;
            vidas      = 5;
            barraX     = largura / 2.0f;
            iniciarObjetos();
            glutPostRedisplay();
        }
    }
}


// TECLADO
void teclado(unsigned char tecla, int x, int y)
{
    // ESC fecha o jogo em qualquer tela
    if (tecla == 27)
        exit(0);

    // Controles dependentes do estado atual
    if (telaAtual == JOGO) {
        if (tecla == 'p' || tecla == 'P') {
            telaAtual = PAUSADO;
        }
        if (tecla == 'm' || tecla == 'M') {
            telaAtual = MENU;
        }
    }
    else if (telaAtual == PAUSADO) {
        if (tecla == 'p' || tecla == 'P') {
            telaAtual = JOGO;
        }
        if (tecla == 'm' || tecla == 'M') {
            telaAtual = MENU;
        }
    }
    else if (telaAtual == GAME_OVER) {
        if (tecla == 'r' || tecla == 'R') {
            // Reinicia o jogo resetando as variáveis
            pontos = 0; 
            vidas = 5;
            barraLargura = 160.0f;
            barraX = largura / 2.0f;
            iniciarObjetos();
            telaAtual = JOGO;
        }
        if (tecla == 'm' || tecla == 'M') {
            telaAtual = MENU;
        }
    }

    // Impede a barra de sair da tela (mantido do seu original)
    if (barraX - barraLargura / 2 < 0) barraX = barraLargura / 2;
    if (barraX + barraLargura / 2 > largura) barraX = largura - barraLargura / 2;
    
    glutPostRedisplay();
}

void tecladoEspecial(int tecla, int x, int y)
{
    if (tecla == GLUT_KEY_LEFT)  
        teclaEsquerda = true;
    if (tecla == GLUT_KEY_RIGHT) 
        teclaDireita  = true;
}

void tecladoEspecialsolta(int tecla, int x, int y)
{
    if (tecla == GLUT_KEY_LEFT)
        teclaEsquerda = false;

    if (tecla == GLUT_KEY_RIGHT)
        teclaDireita = false;
}


// OPENGL INIT
void init()
{
    glClearColor(0, 0.3f, 0.7f, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, largura, 0, altura);
}

void redimensionar(int w, int h)
{
    largura = w; altura = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, largura, 0, altura);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(largura, altura);
    glutCreateWindow("Catch Game");
    glutFullScreen();

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(tecladoEspecial);
    glutSpecialUpFunc(tecladoEspecialsolta);
    glutReshapeFunc(redimensionar);
    glutTimerFunc(16, timer, 0);   
    iniciarObjetos();

    glutMainLoop();
    return 0;
}