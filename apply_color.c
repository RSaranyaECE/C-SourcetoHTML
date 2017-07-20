#include "main.h"

/* Replace symbol by html code */
void replace_html_symbol(char *data)
{
	int i_index = 0, j_index = 0;

	while(data[j_index])
	{
		/* Lessthan symbol*/
		if (data[j_index] == '<')
		{
			temp_buff [i_index++] = '&'; 
			temp_buff [i_index++] = 'l'; 
			temp_buff [i_index++] = 't'; 
			temp_buff [i_index++] = ';'; 
		}
		
		/* Greaterthan symbol*/
		else if (data[j_index] == '>')
		{
			temp_buff[i_index++] = '&'; 
			temp_buff[i_index++] = 'g'; 
			temp_buff[i_index++] = 't'; 
			temp_buff[i_index++] = ';'; 
		}

		/* Other characters */
		else
			temp_buff[i_index++] = data[j_index]; 
	
		j_index++;
	}

	temp_buff[i_index] = '\0';
}

/* function to apply HTML color scheme */
FILE *apply_color (FILE *dst_fp, char *data, State_e state, int pre_flag)
{
	/* check for < and > symbol, if present replace with &lt; and &gt; resp */
	memset (temp_buff, '\0', sizeof(temp_buff));
	replace_html_symbol(data);

	if (state == s0_idle && pre_flag)
		state = s2_preprocessor;

	/* code to apply HTML tags and color scheme on source string */
	switch (state)
	{
		case s0_idle:
			fprintf(dst_fp, "<span class=\"idle\">%s</span>", temp_buff);
			break;

		case  s1_comment:
			fprintf(dst_fp, "<span class=\"comment\">%s</span>", temp_buff);
			break;

		case  s2_preprocessor:
			fprintf(dst_fp, "<span class=\"preprocess_dir\">%s</span>", temp_buff);
			break;

		case  s3_headerfile:
			fprintf(dst_fp, "<span class=\"header_file\">%s</span>", temp_buff);
			break;

		case  s4_res_key1:
			fprintf(dst_fp, "<span class=\"reserved_key1\">%s</span>", temp_buff);
			break;

		case  s5_res_key2:
			fprintf(dst_fp, "<span class=\"reserved_key2\">%s</span>", temp_buff);
			break;

		case  s6_string:
			fprintf(dst_fp, "<span class=\"string\">%s</span>", temp_buff);
			break;

		case  s7_numeric_const:
			fprintf(dst_fp, "<span class=\"numeric_constant\">%s</span>", temp_buff);
			break;

		case  s8_ASCII_char:
			fprintf(dst_fp, "<span class=\"char\">%s</span>", temp_buff);
			break;
		
		case  s9_fmt_spec:
			fprintf(dst_fp, "<span class=\"fmt_spec\">%s</span>", temp_buff);
			break;
	}

	return dst_fp;
}

