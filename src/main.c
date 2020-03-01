/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juanlamarao <juolivei@42.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 16:46:46 by juanlamar         #+#    #+#             */
/*   Updated: 2020/03/01 18:06:51 by juanlamar        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "constants.h"
#include <SDL2/SDL.h>

SDL_Window*		window = NULL;
SDL_Renderer*	renderer = NULL;
int				is_game_running = 0;
int				player_x, player_y;

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
			{
				is_game_running = 0;
				break ;
			}
		}
	}
}

void	setup()
{
	player_x = 0;
	player_y = 0;
}

void	update()
{
	player_x += 1;
	player_y += 1;
}

void	render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_Rect rect = {player_x, player_y, 20, 20};
	SDL_RenderFillRect(renderer, &rect);

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
