NAME = libasm
LIB_NAME = libasm.a

SRCS_DIR = srcs/
OBJS_DIR = objs/
SRCS = $(wildcard srcs/*.s)
OBJS = $(addprefix $(OBJS_DIR),$(SRCS:$(SRCS_DIR)%.s=%.o))
# HEADERS = -I includes/
HDRS = $(wildcard includes/*.h)


CFLAGS = -Wall -Wextra -Werror -g
COMPILER = nasm -f elf64

all: $(LIB_NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.s $(HDRS)
	@mkdir -p $(dir $@)
	@$(COMPILER) $< -o $@

$(LIB_NAME): $(OBJS)
	@echo "$(GREEN)$(NAME)$(NC) compiling..."
	@$(ARCH) $(LIB_NAME) $(OBJS)
	@echo "$(GREEN)$(NAME)$(NC) ready!"

clean:
	@$(RM) -r $(OBJS_DIR)
	@$(RM) -r $(OBJS_DIR_BONUS)
	@echo "$(RED)$(NAME)$(NC) OBJS cleaned!"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(LIB_NAME)
	@$(RM) $(BONUS_NAME)
	@echo "$(RED)$(NAME)$(NC) cleaned!"
	@$(RM) 

re: fclean all

.PHONY: all fclean clean re 