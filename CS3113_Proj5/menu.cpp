#include "menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8
GLuint complete;

menu::~menu()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void menu::initialise()
{

    m_state.player = new Entity();
    m_state.enemies = new Entity[ENEMY_COUNT];

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);


    m_state.bgm = Mix_LoadMUS("assets/audio/Background.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(128 / 16.0f);
    complete = Utility::load_texture("assets/fonts/font1.png");
}

void menu::update(float delta_time)
{
    /*
    *     m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }
    */

}


void menu::render(ShaderProgram* program)
{
    //m_state.map->render(program);
    m_state.player->render(program);
    Utility::draw_text(program, complete, "GOOD GAME", 0.4f, 0.05f, glm::vec3(0.5f, -2.0f, 0.0f));
    Utility::draw_text(program, complete, "Press enter to start", 0.4f, 0.05f, glm::vec3(0.5f,-4.0f,0.0f));
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].render(program);
    }
}
