#include "libasm.h"

int	main(void)
{
	size_t		ft, ref;
	char		dst_ft[64];
	char		dst_ref[64];
	int			ft_cmp, ref_cmp;
	ssize_t		ft_wr, ref_wr;
	ssize_t		ft_rd, ref_rd;
	int			ft_err, ref_err;
	char		buf_ft[16];
	char		buf_ref[16];
	int			fd;
	char		*ft_dup, *ref_dup;

	/* ft_strlen */
	printf("\n=== ft_strlen ===\n");
	ft = ft_strlen("");        ref = strlen("");
	printf("  \"\"        ft: %zu  |  ref: %zu\n", ft, ref);
	ft = ft_strlen("Hello");   ref = strlen("Hello");
	printf("  \"Hello\"   ft: %zu  |  ref: %zu\n", ft, ref);
	ft = ft_strlen("foo bar"); ref = strlen("foo bar");
	printf("  \"foo bar\" ft: %zu  |  ref: %zu\n", ft, ref);

	/* ft_strcpy */
	printf("\n=== ft_strcpy ===\n");
	ft_strcpy(dst_ft, "Hello");  strcpy(dst_ref, "Hello");
	printf("  \"Hello\"   ft: %s  |  ref: %s\n", dst_ft, dst_ref);
	ft_strcpy(dst_ft, "");       strcpy(dst_ref, "");
	printf("  \"\"        ft: \"%s\"  |  ref: \"%s\"\n", dst_ft, dst_ref);

	/* ft_strcmp */
	printf("\n=== ft_strcmp ===\n");
	ft_cmp = ft_strcmp("abc", "abc");   ref_cmp = strcmp("abc", "abc");
	printf("  \"abc\",\"abc\"  ft: %d   |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abc", "abd");   ref_cmp = strcmp("abc", "abd");
	printf("  \"abc\",\"abd\"  ft: %d  |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abd", "abc");   ref_cmp = strcmp("abd", "abc");
	printf("  \"abd\",\"abc\"  ft: %d   |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abc", "abcd");  ref_cmp = strcmp("abc", "abcd");
	printf("  \"abc\",\"abcd\" ft: %d  |  ref: %d\n", ft_cmp, ref_cmp);

	/* ft_write */
	printf("\n=== ft_write ===\n");
	ft_wr  = ft_write(1, "ft:  hello from ft_write\n", 25);
	ref_wr = write(1,    "ref: hello from    write\n", 25);
	printf("  bytes  ft: %zd  |  ref: %zd\n", ft_wr, ref_wr);
	errno = 0; ft_write(-1, "x", 1); ft_err  = errno;
	errno = 0; write(-1, "x", 1);    ref_err = errno;
	printf("  bad fd ft: errno=%d  |  ref: errno=%d\n", ft_err, ref_err);

	/* ft_read */
	printf("\n=== ft_read ===\n");
	fd = open("/tmp/libasm_test", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	write(fd, "hello", 5);
	close(fd);
	fd = open("/tmp/libasm_test", O_RDONLY);
	bzero(buf_ft, sizeof(buf_ft));
	ft_rd = ft_read(fd, buf_ft, 5);
	close(fd);
	fd = open("/tmp/libasm_test", O_RDONLY);
	bzero(buf_ref, sizeof(buf_ref));
	ref_rd = read(fd, buf_ref, 5);
	close(fd);
	printf("  \"hello\" ft: \"%s\" (%zd)  |  ref: \"%s\" (%zd)\n", buf_ft, ft_rd, buf_ref, ref_rd);
	errno = 0; ft_read(-1, buf_ft, 1);  ft_err  = errno;
	errno = 0; read(-1, buf_ref, 1);    ref_err = errno;
	printf("  bad fd  ft: errno=%d  |  ref: errno=%d\n", ft_err, ref_err);

	/* ft_strdup */
	printf("\n=== ft_strdup ===\n");
	ft_dup = ft_strdup("Hello");  ref_dup = strdup("Hello");
	printf("  \"Hello\"  ft: %s  |  ref: %s\n", ft_dup, ref_dup);
	free(ft_dup); free(ref_dup);
	ft_dup = ft_strdup("");       ref_dup = strdup("");
	printf("  \"\"       ft: \"%s\"  |  ref: \"%s\"\n", ft_dup, ref_dup);
	free(ft_dup); free(ref_dup);
	ft_dup = ft_strdup(NULL);     ref_dup = NULL;
	printf("  NULL     ft: %p  |  ref: %p\n", (void *)ft_dup, (void *)ref_dup);

	printf("\n");
	return (0);
}
