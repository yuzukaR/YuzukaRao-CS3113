#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 20

unsigned int LEVEL_DATAB[] = {
    293, 294, 294, 294, 294, 316, 266, 267, 267, 266, 266, 267, 267, 266,
    266, 267, 266, 267, 267, 314, 266, 266, 267, 266, 267, 266, 267, 266,
    267, 266, 267, 266, 267, 314, 267, 315, 294, 294, 294, 162, 267, 266,
    266, 293, 316, 267, 266, 160, 267, 314, 267, 267, 267, 267, 267, 266,
    266, 266, 161, 294, 294, 295, 267, 161, 294, 294, 316, 266, 266, 266,
    266, 267, 267, 267, 267, 160, 266, 266, 267, 267, 314, 267, 266, 267,
    292, 266, 267, 266, 293, 294, 267, 267, 266, 294, 318, 267, 266, 266,
    314, 267, 293, 294, 294, 294, 294, 295, 266, 267, 314, 266, 266, 267,
    314, 267, 266, 267, 266, 267, 267, 266, 266, 267, 164, 295, 266, 266,
    314, 267, 267, 266, 267, 266, 266, 266, 267, 267, 314, 267, 267, 267,
    161, 294, 294, 294, 294, 294, 294, 295, 267, 267, 314, 266, 266, 267,
    267, 266, 266, 293, 294, 294, 294, 295, 267, 267, 314, 266, 266, 266,
    266, 267, 266, 267, 267, 267, 266, 266, 267, 266, 160, 266, 267, 267,
    266, 267, 292, 266, 267, 266, 266, 267, 266, 293, 295, 267, 267, 266,
    266, 267, 314, 266, 293, 294, 294, 294, 294, 294, 294, 295, 267, 266,
    267, 266, 161, 294, 295, 267, 266, 293, 295, 267, 292, 293, 295, 267,
    267, 267, 267, 266, 315, 295, 267, 267, 266, 267, 314, 267, 266, 296,
    293, 295, 266, 293, 162, 267, 267, 292, 266, 316, 294, 294, 295, 266,
    266, 266, 266, 266, 267, 266, 267, 164, 294, 162, 266, 267, 267, 266,
    266, 266, 267, 266, 267, 267, 267, 160, 266, 266, 266, 267, 267, 267
};

LevelB::~LevelB()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/images/TilesetFloor_5.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATAB, map_texture_id, 1.0f, 22, 28);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(0.0f, -18.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    //m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->change_screen = false;
    m_state.next_scene_id = 2;
    m_state.player->m_texture_id = Utility::load_texture("assets/images/215080.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 8, 9, 10, 11 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 4, 5, 6, 7 };
    m_state.player->m_walking[m_state.player->UP] = new int[4] { 12, 13, 14, 15 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 0, 1, 2, 3 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    //m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/images/Enemy.gif");
    m_state.enemies = new Entity[ENEMY_COUNT];
    /*

    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(PATROLLING);
    //m_state.enemies[0].set_ai_state(PATROLLING);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(1.0f);
    */
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_ai_type(PATROLLING);
        m_state.enemies[i].m_texture_id = enemy_texture_id;
        glm::vec3 start_position;
        glm::vec3 patrol_point;

        if (i == 0) {
            start_position = glm::vec3(5.0f, -17.0f, 0.0f);
            patrol_point = glm::vec3(5.0f, -19.0f, 0.0f);

        }
        else if (i == 1) {
            start_position = glm::vec3(0.5f, -12.0f, 0.0f);
            patrol_point = glm::vec3(9.0f, -12.0f, 0.0f);

        }
        else if (i == 2) {
            start_position = glm::vec3(8.0f, -7.0f, 0.0f);
            patrol_point = glm::vec3(8.0f, -9.0f, 0.0f);

        }
        else if (i == 3) {
            start_position = glm::vec3(4.0f, -8.0f, 0.0f);
            patrol_point = glm::vec3(4.0f, -9.0f, 0.0f);

        }
        else if (i == 4) {
            start_position = glm::vec3(0.0f, -5.0f, 0.0f);
            patrol_point = glm::vec3(4.0f, -5.0f, 0.0f);

        }
        else if (i == 5) {
            start_position = glm::vec3(8.0f, -3.0f, 0.0f);
            patrol_point = glm::vec3(13.0f, -3.0f, 0.0f);

        }
        else if (i == 6) {
            start_position = glm::vec3(12.0f, -6.0f, 0.0f);
            patrol_point = glm::vec3(12.0f, -12.0f, 0.0f);

        }
        m_state.enemies[i].set_position(start_position);
        m_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_state.enemies[i].set_speed(2.0f);
        m_state.enemies[i].m_patrol_point = patrol_point;
        m_state.enemies[i].m_start_position = start_position;
    }

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/audio/Background.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(128 / 16.0f);

    m_state.jump_sfx = Mix_LoadWAV("assets/audio/levelup.wav");
}

void LevelB::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }
}


void LevelB::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].render(program);
    }
}
