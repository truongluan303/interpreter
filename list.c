/******************************************************************************
 * Source File Lister
 * 
 *
 * Print the contents of a source file with line number and page headings
 * 
 * File:    list.c
 * 
 * Usage:   list sourcefile
 * 
 *      sourcefile      name of source file to list
 *
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define FORM_FEED_CHAR          '\f'

#define MAX_FILE_NAME_LEN       32
#define MAX_SRC_LINE_LEN        256
#define MAX_PRINT_LINE_LEN      80
#define PAGE_MAX_LINE           50
#define DATE_STR_LEN            26


/*
Boolean
*/
typedef enum {
    FALSE,
    TRUE,
} BOOLEAN;


/*===========================================================================*/
/* Globals                                                                   */
/*===========================================================================*/

unsigned int    line_number = 0;                // current line number
unsigned int    page_number = 0;                // current page number
unsigned int    level       = 0;                // current nesting level
unsigned int    line_count  = PAGE_MAX_LINE;    // no. of lines on current page

char            src_buff[MAX_PRINT_LINE_LEN];   // source file buffer
char            src_name[MAX_FILE_NAME_LEN];    // name of source file
char            date[DATE_STR_LEN];             // current date and time

FILE*           src_file;                       // the source file


/*===========================================================================*/
/* main             Contains the main loop that drives the lister.           */
/*===========================================================================*/

main(argc, argv)

    int     argc;
    char*   argv[];

{
    BOOLEAN get_src_line();

    init_lister(argv[1]);

    /*
    -- repeatedly call `get_src_line` to read and print the next source line
    -- until the end of file
    */
    while (get_src_line());
}


/*===========================================================================*/
/* init_lister      Initialize the lister globals.                           */
/*===========================================================================*/

init_lister(name)

    char*   name;                           // name of the source file

{
    time_t  timer;

    /*
    -- copy the source file name and open the source file
    */
    strcpy(src_name, name);
    src_file = fopen(src_name, "r");

    /*
    -- set the current date and time in the date string
    */
    time(&timer);
    strcpy(date, asctime(localtime(&timer)));
}


/*===========================================================================*/
/* get_src_line     Read the next line from the source file. If there was    */
/*                  one, print it out and return `TRUE`. Else at the end of  */
/*                  the file, so return `FALSE`.                             */
/*===========================================================================*/

BOOLEAN get_src_line() 

{
    char    print_buffer[MAX_SRC_LINE_LEN + 9];
    
    if ((fgets(src_buff, MAX_SRC_LINE_LEN, src_file)) != NULL) {
        line_number++;
        sprintf(print_buffer, "%4d %d %s", line_number, level, src_buff);
        print_line(print_buffer);
        return TRUE;
    }
    return FALSE;
}


/*===========================================================================*/
/* print_line       Print out a line. Start a new page if the current page   */
/*                  is full.                                                 */
/*===========================================================================*/

print_line(line)

    char    line[];                         // the line to be printed

{
    char    save_ch;
    char*   save_chp = NULL;

    if (++line_count > PAGE_MAX_LINE) {
        print_header();
        line_count = 1;
    }

    if (strlen(line) > MAX_PRINT_LINE_LEN) {
        save_chp = &line[MAX_PRINT_LINE_LEN];
    }

    if (save_chp) {
        save_ch     = *save_chp;
        *save_chp   = '\0';
    }

    printf("%s", line);

    if (save_chp) {
        *save_chp = save_ch;
    }
}


/*===========================================================================*/
/* print_header     Print the page header at the top of the next page        */
/*===========================================================================*/

print_header()

{
    putchar(FORM_FEED_CHAR);
    printf("Page %d   %s   %s\n\n", ++page_number, src_name, date);
}