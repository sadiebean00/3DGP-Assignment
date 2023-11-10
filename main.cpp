/******************************************************
     * Main.cpp
     * This is where all of the code is thrown together
     * We utilise the Vertex Buffer and Vertex Array header files in here
     ******************************************************/
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define STB_IMAGE_IMPLEMENTATION

#include "VertexBuffer.h"
#include "VertexArray.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <iostream>
#include <memory>

using namespace std;

float deltaTs;
float lastTime;
float angle;
float rotation;
float rotY;

glm::vec3 position(0, 0, -10);

const GLfloat colors[] = {
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f
};


int main()
{
    /******************************************************
     * Create the OpenGL window
     ******************************************************/
    SDL_Window* window = SDL_CreateWindow("OpenGL Test",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    if (!SDL_GL_CreateContext(window))
    {
        throw std::exception();
    }

    if (glewInit() != GLEW_OK)
    {
        throw std::exception();
    }
    /******************************************************
     * Create position buffer
     ******************************************************/
    std::shared_ptr<VertexBuffer> positionsVbo = std::make_shared<VertexBuffer>();
    positionsVbo->add(glm::vec3(0.0f, 0.5f, 0.0f));
    positionsVbo->add(glm::vec3(-0.5f, -0.5f, 0.0f));
    positionsVbo->add(glm::vec3(0.5f, -0.5f, 0.0f));
    /******************************************************
     * Create color buffer
     ******************************************************/
    std::shared_ptr<VertexBuffer> colorsVbo = std::make_shared<VertexBuffer>();
    colorsVbo->add(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colorsVbo->add(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colorsVbo->add(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    /******************************************************
     * Create vertex array which stores the model
     ******************************************************/
    std::shared_ptr<VertexArray> car =
       std::make_shared<VertexArray>("models/flyingford/skWhompedAngliaLowMesh/skWhompedAngliaLowMesh.obj");
    /******************************************************
     * Create vertex shader
     ******************************************************/
    const GLchar* vertexShaderSrc =
        "attribute vec3 a_Position;"\
        "uniform mat4 u_Projection;"\
        "uniform mat4 u_Model;"\
        "attribute vec2 a_TexCoord;"\
        "varying vec2 v_TexCoord;"\
        "uniform mat4 u_View;"\
        "                    "\
        "void main()"\
        "{"\
        "  gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);"\
        "  v_TexCoord = a_TexCoord;"\
        "}";

    // Create a new vertex shader, attach source code, compile it and
    // check for errors.
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShaderId);
    GLint success = 0;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        throw std::exception();
    }
    /******************************************************
     * Create fragment shader
     ******************************************************/
    const GLchar* fragmentShaderSrc =
        "uniform sampler2D in_Texture;             " \
        "varying vec2 v_TexCoord;             " \
        "void main()                       " \
        "{                                 " \
        "  vec4 tex = texture2D(in_Texture, v_TexCoord);"\
        "  gl_FragColor = tex; " \
        "}                                 ";

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        throw std::exception();
    }
    /******************************************************
     * Create textures for the objects
     ******************************************************/
    int w = 0;
    int h = 0;

    unsigned char* data = stbi_load("models/flyingford/skWhompedAngliaLowMesh/skWhompedAngliaLowTex0.png", &w, &h, NULL, 4);

    if (!data)
    {
        throw std::exception();
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    if (!textureID)
    {
        throw std::exception();
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    free(data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    /******************************************************
     * Create shader program for our objects
     ******************************************************/
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glBindAttribLocation(programId, 0, "a_Position");

    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    if (!success)
    {
        throw std::exception();
    }

    glDetachShader(programId, vertexShaderId);
    glDeleteShader(vertexShaderId);
    glDetachShader(programId, fragmentShaderId);
    glDeleteShader(fragmentShaderId);

    GLint modelLoc = glGetUniformLocation(programId, "u_Model");
    GLint projectionLoc = glGetUniformLocation(programId, "u_Projection");
    GLint texLoc = glGetUniformLocation(programId, "in_Texture");
    GLint viewLoc = glGetUniformLocation(programId, "u_View");

    /******************************************************
     * Main loop
     ******************************************************/
    bool stopped = false;
    float pulse = 0;

    while (!stopped)
    {
        SDL_Event event = { 0 };

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                stopped = true;
            }

            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    std::cout << "Up" << std::endl;
                }

                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    std::cout << "Down" << std::endl;
                }

                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    std::cout << "Left" << std::endl;
                }

                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    std::cout << "Right" << std::endl;
                }
            }
        }
        
        float now = SDL_GetTicks();
        deltaTs = (now - lastTime) / 1000.0f;
        lastTime = now;
        /******************************************************
     * Set up the projection, model and view matrices
     ******************************************************/
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

       projection = glm::ortho(0.0f,
            (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.0f, 1.0f);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(400, WINDOW_HEIGHT - 350, 0));
        model = glm::scale(model, glm::vec3(1, 1, 0));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));

        glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 10.0f, 0.0f));

        glClearColor(0, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        angle += 0.1f;
        /******************************************************
     * Tells the main loop to use the program id and where the different locations are
     ******************************************************/
        glUseProgram(programId);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::inverse(view)));
        /******************************************************
       * Draw the scene
       ******************************************************/
        glBindVertexArray(car->getId());
        glBindTexture(GL_TEXTURE_2D, textureID);

        glDrawArrays(GL_TRIANGLES, 0, car->getVertCount());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        SDL_GL_SwapWindow(window);
    }
    return 0;
}