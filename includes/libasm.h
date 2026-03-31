#ifndef LIBASM_H
#define LIBASM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

extern size_t	ft_strlen(const char *str);
extern char		*ft_strcpy(char *dest, const char *src);
extern int		ft_strcmp(const char *s1, const char *s2);
extern ssize_t	ft_write(int fd, const void *buf, size_t count);

#endif