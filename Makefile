# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: juanlamarao <juolivei@42.fr>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/03/01 16:54:28 by juanlamar         #+#    #+#              #
#    Updated: 2020/03/03 09:53:17 by juolivei         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = wolfenstein

C_FILES = src/main.c

O_FILES = $(C_FILES:.c=.0)

#GRAPH_LIB = -lSDL2
GRAPH_LIB = -L/Users/juolivei/.brew/Cellar/sdl2/2.0.10/lib/ -lSDL2

MATH_LIB = -lm -ldl
CC = gcc

CC_FLAGS = -Wall -Werror -Wextra

all: $(C_FILES)
	$(CC) $(CC_FLAGS) $(C_FILES) $(GRAPH_LIB) $(MATH_LIB) -o $(NAME)

run:
	./$(NAME)

clean:
	rm -rf $(NAME)

re: clean all run
