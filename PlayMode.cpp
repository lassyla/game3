#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Sound.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint bopit_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > bopit_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("bopit.pnct"));
	bopit_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > bopit_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("bopit.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = bopit_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = bopit_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > city_sunshine_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("City Sunshine.wav"));
});

Load< Sound::Sample > pat_bad_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("pat_bad.wav"));
});

PlayMode::PlayMode() : scene(*bopit_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "punch") punch_hand = &transform;
		else if (transform.name == "poke") poke_hand = &transform;
		else if (transform.name == "pat") pat_hand = &transform;
		else if (transform.name == "pinch") pinch_hand = &transform;
		else if (transform.name == "punch_bg") punch_bg = &transform;
		else if (transform.name == "poke_bg") poke_bg = &transform;
		else if (transform.name == "pat_bg") pat_bg = &transform;
		else if (transform.name == "pinch_bg") pinch_bg = &transform;
		else if (transform.name == "plain_bg") plain_bg = &transform;
		else if (transform.name == "punch_part") punch_part = &transform;
		else if (transform.name == "poke_part") poke_part = &transform;
		else if (transform.name == "pat_part") pat_part = &transform;
		else if (transform.name == "pinch_part") pinch_part = &transform;
		else if (transform.name == "base") base = &transform;
		else if (transform.name == "good_face") good_face = &transform;
		else if (transform.name == "ok_face") ok_face = &transform;
		else if (transform.name == "bad_face") bad_face = &transform;
		else if (transform.name == "A") a_letter = &transform;
		else if (transform.name == "B") b_letter = &transform;
		else if (transform.name == "C") c_letter = &transform;
		else if (transform.name == "D") d_letter = &transform;
		else if (transform.name == "F") f_letter = &transform;
	}
	if (punch_hand == nullptr) throw std::runtime_error("punch not found.");
	if (poke_hand == nullptr) throw std::runtime_error("poke not found.");
	if (pat_hand == nullptr) throw std::runtime_error("pat not found.");
	if (pinch_hand == nullptr) throw std::runtime_error("pinch not found.");
	if (pinch_bg == nullptr) throw std::runtime_error("pinch bg not found.");
	if (punch_bg == nullptr) throw std::runtime_error("punch bg not found.");
	if (poke_bg == nullptr) throw std::runtime_error("poke bg not found.");
	if (pat_bg == nullptr) throw std::runtime_error("pat bg not found.");
	if (plain_bg == nullptr) throw std::runtime_error("plain bg not found.");
	if (pinch_part == nullptr) throw std::runtime_error("pinch part not found.");
	if (punch_part == nullptr) throw std::runtime_error("punch part not found.");
	if (poke_part == nullptr) throw std::runtime_error("poke part not found.");
	if (pat_part == nullptr) throw std::runtime_error("pat part not found.");
	if (base == nullptr) throw std::runtime_error("base not found.");
	if (good_face == nullptr) throw std::runtime_error("good_face not found.");
	if (ok_face == nullptr) throw std::runtime_error("ok_face not found.");
	if (bad_face == nullptr) throw std::runtime_error("bad_face not found.");
	if (a_letter == nullptr) throw std::runtime_error("A part not found.");
	if (b_letter == nullptr) throw std::runtime_error("B not found.");
	if (c_letter == nullptr) throw std::runtime_error("C not found.");
	if (d_letter == nullptr) throw std::runtime_error("D not found.");
	if (f_letter == nullptr) throw std::runtime_error("F not found.");

	hand = punch_hand; 

	bgs[0] = punch_bg; 
	bgs[1] = poke_bg; 
	bgs[2] = pat_bg; 
	bgs[3] = pinch_bg; 
	bgs[4] = plain_bg; 

	parts[0] = punch_part; 
	parts[1] = poke_part; 
	parts[2] = pat_part; 
	parts[3] = pinch_part; 

	bg = bgs[4]; 

	poke_hand->position = hidden_position; 
	pat_hand->position = hidden_position; 
	pinch_hand->position = hidden_position; 
	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	
	bad_samples[0] = &pat_bad_sample; 
	bad_samples[1] = &pat_bad_sample; 
	bad_samples[2] = &pat_bad_sample; 
	bad_samples[3] = &pat_bad_sample; 
	
	good_samples[0] = &pat_bad_sample; 
	good_samples[1] = &pat_bad_sample; 
	good_samples[2] = &pat_bad_sample; 
	good_samples[3] = &pat_bad_sample; 

	command_samples[0] = &pat_bad_sample; 
	command_samples[1] = &pat_bad_sample; 
	command_samples[2] = &pat_bad_sample; 
	command_samples[3] = &pat_bad_sample; 

	//start music playing:
	Sound::play(*city_sunshine_sample, 1.0f, 1.0f);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_q) {
			hand->position = hidden_position; 
			current_hand = punch; 
			hand = punch_hand; 
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			hand->position = hidden_position; 
			current_hand = poke; 
			hand = poke_hand; 
			return true;
		} else if (evt.key.keysym.sym == SDLK_e) {
			hand->position = hidden_position; 
			current_hand = pat; 
			hand = pat_hand; 
			return true;
		} else if (evt.key.keysym.sym == SDLK_r) {
			hand->position = hidden_position; 
			current_hand = pinch; 
			hand = pinch_hand; 
			return true;
		}
	} 
	else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
		if(!jabbing) {
			jabbing = true; 
			jab_time_passed = 0.0f;
		}

		return true;
	} 
	else if (evt.type == SDL_MOUSEMOTION) {
		//if(jabbing) return true; 
		
		//move hand to follow the mouse
		//raycast to plane code + guide from this website https://antongerdelan.net/opengl/raycasting.html

		//mouse to clip space taken from game0 code https://github.com/15-466/15-466-f20-base0/blob/master/PongMode.cpp  
		//convert mouse from window pixels (top-left origin, +y is down) to clip space ([-1,1]x[-1,1], +y is up):
		glm::vec2 clip_mouse = glm::vec2(
			(evt.motion.x + 0.5f) / window_size.x * 2.0f - 1.0f,
			(evt.motion.y + 0.5f) / window_size.y *-2.0f + 1.0f
		);
		glm::vec4 ray_clip = glm::vec4(clip_mouse.x,clip_mouse.y, 1.0,  1.0);
		glm::vec4 ray_eye = glm::inverse(camera->make_projection()) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0,  0.0); 
		glm::vec3 ray_wor = glm::vec3(glm::mat4(camera->transform->make_local_to_world()) * ray_eye);
		ray_wor = glm::normalize(ray_wor);

		//find intersection with the y=-15.0 plane
		float t = (-15.0f - camera->transform->position.y) / ray_wor.y; 
		hand_position.y = -15.0f; 
		hand_position.x = t * ray_wor.x + camera->transform->position.x; 
		hand_position.z = t * ray_wor.z + camera->transform->position.z; 
		
		return true;
	}

	return false;
}

void PlayMode::update(float elapsed) {
	//change the position of active hand 
	hand->position = hand_position; 

	//faces float up 
	ok_face->position.z += elapsed * face_speed; 
	good_face->position.z += elapsed * face_speed; 
	bad_face->position.z += elapsed * face_speed; 

	//the song is over
	if(time_passed > music_time) {
		if(!game_over) {
			//calculate scores and display them 
			std::cout << "\nyou got " << std::to_string(good_count) << " :) faces, " << std::to_string(ok_count) << " :| faces, and " << std::to_string(bad_count) << " :( faces."; 
			float score = 100.0f * (good_count + ok_count * 0.5f - 0.2f * bad_count) / 25.0f; 
			std::cout << "\nyour score is " << std::to_string(score) << "\n"; 
			
			if(score > 90.0f) a_letter->position = letter_position; 
			else if(score > 80.0f) b_letter->position = letter_position; 
			else if(score > 70.0f) c_letter->position = letter_position; 
			else if(score > 60.0f) d_letter->position = letter_position; 
			else f_letter->position = letter_position; 

			//remove the background 
			bg->position = hidden_position; 
			bg = bgs[4]; 
			bg->position = bg_position; 
			game_over = true; 
		}

		return; 
	}

	beat_time_passed += elapsed; 
	time_passed += elapsed; 
	
	//new beat 
	if(beat_time_passed > beat_time) {
		num_beats ++; 
		beat_time_passed = 0; 
		
		if(num_beats % 2 == 1) {
			hit = false; 

			//pick a new command
			current_command = command(rand() % 4); 

			//current_command = poke; 
			bg->position = hidden_position; 
			bg = bgs[current_command]; 
			bg->position = bg_position; 
		}
	}

	//scale the gizmo according to the beat 
	if(beat_time_passed <= scale_threshold) {
		float scale = 1.0f + scale_amount * (scale_threshold - beat_time_passed);
		if(num_beats % 2 == 1) base->scale = glm::vec3(scale); 
		else if(current_command < 4) parts[current_command]->scale = glm::vec3(scale); 
	}
	if(beat_time - beat_time_passed <= scale_threshold) {
		float scale = 1.0f + scale_amount * (scale_threshold - beat_time + beat_time_passed );
		if(num_beats % 2 == 0) base->scale = glm::vec3(scale); 
		else if(current_command < 4) parts[current_command]->scale = glm::vec3(scale); 
	}


	//jabbing motion of hand 
	if(jabbing) {
		jab_time_passed += elapsed; 
		
		glm::vec3 jab_direction; 
		
		//if you are patting or pinching, the jab direction should go mostly down. 
		if(current_hand == pat || current_hand == pinch) {
			jab_direction = hand_position; 
			jab_direction.y = 0; 
			jab_direction = glm::normalize(jab_direction);
		}
		//if you are punching or poking, it should go forward 
		else {
			jab_direction = glm::vec3(0.0f, -1.0f, 0.0f); 
		}
		float jab_amt = 2.0f * jab_time_passed / jab_time; 
		if(jab_amt > 1.0f) jab_amt = 2.0f - jab_amt; 

		hand->position = hand_position - jab_magnitude * jab_amt * jab_direction; 
		float angle = atan2(hand_position.x, hand_position.z); 
		
		//called once per jab 
		if(!hit && jab_time_passed >= 0.5f * jab_time) {
			hit = true; 
			//remove the background 
			bg->position = hidden_position; 
			bg = bgs[4]; 
			bg->position = bg_position; 
			//correct hand command
			if(current_hand == current_command) {
				//find out which area you are jabbing 
				float r = sqrt(hand_position.x * hand_position.x + hand_position.z * hand_position.z);
				if(r < 4.5f) {
					current_area = punch; 
				}
				else if(r > 11.0f){
					current_area = none; 
				}
				else if(angle >= 0 && angle < 2.0f/3.0f * pi) {
					current_area = pat; 
				}
				else if(angle < 0  && angle > - 2.0f/3.0f * pi) {
					current_area = pinch; 
				}
				else {
					current_area = poke; 
				}

				//if you jabbed the correct area 
				if(current_area == current_command) {
					//if you had the correct timing 
					if((num_beats % 2 == 0 && beat_time_passed <= good_threshold) 
					|| (num_beats % 2 == 1 && beat_time - beat_time_passed <= good_threshold)){
						good_face->position = hand_position; 
						good_count++;
					}
					else {
						ok_face->position = hand_position; 
						ok_count++;
					}
				}
				//if you jabbed the wrong area
				else {
					bad_face->position = hand_position; 
					Sound::play(*(*(bad_samples[0])), 1.0f, 1.0f);
					bad_count++; 
				}
			}
			//if you jabbed with the wrong hand 
			else {
				bad_face->position = hand_position; 
				Sound::play(*(*(bad_samples[0])), 1.0f, 1.0f);
				bad_count++; 
			}
		}

		if(jab_time_passed > jab_time) {
			jabbing = false; 
		}
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}

