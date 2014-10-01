#ifndef _CSVPARSER_HH
#define _CSVPARSER_HH

#include <stdio.h>
#include <stdlib.h>
#include <vector>

inline void _no_eol (char* s, unsigned len)
{
    for (unsigned i = 0; i < len; i++)
    {
	if (s[i] == '\0')
	    break;
	else if (s[i] == '\n')
	{
	    s[i] = '\0';
	    break;
	}
    }
}

class CSVParser {
    
    FILE *filep;
    int length;
    std::vector<int> fields;
    char line[4096];
    char header[4096];

public:
    CSVParser (FILE* fp)
    {
	filep = fp;	
    }

    char* parse_header ()
    {
	if (fgets (header, 4096, filep) == NULL)
	    return NULL;
	_no_eol (header, 4096);
	return header;
    }

    int parse_next ()
    {
	length = 0;
	fields.clear ();

	if (fgets (line, 4096, filep) == NULL)
	    return 0;
	_no_eol (line, 4096);

	int curr_field = 0;
	for (int i = 0; i < 4096; i++)
	{
	    switch(line[i])
	    {
	    case ',':
		fields.push_back (curr_field);
		line[i] = '\0';
		curr_field = i+1;
		break;
		
	    case '\0':
		fields.push_back (curr_field);
		goto outta_for;
		
	    default:
		break;
	    }
	}
	
    outta_for:
	return fields.size (); 
    }
    
    char* field (int idx)
    {
	return line + fields[idx];
    }
};
    
#endif // _CSVPARSER_HH
