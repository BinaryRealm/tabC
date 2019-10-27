#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help_message.h"

char *line;
size_t len = 0;
ssize_t nread;

#define WORDCOUNT 15
const char * kwords[WORDCOUNT]={"if","else","for","while","do","switch",
"case","default","#","//","\"","enum","struct","union","<$>"};

int countTabs(void)
{
	int tab_counter;
	for(int i = 0; i< nread; i++)
	{
		if(line[i]=='\t')
		{
			tab_counter++;
		}
		else
		{
			break;
		}
	}
	return tab_counter;
}


void multiTab(int tab_count, FILE *outf)
{
	for(int i = 0; i<tab_count; i++)
	{
		fputc('\t', outf);
	}
}

void printOpenBracket(int tab_count, FILE *outf)
{
	multiTab(tab_count, outf);
	fputc('{', outf);
	fputc('\n', outf);
}

void printClosedBracket(int tab_count, FILE *outf)
{
	multiTab(tab_count, outf);
	fputc('}', outf);
	fputc('\n', outf);
}

void fwriteWithSemicolon(FILE *outf)
{
	if(line[nread-1] == '\n')
	{
		line[nread-1] = ';';
		fwrite(line, nread, 1, outf);
		fputc('\n',outf);
	}

	else
	{
		fwrite(line, nread, 1, outf);
		fputc(';',outf);
		fputc('\n',outf);
	}
}

int lineOnlyTabs(void)
{
	for(int i = 0; i < nread-1; i++)
	{
		if(line[i]!='\t')
		{
			return 0;
		}
	}
	return 1;
}

void printLine(int tabs, FILE *outf)
{
	if(line[nread-3]==':' && line[nread-2]==':')//:: at the end is a (mostly) equivalent to <$> at the start
	{
		if(line+tabs == strstr(line,kwords[14]))//if both :: and <$> exist get rid of both of them
		{
			multiTab(tabs, outf);
			fwrite(line+tabs+3, nread-tabs-6, 1, outf);
			fputc('\n', outf);
			return;
		}
		fwrite(line, nread-3, 1, outf);
		fputc('\n', outf);
		return;
	}

	for(int i = 0; i < WORDCOUNT; i++)
	{
		char *kword_loc;
		if(line+tabs == (kword_loc = strstr(line, kwords[i])))
		{
			//if there are any non-tab spaces in front of a keyword it will not work
			if(i == 14)//prints without the no-process symbol.  This is a total override of any processing of the line
			{
				multiTab(tabs, outf);
				fwrite(line+tabs+3, nread-tabs-3, 1, outf);
				return;
			}
			else if ( (nread>strlen(kwords[i]))/*if by chance it is at the end and there is no newline*/
				&& (*(kword_loc+strlen(kwords[i])) != ' ') && (*(kword_loc+strlen(kwords[i])) != '\n')
				&& (*(kword_loc+strlen(kwords[i])) != '(')&&(*(kword_loc+strlen(kwords[i])) != '/')
				&&(i==0||i==1||i==2||i==3||i==4||i==5||i==6||i==7||i==11||i==12||i==13))//this of course only applies to keywords that can actually be in a variable
				//if there is not some gap in front of keyword
				//do nothing so it can be handled by comment if-clause below
			{}
			else
			{
				fwrite(line, nread, 1, outf);
				return;
			}
		}
	}
	//so now the line certainly has no keywords at the start
	char *p,*q,*r;
	if((p=strstr(line, kwords[9]))!= NULL)//if there is a line comment not at the start
	{
		if((q=strstr(line,"\"")) < p && q!=NULL)//if the comment is in a quote
		{
			if((r=strstr(p, "\""))> p&& r!=NULL)
			{
				fwriteWithSemicolon(outf);
				return;
			}
		}

		//this handles a comment after '::' directive or ':' C operator
		fwrite(line, p-line-2, 1, outf);//example hello:://->hello

		if(*(p-2)==':'&&*(p-1)==':')
		{}//do nothing

		else if(*(p-1)==':')
		{
			fwrite(p-2, nread-(p-line-2), 1, outf);
			return;
			//handles the label exception, of course it does not need a semicolon added
		}

		else
		{
			fwrite(p-2, 2, 1, outf);
			fputc(';', outf);
		}
		fwrite(p, nread-(p-line), 1, outf);
		return;
	}

	else if(line[nread-2]==':')
	{
		fwrite(line, nread, 1, outf);
		return;
	}

	else//at the end assume it is a standard line
	{
		fwriteWithSemicolon(outf);
	}

}

void convertToC(FILE *inf, FILE *outf)
{
	char * no_process_start = "<$$>\n";
	char * no_process_end = "</$$>\n";
	static int no_process = 0;
	int curr_line_tabs;
	static int prev_line_tabs = 0;

	if( (nread = getline(&line, &len, inf)) != -1)
	{
		curr_line_tabs= countTabs();

		if(nread==1)//preserve newlines
		{
			fputc('\n',outf);
			convertToC(inf, outf);
		}

		else if(line== strstr(line, no_process_end))//recognizes end of noprocess
		{
			no_process = 0;
			convertToC(inf, outf);
		}

		else if(no_process == 1)// main noprocess action when flag activated
		{
			fwrite(line, nread, 1, outf);
			convertToC(inf, outf);
		}

		else if(line== strstr(line, no_process_start))//opens noprocess block
		{
			no_process = 1;
			convertToC(inf, outf);
		}

		else if(lineOnlyTabs() == 1)//blank lines with only tabs are ok
		{
			fwrite(line, nread, 1, outf);
			convertToC(inf, outf);
		}
		else if(curr_line_tabs > prev_line_tabs)
		{
			for(int i = 0; i < curr_line_tabs - prev_line_tabs; i++)
			{
				printOpenBracket(curr_line_tabs +i - 1, outf);
			}
			prev_line_tabs = curr_line_tabs;
			printLine(curr_line_tabs, outf);
			convertToC(inf, outf);
		}
		else if(curr_line_tabs < prev_line_tabs)
		{
			for(int i = prev_line_tabs - curr_line_tabs; i > 0 ; i--)
			{
				printClosedBracket(curr_line_tabs+ i - 1, outf);
			}
			prev_line_tabs = curr_line_tabs;
			printLine(curr_line_tabs, outf);
			convertToC(inf, outf);
		}
		else
		{
			printLine(curr_line_tabs, outf);
			convertToC(inf, outf);
		}
	}
	else    //if the last line has tabs, then seal the block closed with brackets.
	{
		if(no_process==1)//multiline no_processing means absolutely no changes, so it can't be sealed off at the end with brackets
		{
			return;
		}
		if(prev_line_tabs>0)
		{
			for(int i = prev_line_tabs; i > 0 ; i--)
			{
				printClosedBracket(i-1, outf);
			}
		}
	}
}




int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		fprintf(stderr, "tabc: at least 1 input file path required.  Type option -help for help\n");
		exit(EXIT_FAILURE);
	}
	if(argc == 2)
	{
		if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0)
		{
			printf(HELP_MESSAGE);
			exit(EXIT_SUCCESS);
		}
		else
		{
			FILE *inf = fopen(argv[1], "r");
			if(inf==NULL)
			{
				fprintf(stderr, "tabc: unable to open input file\n");
				exit(EXIT_FAILURE);
			}
			convertToC(inf, stdout);
			free(line);
			fclose(inf);
			exit(EXIT_SUCCESS);
		}
	}

	if(argc == 3)
	{
		FILE *inf = fopen(argv[1], "r");

		if(inf==NULL)
		{
			fprintf(stderr, "tabc: unable to open input file\n");
			exit(EXIT_FAILURE);
		}
		FILE *outf = fopen(argv[2], "w");

		if(outf==NULL)
		{
			fprintf(stderr, "tabc: unable to open output file\n");
			exit(EXIT_FAILURE);
		};

		convertToC(inf, outf);
		free(line);
		fclose(inf);
		fclose(outf);
		exit(EXIT_SUCCESS);
	}
}
