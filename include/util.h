#ifndef  __UTIL_H__
#define  __UTIL_H__

#include  <vector>
#include  <string>

#define  NUMVALUE							1
#define  HEXVALUE							2
#define  STRINGVALUE						3
#define  FAILURE							0xffff
#define  SKIPSPACE(p)						while(*p == '\t' || *p == ' ') ++p;


typedef struct t_argval
{
    int keyindex;          /* keyword (index into passed list) */
    int numval;            /* value */
	char strval[1024];       /* string value*/
} argval;


int get_args(char *p, argval *value, int type);
void strip_comments(const char *comment_chars, char *text);
void strip_comments_nocr(const char *comment_chars, char *text);
void tokenize(const std::string &strString, const char *pDelimiters, std::vector<std::string> &vecTokens);
void c_trim(std::string &strString);


#endif  // end of __UTIL_H__
