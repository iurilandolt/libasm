#include "libasm.h"

int main(void)
{
	printf("ft_strlen(\"Hello\")   = %zu\n", ft_strlen("Hello"));
	printf("  strlen(\"Hello\")   = %zu\n",   strlen("Hello"));

	printf("ft_strlen(\"\")        = %zu\n", ft_strlen(""));
	printf("  strlen(\"\")        = %zu\n",   strlen(""));

	printf("ft_strlen(\"abc\")     = %zu\n", ft_strlen("abc"));
	printf("  strlen(\"abc\")     = %zu\n",   strlen("abc"));

	char dst[50];
	char ref[50];

	ft_strcpy(dst, "Hello, World!");
	strcpy(ref, "Hello, World!");
	printf("ft_strcpy(\"Hello, World!\") = %s\n", dst);
	printf("  strcpy(\"Hello, World!\") = %s\n",   ref);

	ft_strcpy(dst, "");
	strcpy(ref, "");
	printf("ft_strcpy(\"\")             = \"%s\"\n", dst);
	printf("  strcpy(\"\")             = \"%s\"\n",   ref);

	printf("ft_strcmp(\"abc\", \"abc\") = %d\n", ft_strcmp("abc", "abc"));
	printf("  strcmp(\"abc\", \"abc\") = %d\n",   strcmp("abc", "abc"));

	printf("ft_strcmp(\"abc\", \"abd\") = %d\n", ft_strcmp("abc", "abd"));
	printf("  strcmp(\"abc\", \"abd\") = %d\n",   strcmp("abc", "abd"));

	printf("ft_strcmp(\"abd\", \"abc\") = %d\n", ft_strcmp("abd", "abc"));
	printf("  strcmp(\"abd\", \"abc\") = %d\n",   strcmp("abd", "abc"));

	printf("ft_strcmp(\"\", \"\")       = %d\n", ft_strcmp("", ""));
	printf("  strcmp(\"\", \"\")       = %d\n",   strcmp("", ""));

	printf("ft_write stdout: ");
	ft_write(1, "Hello from ft_write!\n", 21);
	printf("  write stdout: ");
	write(1, "Hello from write!\n", 18);

	ssize_t r1 = ft_write(-1, "x", 1);
	printf("ft_write bad fd: ret=%zd errno=%d\n", r1, errno);

	ssize_t r2 = write(-1, "x", 1);
	printf("  write bad fd: ret=%zd errno=%d\n", r2, errno);

	return (0);
}
