#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include <SDL.h>
#include "glm/gtc/matrix_transform.hpp"

SDL_Window* ventana = nullptr;
SDL_Renderer* renderizador = nullptr;
const int ANCHO_PANTALLA = 690;
const int ALTO_PANTALLA = 500;
float anguloRotacion = 0.0f;
Uint32 tiempoInicio = 0;


struct Vertice {
    glm::vec3 posicion;
    glm::vec3 normal;
    glm::vec2 coordenadasTextura;
};

struct Cara {
    std::vector<unsigned int> indicesVertices;
    std::vector<unsigned int> indicesUV;
    std::vector<unsigned int> indicesNormales;
};

std::vector<Vertice> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normales;
std::vector<Cara> caras;

bool cargarOBJ(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo) {
        std::cerr << "No se pudo cargar de forma correcta el archivo OBJ: " << rutaArchivo << std::endl;
        return false;
    }

    std::vector<glm::vec3> verticesTemp;
    std::vector<glm::vec2> uvsTemp;
    std::vector<glm::vec3> normalesTemp;
    std::vector<Cara> carasTemp;

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::istringstream iss(linea);
        std::string tipo;
        iss >> tipo;

        if (tipo == "v") {
            glm::vec3 vertice;
            iss >> vertice.x >> vertice.y >> vertice.z;
            verticesTemp.push_back(vertice);
        } else if (tipo == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            uvsTemp.push_back(uv);
        } else if (tipo == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normalesTemp.push_back(normal);
        } else if (tipo == "f") {
            Cara cara;
            char barra;
            unsigned int indiceVertice, indiceUV, indiceNormal;
            for (int i = 0; i < 3; i++) {
                iss >> indiceVertice >> barra >> indiceUV >> barra >> indiceNormal;
                cara.indicesVertices.push_back(indiceVertice - 1);
                cara.indicesUV.push_back(indiceUV - 1);
                cara.indicesNormales.push_back(indiceNormal - 1);
            }
            carasTemp.push_back(cara);
        }
    }

    archivo.close();

    // Copiar los datos a las variables globales
    vertices.resize(verticesTemp.size());
    uvs.resize(uvsTemp.size());
    normales.resize(normalesTemp.size());

    for (size_t i = 0; i < verticesTemp.size(); i++) {
        vertices[i].posicion = verticesTemp[i];
    }

    for (size_t i = 0; i < uvsTemp.size(); i++) {
        uvs[i] = uvsTemp[i];
    }

    for (size_t i = 0; i < normalesTemp.size(); i++) {
        normales[i] = normalesTemp[i];
    }

    caras.assign(carasTemp.begin(), carasTemp.end());

    return true;
}

void inicializar() {
    SDL_Init(SDL_INIT_VIDEO);
    ventana = SDL_CreateWindow("Nave Espacial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ANCHO_PANTALLA, ALTO_PANTALLA, SDL_WINDOW_SHOWN);
    renderizador = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
}

void setColor(const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderizador, color.r, color.g, color.b, color.a);
}

void limpiar() {
    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
    SDL_RenderClear(renderizador);
}

void punto(int x, int y) {
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderizador, x, y);
}

void renderizar() {
    // Calcular la matriz de transformación de escala
    glm::mat4 matrizEscala = glm::scale(glm::mat4(1.0f), glm::vec3(0.08f)); // Escala de 0.01
    glm::mat4 matrizRotacion = glm::rotate(glm::mat4(1.0f), anguloRotacion, glm::vec3(0.0f, 1.0f, 0.02f));
    glm::mat4 matrizModelo = glm::translate(glm::mat4(1.0f), glm::vec3(0.06f, -0.1f, 0.0f)) * matrizRotacion * matrizEscala;


    // Dibujar los triángulos del modelo OBJ
    for (const Cara& cara : caras) {
        for (size_t i = 0; i < cara.indicesVertices.size(); i += 3) {
            unsigned int indiceVertice1 = cara.indicesVertices[i];
            unsigned int indiceVertice2 = cara.indicesVertices[i + 1];
            unsigned int indiceVertice3 = cara.indicesVertices[i + 2];

            glm::vec3 vertice1 = vertices[indiceVertice1].posicion;
            glm::vec3 vertice2 = vertices[indiceVertice2].posicion;
            glm::vec3 vertice3 = vertices[indiceVertice3].posicion;

            // Aplicar la transformación de escala
            glm::vec4 verticeTransformado1 = matrizModelo * glm::vec4(vertice1, 1.0f);
            glm::vec4 verticeTransformado2 = matrizModelo * glm::vec4(vertice2, 1.0f);
            glm::vec4 verticeTransformado3 = matrizModelo * glm::vec4(vertice3, 1.0f);

            // Reflejar en el eje Y
            verticeTransformado1.y *= -1.0f;
            verticeTransformado2.y *= -1.0f;
            verticeTransformado3.y *= -1.0f;

            // Convertir las coordenadas de los vértices a espacio de pantalla
            int x1 = static_cast<int>(verticeTransformado1.x * ANCHO_PANTALLA / 2 + ANCHO_PANTALLA / 2);
            int y1 = static_cast<int>(verticeTransformado1.y * ALTO_PANTALLA / 2 + ALTO_PANTALLA / 2);

            int x2 = static_cast<int>(verticeTransformado2.x * ANCHO_PANTALLA / 2 + ANCHO_PANTALLA / 2);
            int y2 = static_cast<int>(verticeTransformado2.y * ALTO_PANTALLA / 2 + ALTO_PANTALLA / 2);

            int x3 = static_cast<int>(verticeTransformado3.x * ANCHO_PANTALLA / 2 + ANCHO_PANTALLA / 2);
            int y3 = static_cast<int>(verticeTransformado3.y * ALTO_PANTALLA / 2 + ALTO_PANTALLA / 2);

            // Dibujar el triángulo
            SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderizador, x1, y1, x2, y2);
            SDL_RenderDrawLine(renderizador, x2, y2, x3, y3);
            SDL_RenderDrawLine(renderizador, x3, y3, x1, y1);
        }
    }
}

int main(int argc, char** argv) {
    inicializar();
    bool ejecutando = true;
    tiempoInicio = SDL_GetTicks();

    if (!cargarOBJ("C:/Users/rodas/Desktop/CODING11/Repoitorios_GIT/CargaModelosOBJ_Grafica_C//nave.obj")) {
        std::cerr << "No se pudo cargar de forma correcta el archivo OBJ." << std::endl;
        SDL_DestroyRenderer(renderizador);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }

    while (ejecutando) {
        SDL_Event evento;
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                ejecutando = false;
            }
        }

        limpiar();
        const float velocidadRotacion = 0.0008f; // Puedes ajustar la velocidad de rotación aquí

        // Aplicar rotación solo después de 0.001 segundos
        if (SDL_GetTicks() - tiempoInicio > 1) {
            anguloRotacion += velocidadRotacion;
        }

        renderizar();

        SDL_RenderPresent(renderizador);
    }

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}
