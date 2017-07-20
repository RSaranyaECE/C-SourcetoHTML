#include "main.h"

/* Files open function */
int open_files(FILE **src, FILE **dst, char *argv[])
{
	/* open the src file, check error on open */
	if ((*src = fopen(argv[1], "r")) == NULL)
	{
		perror("source");
		return FAILURE;
	}
	
	/* open the dest file, check error on open */
	if ((*dst = fopen(argv[2], "w")) == NULL)
	{
		perror("destination");
		return FAILURE;
	}
	
	return SUCCESS;
}

/* Main function */
int main(int argc, char *argv[])
{
	FILE *src_fp, *dst_fp;

	/* Error handler */
	if (argc < 3 || argc > 4)
	{
		printf ("ERROR: ./<exe_file> <src_file.c> <output.html> <-n>\n");
		return FAILURE;
	}

	/* line number option */
	if (argc == 4 && strcmp(argv[3], "-n") == 0)
		l_flag = 1;

	/* Open source and destination files */
	if (open_files(&src_fp, &dst_fp, argv) != SUCCESS)
		return FAILURE;

	/* Main Part */
	dst_fp = begin_html(dst_fp);
	dst_fp = src_to_html(src_fp, dst_fp);
	dst_fp = end_html(dst_fp);

	printf("Successfully converted source file to html file\n");
	/* Close source and destination files */
	fclose(src_fp);
	fclose(dst_fp);
	
	return SUCCESS;
}
