#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>

using namespace std;
using namespace sf;

//Bloque de Vidrio
struct Bloque{
    bool estado = true;
    RectangleShape bloque;
}bloques[8][10];
//Pastilla
struct Pastilla{
    bool estado = false;
    RectangleShape pastilla;
}pastilla;
int auxPastilla;
//Cantidad de Bloques Activos
int cantBloques = 56;
//Bola
CircleShape bola;
//Direccion y Velocidad Bola
Vector2f velDirBola(-3,-3);
//Paleta
RectangleShape paleta;
//Detectar Colision
bool enColision = false;
//Estado del Juego
bool activo = true;
//Fuente y Texto
Font fuente;
Text texto;
//Auxiliares para Colores
int idCV = 1,idCH = 1;
//Sonidos
SoundBuffer paredSB,paletaSB,bloqueSB;
Sound paredS,paletaS,bloqueS;
//Timer/Contador
int timer = 0;

int main(){
    //Ventana
    RenderWindow ventana(VideoMode(650,500),"Arcanoid");
    ventana.setFramerateLimit(60);
    //Bola
    bola = CircleShape(5.f);
    bola.setOrigin(2.5f,2.5f);
    bola.setFillColor(Color::Blue);
    bola.setPosition(ventana.getSize().x/2,ventana.getSize().y/2);
    //Paleta
    paleta = RectangleShape(Vector2f(60,10));
    paleta.setOrigin(paleta.getSize().x/2,paleta.getSize().y/2);
    paleta.setFillColor(Color::Green);
    paleta.setPosition(ventana.getSize().x/2,ventana.getSize().y-20);
    //Fuente y Texto
    if(!fuente.loadFromFile("Fonts/ARCADE.TTF")){
        cout<<"Error al cargar la fuente."<<endl;
    }
    texto.setFont(fuente);
    //Sonidos
    if(!paredSB.loadFromFile("Sounds/ReboteParedes.wav") || !paletaSB.loadFromFile("Sounds/RebotePaleta.wav") || !bloqueSB.loadFromFile("Sounds/ReboteLadrillo.wav")){
        cout<<"Error al cargar los sonidos."<<endl;
    }
    paredS.setBuffer(paredSB);
    paletaS.setBuffer(paletaSB);
    bloqueS.setBuffer(bloqueSB);
    //Incilaizar Pastilla
    pastilla.pastilla = RectangleShape(Vector2f(30,10));
    pastilla.pastilla.setFillColor(Color::White);
    //Inicializacion de Bloques de Vidrio
    for(int i = 1; i < 8; i++){
        idCV*=-1;
        for(int j = 1; j < 9; j++){
            //Tamaño Bloques
            bloques[i][j].bloque = RectangleShape(Vector2f(65,10)); 
            //Posicion Bloques
            bloques[i][j].bloque.setPosition(j*66,(i+1)*11);
            //Colores Bloques
            if(idCV == 1){ //Cian Verde
                if(idCH == 1){
                    bloques[i][j].bloque.setFillColor(Color::Cyan);
                }else{
                    bloques[i][j].bloque.setFillColor(Color::Green);
                }
            }else{ //Amarillo y Magenta
                if(idCH == 1){
                    bloques[i][j].bloque.setFillColor(Color::Yellow);
                }else{
                    bloques[i][j].bloque.setFillColor(Color::Magenta);
                }
            }
            idCH*=-1;
        }
    }

    while(ventana.isOpen()){
        Event event;
        while(ventana.pollEvent(event)){
            if(event.type == Event::Closed){
                ventana.close();
            }
        }
        timer++;
        if(activo){ //Si esta activo el juego ejecutamos el codigo
            //Rebote Bola
            if(bola.getPosition().x >= ventana.getSize().x || bola.getPosition().x <= 0){
                //Cambiamos Direccion en X
                velDirBola.x *= -1;
                paredS.play();
            }
            if(bola.getPosition().y <= 0){
                //Cambiamos Direccion en Y
                velDirBola.y *= -1;
                paredS.play();
            }
            if(bola.getPosition().y >= ventana.getSize().y){
                //Perdio el juego
                activo = false;
                texto.setString("Perdiste");
                texto.setPosition(200,ventana.getSize().y/2);
            }
            //Movimiento Paleta
            if(Keyboard::isKeyPressed(Keyboard::Right) && paleta.getPosition().x <= ventana.getSize().x){
                paleta.move(3.5,0);
            }
            if(Keyboard::isKeyPressed(Keyboard::Left) && paleta.getPosition().x  >= 0){
                paleta.move(-3.5,0);
            }
            //Movimiento Bola
            bola.move(velDirBola.x,velDirBola.y);
            //Colision Pastilla
            if(pastilla.estado && paleta.getGlobalBounds().contains(pastilla.pastilla.getPosition().x,pastilla.pastilla.getPosition().y)){
                paleta.setScale(2,1);
                pastilla.estado = false;
                timer = 0;
            }
            //Colisiones Bola
            if(!enColision){
                //Colision Bola Paleta
                if(paleta.getGlobalBounds().contains(bola.getPosition().x,bola.getPosition().y)){
                    if(paleta.getPosition().x-bola.getPosition().x >= 20){ //Rebote Lado Izquierdo
                        velDirBola.x = -3;
                    }else if(paleta.getPosition().x-bola.getPosition().x <= -20){ //Rebote Lado Derecho
                        velDirBola.x = 3;
                    }else{ //Rebote en el Medio
                        if(velDirBola.x > 0){
                            velDirBola.x = 1.5;
                        }else{
                            velDirBola.x = -1.5;
                        }
                    }
                    velDirBola.y *= -1; //Cambiar Direccion Y
                    enColision = true;
                    paletaS.play();
                }
                //Colision Bola Bloque Vidrio
                for(int i = 1; i < 8; i++){
                    for(int j = 1; j < 9; j++){
                        if(bloques[i][j].estado && bloques[i][j].bloque.getGlobalBounds().contains(bola.getPosition().x,bola.getPosition().y)){
                            velDirBola.y*=-1;
                            bloques[i][j].estado = false;
                            enColision = true;
                            cantBloques--;
                            bloqueS.play();
                            //Pastilla Aleatoria
                            srand(time(NULL));
                            auxPastilla = 1+rand()%6; //Num Aleatorio 1-5
                            if(auxPastilla == 1 && !pastilla.estado){
                                pastilla.estado = true;
                                pastilla.pastilla.setPosition(bloques[i][j].bloque.getPosition().x,bloques[i][j].bloque.getPosition().y);
                            }
                        }
                    }
                }
            }else{
                enColision = false;
                if(paleta.getGlobalBounds().contains(bola.getPosition().x,bola.getPosition().y)){
                    enColision = true;
                }
                for(int i = 1; i < 8; i++){
                    for(int j = 1; j < 9; j++){
                        if(bloques[i][j].estado && bloques[i][j].bloque.getGlobalBounds().contains(bola.getPosition().x,bola.getPosition().y)){
                            enColision = true;
                            break;
                        }
                    }
                    if(enColision){
                        break;
                    }
                }
            }
        }
        //Movimiento Pastilla
        if(pastilla.estado){
            pastilla.pastilla.move(0,2.5f);
        }
        //Comprobar Efecto Pastilla
        if(timer >= 600){
            paleta.setScale(1,1);
        }
        //Comprobar si quedan bloques
        if(cantBloques <= 0){
            activo = false;
            texto.setString("Ganaste");
            texto.setPosition(200,ventana.getSize().y/2);
        }
        //Limpiar Pantalla
        ventana.clear();
        if(activo){
            //Dibujar Pantalla
            ventana.draw(bola);
            //Dibujar Paleta
            ventana.draw(paleta);
            //Dibujar Vitral de Bloques
            for(int i = 1; i < 8; i++){
                for(int j = 1; j < 9; j++){
                    if(bloques[i][j].estado){
                        ventana.draw(bloques[i][j].bloque);
                    }
                }
            }
            //Dibujar Pastilla
            if(pastilla.estado){
                ventana.draw(pastilla.pastilla);
            }        
        }else{
            ventana.draw(texto);
        }
        //Mostrar Pantalla
        ventana.display();
    }
    return 0;
}
