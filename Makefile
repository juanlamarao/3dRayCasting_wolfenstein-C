# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: juanlamarao <juolivei@42.fr>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/03/01 16:54:28 by juanlamar         #+#    #+#              #
#    Updated: 2020/03/01 17:02:55 by juanlamar        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = wolfenstein

C_FILES = src/main.c

O_FILES = $(C_FILES:.c=.0)

GRAPH_LIB = -lSDL2

CC = gcc

CC_FLAGS = -Wall -Werror -Wextra

all: $(C_FILES)
	$(CC) $(CC_FLAGS) $(C_FILES) $(GRAPH_LIB) -o $(NAME);

run:
	./$(NAME)

clean:
	rm $(NAME)
