/******************************************************************************
 * Simple Tokenizer
 * 
 *
 * Recognize words, small integers, and the period
 * 
 * File:    tokenl.c
 * 
 * Usage:   tokenl sourcefile
 * 
 *      sourcefile      name of source file to tokenize
 *
 *****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define FORM_FEED_CHAR              '\f'
#define EOF_CHAR                    '\x7f'

#define MAX_FILE_NAME_LEN           32
#define MAX_SRC_LINE_LEN            256
#define MAX_PRINT_LINE_LEN          80
#define PAGE_MAX_LINES              50
#define DATE_STR_LEN                26
#define MAX_TOKEN_STR_LEN           MAX_SRC_LINE_LEN
#define MAX_CODE_BUFFER_SIZE        4096

#define MAX_INT                     23767               // 2^15 - 1
#define MAX_DIGIT_COUNT             20

#define CHAR_TABLE_SIZE             256


/*
Boolean
*/
typedef enum {
    FALSE,
    TRUE,
} BOOLEAN;

/*
Character codes
*/
typedef enum {
    LETTER,
    DIGIT,
    SPECIAL,
    EOF_CODE,
} CHAR_CODE;

/*
Token codes
*/
typedef enum {
    NO_TOKEN,
    WORD,
    NUMBER,
    PERIOD,
    END_OF_FILE,
    ERROR,
} TOKEN_CODE;

/*
Token name strings
*/
char* symbol_strings[] = {
    "<no token>",
    "<WORD>",
    "<NUMBER>",
    "<PERIOD>",
    "<END OF FILE>",
    "<ERROR>",
};

/*
Literal type
*/
typedef enum {
    INTEGER_LIT,
    STRING_LIT,
} LITERAL_TYPE;

/*
Literal structure
*/
typedef struct {
    LITERAL_TYPE type;
    union {
        int     integer;
        char    string[MAX_SRC_LINE_LEN];
    } value;
} LITERAL;


/*===========================================================================*/
/* Globals                                                                   */
/*===========================================================================*/

char            ch;                             // current input character
TOKEN_CODE      token;                          // code of the current token
LITERAL         literal;                        // value of literal
int             buffer_offset;                  // offset into source buffer
unsigned int    level       = 0;                // current nesting level
unsigned int    line_number = 0;                // current line number

char            src_buff[MAX_SRC_LINE_LEN];     // source file buffer
char            token_str[MAX_TOKEN_STR_LEN];   // token string

char*           bufferp     = src_buff;         // source buffer pointer
char*           tokenp      = token_str;        // token string pointer

unsigned int    digit_count;                    // no. of digits in number
BOOLEAN         count_error;                    // if too many digits in number

unsigned int    page_number = 0;
unsigned int    line_count  = PAGE_MAX_LINES;   // no. of lines on current page

char            src_name[MAX_FILE_NAME_LEN];    // name of the source file
char            date[DATE_STR_LEN];             // current date and time

FILE*           source_file;

CHAR_CODE       char_table[CHAR_TABLE_SIZE];


/*===========================================================================*/
/* char_code                Return the character code of give character      */
/*===========================================================================*/

#define char_code(ch)       char_table[ch]


/*===========================================================================*/
/* main                     Loop to tokenize source file                     */
/*===========================================================================*/

main(argc, argv)

    int     argc;
    char*   argv[];

{
    /*
    -- initialize the scanner
    */
    init_scanner(argv[1]);

    /*
    -- repeatedly fetch token until a period or the end of file
    */
    do {
        get_token();
        if (token == END_OF_FILE) {
            print_line("*** ERROR: Unexpected end of file.\n");
            break;
        }
        print_token();
    } while (token != PERIOD);

    quite_scanner();
}