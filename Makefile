BBLU		=	\033[1;34m
BGREEN		=	\033[1;32m
BRED		=	\033[1;31m
BLU			=	\033[0;34m
GRN			=	\033[0;32m
RED			=	\033[0;31m
RST			=	\033[0m

SRC			=	$(shell find ./src -type f -name "*.c" | cut -c 3-)
HEADER		=	$(shell find ./inc -type f -name "*.h" | cut -c 3-)

OBJ			=	$(SRC:.c=.o)

CC			=	gcc
FLAGS		=	-Wall -Werror -Wextra
FLAGS_H		=	-I inc/
NAME		=	ft_ping

all: $(NAME)
.PHONY: all

print_header:
	echo "\033[1;34m\033[5G========================================================================"
	echo "\033[1;34m\033[5G"
	echo "\033[1;34m\033[5G███████╗████████╗     ██████╗ ██╗███╗   ██╗ ██████╗ "
	echo "\033[1;34m\033[5G██╔════╝╚══██╔══╝     ██╔══██╗██║████╗  ██║██╔════╝ "
	echo "\033[1;34m\033[5G█████╗     ██║        ██████╔╝██║██╔██╗ ██║██║  ███╗"
	echo "\033[1;34m\033[5G██╔══╝     ██║        ██╔═══╝ ██║██║╚██╗██║██║   ██║"
	echo "\033[1;34m\033[5G██║        ██║███████╗██║     ██║██║ ╚████║╚██████╔╝"
	echo "\033[1;34m\033[5G╚═╝        ╚═╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═══╝ ╚═════╝ "
	echo "\033[1;34m\033[5G"
	echo "\033[1;34m\033[5G=======================> By kbarbry ft tjolivea <======================="
.PHONY: print_header

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) $(FLAGS_H) -c $< -o $@
	echo "$(BBLU)[$(NAME) OBJ] :$(RST) $@ $(BGREEN)\033[47G[✔]$(RST)"

$(NAME): print_header $(OBJ)
	$(CC) $(FLAGS) $(FLAGS_H) $(OBJ) -o $(NAME)
	echo "$(BGREEN)[$(NAME) END] :$(RST)$(RST) ./$(NAME) $(BGREEN)\033[47G[✔]$(RST)"

test: all
	./$(NAME)
.PHONY: test

clean:
	rm -f $(OBJ)
	echo "$(RED)[CLEAN]  :$(RST) Deleting objects...$(BGREEN)\033[47G[✔]$(RST)"
.PHONY: clean

fclean: clean
	rm -f $(NAME)
	echo "$(RED)[FCLEAN] :$(RST) Deleting executable...$(BGREEN)\033[47G[✔]$(RST)"
.PHONY: fclean

re:	fclean all
.PHONY: re

.SILENT: