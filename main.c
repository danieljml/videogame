#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "vector.h"

const char* VIDEOGAME_NAME = "Videogame";
const int INITIAL_WIDTH = 1024;
const int INITIAL_HEIGHT = 768;
const int SPRITE_SIZE = 16;
const int CHARACTER_SIZE = SPRITE_SIZE * 8;

int main(int argc, char* argv[]) {
	const int init_status = SDL_Init(SDL_INIT_EVERYTHING);
	if (init_status < 0)
	{
		printf("There was an error initializing SDL: %s", SDL_GetError());
		exit(init_status);
	}

	SDL_Window* window = SDL_CreateWindow(VIDEOGAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, INITIAL_WIDTH, INITIAL_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) {
		printf("There was an error creating the main window: %s", SDL_GetError());
		exit(1);
	}

	time_t timer;
	srand((unsigned) time(&timer));

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Rect warrior_sprite_rect;
	warrior_sprite_rect.x = 0;
	warrior_sprite_rect.y = 0;
	warrior_sprite_rect.w = SPRITE_SIZE;
	warrior_sprite_rect.h = SPRITE_SIZE;

	SDL_Rect dead_warrior_sprite_rect;
	dead_warrior_sprite_rect.x = SPRITE_SIZE * 7;
	dead_warrior_sprite_rect.y = 0;
	dead_warrior_sprite_rect.w = SPRITE_SIZE;
	dead_warrior_sprite_rect.h = SPRITE_SIZE;

	int character_selection = 3;
	SDL_Rect player_sprite_rect;
	player_sprite_rect.x = 0;
	player_sprite_rect.y = SPRITE_SIZE * character_selection;
	player_sprite_rect.w = SPRITE_SIZE;
	player_sprite_rect.h = SPRITE_SIZE;
	
	SDL_Rect player_block;
	player_block.x = 500;
	player_block.y = 500;
	player_block.w = CHARACTER_SIZE;
	player_block.h = CHARACTER_SIZE;
	SDL_RendererFlip player_flip = SDL_FLIP_NONE;
	bool is_player_attacking = false;
	int player_attack = 20;

	Vector* enemy_blocks_vector = allocate_Vector();
	for (int index = 0; index < 5; index++) {
		SDL_Rect* enemy_block = (SDL_Rect *) malloc(sizeof(SDL_Rect));
		enemy_block->x = 100 * (index + 1);
		enemy_block->y = (index % 2 == 0) ? 100: 100 * (index + 1);
		enemy_block->w = CHARACTER_SIZE;
		enemy_block->h = CHARACTER_SIZE;
		Vector_add_item(enemy_blocks_vector, (void *) enemy_block);
	}

	Vector* enemy_health_vector = allocate_Vector();
	for (int index = 0; index < enemy_blocks_vector->length; index++) {
		int* enemy_health = (int *) malloc(sizeof(int));
		*enemy_health = 100;
		Vector_add_item(enemy_health_vector, (void *) enemy_health);
	}

	Vector* enemy_total_health_vector = allocate_Vector();
	for (int index = 0; index < enemy_blocks_vector->length; index++) {
		int* enemy_total_health = (int *) malloc(sizeof(int));
		*enemy_total_health = 100;
		Vector_add_item(enemy_total_health_vector, (void *) enemy_total_health);
	}

	Vector* dead_bodies_vector = allocate_Vector();

	SDL_Surface* sprite_surface = IMG_Load("./sprites.png");
	SDL_Texture* sprite_texture = SDL_CreateTextureFromSurface(renderer, sprite_surface);

	bool is_running = true;
	for(int i = 0; is_running; i = i >= 3 ? 0: i + 1) {

		// Section: Scenary setup

		SDL_SetRenderDrawColor(renderer, 0xC4, 0xC6, 0xC5, 0xFF);
		SDL_RenderClear(renderer);

		warrior_sprite_rect.x = SPRITE_SIZE * i;
		player_sprite_rect.x = SPRITE_SIZE * i;

		// Section: Collision detection

		if (is_player_attacking) {
			player_sprite_rect.x = SPRITE_SIZE * 4;
			SDL_Rect attack_surface;
			attack_surface.x = player_flip == SDL_FLIP_NONE ? player_block.x: player_block.x + player_block.w - CHARACTER_SIZE / 4;
			attack_surface.y = player_block.y + (CHARACTER_SIZE / 4);
			attack_surface.w = CHARACTER_SIZE / 4;
			attack_surface.h = CHARACTER_SIZE / 2;

			for (int index = 0; index < enemy_blocks_vector->length; index++) {
				SDL_Rect* enemy_block = (SDL_Rect *) Vector_get_item(enemy_blocks_vector, index);
				bool attack_hits_enemy = SDL_HasIntersection(&attack_surface, enemy_block);
				if (attack_hits_enemy) {
					int* enemy_health = (int *) Vector_get_item(enemy_health_vector, index);
					*enemy_health = *enemy_health - player_attack;
					*enemy_health = *enemy_health >= 0 ? *enemy_health: 0;
				}
			}

			is_player_attacking = false;
		}
		
		// Section: Renders

		for (int index = 0; index < enemy_blocks_vector->length; index++) {
			SDL_Rect* enemy_block = (SDL_Rect *) Vector_get_item(enemy_blocks_vector, index);
			int* enemy_health = (int *) Vector_get_item(enemy_health_vector, index);
			int* enemy_total_health = (int *) Vector_get_item(enemy_total_health_vector, index);
			float health_percentage = (float) (*enemy_health) / (float) (*enemy_total_health) * (float) 100;
			if (*enemy_health > 0) {
				SDL_Rect life_bar;
				life_bar.x = enemy_block->x;
				life_bar.y = enemy_block->y - 20;
				life_bar.w = enemy_block->w;
				life_bar.h = 20;
				SDL_Rect life_bar_value;
				life_bar_value.x = life_bar.x + 2;
				life_bar_value.y = life_bar.y + 2;
				life_bar_value.w = (life_bar.w - 4) * health_percentage / 100;
				life_bar_value.h = life_bar.h - 4;
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderFillRect(renderer, &life_bar);
				SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
				SDL_RenderFillRect(renderer, &life_bar_value);
			}
			SDL_RenderCopy(renderer, sprite_texture, &warrior_sprite_rect, enemy_block);
		}

		for (int index = 0; index < dead_bodies_vector->length; index++) {
			SDL_Rect* dead_body = (SDL_Rect *) Vector_get_item(dead_bodies_vector, index);
			SDL_RenderCopy(renderer, sprite_texture, &dead_warrior_sprite_rect, dead_body);
		}

		SDL_RenderCopyEx(renderer, sprite_texture, &player_sprite_rect, &player_block, 0, NULL, player_flip);

		// Section: Event Management

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					is_running = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						SDL_Rect* enemy_block = (SDL_Rect *) malloc(sizeof(SDL_Rect));
						enemy_block->x = event.button.x;
						enemy_block->y = event.button.y;
						enemy_block->w = CHARACTER_SIZE;
						enemy_block->h = CHARACTER_SIZE;
						Vector_add_item(enemy_blocks_vector, (void *) enemy_block);
						int* enemy_health = (int *) malloc(sizeof(int));
						*enemy_health = 100;
						Vector_add_item(enemy_health_vector, (void *) enemy_health);
						int* enemy_total_health = (int *) malloc(sizeof(int));
						*enemy_total_health = 100;
						Vector_add_item(enemy_total_health_vector, (void *) enemy_total_health);
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_UP:
							player_block.y -= 10;
							break;
						case SDLK_DOWN:
							player_block.y += 10;
							break;
						case SDLK_LEFT:
							player_block.x -= 10;
							player_flip = SDL_FLIP_NONE;
							break;
						case SDLK_RIGHT:
							player_block.x += 10;
							player_flip = SDL_FLIP_HORIZONTAL;
							break;
						case SDLK_p:
							character_selection = character_selection + 1 >= 8 ? 0: character_selection + 1;
							player_sprite_rect.y = 16 * character_selection;
							break;
						case SDLK_z: 
							is_player_attacking = true;
							break;
						case SDLK_x:
							break;
						case SDLK_q:
							is_running = false;
							break;
						default:
							break;
					}
					break;
				default: 
					break;
			}
		}

		// Section: Physics


		// Section: Post processing

		for (int index = 0; index < enemy_blocks_vector->length; index++) {
			int enemy_health = (int) *((int *) Vector_get_item(enemy_health_vector, index));
			if (enemy_health <= 0) {
				SDL_Rect* enemy_block = (SDL_Rect *) Vector_get_item(enemy_blocks_vector, index);
				Vector_add_item(dead_bodies_vector, (void *) enemy_block);
				Vector_remove_item(enemy_blocks_vector, index);
				Vector_remove_item(enemy_health_vector, index);
				Vector_remove_item(enemy_total_health_vector, index);
				player_attack += 10;
			}
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(60);

		// Section: Loop validation

		is_running = is_running ? enemy_blocks_vector->length: false;
	}

	destroy_Vector(enemy_blocks_vector);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
