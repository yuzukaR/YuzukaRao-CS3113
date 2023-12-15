 /**
* Author: Yuzuka Rao
* Assignment: Pong Clone
* Date due: 2023-10-21, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_GLEXT_PROTOTYPES 1
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

// �� NEW STUFF �� //
#include <ctime>   //
#include "cmath"   //
// ��������������� //

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.0f,
BG_BLUE = 0.0f,
BG_GREEN = 0.0f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
PLAYER_SPRITE_FILEPATH[] = "sans.jpg",
BALL_SPRITE_FILEPATH[] = "heart.png",
RIGHTWON_SPRITE_FILEPATH[] = "rightwin.jpg",
LEFTWON_SPRITE_FILEPATH[] = "leftwin.jpg";


const float MILLISECONDS_IN_SECOND = 1000.0;
const float MINIMUM_COLLISION_DISTANCE = 0.5f;

const int   NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

SDL_Window* g_display_window;
bool  g_game_is_running = true;
float g_previous_ticks = 0.0f;

ShaderProgram g_shader_program;
glm::mat4     g_view_matrix,
g_model_matrix,
g_projection_matrix,
g_other_model_matrix,
g_ball_model_matrix,
g_win_model_matrix;

GLuint g_player_texture_id,
g_ball_texture_id,
g_other_texture_id,
g_leftwon_texture_id,
g_rightwon_texture_id;

glm::vec3 g_player_position = glm::vec3(4.7f, 0.0f, 0.0f);
glm::vec3 g_player_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_oneplayer_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_other_position = glm::vec3(-4.7f, 0.0f, 0.0f);
glm::vec3 g_other_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(1.0f, 1.0f, 0.0f);
float ball_x = 0.0f;


float g_player_speed = 1.0f;
float g_other_speed = 1.0f;
float g_ball_speed = 1.5f;

bool ball_right = true;
bool ball_up = true;
bool oneplayer_up = true;

glm::vec3 g_player_scale = glm::vec3(0.5f, 1.0f, 0.0f),
g_other_scale = glm::vec3(0.5f, 1.0f, 0.0f),
g_ball_scale = glm::vec3(0.2f, 0.2f, 0.0f);

const float upper = 3.75f,
lower = -3.75f,
lefter = -5.0f,
righter = 5.0f;
bool one_player_mode = false;
bool game = true;
bool leftwon = false;
bool rightwon = false;


GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Collisions!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_model_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::scale(g_model_matrix, g_player_scale);

    g_other_model_matrix = glm::mat4(1.0f);
    g_other_model_matrix = glm::scale(g_other_model_matrix, g_other_scale);

    g_ball_model_matrix = glm::mat4(1.0f);
    g_ball_model_matrix = glm::scale(g_ball_model_matrix, g_ball_scale);
    
    g_win_model_matrix = glm::mat4(1.0f);
    g_win_model_matrix = glm::translate(g_win_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    g_win_model_matrix = glm::scale(g_win_model_matrix, glm::vec3(3.5f, 1.0f, 0.0f));

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    g_other_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);


    g_rightwon_texture_id = load_texture(RIGHTWON_SPRITE_FILEPATH);


    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    g_player_movement = glm::vec3(0.0f);
    g_other_movement = glm::vec3(0.0f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q:
                g_game_is_running = false;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    
    if (game) {
        if (!one_player_mode) {
            if (key_state[SDL_SCANCODE_UP] && g_player_position.y + g_player_scale.y / 2.0f < upper)
            {
                g_player_movement.y = 1.0f;
            }
            else if (key_state[SDL_SCANCODE_DOWN] && g_player_position.y - g_player_scale.y / 2.0f > 
                )
            {
                g_player_movement.y = -1.0f;
            }
        }

        if (key_state[SDL_SCANCODE_W] && g_other_position.y + g_other_scale.y / 2.0f < upper)
        {
            g_other_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_S] && g_other_position.y - g_other_scale.y / 2.0f > lower)
        {
            g_other_movement.y = -1.0f;
        }

        if (glm::length(g_player_movement) > 1.0f)
        {
            g_player_movement = glm::normalize(g_player_movement);
        }

        if (glm::length(g_other_movement) > 1.0f)
        {
            g_other_movement = glm::normalize(g_other_movement);
        }

        if (key_state[SDL_SCANCODE_T]) {
            one_player_mode = true;
        }
    }
   
}



void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    if (game) {
        g_model_matrix = glm::mat4(1.0f);


        if (one_player_mode) {
            float one_player_upper = fabs(g_player_position.y - upper) - ((g_player_scale.y + 0.2f) / 2.0f);
            float one_player_lower = fabs(g_player_position.y - lower) - ((g_player_scale.y + 0.2f) / 2.0f);

            if (one_player_upper < 0.0f || one_player_lower < 0.0f) {
                oneplayer_up = !oneplayer_up;
            }

            if (oneplayer_up) {
                g_player_movement.y = 1.0f;
            }
            else {
                g_player_movement.y = -1.0f;
            }

            g_player_position += g_player_movement * g_player_speed * delta_time;
            g_model_matrix = glm::translate(g_model_matrix, g_player_position);
            g_model_matrix = glm::scale(g_model_matrix, g_player_scale);
        }
        else {
            g_player_position += g_player_movement * g_player_speed * delta_time;
            g_model_matrix = glm::translate(g_model_matrix, g_player_position);
            g_model_matrix = glm::scale(g_model_matrix, g_player_scale);
        }

        g_other_model_matrix = glm::mat4(1.0f);
        g_other_position += g_other_movement * g_other_speed * delta_time;
        g_other_model_matrix = glm::translate(g_other_model_matrix, g_other_position);
        g_other_model_matrix = glm::scale(g_other_model_matrix, g_other_scale);



        g_ball_model_matrix = glm::mat4(1.0f);

        //float x_distance = fabs(x_coord_a - x_coord_b) - ((width_a + width_b) / 2.0f);
        //float y_distance = fabs(y_coord_a - y_coord_b) - ((height_a + height_b) / 2.0f);

        //right paddle and ball
        float x_distance_right = fabs(g_player_position.x - g_ball_position.x) - ((g_player_scale.x + g_ball_scale.x) / 2.0f);
        float y_distance_right = fabs(g_player_position.y - g_ball_position.y) - ((g_player_scale.y + g_ball_scale.y) / 2.0f);

        //left paddle and ball
        float x_distance_left = fabs(g_other_position.x - g_ball_position.x) - ((g_other_scale.x + g_ball_scale.x) / 2.0f);
        float y_distance_left = fabs(g_other_position.y - g_ball_position.y) - ((g_other_scale.y + g_ball_scale.y) / 2.0f);

        //ball upper and down
        float ball_distance_upper = fabs(g_ball_position.y - upper) - ((g_ball_scale.y + 0.2f) / 2.0f);
        float ball_distance_lower = fabs(g_ball_position.y - lower) - ((g_ball_scale.y + 0.2f) / 2.0f);

        //ball right and left
        float ball_distance_left = fabs(g_ball_position.x - lefter) - ((g_ball_scale.x + 0.2f) / 2.0f);
        float ball_distance_right = fabs(g_ball_position.x - righter) - ((g_ball_scale.x + 0.2f) / 2.0f);


        if (ball_distance_upper < 0.0f || ball_distance_lower < 0.0f) {
            ball_up = !ball_up;
        }

        if (x_distance_right < 0.0f && y_distance_right < 0.0f || (x_distance_left < 0.0f && y_distance_left < 0.0f))
        {
            ball_right = !ball_right;
        }

        if (ball_distance_left < 0.0f) {
            game = false;
            rightwon = true;
        }
        if (ball_distance_right < 0.0f) {
            game = false;
            leftwon = true;
        }

        if (ball_right) {
            g_ball_movement.x = 1.0f;
        }
        else {
            g_ball_movement.x = -1.0f;
        }

        if (ball_up) {
            g_ball_movement.y = 1.0f;
        }
        else {
            g_ball_movement.y = -1.0f;
        }


        if (glm::length(g_ball_movement) > 1.0f)
        {
            g_ball_movement = glm::normalize(g_ball_movement);
        }

        g_ball_position += g_ball_movement * g_ball_speed * delta_time;
        g_ball_model_matrix = glm::translate(g_ball_model_matrix, g_ball_position);
        g_ball_model_matrix = glm::scale(g_ball_model_matrix, g_ball_scale);
    }
   

}


void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    draw_object(g_model_matrix, g_player_texture_id);
    draw_object(g_ball_model_matrix, g_ball_texture_id);
    draw_object(g_other_model_matrix, g_other_texture_id);
    
    if (leftwon) {
        draw_object(g_win_model_matrix, g_leftwon_texture_id);
    }
    if (rightwon) {
        draw_object(g_win_model_matrix, g_rightwon_texture_id);
    }

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
