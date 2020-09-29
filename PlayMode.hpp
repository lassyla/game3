#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Load.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//game state
	size_t good_count = 0; 
	size_t ok_count = 0; 
	size_t bad_count = 0; 
	enum command {punch=0, poke=1, pat=2, pinch=3, none=4};
	command current_hand= punch; 
	command current_area = punch; 
	command current_command = none; 
	float time_passed = -.71f; 
	float beat_time_passed = -.71f; 
	float beat_time = 1.4f; 
	float music_time = 72.0f; 
	size_t num_beats = 0; 
	bool hit = false; 
	bool game_over = false; 
	glm::vec3 hand_position; 
	glm::vec3 hidden_position = glm::vec3(0.0f, 50.0f, 0.0f); 
	glm::vec3 bg_position = glm::vec3(0.0f, 7.0f, 0.0f); 
	glm::vec3 letter_position = glm::vec3(0.0f, -20.0f, 0.0f); 

	bool jabbing = false;
	float jab_time = .3f; 
	float jab_time_passed = 0.0f; 
	float jab_magnitude = 2.0f; 

	float scale_threshold = .1f; 
	float scale_amount = 0.2f; 

	//how lenient is the scoring
	float good_threshold = .1f; 
	//pi 
	float pi = 3.1415926f;

	//Objects in scene
	Scene::Transform *punch_part = nullptr;
	Scene::Transform *poke_part = nullptr;
	Scene::Transform *pat_part = nullptr;
	Scene::Transform *pinch_part = nullptr;
	Scene::Transform *base = nullptr;
	Scene::Transform *parts[4]; 

	Scene::Transform *punch_hand = nullptr;
	Scene::Transform *poke_hand = nullptr;
	Scene::Transform *pat_hand = nullptr;
	Scene::Transform *pinch_hand = nullptr;
	Scene::Transform *hand = nullptr;

	Scene::Transform *punch_bg = nullptr;
	Scene::Transform *poke_bg = nullptr;
	Scene::Transform *pat_bg = nullptr;
	Scene::Transform *pinch_bg = nullptr;
	Scene::Transform *plain_bg = nullptr;
	Scene::Transform *bgs[5]; 
	Scene::Transform *bg = nullptr;

	Scene::Transform *good_face = nullptr;
	Scene::Transform *ok_face = nullptr;
	Scene::Transform *bad_face = nullptr;

	Scene::Transform *a_letter = nullptr;
	Scene::Transform *b_letter = nullptr;
	Scene::Transform *c_letter = nullptr;
	Scene::Transform *d_letter = nullptr;
	Scene::Transform *f_letter = nullptr;


	float face_speed = 10.0f; 

	glm::vec3 get_leg_tip_position();

	
	//SFX: 
	Load<Sound::Sample> *good_samples[4]; 
	Load<Sound::Sample> *bad_samples[4]; 
	Load<Sound::Sample> *command_samples[4]; 

	//camera:
	Scene::Camera *camera = nullptr;

};
