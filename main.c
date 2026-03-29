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

	return (0);
}
