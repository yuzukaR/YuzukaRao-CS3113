/**
* Author: [Yuzuka Rao]
* Assignment: Simple 2D Scene
* Date due: 2023-09-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION

#define LOG(argument) std::cout << argument << '\n'

#include "glm/mat4x4.hpp"                
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"             
#include "stb_image.h"

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

//background color
const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;


const char TRIANGLE_SPRITE[] = "C:/Users/rao18/source/repos/SDLSimple/CS31131/DanHeng1.jpg";
const char RECTANGLE_SPRITE[] = "C:/Users/rao18/source/repos/SDLSimple/CS31131/DanHeng2.jpg";

// Our viewport—or our "camera"'s—position and dimensions
const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

// Rotation stuff
float ROT_ANGLE = 0.0f;

// Translation stuff 
float TRAN_VALUE = 0.0f;

float g_previous_ticks = 0.0f;

const float MILLISECONDS_IN_SECOND = 1000.0;
const float DEGREES_PER_SECOND = 90.0f;

//object color triangle
const float TRIANGLE_RED = 1.0,
TRIANGLE_BLUE = 0.4,
TRIANGLE_GREEN = 0.4,
TRIANGLE_OPACITY = 1.0;

//object color rectangle
const float RECTANGLE_RED = 1.0,
RECTANGLE_BLUE = 0.4,
RECTANGLE_GREEN = 0.4,
RECTANGLE_OPACITY = 1.0;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_is_growing = true;
int g_frame_counter = 0;
float scale_factor = 1.0f;

ShaderProgram g_shader_program;

glm::mat4 g_triangle_matrix,
g_rectangle_matrix,
g_view_matrix,
g_projection_matrix;

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;   // this value MUST be zero

GLuint triangle_texture_id, rectangle_texture_id;

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        ("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Hello, Triangle!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    // Load up our shaders
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // Initialise our view, model, and projection matrices
    g_view_matrix = glm::mat4(1.0f);  
    g_triangle_matrix = glm::mat4(1.0f);  
    g_rectangle_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f); 


    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    g_shader_program.set_colour(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    rectangle_texture_id = load_texture(RECTANGLE_SPRITE);
    triangle_texture_id = load_texture(TRIANGLE_SPRITE);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_game_is_running = false;
        }
    }
}

void update() {
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    g_frame_counter += 1;
    
    float growth_time = 5.0f;

    if (g_frame_counter >= growth_time) {
        g_frame_counter = 0;
        g_is_growing = !g_is_growing;
    }


    if (g_is_growing) {
        scale_factor += 1.0f * delta_time;
    }
    else {
        scale_factor -= 1.0f * -delta_time;

    }
    /*
    *     scale_vector = glm::vec3(g_is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
        g_is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
        1.0f);
    */
    glm::vec3 scale_vector = glm::vec3(scale_factor, scale_factor, 1.0f);

    TRAN_VALUE += 1.0f * delta_time;
    ROT_ANGLE += DEGREES_PER_SECOND * delta_time;
    g_triangle_matrix = glm::mat4(1.0f);
    g_triangle_matrix = glm::translate(g_triangle_matrix, glm::vec3(TRAN_VALUE, TRAN_VALUE, 0.0f));


    g_rectangle_matrix = glm::mat4(1.0f);
    g_rectangle_matrix = glm::translate(g_triangle_matrix, glm::vec3(-1 + TRAN_VALUE, -3 + TRAN_VALUE, 0.0f));
    g_rectangle_matrix = glm::rotate(g_rectangle_matrix, glm::radians(ROT_ANGLE), glm::vec3(0.0f, 0.0f, 1.0f));
    g_triangle_matrix = glm::scale(g_triangle_matrix, scale_vector);
       
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    

    float vertices2[] = {
       -1.5f, -1.0f,  
        -0.5f, -1.0f,  
        -0.5f,  1.0f,  
        -1.5f, -1.0f,  
        -0.5f,  1.0f,  
        -1.5f,  1.0f  
    };

    float texture_coordinates2[] = {
        0.0f, 1.0f,  
        1.0f, 1.0f,  
        1.0f, 0.0f,  
        0.0f, 1.0f,  
        1.0f, 0.0f, 
        0.0f, 0.0f   
    };

    g_shader_program.set_model_matrix(g_rectangle_matrix);
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices2);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates2);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    draw_object(g_rectangle_matrix, rectangle_texture_id);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    float vertices1[] = {
        -0.5f, -1.5f,  
         0.5f, -1.5f,  
         0.5f, -0.5f,  
        -0.5f, -1.5f,  
         0.5f, -0.5f,  
        -0.5f, -0.5f 
    };

    float texture_coordinates1[] = {
         0.0f, 1.0f,  
        1.0f, 1.0f,  
        1.0f, 0.0f,  
        0.0f, 1.0f,  
        1.0f, 0.0f,  
        0.0f, 0.0f    
    };
    g_shader_program.set_model_matrix(g_triangle_matrix);

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices1);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates1);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    draw_object(g_triangle_matrix, triangle_texture_id);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());


    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { 
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    // Initialise our program—whatever that means
    initialise();

    while (g_game_is_running)
    {
        process_input();  // If the player did anything—press a button, move the joystick—process it
        update();         // Using the game's previous state, and whatever new input we have, update the game's state
        render();         // Once updated, render those changes onto the screen
    }

    shutdown();  // The game is over, so let's perform any shutdown protocols
    return 0;
}