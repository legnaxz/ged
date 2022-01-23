#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <errno.h>
#include  <sys/time.h>
#include  <time.h>
#include  <netdb.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <unistd.h>

#include  "util.h"
#include  "log.h"
#include  "cfg.h"

#include  <algorithm> 
#include  <functional> 
#include  <cctype>
#include  <locale>

// trim from start
static inline std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s)
{
	return ltrim(rtrim(s));
}

void c_trim(std::string &strString)
{
	strString = trim(strString);
}

int get_args(char *p, argval *value, int type)
{
	char *ptoken;
	char  left[1024];
	int   size = 0;

	left[0] = '\0';

	/* Skip leading spaces */
	SKIPSPACE(p);

	/* get target string: (all chars up to eol or comment) */
	if(*p == '\0')
	{
		ptoken = NULL;
	}
	else
	{
		ptoken = p;
	}

	if(*p == '=')
	{
		++p;
	}
	else
	{
		return 0;
	}

	SKIPSPACE(p);
	ptoken = p;
	while(isalnum(*p) || *p == '_' || *p == '-' || *p == '.' || *p == ',' ||
				*p == '^' || *p == '$' || *p == '+' || *p == '?' || *p == '*' || *p == '|' ||
				*p == '(' || *p == ')' || *p == '[' || *p == ']' || *p == '{' || *p == '}' ||
				*p == ':' || *p == ';' || *p == '\\' || *p == '/' || *p == '%')
	{
		++ p;
		++ size;
	}
	strncpy(left, ptoken, size);
	left[size] = '\0';

	if(type == NUMVALUE)
	{
		if(sscanf(left, "%i", &value->numval) == 1)
		{
			return 1;
		}
		return 0;
	}
	if(type == HEXVALUE)
	{
		if(sscanf(left, "%x", &value->numval) == 1)
		{
			return 1;
		}
		return 0;
	}    
	else // STRINGVALUE
	{
		if(sscanf(left, "%s", &value->strval) == 1)
		{
			return 1;
		}
		return 0;
	}
}

void strip_comments(const char *comment_chars, char *text)
{
	int quoted = 0;
	char *ps;

	for(ps = text; *ps != '\0'; ps ++)
	{
		if(*ps == '"')
		{
			quoted = !quoted;
			continue;
		}
		if((!quoted) && (strchr(comment_chars, *ps) != NULL))
		{
			*ps = '\n';
			ps++;
			*ps = '\0';
			break;
		}
	}
}

void strip_comments_nocr(const char *comment_chars, char *text)
{
	int quoted = 0;
	char *ps;
	std::string		str;

	for(ps = text; *ps != '\0'; ps ++)
	{
		if(*ps == '"')
		{
			quoted = !quoted;
			continue;
		}
		if((!quoted) && (strchr(comment_chars, *ps) != NULL))
		{
			*ps = '\0';
			str = text;
//			str.trim();
			str = trim(str);
			strcpy(text, str.c_str());
			break;
		}
	}
}

void tokenize(const std::string &strString, const char *pDelimiters, std::vector<std::string> &vecTokens)
{
	vecTokens.clear();

	// skip the first delimiter. NOTE: "not_of"
	std::string::size_type lastPos = strString.find_first_not_of(pDelimiters, 0);
	// find the delimiter after the found position above
	std::string::size_type pos = strString.find_first_of(pDelimiters, lastPos);

	while(std::string::npos != pos || std::string::npos != lastPos)
	{
		// insert a token to a vector
		std::string	str = strString.substr(lastPos, pos - lastPos);
		str = trim(str);
		vecTokens.push_back(str);

		lastPos = strString.find_first_not_of(pDelimiters, pos);
		pos = strString.find_first_of(pDelimiters, lastPos);
	}
}
