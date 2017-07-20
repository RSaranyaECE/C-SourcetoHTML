#include "main.h"

/* Reserved Key words 1 */
char *res_key1[] = {"const", "volatile", "extern", "auto", "register", "static", "signed", "unsigned", "short", "long", "double", "char", "int", "float", "struct", "union", "enum", "void", "typedef", "FILE", NULL};

/* Reserved Key words 2 */
char *res_key2[] = {"goto", "return", "continue", "break", "if", "else", "for", "while", "do", "switch", "case", "default", "sizeof", "EOF", "stderr", "stdout", "stdin", NULL};

/* Format specifiers */
char *fmt_spec[] = {"%", "d", "c", "f", "s", "p", "u", "i", "o", "x", "X", "u", "F", "e", "E", "a", "A", "g", "G", "ld", "lf", "hX", "lX", "llX", "Lf", NULL};

/* Declare the flags */
int end_flag, stop_flag, str_flag, prev_state, preprocessor_flag, preprocessor_end, pr_flag = 1;

/* Get Event */
int get_event(FILE *src)
{
	char ch, buff[100];
	int length, index;

	/* get a character and is not EOF */
	if ((ch = fgetc(src)) != EOF)
	{
		/* Single and Multiline command */
		if (ch == '/')
		{
			if ((ch = fgetc(src)) != EOF)
			{
				if(ch == '*')
					return MULTI_LINE_COMMENT;
				else if (ch == '/')
					return SINGLE_LINE_COMMENT;
				else
				{
					fseek(src, -1, SEEK_CUR);
					return OTHERS;
				}
			}
		}

		/* ASCII character */
		else if (ch == 39)
			return ASCII_CHAR;

		/* Preprocessor Directive */
		else if (ch == '#')
			return PREPROCESSOR_DIRECTIVE;

		/* Format Specifiers */
		else if (str_flag && (ch == '%' || ch == '\\'))
			return FORMAT_SPECIFIERS;

		/* Header File */
		else if (ch == '<' || ch == '"')
		{
			if (prev_state == PREPROCESSOR_DIRECTIVE)
				return HEADER_FILE;

			/* String */
			else if (ch == '"')
				return STRING;
			else
				return OTHERS;
		}

		/* Numeric Constant */
		else if (ch >= '0' && ch <= '9')
		{
			fseek(src, -2, SEEK_CUR);
			ch = fgetc(src);

			fseek(src, 1, SEEK_CUR);
			if (ch == '_' || 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z')
				return OTHERS;

			return NUMERIC_CONST;
		}

		/* Reserved Keywords */
		else if (ch >= 'a' && ch <= 'z')
		{
			fseek(src, -2, SEEK_CUR);
			ch = fgetc(src);

			/* String */
			if ('0' <= ch && ch <= '9' || 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z')
			{
				fseek(src, 1, SEEK_CUR);

				if (str_flag)
					return STRING;

				return OTHERS;
			}

			int rbytes = fread(buff, 1, 8, src);
			buff[rbytes] = '\0';

			index = 0;

			/* Reserved Keyword 1 */
			while (res_key1[index] != NULL && strncmp(buff, res_key1[index], strlen(res_key1[index])) != 0)
				index++;

			if (res_key1[index] != NULL)
			{
				length = strlen(res_key1[index]);

				if (buff[length] == ' ' || buff[length] == '(' || buff[length] == ')' || buff[length] == ';' || buff[length] == '\n')
					return RESERVED_KEY1;
				else
				{
					fseek(src, -7, SEEK_CUR);
					return OTHERS;
				}
			}
			index = 0;

			/* Reserved Keyword 2 */
			while (res_key2[index] != NULL && strncmp(buff, res_key2[index], strlen(res_key2[index])) != 0)
				index++;

			if (res_key2[index] != NULL)
			{
				length = strlen(res_key2[index]);

				if (buff[length] == ' ' || buff[length] == '(' || buff[length] == ')' || buff[length] == ';' || buff[length] == '\n')
					return RESERVED_KEY2;
				else
				{
					fseek(src, -7, SEEK_CUR);
					return OTHERS;
				}
			}

			/* String */
			else if (str_flag)
			{
				fseek(src, -7, SEEK_CUR);
				return STRING;
			}

			/* Others */
			else
			{
				fseek(src, -7, SEEK_CUR);
				return OTHERS;
			}
		}

		/* String */
		else if (str_flag)
			return STRING;

		/* Others */
		else
			return OTHERS;
	}

	if (ch == EOF)
		stop_flag = 1;
}

/* Get string */
FILE *get_string(FILE *src_fp, int event, char *string, int *state)
{
	char ch;
	int index = 0, len = 0;
	char buff[100], temp[5];

	/* Select event */
	switch(event)
	{
		/* Single line comment */
		case SINGLE_LINE_COMMENT:

			fseek(src_fp, -2, SEEK_CUR);
			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of Single line comment */
				if (ch == '\n')
				{
					pr_flag = 1;
					string[index++] = ch;
					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s1_comment;
			prev_state = *state;

			break;

			/* Multi Line comment */
		case MULTI_LINE_COMMENT:

			fseek(src_fp, -2, SEEK_CUR);
			while ((ch = fgetc(src_fp)) != EOF)
			{
				if (ch == '*')
				{
					string[index++] = ch;

					/* End of Multi Line comment */
					if ((ch = fgetc(src_fp)) == '/')
					{
						string[index++] = ch;
						break;
					}
					else
						string[index++] = ch;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s1_comment;
			prev_state = *state;

			break;

			/* Preprocessor Directive */
		case PREPROCESSOR_DIRECTIVE:

			fseek(src_fp, -1, SEEK_CUR);
			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of Preprocessor Directive */
				if (ch == ' ' || ch == '<')
				{
					if (ch == ' ')
						string[index++] = ch;
					else
						fseek(src_fp, -1, SEEK_CUR);

					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s2_preprocessor;
			prev_state = *state;

			break;

			/* Header File */
		case HEADER_FILE:

			fseek(src_fp, -1, SEEK_CUR);

			if ((ch = getc(src_fp)) != EOF)
				string[index++] = ch;

			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of Header File */
				if (ch == '"' || ch == '>')
				{
					string[index++] = ch;
					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s3_headerfile;
			prev_state = *state;

			break;

			/* String */
		case STRING:

			fseek(src_fp, -1, SEEK_CUR);

			if (str_flag == 0 && (ch = fgetc(src_fp)) != EOF)
				string[index++] = ch;

			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* Format specifier state change  */
				if (ch == '%' || ch == '\\')
				{
					fseek(src_fp, -1, SEEK_CUR);
					str_flag = 1;
					break;
				}

				/* End of String */
				else if (ch == '"')
				{
					string[index++] = ch;

					if (str_flag)
						str_flag = 0;

					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s6_string;
			prev_state = *state;

			break;

			/* Format Specifiers */
		case FORMAT_SPECIFIERS:

			fseek(src_fp, -1, SEEK_CUR);

			ch = fgetc(src_fp);
			string[index++] = ch;

			/* Format specifiers */
			if (ch == '%')
			{
				while ((ch = fgetc(src_fp)) != EOF)
				{
					if (ch == '[')
						break;

					/* Start of Format specifiers */
					if ('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '%')
					{
						string[index++] = '\0';
						break;
					}

					string[index++] = ch;
				}

				string[index++] = ch;

				/* Pattern Matching */
				if (ch == '[')
				{
					while ((ch = fgetc(src_fp)) != EOF)
					{
						string[index++] = ch;

						/* End of Pattern Matching */
						if (ch == ']')
							break;
					}

					string[index++] = '\0';

					/* Setting the current and previous state */
					*state = s9_fmt_spec;
					prev_state = *state;
				}

				else
				{
					temp[0] = ch;
					fread(temp + 1, 1, 2, src_fp);

					int iindex = 0;

					while (fmt_spec[iindex] != NULL && strncmp(temp, fmt_spec[iindex], strlen(fmt_spec[iindex])) != 0)
						iindex++;

					/* String */
					if (fmt_spec[iindex] == NULL)
					{
						temp[3] = '\0';
						fseek(src_fp, -4, SEEK_CUR);

						/* Setting the current and previous state */
						*state = s6_string;
						prev_state = *state;
					}
					/* Format specifiers */
					else
					{
						len = strlen(fmt_spec[iindex]);
						temp[len] = '\0';
						fseek(src_fp, -3+len, SEEK_CUR);
						strcat(string, temp);

						/* Setting the current and previous state */
						*state = s9_fmt_spec;
						prev_state = *state;
					}
				}
			}

			/* Escape Sequence */
			else 
			{
				while (index < 2)
					string[index++] = fgetc(src_fp);

				string[index++] = '\0';

				/* String */
				if (string[1] == ' ' || string[1] == '\t' || string[1] == '\\')
				{
					/* Setting the current and previous state */
					*state = s6_string;
					prev_state = *state;
				}
				/* Fromat specifiers */
				else
				{
					/* Setting the current and previous state */
					*state = s9_fmt_spec;
					prev_state = *state;
				}
			}

			break;

			/* Reserved Keyword 1 */
		case RESERVED_KEY1:

			fseek(src_fp, -8, SEEK_CUR);

			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of Reserved keyword 1 */
				if (ch == ' ' || ch == '(' || ch == ')' || ch == ';' || ch == '\n')
				{
					fseek(src_fp, -1, SEEK_CUR);
					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s4_res_key1;
			prev_state = *state;

			break;

			/* Reserved keyword 2 */
		case RESERVED_KEY2:

			fseek(src_fp, -8, SEEK_CUR);
			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of Reserved keyword 2 */
				if (ch == ' ' || ch == '(' || ch == ')' || ch == ';' || ch == '\n')
				{
					fseek(src_fp, -1, SEEK_CUR);
					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s5_res_key2;
			prev_state = *state;

			break;

			/* Numeric constant */
		case NUMERIC_CONST:

			fseek(src_fp, -1, SEEK_CUR);
			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of Numeric constant */
				if (ch == ' ' || ch == ';' || ch == ')' || ch == '}' || ch == ']' || ch == ',' || ch == ':' || ch == '\n')
				{
					fseek(src_fp, -1, SEEK_CUR);
					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s7_numeric_const;
			prev_state = *state;

			break;

			/* ASCII character */
		case ASCII_CHAR:

			fseek(src_fp, -1, SEEK_CUR);

			if ((ch = fgetc(src_fp)) != EOF)
				string[index++] = ch;

			while ((ch = fgetc(src_fp)) != EOF)
			{
				/* End of ASCII character */
				if (ch == 39)
				{
					string[index++] = ch;
					break;
				}
				else
					string[index++] = ch;
			}

			string[index] = '\0';

			/* Setting the current and previous state */
			*state = s8_ASCII_char;
			prev_state = *state;

			break;

			/* others */
		case OTHERS:

			fseek(src_fp, -1, SEEK_CUR);

			if ((ch = fgetc(src_fp)) != EOF)
			{
				/* Macro names or functions */
				if (prev_state == s2_preprocessor)
				{
					string[index++] = ch;

					if (ch == '\n')
					{
						pr_flag = 1;
						string[index] = '\0';
						return src_fp;
					}
					else if (ch == '}')
						end_flag = 1;

					while ((ch = fgetc(src_fp)) != EOF)
					{
						string[index++] = ch;

						if (ch == '\n')
						{
							if (end_flag)
							{
								end_flag = 0;
								preprocessor_end = 1;
							
								/* Setting the current and previous state */
								*state = s0_idle;
								prev_state = *state;
							}
							pr_flag = 1;
							break;
						}

						/* End of Macro names */
						if (ch == ' ' || ch == '\t')
							break;

						/* Start of Macro functions */
						else if (ch == '{' || ch == '\\')
						{
							preprocessor_flag = 1;
							/* Setting the current and previous state */
							*state = s0_idle;
							prev_state = *state;
							break;
						}

						/* End of Macro functions */
						else if (ch == '}')
						{
							preprocessor_end = 1;
						
							/* Setting the current and previous state */
							*state = s0_idle;
							prev_state = *state;
							break;
						}
					}

					string[index] = '\0';
				}

				else
				{
					/* End of Macro functions */
					if (preprocessor_flag && ch == '}')
						preprocessor_end = 1;

					string[index++] = ch;
					string[index] = '\0';

					if (ch == '\n')
						pr_flag = 1;

					/* Setting the current and previous state */
					*state = s0_idle;
					prev_state = *state;
				}
			}

			/* EOF set the stop flag */
			else
				stop_flag = 1;

			break;
	}

	return src_fp;
}

FILE *add_line_no(FILE *dst_fp, int line_no)
{
	/* code to enable line number */
	fprintf (dst_fp, "<span style=\"color:crimson\">%3d</span> ", line_no);

	return dst_fp;
}

/* Source to html function */
FILE *src_to_html(FILE *src_fp, FILE *dst_fp)
{
	int event, state, count = 0;
	char string[200];

	while (1)
	{
		memset(string, 0, sizeof(string));

		/* stop the loop if EOF arrives */
		if (stop_flag)
			break;

		/* Add line numbers */
		if (l_flag && pr_flag)
		{
			count++;
			dst_fp = add_line_no(dst_fp, count);
			pr_flag = 0;
		}
		
		/* Get the event */
		event = get_event(src_fp);

		/* Perform that event and get the string */
		src_fp = get_string(src_fp, event, string, &state);

		/* Apply color */
		dst_fp = apply_color(dst_fp, string, state, preprocessor_flag);

		/* Change the state from pre-processor to idle */
		if (preprocessor_end)
		{
			preprocessor_flag = 0;
			preprocessor_end = 0;
		}
	}

	/* return dst_file ptr */
	return dst_fp;
}
