NAME = libasm
LIB_NAME = libasm.a

SRCS_DIR = srcs/
OBJS_DIR = objs/
SRCS = $(wildcard srcs/*.s)
OBJS = $(addprefix $(OBJS_DIR),$(SRCS:$(SRCS_DIR)%.s=%.o))
HDRS = $(wildcard includes/*.h)


RM = rm -f
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
COMPILER = nasm -f elf64
ARCH = ar rcs

all: $(LIB_NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.s $(HDRS)
	@mkdir -p $(dir $@)
	@$(COMPILER) $< -o $@

$(LIB_NAME): $(OBJS)
	@echo "$(NAME)$(NC) compiling..."
	@$(ARCH) $(LIB_NAME) $(OBJS)
	@echo "$(NAME)$(NC) ready!"

clean:
	@$(RM) -r $(OBJS_DIR)
	@$(RM) -r $(OBJS_DIR_BONUS)
	@echo "$(NAME)$(NC) OBJS cleaned!"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(LIB_NAME)
	@$(RM) $(BONUS_NAME)
	@echo "$(NAME)$(NC) cleaned!"
	@$(RM) 

run: all
	@$(CC) $(CFLAGS) main.c $(LIB_NAME) -o $(NAME) -I includes/
	@./$(NAME)

re: fclean all

.PHONY: all fclean clean re run