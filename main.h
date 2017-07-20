#ifndef MAIN_H
#define MAIN_H

/* Header include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* user defined macros */
#define FAILURE -1
#define SUCCESS 0
#define SET 1
#define RESET 0

/* global Declarations */
char line_no, headerfile_flag;
char string_buff[100], temp_buff[100];
char buffer[2048];
char start_string;

int l_flag;

/* user defined enums for States */
typedef enum States
{
	s0_idle,
	s1_comment,
	s2_preprocessor,
	s3_headerfile,
	s4_res_key1,
	s5_res_key2,
	s6_string,
	s7_numeric_const,
	s8_ASCII_char,
	s9_fmt_spec

}State_e;

/* user defined enums for Events */
typedef enum Events
{
	SINGLE_LINE_COMMENT,
	MULTI_LINE_COMMENT,
	PREPROCESSOR_DIRECTIVE,
	HEADER_FILE,
	STRING,
	RESERVED_KEY1,
	RESERVED_KEY2,
	NUMERIC_CONST,
	ASCII_CHAR,
	FORMAT_SPECIFIERS,
	OTHERS

}Event_e;

/* user defined enums for colors */
typedef enum Colors
{
	black,
	blue,
	purple,
	green,
	yellow,
	orange,
	magenta,
	red

}color_e;

/* user defined function prototype */
FILE * apply_color(FILE *dst_fp, char *data, State_e state, int flag);
FILE *src_to_html(FILE *src_fd, FILE *dst_fp);
FILE *begin_html (FILE *dst_fp); 
FILE *end_html (FILE *dest_fp);


#endif
