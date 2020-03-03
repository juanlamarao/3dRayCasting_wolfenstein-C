/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juanlamarao <juolivei@42.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 16:46:46 by juanlamar         #+#    #+#             */
/*   Updated: 2020/03/03 12:39:36 by juolivei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "constants.h"
//#include <SDL2/SDL.h>
#include "/Users/juolivei/.brew/Cellar/sdl2/2.0.10/include/SDL2/SDL.h"
#include <math.h>
#include <limits.h>

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
typedef struct	s_ray
{
	float	ray_angle;
	float	wall_hit_x;
	float	wall_hit_y;
	float	distance;
	int		was_hit_vert;
	int		is_ray_facing_down;
	int		is_ray_facing_right;
	int		wall_hit_content;
}				t_ray;

// casting dos raios
t_ray			rays[NUM_RAYS];
// jogador
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

int		find_horz_hit(float *wall_hit_x, float *wall_hit_y, int is_ray_facing_down, int is_ray_facing_right, float ray_angle, int *wall_content)
{
	float	x_intercept;
	float	y_intercept;
	float	x_step;
	float	y_step;
	float	next_touch_x;
	float	next_touch_y;
	float	x_to_check;
	float	y_to_check;
	int		found_hit;

	found_hit = 0;
	//
	y_intercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	y_intercept += is_ray_facing_down ? TILE_SIZE : 0;
	//
	x_intercept = player.x + (y_intercept - player.y) / tan(ray_angle);
	//
	y_step = TILE_SIZE;
	y_step *= !is_ray_facing_down ? -1: 1;
	//
	x_step = TILE_SIZE / tan(ray_angle);
	x_step *= (!is_ray_facing_right && x_step > 0) ? -1 : 1;
	x_step *= (is_ray_facing_right && x_step < 0) ? -1 : 1;
	//
	next_touch_x = x_intercept;
	next_touch_y = y_intercept;
	//
	while (next_touch_x >= 0 && next_touch_x <= WINDOW_WIDTH && next_touch_y >= 0 && next_touch_y <= WINDOW_HEIGHT)
	{
		x_to_check = next_touch_x;
		y_to_check = next_touch_y + (!is_ray_facing_down ? -1 : 0);

		if (map_has_wall_at(x_to_check, y_to_check))
		{
			found_hit = 1;
			*wall_hit_x = next_touch_x;
			*wall_hit_y = next_touch_y;
			*wall_content = map[(int)floor(y_to_check / TILE_SIZE)][(int)floor(x_to_check / TILE_SIZE)];
			break ;
		}
		else
		{
			next_touch_x += x_step;
			next_touch_y += y_step;
		}
	}
	return (found_hit);
}

int		find_vert_hit(float *wall_hit_x, float *wall_hit_y, int is_ray_facing_down, int is_ray_facing_right, float ray_angle, int *wall_content)
{
	float	x_intercept;
	float	y_intercept;
	float	x_step;
	float	y_step;
	float	next_touch_x;
	float	next_touch_y;
	float	x_to_check;
	float	y_to_check;
	int		found_hit;

	found_hit = 0;
	(void)wall_content;
	x_intercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	x_intercept += is_ray_facing_right ? TILE_SIZE : 0;
	y_intercept = player.y + (x_intercept - player.x) * tan(ray_angle);
	x_step = TILE_SIZE;
	x_step *= !is_ray_facing_right ? -1: 1;
	y_step = TILE_SIZE * tan(ray_angle);
	y_step *= (!is_ray_facing_down && y_step > 0) ? -1 : 1;
	y_step *= (is_ray_facing_down && y_step < 0) ? -1 : 1;
	next_touch_x = x_intercept;
	next_touch_y = y_intercept;
	while (next_touch_x >= 0 && next_touch_x <= WINDOW_WIDTH && next_touch_y >= 0 && next_touch_y <= WINDOW_HEIGHT)
	{
		x_to_check = next_touch_x + (!is_ray_facing_right ? -1 : 0);
		y_to_check = next_touch_y;

		if (map_has_wall_at(x_to_check, y_to_check))
		{
			found_hit = 1;
			*wall_hit_x = next_touch_x;
			*wall_hit_y = next_touch_y;
			*wall_content = map[(int)floor(y_to_check / TILE_SIZE)][(int)floor(x_to_check / TILE_SIZE)];
			break ;
		}
		else
		{
			next_touch_x += x_step;
			next_touch_y += y_step;
		}
	}
	return (found_hit);
}

float	normalize_angle(float angle)
{
	angle = remainder(angle, TWO_PI);
	if (angle < 0)
		angle = TWO_PI + angle;
	return (angle);
}

float	distance_beteewn_points(float x1, float y1, float x2, float y2)
{
	return (sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))));
}

void	cast_ray(float ray_angle, int strip_id)
{
	int		is_ray_facing_down;
	int		is_ray_facing_right;
	int		found_horz_hit;
	int		found_vert_hit;
	int		horz_wall_content;
	int		vert_wall_content;
	float	horz_wall_hit_x;
	float	horz_wall_hit_y;
	float	vert_wall_hit_x;
	float	vert_wall_hit_y;
	float	horz_distance;
	float	vert_distance;

	ray_angle = normalize_angle(ray_angle);
	is_ray_facing_down = ray_angle > 0 && ray_angle < PI;
	is_ray_facing_right = ray_angle < 0.5 * PI || ray_angle > 1.5 * PI;

	found_horz_hit = find_horz_hit(&horz_wall_hit_x, &horz_wall_hit_y, is_ray_facing_down, is_ray_facing_right, ray_angle, &horz_wall_content);
	found_vert_hit = find_vert_hit(&vert_wall_hit_x, &vert_wall_hit_y, is_ray_facing_down, is_ray_facing_right, ray_angle, &vert_wall_content);
	horz_distance = found_horz_hit
		? distance_beteewn_points(player.x, player.y, horz_wall_hit_x, horz_wall_hit_y)
		: INT_MAX;
	vert_distance = found_vert_hit
		? distance_beteewn_points(player.x, player.y, vert_wall_hit_x, vert_wall_hit_y)
		: INT_MAX;
	if (vert_distance < horz_distance)
	{
		rays[strip_id].distance = vert_distance;
		rays[strip_id].wall_hit_x = vert_wall_hit_x;
		rays[strip_id].wall_hit_y = vert_wall_hit_y;
		rays[strip_id].wall_hit_content = vert_wall_content;
		rays[strip_id].was_hit_vert = 1;
	}
	else
	{
		rays[strip_id].distance = horz_distance;
		rays[strip_id].wall_hit_x = horz_wall_hit_x;
		rays[strip_id].wall_hit_y = horz_wall_hit_y;
		rays[strip_id].wall_hit_content = horz_wall_content;
		rays[strip_id].was_hit_vert = 0;
	}
	rays[strip_id].ray_angle = ray_angle;
	rays[strip_id].is_ray_facing_down = is_ray_facing_down;
	rays[strip_id].is_ray_facing_right = is_ray_facing_right;
}

void	cast_all_rays()
{
	int		strip_id;
	float	ray_angle;
	
	strip_id = 0;
	ray_angle = player.rotation_angle - (FOV_ANGLE / 2);
	while (strip_id < NUM_RAYS)
	{
		cast_ray(ray_angle, strip_id);
		ray_angle += FOV_ANGLE / NUM_RAYS;
		strip_id++;
	}
}

void	render_3d_projected_walls()
{
	int	strip_id;

	strip_id = 0;
	while (strip_id < NUM_RAYS)
	{
		int		alpha;
		int		color;
		int		color1;
		int		color2;
		int		color3;
/*		int		y;
		int		wall_top_pixel;
		int		wall_bottom_pixel;*/
		int		wall_strip_width = 1;
		t_ray	ray;
		float	dist_proj_plane;
		float	correct_wall_dist;
		float	wall_strip_height;
	   
		ray = rays[strip_id];
		correct_wall_dist = ray.distance * cos(player.rotation_angle - ray.ray_angle);
		alpha = 50000 / floor(correct_wall_dist);
		alpha = alpha > 255 ? 255 : alpha;
		alpha = alpha < 0 ? 0 : alpha;
		alpha += ray.was_hit_vert ? -50 : 0;
		alpha = alpha > 255 ? 255 : alpha;
		alpha = alpha < 0 ? 0 : alpha;
		/* single color */
		color = 255;
		color1 = color;
		color2 = color;
		color3 = color;
		/* color based on cardial position */
		/* procurar implementação hexadecimal 0xff112233 0x(alpha)(red)(green)(blue)*/
		if (ray.was_hit_vert && !ray.is_ray_facing_right) // WEST
		{
			color1 = 70;
			color2 = 130;
			color3 = 180;
		}
		else if (ray.was_hit_vert && ray.is_ray_facing_right) // EAST
		{
			color1 = 0;
			color2 = 128;
			color3 = 0;
		}
		else if (!ray.was_hit_vert && !ray.is_ray_facing_down) // NORTH
		{
			color1 = 75;
			color2 = 0;
			color3 = 130;
		}
		else if (!ray.was_hit_vert && ray.is_ray_facing_down) // SOUTH
		{
			color1 = 255;
			color2 = 140;
			color3 = 0;
		}
		dist_proj_plane = (WINDOW_WIDTH / 2) * tan(FOV_ANGLE / 2);
		wall_strip_height = (TILE_SIZE / correct_wall_dist) * dist_proj_plane;

		//render
		SDL_SetRenderDrawColor(renderer, color1, color2, color3, 255);
		SDL_Rect map_tile_rect = {
			strip_id * wall_strip_width,
			(WINDOW_HEIGHT / 2) - (wall_strip_height / 2),
			wall_strip_width,
			wall_strip_height,
		};
		SDL_RenderFillRect(renderer, &map_tile_rect);
		strip_id++;
	}
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

void	render_rays()
{
	int	i;

	i = 0;
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	while (i < NUM_RAYS)
	{
		SDL_RenderDrawLine(
			renderer,
			MINIMAP_SCALE_FACTOR * player.x,
			MINIMAP_SCALE_FACTOR * player.y,
			MINIMAP_SCALE_FACTOR * rays[i].wall_hit_x,
			MINIMAP_SCALE_FACTOR * rays[i].wall_hit_y
		);
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
	player.rotation_angle = 0.5 * PI;
	player.walk_speed = 100;
	player.turn_speed = 45 * (PI / 180);
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

void	render_celling()
{
	int	color1;
	int	color2;
	int	color3;
	int	alpha;

	color1 = 176;
	color2 = 224;
	color3 = 230;
	alpha = 255;
	SDL_SetRenderDrawColor(renderer, color1, color2, color3, alpha);
	SDL_Rect rect = {
		0,
		0,
		WINDOW_WIDTH,
		WINDOW_HEIGHT / 2,
	};
	SDL_RenderFillRect(renderer, &rect);
}

void	render_floor()
{
	int	color1;
	int	color2;
	int	color3;
	int	alpha;

	color1 = 0;
	color2 = 100;
	color3 = 0;
	alpha = 255;
	SDL_SetRenderDrawColor(renderer, color1, color2, color3, alpha);
	SDL_Rect rect = {
		0,
		WINDOW_HEIGHT / 2,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
	};
	SDL_RenderFillRect(renderer, &rect);
}

void	update()
{
	// "gastar" tempo até respeitar o frame rate
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticks_last_frame + FRAME_TIME_LENGTH));

	// atualizar valores de controle de frame rate
	float	delta_time = (SDL_GetTicks() - ticks_last_frame) / 1000.0f;
	ticks_last_frame = SDL_GetTicks();

	move_player(delta_time);
	cast_all_rays();
}

void	render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	render_celling();
//	SDL_RenderClear(renderer);
	render_floor();
//	SDL_RenderClear(renderer);
	
	render_3d_projected_walls();

	// renderizar objetos do jogo
	render_map();
	render_rays();
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
