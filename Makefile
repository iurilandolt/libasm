NAME     = libasm
LIB_NAME = libasm.a

SRCS_DIR = srcs/
OBJS_DIR = objs/
SRCS     = $(wildcard srcs/*.s)
OBJS     = $(addprefix $(OBJS_DIR),$(SRCS:$(SRCS_DIR)%.s=%.o))
HDRS     = $(wildcard includes/*.h)

RM       = rm -f
CC       = cc
CFLAGS   = -Wall -Wextra -Werror -g
COMPILER = nasm -f elf64
ARCH     = ar rcs

all: $(LIB_NAME) $(NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.s
	@mkdir -p $(dir $@)
	@$(COMPILER) $< -o $@

$(LIB_NAME): $(OBJS)
	@echo "$(NAME) compiling..."
	@$(ARCH) $(LIB_NAME) $(OBJS)
	@echo "$(NAME) ready!"

$(NAME): main.c $(LIB_NAME) $(HDRS)
	@$(CC) $(CFLAGS) main.c -L. -lasm -o $(NAME) -I includes/

clean:
	@$(RM) -r $(OBJS_DIR)
	@echo "$(NAME) objects cleaned"

fclean: clean
	@$(RM) $(LIB_NAME)
	@$(RM) $(NAME)
	@echo "$(NAME) cleaned"

run: $(NAME)
	@./$(NAME)

re: fclean all

.PHONY: all clean fclean re run
