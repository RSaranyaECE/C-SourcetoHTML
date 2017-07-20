#include "main.h"

/* Begin_html function definitation */
FILE *begin_html(FILE *dst_fp) 
{
	/* Add HTML begining tags into destination file */
	fprintf(dst_fp, "<!DOCTYPE html>\n");
	fprintf(dst_fp, "<html lang=\"en-US\">\n");
	fprintf(dst_fp, "<head>\n");
	fprintf(dst_fp, "<title>%s</title>\n", "code2html");
	fprintf(dst_fp, "<meta charset=\"UTF-8\">\n");
	fprintf(dst_fp, "<link rel=\"stylesheet\" href=\"styles.css\">\n");
	fprintf(dst_fp, "</head>\n");
	fprintf(dst_fp, "<body style=\"background-color:black; font-size:16px\">\n");
	fprintf(dst_fp, "<pre>\n");

	return dst_fp;
}

/* End_html function definitation */
FILE *end_html(FILE *dst_fp) 
{
	/* Add HTML closing tags into destination file */
	fprintf (dst_fp, "</pre>\n");
	fprintf (dst_fp, "</body>\n");
	fprintf (dst_fp, "</html>\n");

	return dst_fp;
}

