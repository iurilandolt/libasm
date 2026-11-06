#include "libasm.h"

int	main(void)
{
	size_t		ft, ref;
	char		dst_ft[4096];
	char		dst_ref[4096];
	char		long_str[4096];
	int			ft_cmp, ref_cmp;
	ssize_t		ft_wr, ref_wr;
	ssize_t		ft_rd, ref_rd;
	int			ft_err, ref_err;
	char		buf_ft[16];
	char		buf_ref[16];
	int			fd;
	char		*ft_dup, *ref_dup;
	memset(long_str, 'A', sizeof(long_str) - 1);
	long_str[sizeof(long_str) - 1] = '\0';

	/* ft_strlen */
	printf("\n=== ft_strlen ===\n");
	ft = ft_strlen("");          ref = strlen("");
	printf("  \"\"          ft: %zu  |  ref: %zu\n", ft, ref);
	ft = ft_strlen("Hello");     ref = strlen("Hello");
	printf("  \"Hello\"     ft: %zu  |  ref: %zu\n", ft, ref);
	ft = ft_strlen(long_str);    ref = strlen(long_str);
	printf("  long (4095) ft: %zu  |  ref: %zu\n", ft, ref);

	/* ft_strcpy */
	printf("\n=== ft_strcpy ===\n");
	ft_strcpy(dst_ft, "Hello");  strcpy(dst_ref, "Hello");
	printf("  \"Hello\"   ft: %s  |  ref: %s\n", dst_ft, dst_ref);
	ft_strcpy(dst_ft, "");       strcpy(dst_ref, "");
	printf("  \"\"        ft: \"%s\"  |  ref: \"%s\"\n", dst_ft, dst_ref);
	ft_strcpy(dst_ft, long_str); strcpy(dst_ref, long_str);
	printf("  long (4095) ft: %zu chars  |  ref: %zu chars\n", strlen(dst_ft), strlen(dst_ref));

	/* ft_strcmp */
	printf("\n=== ft_strcmp ===\n");
	ft_cmp = ft_strcmp("", "");          ref_cmp = strcmp("", "");
	printf("  \"\",\"\"         ft: %d   |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("", "abc");       ref_cmp = strcmp("", "abc");
	printf("  \"\",\"abc\"      ft: %d  |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abc", "");       ref_cmp = strcmp("abc", "");
	printf("  \"abc\",\"\"      ft: %d   |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abc", "abc");    ref_cmp = strcmp("abc", "abc");
	printf("  \"abc\",\"abc\"   ft: %d   |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abc", "abd");    ref_cmp = strcmp("abc", "abd");
	printf("  \"abc\",\"abd\"   ft: %d  |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abd", "abc");    ref_cmp = strcmp("abd", "abc");
	printf("  \"abd\",\"abc\"   ft: %d   |  ref: %d\n", ft_cmp, ref_cmp);
	ft_cmp = ft_strcmp("abc", "abcd");   ref_cmp = strcmp("abc", "abcd");
	printf("  \"abc\",\"abcd\"  ft: %d  |  ref: %d\n", ft_cmp, ref_cmp);

	/* ft_write */
	printf("\n=== ft_write ===\n");
	ft_wr  = ft_write(1, "ft:  hello from ft_write\n", 25);
	ref_wr = write(1,    "ref: hello from    write\n", 25);
	printf("  stdout  ft: %zd  |  ref: %zd\n", ft_wr, ref_wr);
	fd     = open("/tmp/libasm_write_test", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ft_wr  = ft_write(fd, "hello", 5);
	ref_wr = write(fd,    "hello", 5);
	close(fd);
	printf("  open fd ft: %zd  |  ref: %zd\n", ft_wr, ref_wr);
	errno = 0; ft_write(-1, "x", 1); ft_err  = errno;
	errno = 0; write(-1, "x", 1);    ref_err = errno;
	printf("  bad fd  ft: errno=%d  |  ref: errno=%d\n", ft_err, ref_err);

	/* ft_read */
	printf("\n=== ft_read ===\n");

	int pipefd[2];
	pipe(pipefd);
	ft_write(pipefd[1], "helloworld", 10);
	close(pipefd[1]);
	dup2(pipefd[0], 0);
	bzero(buf_ft, sizeof(buf_ft));
	ft_rd = ft_read(0, buf_ft, 5);
	bzero(buf_ref, sizeof(buf_ref));
	ref_rd = read(0, buf_ref, 5);
	close(pipefd[0]);
	printf("  stdin  ft: \"%s\" (%zd)  |  ref: \"%s\" (%zd)\n", buf_ft, ft_rd, buf_ref, ref_rd);

	fd = open("/tmp/libasm_read_test", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ft_write(fd, "hello", 5);
	close(fd);
	fd = open("/tmp/libasm_read_test", O_RDONLY);
	bzero(buf_ft, sizeof(buf_ft));
	ft_rd = ft_read(fd, buf_ft, 5);
	close(fd);
	fd = open("/tmp/libasm_read_test", O_RDONLY);
	bzero(buf_ref, sizeof(buf_ref));
	ref_rd = read(fd, buf_ref, 5);
	close(fd);

	printf("  open fd ft: \"%s\" (%zd)  |  ref: \"%s\" (%zd)\n", buf_ft, ft_rd, buf_ref, ref_rd);
	errno = 0; ft_read(-1, buf_ft, 1);  ft_err  = errno;
	errno = 0; read(-1, buf_ref, 1);    ref_err = errno;
	printf("  bad fd  ft: errno=%d  |  ref: errno=%d\n", ft_err, ref_err);


	/* ft_strdup */
	printf("\n=== ft_strdup ===\n");
	ft_dup = ft_strdup("Hello");   ref_dup = strdup("Hello");
	printf("  \"Hello\"  ft: %s  |  ref: %s\n", ft_dup, ref_dup);
	free(ft_dup); free(ref_dup);
	ft_dup = ft_strdup("");        ref_dup = strdup("");
	printf("  \"\"       ft: \"%s\"  |  ref: \"%s\"\n", ft_dup, ref_dup);
	free(ft_dup); free(ref_dup);
	ft_dup = ft_strdup(NULL);      ref_dup = NULL;
	printf("  NULL     ft: %p  |  ref: %p\n", (void *)ft_dup, (void *)ref_dup);
	ft_dup = ft_strdup(long_str);  ref_dup = strdup(long_str);
	printf("  long (4095) ft: %zu chars  |  ref: %zu chars\n", strlen(ft_dup), strlen(ref_dup));
	free(ft_dup);
	free(ref_dup);

	/* ft_calloc */
	printf("\n=== ft_calloc ===\n");
	char *ft_cal = ft_calloc(10, 4);
	char *ref_cal = calloc(10, 4);

	int ft_nz = 0, ref_nz = 0;
	for (int i = 0; i < 40; i++) {
		if (ft_cal[i]) ft_nz++;
		if (ref_cal[i]) ref_nz++;
	}
	printf("  10*4    ft: %d nonzero  |  ref: %d nonzero\n", ft_nz, ref_nz);
	free(ft_cal); free(ref_cal);

	ft_cal = ft_calloc(0, 0);
	ref_cal = calloc(0, 0);
	printf("  0*0     ft: %p  |  ref: %p\n", (void *)ft_cal, (void *)ref_cal);
	free(ft_cal); free(ref_cal);

	size_t huge = SIZE_MAX;
	errno = 0;
	ft_cal = ft_calloc(huge, 2);  ft_err = errno;
	errno = 0;
	ref_cal = calloc(huge, 2);    ref_err = errno;
	printf("  overflow ft: %p errno=%d  |  ref: %p errno=%d\n", (void *)ft_cal, ft_err, (void *)ref_cal, ref_err);
	printf("\n");
	return (0);
}
