/*
---------------------------------------------------
  Filename: preprocess.lex                      
  Author:   Kevin Labille                             
  Date:     August 2017
  Purpose:  Pass through HTML documents
            to remove HTML tags, URL,
            email addresses and punctuation.
---------------------------------------------------
*/

%{
#include <iostream>        
#include <sstream>
#include <fstream>
#include <string>        
#include <cctype>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

//pointer to the input and output folder
DIR* outputdir;
DIR* inputdir;
stringstream stream;
struct dirent *entry;
struct stat filestat;
ofstream file;

// Downcase and print token verbatim on a line
void downcasePrint(const char* cstr)
{
   string st(cstr); 
   for (size_t i=0; i<st.size(); i++) 
      stream << (char) tolower(st[i]);
   stream << "\n"; 
}

// Process <meta> tag to get more information
void processTag(const char* cstr, string key)
{
   string st(cstr); 
   size_t start = st.find(key);
   if(start==string::npos || ((key=="content=\"") && (st.find("http-equiv") != string::npos))) return;
   size_t i = start+string(key).size();
   string word = "";
   while (i<st.size() && st[i] != '"')
   {
     if(st[i]==' '){ 
       downcasePrint(word.c_str());
       word= "";
     }
     else if(isalpha(st[i]) || isdigit(st[i]))
       word += (char) st[i];
     ++i;
   }
   downcasePrint(word.c_str());
}

void processTextModifier(const char* cstr)
{
  string st(cstr);
  size_t start = st.find("<"); size_t end = st.find(">");
  while(start != string::npos && end != string::npos)
  {
    st=st.substr(0,start)+st.substr(end+1, st.size()-end);
    start = st.find("<");
    end = st.find(">");
  }
  downcasePrint(st.c_str());
}

//process html accent and replace with regular letter (e.g., participaci&oacute;nnnn becomes participationnnn)
void processAccent(const char* cstr)
{
  string st(cstr);
  size_t start = st.find("&");
  if(start==string::npos) return;
  size_t end = st.find(";");
  if(end==string::npos) return;

  downcasePrint(string(st.substr(0, start)+st.substr(start+1, 1)+st.substr(end+1, st.size()-end)).c_str());
}

%}

LETTER   [A-Za-z]
DIGIT    [0-9]     
ALPHANUM ({LETTER}|{DIGIT})
URL (((http[s]?:\/\/www))|(http[s]?:\/\/))[A-Za-z0-9_-]+\.[A-Za-z0-9\.\/\%&=\\\?_:;\-~]+
EMAIL [-A-Za-z0-9._%+<>\\\/]+@[A-Za-z0-9.-]+\.[A-Za-z][A-Za-z]+ 
TAG \<[!\/]?({LETTER}|-)+[^>]*\>
TEXTMODIFIER {ALPHANUM}*\<([bB]|[Ss][Tt][Rr][Oo][Nn][Gg]|[iI]|[sS]|[Ee][mM]|[Mm][Aa][Rr][Kk]|[Ss][Mm][Aa][Ll][Ll]|[Dd][Ee][Ll]|[Ii][Nn][Ss]|[Ss][Uu][Bb]|[Ss][Uu][Pp])\>{ALPHANUM}+\<\/([bB]|[Ss][Tt][Rr][Oo][Nn][Gg]|[iI]|[sS]|[Ee][mM]|[Mm][Aa][Rr][Kk]|[Ss][Mm][Aa][Ll][Ll]|[Dd][Ee][Ll]|[Ii][Nn][Ss]|[Ss][Uu][Bb]|[Ss][Uu][Pp])\>{ALPHANUM}*
ACCENT {LETTER}+\&{LETTER}(grave|acute|circ|tilde|uml|ring);{LETTER}+
ASCII \&({LETTER}+|#{DIGIT}+);


%%
{EMAIL}                             {}
{TEXTMODIFIER}                      {processTextModifier(yytext);}
{URL}                               {}
\<[Mm][Ee][Tt][Aa][^>]*\>           processTag(yytext, "content=\"");
\<[Ii][Mm][Gg][^>]*\>               processTag(yytext, "alt=\"");
{TAG}                               {} 
{ACCENT}                            processAccent(yytext);
{ASCII}                             {}
{ALPHANUM}+'[Ss]                    downcasePrint(string(yytext).substr(0, yyleng-2).c_str());
{LETTER}+\'[Tt]                     downcasePrint((string(yytext).substr(0, yyleng-2)+"t").c_str());
{LETTER}+[\-\.]?{LETTER}+           downcasePrint(yytext);
.                                   {}
[\n]                                {}
%%

/*----------------------------------------------------------*/
/*Section of code to be passed directly to C++ compiler       */
/*which normally contains functions called from above and   */
/*the main function(when not called from yacc).             */
/*----------------------------------------------------------*/

int main(int argc, char **argv)
{ 
  int total_file = 0;
  int parsed = 0;
  //check if the arguments are correctly provided
  if(argc != 3)
  {
    cout << "USAGE : $./preprocess input-dir output-dir" << endl;
    exit(-1);
  }
  string input = string(argv[1]);
  //Check if we can read the input folder
  if((inputdir = opendir(argv[1])) == NULL)
    cout << "Could not open the input folder." << endl;
  else
  {
    //Check if the output folder already exists, if not we create it
    struct stat st = {0};
    if(stat(argv[2], &st) == -1) {
      mkdir(argv[2], 0700);
      outputdir = opendir(argv[2]);
    }
    //read every files in the folder
    while(entry = readdir(inputdir))
    {
      //path = path of the file
      string inputP = (input[input.length()-1]=='/')?input:input+'/';
      string path = inputP+string(entry->d_name);
      //if the path is incorrect or if it is a folder we skip it
      if(stat(path.c_str(), &filestat)) continue;
      if(S_ISDIR(filestat.st_mode)) continue;
      //gives the file to the Lex parser
      total_file++;
      cout << "Parsing file: " << path << endl;
      yyin = fopen(path.c_str(), "r");
      //clear the previous stream
      stream.str(std::string()); //stream.str("");
      yylex();
      //create, write, and close the output files 
      string outputfile = string(argv[2])+"/"+string(entry->d_name)+".txt";
      file.open(outputfile.c_str(), ofstream::out);
      file << stream.str();
      file.close();
      parsed++;
    }
    cout << "\nParsed " << parsed << " out of " << total_file << " files." << endl;
   /*if((yyin = fopen(argv[1],"r"))==NULL)
      cout << "\n Error opening input file \n";

   yylex();
   */
  }
}
