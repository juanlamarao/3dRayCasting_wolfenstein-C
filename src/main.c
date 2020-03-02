/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juanlamarao <juolivei@42.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 16:46:46 by juanlamar         #+#    #+#             */
/*   Updated: 2020/03/02 14:18:10 by juanlamar        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "constants.h"
#include <SDL2/SDL.h>
#include <math.h>

const int		map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

typedef struct	s_player
{
	float	x;
	float	y;
	float	width;
	float	height;
	int		turn_direction; // -1 left, 1 rigth
	int		walk_direction; // -1 back, 1 front
	float	rotation_angle;
	float	walk_speed;
	float	turn_speed;
}				t_player;

t_player		player;
// janela SDL
SDL_Window*		window = NULL;
// renderização SDL
SDL_Renderer*	renderer = NULL;
// flag de verificação de status do game
int				is_game_running = 0;
// posição x / y do jogador
int				player_x, player_y;
// fps
int				ticks_last_frame = 0;

int		initialize_window()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return (0);
	}
	window = SDL_CreateWindow(
		"RayCasting",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS
	);
	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return (0);
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL render.\n");
		return (0);
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	return (1);
}

void	destroy_window()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void	render_map()
{
	int	i;
	int	j;
	int	tile_x;
	int	tile_y;
	int	tile_color;

	i = 0;
	while (i < MAP_NUM_ROWS)
	{
		j = 0;
		while (j < MAP_NUM_COLS)
		{
			tile_x = j * TILE_SIZE;
			tile_y = i * TILE_SIZE;
			tile_color = map[i][j] != 0 ? 255 : 0;

			SDL_SetRenderDrawColor(renderer, tile_color, tile_color, tile_color, 255);
			SDL_Rect map_tile_rect = {
				tile_x * MINIMAP_SCALE_FACTOR,
				tile_y * MINIMAP_SCALE_FACTOR,
				TILE_SIZE * MINIMAP_SCALE_FACTOR,
				TILE_SIZE * MINIMAP_SCALE_FACTOR,
			};
			SDL_RenderFillRect(renderer, &map_tile_rect);
			j++;
		}
		i++;
	}
}

void	process_input()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
		case SDL_QUIT:
		{
			is_game_running = 0;
			break ;
		}
		case SDL_KEYDOWN:
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_game_running = 0;
			else if (event.key.keysym.sym == SDLK_UP)
				player.walk_direction = 1;
			else if (event.key.keysym.sym == SDLK_DOWN)
				player.walk_direction = -1;
			else if (event.key.keysym.sym == SDLK_RIGHT)
				player.turn_direction = 1;
			else if (event.key.keysym.sym == SDLK_LEFT)
				player.turn_direction = -1;
			break ;
		}
		case SDL_KEYUP:
		{
			if (event.key.keysym.sym == SDLK_UP)
				player.walk_direction = 0;
			else if (event.key.keysym.sym == SDLK_DOWN)
				player.walk_direction = 0;
			else if (event.key.keysym.sym == SDLK_RIGHT)
				player.turn_direction = 0;
			else if (event.key.keysym.sym == SDLK_LEFT)
				player.turn_direction = 0;
			break ;
		}
	}
}

void	setup()
{
	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.width = 1;
	player.height = 1;
	player.turn_direction = 0;
	player.walk_direction = 0;
	player.rotation_angle = PI / 2;
	player.walk_speed = 100;
	player.turn_speed = 45 * (PI / 180);
}

int		map_has_wall_at(float x, float y)
{
	int	map_grid_index_x;
	int	map_grid_index_y;

	if (x < 0 || y < 0 || x > WINDOW_WIDTH || y > WINDOW_HEIGHT)
		return 1;
	map_grid_index_x = floor(x / TILE_SIZE);
	map_grid_index_y = floor(y / TILE_SIZE);
	return (map[map_grid_index_y][map_grid_index_x] != 0);
}

void	move_player(float delta_time)
{
	float	move_step;
	float	new_player_x;
	float	new_player_y;

	player.rotation_angle += player.turn_direction * (player.turn_speed * delta_time);
	move_step = player.walk_direction * (player.walk_speed * delta_time);
	new_player_x = player.x + cos(player.rotation_angle) * move_step;
	new_player_y = player.y + sin(player.rotation_angle) * move_step;

	if (!map_has_wall_at(new_player_x, new_player_y))
	{
		player.x = new_player_x;
		player.y = new_player_y;
	}
}

void	render_player()
{
	// retangulo do jogador
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect player_rect =
	{
		player.x * MINIMAP_SCALE_FACTOR,
		player.y * MINIMAP_SCALE_FACTOR,
		player.width * MINIMAP_SCALE_FACTOR,
		player.height * MINIMAP_SCALE_FACTOR
	};
	SDL_RenderFillRect(renderer, &player_rect);

	//direção da linha
	SDL_RenderDrawLine(
		renderer,
		MINIMAP_SCALE_FACTOR * player.x,
		MINIMAP_SCALE_FACTOR * player.y,
		MINIMAP_SCALE_FACTOR * player.x + cos(player.rotation_angle) * 40,
		MINIMAP_SCALE_FACTOR * player.y + sin(player.rotation_angle) * 40
	);
}

void	update()
{
	// "gastar" tempo até respeitar o frame rate
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticks_last_frame + FRAME_TIME_LENGTH));

	// atualizar valores de controle de frame rate
	float	delta_time = (SDL_GetTicks() - ticks_last_frame) / 1000.0f;
	ticks_last_frame = SDL_GetTicks();

	move_player(delta_time);
	// TODO: lembrar de multiplicar qualquer update por delta_time
	//player_x += 50 * delta_time;
	//player_y += 50 * delta_time;
}

void	render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// renderizar objetos do jogo
	render_map();
	//render_rays();
	render_player();

	SDL_RenderPresent(renderer);

}

int		main(void)
{
	is_game_running = initialize_window();
	setup();
	while (is_game_running)
	{
		process_input();
		update();
		render();
	}
	destroy_window();
	return (0);
}
