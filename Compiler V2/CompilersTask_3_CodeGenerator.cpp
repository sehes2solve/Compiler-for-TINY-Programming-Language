#include <cstdio>
#include <cstdlib>
#include <cstring>
#define EPSILON 1e-6
using namespace std;

/*
{ Sample program
  in TINY language
  compute factorial
}

read x; {input an integer}
if 0<x then {compute only if x>=1}
  fact:=1;
  repeat
    fact := fact * x;
    x:=x-1
  until x=0;
  write fact {output factorial}
end
*/

/**Test Cases**/
/*
first program
-------------------------------------
bool x;
bool fact;

read x; {Try input x=5.2 it will terminate fraction part like in c++}
if 0<x then {compute only if x>=1}
  fact:=1;
  repeat
    write x;
    fact := fact * x;
    x:=x-1
  until x=0;
  write x;
  write fact{output factorial}
end
-------------------------------------
second program
-------------------------------------
int x;
int fact;

read x; {Try input x=5.2 it will terminate fraction part like in c++}
if 0<x then {compute only if x>=1}
  fact:=1;
  repeat
    write x;
    fact := fact * x;
    x:=x-1
  until x=0;
  write x;
  write fact{output factorial}
end
-------------------------------------
third program
-------------------------------------
real x;
real fact;

read x; {input an integer}
if 0<x then {compute only if x>=1}
  fact:=1;
  repeat
    write x;
    fact := fact * x;
    x:=x-1
  until x=0;
  write x;
  write fact{output factorial}
end
-------------------------------------
fourth program
-------------------------------------
bool var_bool;
int var_int;
bool var_int;
real var_real;
int var_real;

y := 12;
var_bool := 0;
var_int := 1 * 12;
var_real := 12.5 * x;
var_real := 15.2;

var_bool := var_int;
var_int := var_bool;
var_real := var_bool;
var_bool := var_real;
var_real := var_int;
var_int := var_real;
var_bool := var_int + 0 + 1;
var_int := 12 ^ 5 / var_bool;
var_real := 12.5 - 5.6 * var_int ;

write var_real + var_int;

if 0 + int_var then
    write x
end;

repeat
    write x
until 1 - real_var
-------------------------------------
fifth program
-------------------------------------
bool var_bool;
bool var_booll;
int var_int;
int var_intt;
real var_real;
real var_reall;

var_bool := 0;
var_booll := 1;
write var_bool;
write var_booll;
var_bool := 0.2;
var_booll := 12.2;
write var_bool;
write var_booll;
var_bool := 5;
var_booll := var_bool;
write var_bool;
write var_booll;
var_bool := 0;

var_int := 15;
var_intt := var_int;
write var_int;
write var_intt;
var_intt := 9.8;
write var_intt;
var_int:= var_int ^ 2.2 + 5.3 * var_intt;
write var_int;
write var_int ^ 2.2 + 5.3 * var_intt;

var_real := 5;
var_reall := var_real;
write var_real;
write var_reall;
var_reall := 25.5;
write var_reall;
var_real := 5 / 2.2 * var_real - var_reall;
write var_real;
write 5 / 2.2 * var_real - var_reall;

if var_booll then
    write 99999999
end;

repeat
    write 8888888888
until var_booll
*/

// sequence of statements separated by ;
// no procedures - no declarations
// all variables are integers
// variables are declared simply by assigning values to them :=
// if-statement: if (boolean) then [else] end
// repeat-statement: repeat until (boolean)
// boolean only in if and repeat conditions < = and two mathematical expressions
// math expressions integers only, + - * / ^
// I/O read write
// Comments {}

////////////////////////////////////////////////////////////////////////////////////
// Strings /////////////////////////////////////////////////////////////////////////

bool Equals(const char* a, const char* b)
{
    return strcmp(a, b)==0;
}

bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

void AllocateAndCopy(char** a, const char* b)
{
    if(b==0) {*a=0; return;}
    int n=strlen(b);
    *a=new char[n+1];
    strcpy(*a, b);
}

////////////////////////////////////////////////////////////////////////////////////
// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char* str) {file=0; if(str) file=fopen(str, "r"); cur_line_size=0; cur_ind=0; cur_line_num=0;}
    ~InFile(){if(file) fclose(file);}

    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}

            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }

    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {if(!GetNewLine()) return 0; SkipSpaces();}
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    OutFile(const char* str) {file=0; if(str) file=fopen(str, "w");}
    ~OutFile(){if(file) fclose(file);}

    void Out(const char* s)
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
                : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////
// Scanner /////////////////////////////////////////////////////////////////////////

#define MAX_TOKEN_LEN 40

enum TokenType{
                IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE, DATATYPE, /**Add DT token Type**/
                ASSIGN, EQUAL, LESS_THAN,
                PLUS, MINUS, TIMES, DIVIDE, POWER,
                SEMI_COLON,
                LEFT_PAREN, RIGHT_PAREN,
                LEFT_BRACE, RIGHT_BRACE,
                ID, NUM,
                ENDFILE, ERROR
              };

// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
            {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write", "Datatype", /**Add DT token Type String**/
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID", "Num",
                "EndFile", "Error"
            };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1];

    Token(){str[0]=0; type=ERROR;}
    Token(TokenType _type, const char* _str) {type=_type; Copy(str, _str);}
};

const Token reserved_words[]=
{
    Token(IF, "if"),
    Token(THEN, "then"),
    Token(ELSE, "else"),
    Token(END, "end"),
    Token(REPEAT, "repeat"),
    Token(UNTIL, "until"),
    Token(READ, "read"),
    Token(WRITE, "write"),
    Token(DATATYPE, "real"),
    Token(DATATYPE, "int"), /**Add Datatypes Reserved Words**/
    Token(DATATYPE, "bool")
};
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
{
    Token(ASSIGN, ":="),
    Token(EQUAL, "="),
    Token(LESS_THAN, "<"),
    Token(PLUS, "+"),
    Token(MINUS, "-"),
    Token(TIMES, "*"),
    Token(DIVIDE, "/"),
    Token(POWER, "^"),
    Token(SEMI_COLON, ";"),
    Token(LEFT_PAREN, "("),
    Token(RIGHT_PAREN, ")"),
    Token(LEFT_BRACE, "{"),
    Token(RIGHT_BRACE, "}")
};
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9');}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));}
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_');}

/**updated to read middle dotted num**/
void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    ptoken->type=ERROR;
    ptoken->str[0]=0;

    int i;
    char* s=pci->in_file.GetNextTokenStr();
    if(!s)
    {
        ptoken->type=ENDFILE;
        ptoken->str[0]=0;
        return;
    }

    for(i=0;i<num_symbolic_tokens;i++)
    {
        if(StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if(i<num_symbolic_tokens)
    {
        if(symbolic_tokens[i].type==LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if(!pci->in_file.SkipUpto(symbolic_tokens[i+1].str)) return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type=symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if(IsDigit(s[0]))
    {
        int j=1;
        /**allow scan number divided by a dot (double value)**/
        bool is_dot_passed = false;
        while(true)
        {
            if(!IsDigit(s[j]))
            {
                if(is_dot_passed) break;
                else if(s[j] == '.') is_dot_passed = true;
                else break;
            }
            j++;
        }

        ptoken->type=NUM;
        Copy(ptoken->str, s, j);
    }
    else if(IsLetterOrUnderscore(s[0]))
    {
        int j=1;
        while(IsLetterOrUnderscore(s[j])) j++;

        ptoken->type=ID;
        Copy(ptoken->str, s, j);

        for(i=0;i<num_reserved_words;i++)
        {
            if(Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type=reserved_words[i].type;
                break;
            }
        }
    }

    int len=strlen(ptoken->str);
    if(len>0) pci->in_file.Advance(len);
}

////////////////////////////////////////////////////////////////////////////////////
// Parser //////////////////////////////////////////////////////////////////////////

/**
        ADD Declaration Statements Rules & Update EBNF
// program -> declseq ; stmtseq
// declseq -> declstmt { ; declstmt }
// declstmt -> datatype identifier
**/
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }    left associative
// term -> factor { (*|/) factor }    left associative
// factor -> newexpr { ^ newexpr }    right associative
// newexpr -> ( mathexpr ) | number | identifier

enum NodeKind{
                ROOT, /**Add root of parse tree node type that seperates declaration & processing**/
                IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE, DECL_NODE, /**Add declare node type**/
                OPER_NODE, NUM_NODE, ID_NODE
             };

// Used for debugging only /////////////////////////////////////////////////////////
const char* NodeKindStr[]=
            {
                "Root",            /**Add root of parse tree node type String**/
                "If", "Repeat", "Assign", "Read", "Write", "Declare",           /**Add declare node type String**/
                "Oper", "Num", "ID"
            };

enum ExprDataType {VOID, INTREAL, REAL, INTEGER, BOOLEAN}; /**Add Real & intreal Expression data types*/
/**
intreal is literals datatype used with c++ double variables to simulate c++ literals that can have decimal or not & even though
assigned to Vars of any three types (note it's var to literal assignment not var to var assignment as it's allowed to assign Vars
with Vars of different datatypes according assignment description)
**/
// Used for debugging only /////////////////////////////////////////////////////////
const char* ExprDataTypeStr[]=
            {
                "Void", "Integer Real", "Real", "Integer", "Boolean"  /**Add Real & intreal Expression data type Strings**/
            };

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling; // used for sibling statements only

    NodeKind node_kind;
/**make num double for generic & flexibility so can store all types & also to simulate c++ casting of different types literals**/
    union{TokenType oper; double num; char* id;}; // defined for expression/int/identifier only
    ExprDataType expr_data_type; // defined for expression/int/identifier only

    int line_num;

    TreeNode() {int i; for(i=0;i<MAX_CHILDREN;i++) child[i]=0; sibling=0; expr_data_type=VOID;}
};

struct ParseInfo
{
    Token next_token;
};

void Match(CompilerInfo* pci, ParseInfo* ppi, TokenType expected_token_type)
{
    pci->debug_file.Out("Start Match");

    if(ppi->next_token.type!=expected_token_type) throw 0;
    GetNextToken(pci, &ppi->next_token);

    fprintf(pci->debug_file.file, "[%d] %s (%s)\n", pci->in_file.cur_line_num, ppi->next_token.str, TokenTypeStr[ppi->next_token.type]); fflush(pci->debug_file.file);
}

TreeNode* Expr(CompilerInfo*, ParseInfo*);

/**updated to read value of real num**/
// newexpr -> ( mathexpr ) | number | identifier
TreeNode* NewExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewExpr");

    // Compare the next token with the First() of possible statements
    if(ppi->next_token.type==NUM)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=NUM_NODE;
        char* num_str=ppi->next_token.str;

        tree->num=0;
        bool is_before_dot = true;
        double fract_digit_degree = 0.1;
        while(*num_str)
        {
            if(*num_str != '.')
                if(is_before_dot)
                    tree->num=tree->num*10+((*num_str)-'0');
                else
                {
                    tree->num+=((*num_str)-'0')*fract_digit_degree;
                    fract_digit_degree*=0.1;
                }
            else
                is_before_dot = false;

            num_str++;
        }
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==ID)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode* tree=Expr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    throw 0;
    return 0;
}

// factor -> newexpr { ^ newexpr }    right associative
TreeNode* Factor(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode* tree=NewExpr(pci, ppi);

    if(ppi->next_token.type==POWER)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// term -> factor { (*|/) factor }    left associative
TreeNode* Term(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode* tree=Factor(pci, ppi);

    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode* tree=Term(pci, ppi);

    while(ppi->next_token.type==PLUS || ppi->next_token.type==MINUS)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Term(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

// expr -> mathexpr [ (<|=) mathexpr ]
TreeNode* Expr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode* tree=MathExpr(pci, ppi);

    if(ppi->next_token.type==EQUAL || ppi->next_token.type==LESS_THAN)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=MathExpr(pci, ppi);

        pci->debug_file.Out("End Expr");
        return new_tree;
    }
    pci->debug_file.Out("End Expr");
    return tree;
}

// writestmt -> write expr
TreeNode* WriteStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start WriteStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=WRITE_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, WRITE);
    tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End WriteStmt");
    return tree;
}

// readstmt -> read identifier
TreeNode* ReadStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start ReadStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=READ_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, READ);
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    pci->debug_file.Out("End ReadStmt");
    return tree;
}

// assignstmt -> identifier := expr
TreeNode* AssignStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start AssignStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=ASSIGN_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN); tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End AssignStmt");
    return tree;
}

TreeNode* StmtSeq(CompilerInfo*, ParseInfo*);

// repeatstmt -> repeat stmtseq until expr
TreeNode* RepeatStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start RepeatStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=REPEAT_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, REPEAT); tree->child[0]=StmtSeq(pci, ppi);
    Match(pci, ppi, UNTIL); tree->child[1]=Expr(pci, ppi);

    pci->debug_file.Out("End RepeatStmt");
    return tree;
}

// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode* IfStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start IfStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=IF_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, IF); tree->child[0]=Expr(pci, ppi);
    Match(pci, ppi, THEN); tree->child[1]=StmtSeq(pci, ppi);
    if(ppi->next_token.type==ELSE) {Match(pci, ppi, ELSE); tree->child[2]=StmtSeq(pci, ppi);}
    Match(pci, ppi, END);

    pci->debug_file.Out("End IfStmt");
    return tree;
}

// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
TreeNode* Stmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Stmt");

    // Compare the next token with the First() of possible statements
    TreeNode* tree=0;
    if(ppi->next_token.type==IF) tree=IfStmt(pci, ppi);
    else if(ppi->next_token.type==REPEAT) tree=RepeatStmt(pci, ppi);
    else if(ppi->next_token.type==ID) tree=AssignStmt(pci, ppi);
    else if(ppi->next_token.type==READ) tree=ReadStmt(pci, ppi);
    else if(ppi->next_token.type==WRITE) tree=WriteStmt(pci, ppi);
    else throw 0;

    pci->debug_file.Out("End Stmt");
    return tree;
}

// stmtseq -> stmt { ; stmt }
TreeNode* StmtSeq(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start StmtSeq");

    TreeNode* first_tree=Stmt(pci, ppi);
    TreeNode* last_tree=first_tree;

    // If we did not reach one of the Follow() of StmtSeq(), we are not done yet
    while(ppi->next_token.type!=ENDFILE && ppi->next_token.type!=END &&
          ppi->next_token.type!=ELSE && ppi->next_token.type!=UNTIL)
    {
        Match(pci, ppi, SEMI_COLON);
        TreeNode* next_tree=Stmt(pci, ppi);
        last_tree->sibling=next_tree;
        last_tree=next_tree;
    }

    pci->debug_file.Out("End StmtSeq");
    return first_tree;
}

/**
// declstmt -> datatype identifier  // set the added rule function definition using recursive descent of the lecture
**/
TreeNode* DeclStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start DeclStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=DECL_NODE;
    tree->line_num=pci->in_file.cur_line_num;
    /**
    save specific data type in DECL_NODE Expression data type from vars declaration
    to be used by analyzer in future for type & static semantics Checking
    **/
                                                 /**num_reserved_words - 1 => bool token idx in reserved words**/
    if(Equals(ppi->next_token.str,reserved_words[num_reserved_words - 1].str))      tree->expr_data_type=BOOLEAN;
                                                     /**num_reserved_words - 2 => int token idx in reserved words**/
    else if(Equals(ppi->next_token.str,reserved_words[num_reserved_words - 2].str)) tree->expr_data_type=INTEGER;
        /**otherwise it's Real (also since it's else then real is default default) but based on scanner probably won't happen**/
    else  tree->expr_data_type=REAL;
    Match(pci, ppi, DATATYPE);

    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str); /**save identfier declared by this Datatype**/
    Match(pci, ppi, ID);

    pci->debug_file.Out("End DeclStmt");
    return tree;
}

/**
// declseq -> declstmt { ; declstmt }   // set the added rule function definition using recursive descent of the lecture
**/
TreeNode* DeclSeq(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start DeclSeq");

    TreeNode* first_tree=DeclStmt(pci, ppi);
    TreeNode* last_tree=first_tree;

    // If we did not reach one of the Follow() of DeclStmt(), we are not done yet
    while(true)
    {
        Match(pci, ppi, SEMI_COLON); /**according to EBNF last DeclStmt in DeclSeq must be terminated by semicolon**/
        if(ppi->next_token.type != DATATYPE) break;/**if next stmt doesn't begin with datatype then the previous
                                                    was last DeclStmt & DeclSeq is finished**/
        TreeNode* next_tree=DeclStmt(pci, ppi);
        last_tree->sibling=next_tree;
        last_tree=next_tree;
    }

    pci->debug_file.Out("End DeclSeq");
    return first_tree;
}

/**
// program -> declseq ; stmtseq
**/
TreeNode* Parse(CompilerInfo* pci)
{
    pci->debug_file.Out("Start Parse");

    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);
    TreeNode* syntax_tree= new TreeNode;
    syntax_tree->line_num=pci->in_file.cur_line_num;
    syntax_tree->node_kind=ROOT;

    syntax_tree->child[0]=DeclSeq(pci, &parse_info);/**DeclSeq subtree is Root node left child**/
    /**no need to match semicolon as Declseq already match semicolon to reach the DeclSeq last DeclStmt**/
    syntax_tree->child[1]=StmtSeq(pci, &parse_info);/**DeclSeq subtree is Root node mid child as there's third(right) null child**/

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    pci->debug_file.Out("End Parse");

    return syntax_tree;
}

/**update print to suit new tree structure**/
void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==NUM_NODE) printf("[%f]", node->num);                  /**add showing identifier for new DECLNODE **/
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE || node->node_kind==DECL_NODE)
        printf("[%s]", node->id);

    if(node->expr_data_type!=VOID) printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) PrintTree(node->child[i], sh+NSH);
    if(node->sibling) PrintTree(node->sibling, sh);
}

void DestroyTree(TreeNode* node)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
        if(node->id) delete[] node->id;

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) DestroyTree(node->child[i]);
    if(node->sibling) DestroyTree(node->sibling);

    delete node;
}

////////////////////////////////////////////////////////////////////////////////////
// Analyzer ////////////////////////////////////////////////////////////////////////

const int SYMBOL_HASH_SIZE=10007;

struct LineLocation
{
    int line_num;
    LineLocation* next;
};

struct VariableInfo
{
    char* name;
    int memloc;
    ExprDataType datatype;  /**Add var datatype to be used for saving datatype of declared var & update it's identifier nodes**/
    LineLocation* head_line; // the head of linked list of source line locations
    LineLocation* tail_line; // the tail of linked list of source line locations
    VariableInfo* next_var; // the next variable in the linked list in the same hash bucket of the symbol table
};

struct SymbolTable
{
    int num_vars;
    VariableInfo* var_info[SYMBOL_HASH_SIZE];

    SymbolTable() {num_vars=0; int i; for(i=0;i<SYMBOL_HASH_SIZE;i++) var_info[i]=0;}

    int Hash(const char* name)
    {
        int i, len=strlen(name);
        int hash_val=11;
        for(i=0;i<len;i++) hash_val=(hash_val*17+(int)name[i])%SYMBOL_HASH_SIZE;
        return hash_val;
    }

    VariableInfo* Find(const char* name)
    {
        int h=Hash(name);
        VariableInfo* cur=var_info[h];
        while(cur)
        {
            if(Equals(name, cur->name)) return cur;
            cur=cur->next_var;
        }
        return 0;
    }
    /**Allow inserting Var datatype to set it at var declaration**/
    void Insert(const char* name, int line_num, ExprDataType var_datatype = VOID)
    {
        LineLocation* lineloc=new LineLocation;
        lineloc->line_num=line_num;
        lineloc->next=0;

        int h=Hash(name);
        VariableInfo* prev=0;
        VariableInfo* cur=var_info[h];

        while(cur)
        {
            if(Equals(name, cur->name))
            {
                // just add this line location to the list of line locations of the existing var
                cur->tail_line->next=lineloc;
                cur->tail_line=lineloc;
                return;
            }
            prev=cur;
            cur=cur->next_var;
        }

        VariableInfo* vi=new VariableInfo;
        vi->head_line=vi->tail_line=lineloc;
        vi->next_var=0;
        vi->datatype=var_datatype;      /**set datatype passed**/
        vi->memloc=num_vars++;
        AllocateAndCopy(&vi->name, name);

        if(!prev) var_info[h]=vi;
        else prev->next_var=vi;
    }

    void Print()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                printf("[Var=%s][Mem=%d][Datatype=%s]\n", curv->name, curv->memloc,ExprDataTypeStr[curv->datatype]);
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    printf("[Line=%d]", curl->line_num);
                    curl=curl->next;
                }
                printf("\n");
                curv=curv->next_var;
            }
        }
    }

    void Destroy()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    LineLocation* pl=curl;
                    curl=curl->next;
                    delete pl;
                }
                VariableInfo* p=curv;
                curv=curv->next_var;
                delete p;
            }
            var_info[i]=0;
        }
    }
};

/**
static_semantics_error indicate whether Analyzer met error or not during execution of all analyzer calls
NOTE I always try resolve errors to proceed analysis to detect most possible
number of errors as this what compiler should do unlike interpreters
**/
void Analyze(TreeNode* node, SymbolTable* symbol_table, bool* static_semantics_error)
{
    int i;
    /**Set identifiers Datatypes & Add them to Symbol Table Part**/
    /**save declared vars with there datatypes**/
    if(node->node_kind==DECL_NODE)
    {
        /**check if var is declared multiple times**/
        if(symbol_table->Find(node->id))
        {
            printf("ERROR Var Multiple Declaration\n");
            *static_semantics_error = true;
        }
        /**put the datatype of last var declaration in case of multiple declaration just to proceed analysis**/
        symbol_table->Insert(node->id, node->line_num, node->expr_data_type);
    }
    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
    {
        VariableInfo* varinfo = symbol_table->Find(node->id);
        /**check if identifier is of declared var**/
        if(!varinfo)
        {
            printf("ERROR Var Must be Declared at program begin\n");
            /**Note:To porceed Analysis I Set Var type on Default type(which I choosed) => real**/
            symbol_table->Insert(node->id, node->line_num, REAL);
            node->expr_data_type=REAL;  /**Set identifier datatype**/
            *static_semantics_error = true;
        }
        else
        {
            symbol_table->Insert(node->id, node->line_num);
            node->expr_data_type=varinfo->datatype;
        }
    }
    /**End Part**/
    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table, static_semantics_error);

    /**evaluate literals & expression type part**/
    if(node->node_kind==OPER_NODE)
    {
        /**Condition Expressions Can contain boolean, integer, real or intreal or pair of mix
        of them as the restriction described in assignment was on arithematic operations only**/
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN;
        /**In case of Arithmetic Operators**/
        else
        {
            TreeNode* loprand = node->child[0],* roprand = node->child[1];
            /**check if arithmetic applied on boolean**/
            if(loprand->expr_data_type==BOOLEAN || roprand->expr_data_type==BOOLEAN)
            {
                printf("ERROR Arithmetic Operations Applied on Boolean\n");
                node->expr_data_type=INTREAL;
                *static_semantics_error = true;
            }
            /**check if arithmetic applied on Vars of same datatypes (Note execluding literals as they aren't Vars)**/
            else if(loprand->expr_data_type==roprand->expr_data_type) node->expr_data_type=loprand->expr_data_type;
            /**set the resulted expression of a literal & Var or sub-expression that
            contain at least Var or cosists all of it of literals with the Var datatype**/
            else if(loprand->expr_data_type==INTREAL) node->expr_data_type=roprand->expr_data_type;
            else if(roprand->expr_data_type==INTREAL) node->expr_data_type=loprand->expr_data_type;
            else/**the case that arithmetic applied on Vars of different datatypes**/
            {
                printf("ERROR Arithmetic Operations Applied on Vars of Different Datatypes\n");
                node->expr_data_type=REAL;
                *static_semantics_error = true;
            /**Note : I try to resolve error by make the resulted datatype is most generic datatype(REAL)
                      to proceed analysis Also Same As the default Value for Undeclared Vars             **/
            }
        }
    }
    /**set literal node with the literal datatype(I choosed for simulating c++) => intreal**/
    else if(node->node_kind==NUM_NODE)
        node->expr_data_type=INTREAL;
    /**End part**/


    /**check if operator applied on VOID**/
    if(node->node_kind==OPER_NODE)
        if(node->child[0]->expr_data_type==VOID || node->child[1]->expr_data_type==VOID)
        {
            printf("ERROR Operator applied to VOID\n");
            *static_semantics_error = true;
        }
    if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN)
    {
        printf("ERROR If test must be BOOLEAN\n");
        *static_semantics_error = true;
    }
    if(node->node_kind==REPEAT_NODE && node->child[1]->expr_data_type!=BOOLEAN)
    {
        printf("ERROR Repeat test must be BOOLEAN\n");
        *static_semantics_error = true;
    }
    /**update to cover new datatypes**/
    if(node->node_kind==WRITE_NODE && node->child[0]->expr_data_type==VOID)
    {
        printf("ERROR Write works only for Numbers\n");
        *static_semantics_error = true;
    }
    /**update to cover new datatypes**/
    if(node->node_kind==ASSIGN_NODE)
    {
        if(node->child[0]->expr_data_type==VOID)
        {
            printf("ERROR Assign works only for Numbers\n");
            *static_semantics_error = true;
        }
        /**incase assignment expression contain a Var (indicated by that resulted expression datatype isn't intreal)
           then check if Vars inside assignment expression is of same as Var datatype
          (Note I allow assigning Var of any datatype with literal with or without decimal & terminate decimal part if Var is
           integer trying to simulate c++ also as it doesn't violate assignment restriction as it's only on Var to Var operations
           & not literal to literal or Var to literal)**/
        else if(node->child[0]->expr_data_type!=INTREAL && node->expr_data_type!=node->child[0]->expr_data_type)
        {
            printf("ERROR Assign Expression Contain Var of Different Datatype\n");
            *static_semantics_error = true;
        }
    }

    if(node->sibling) Analyze(node->sibling, symbol_table, static_semantics_error);
}

////////////////////////////////////////////////////////////////////////////////////
// Code Generator //////////////////////////////////////////////////////////////////
/**set to double data type for consistency as main number storage type is double**/
double Power(double a, int b)
{
    if(a==0) return 0;
    if(b==0) return 1;
    if(b>=1) return a*Power(a, b-1);
    return 0;
}
double myabs(double a)
{
   return (a < 0) ? a*-1 : a;
}
/**
set to double data type for same reasons also to simulate c++ behaviour with literals & casting it
For example in c++:
int x;
bool y;
y = 0; sets y with false(0) while any other value sets y with true(1) like
y = 1;
y = 10;
y = 8.7;
---------
x = 12.5 sets x with 12
**/
/**
Note: intreal is intermediate datatype for literals i made it as it can't be defined whether literal is real or integer
so it results an expression of integer datatype if it's mixed with integer Vars & expression of real datatype if mixed with
real variables & intreal if the whole expression is literals that would be casted based on which type it required to be(like in c++
**/
double Evaluate(TreeNode* node, SymbolTable* symbol_table, double* variables)
{

    if(node->node_kind==NUM_NODE) return node->num;
    if(node->node_kind==ID_NODE) return variables[symbol_table->Find(node->id)->memloc];

    double a=Evaluate(node->child[0], symbol_table, variables);
    double b=Evaluate(node->child[1], symbol_table, variables);

    /**as == operator not accurate with double data types**/
    if(node->oper==EQUAL) return myabs(a-b)<EPSILON;
    if(node->oper==LESS_THAN) return a<b;
    /**
    to simulate c++ on dealing with literals(of intreal datatype) that might have fraction part & is operand for operator that
    it's other operand is int Var so to handle it cast operands to int always if operator resulted expression is integer also to
    handle if what required was integer division(as it's int expression) the main datatype for Vars to store values is double
    **/
    if(node->oper==PLUS)
    {
        if(node->expr_data_type != INTEGER)  return a+b;
        else return (int)a+(int)b;
    }
    if(node->oper==MINUS)
    {
        if(node->expr_data_type != INTEGER)  return a-b;
        else return (int)a-(int)b;
    }
    if(node->oper==TIMES)
    {
        if(node->expr_data_type != INTEGER)  return a*b;
        else return (int)a*(int)b;
    }
    if(node->oper==DIVIDE)
    {
        if(node->expr_data_type != INTEGER)  return a/b;
        else return (int)a/(int)b;
    }
    if(node->oper==POWER)
    {
        /**note even if b had fraction it's casted to int due to power function declaration so it can evaluate the power**/
        if(node->expr_data_type != INTEGER)  Power(a,b);
        else return Power((int)a,(int)b);
    }
    throw 0;
    return 0;
}

void RunProgram(TreeNode* node, SymbolTable* symbol_table, double* variables)
{
    if(node->node_kind==IF_NODE)
    {
        int cond=Evaluate(node->child[0], symbol_table, variables);
        if(cond) RunProgram(node->child[1], symbol_table, variables);
        else if(node->child[2]) RunProgram(node->child[2], symbol_table, variables);
    }
    /**
    on reading or assigning Var if it's boolean & assigned or reading zero set it zero(false) otherwise set it one(true)
    if it's integer terminate fraction part otherwise it would be real that can read fraction part
    **/
    if(node->node_kind==ASSIGN_NODE)
    {
        double v = Evaluate(node->child[0], symbol_table, variables);
        if(node->expr_data_type==BOOLEAN) v=(v) ? 1:0;
        else if(node->expr_data_type==INTEGER) v=(int)v;
        variables[symbol_table->Find(node->id)->memloc]=v;
    }
    if(node->node_kind==READ_NODE)
    {
        double v;
        printf("Enter %s: ", node->id);
        scanf("%lf", &v);
        if(node->expr_data_type==BOOLEAN) v=(v) ? 1:0;
        else if(node->expr_data_type==INTEGER) v=(int)v;
        variables[symbol_table->Find(node->id)->memloc]=v;
    }
    if(node->node_kind==WRITE_NODE)
    {
        double v=Evaluate(node->child[0], symbol_table, variables);
        /**check child node type as expression to be written is the write_node child with expression type also write_node type is
        always void so can't be used to define expression type that would be printed as int if it's integer or boolean**/
        if(node->child[0]->expr_data_type==BOOLEAN || node->child[0]->expr_data_type==INTEGER) printf("Val: %d\n", (int)v);
        else printf("Val: %lf\n", v);
    }
    if(node->node_kind==REPEAT_NODE)
    {
        do
        {
           RunProgram(node->child[0], symbol_table, variables);
        }
        while(!Evaluate(node->child[1], symbol_table, variables));
    }
    if(node->sibling) RunProgram(node->sibling, symbol_table, variables);
}

void RunProgram(TreeNode* syntax_tree, SymbolTable* symbol_table)
{
    int i;
    double* variables=new double[symbol_table->num_vars];
    for(i=0;i<symbol_table->num_vars;i++) variables[i]=0;
    /**leave the declarations tree half & pass to processing stmts in the other tree half**/
    RunProgram(syntax_tree->child[1], symbol_table, variables);
    delete[] variables;
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner and Compiler ////////////////////////////////////////////////////////////

void StartCompiler(CompilerInfo* pci)
{
    TreeNode* syntax_tree=Parse(pci);

    bool static_semantics_error = false;
    SymbolTable symbol_table;
    Analyze(syntax_tree, &symbol_table, &static_semantics_error);

    if(static_semantics_error) printf("Please Resolve Syntax Errors\n\n\n");

    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n"); fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n"); fflush(NULL);

    /**Don't Proceed If Analyzer detected error**/
    if(!static_semantics_error)
    {
        printf("Run Program:\n");
        RunProgram(syntax_tree, &symbol_table);
        printf("---------------------------------\n"); fflush(NULL);
    }

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner only ////////////////////////////////////////////////////////////////////

void StartScanner(CompilerInfo* pci)
{
    Token token;

    while(true)
    {
        GetNextToken(pci, &token);
        printf("[%d] %s (%s)\n", pci->in_file.cur_line_num, token.str, TokenTypeStr[token.type]); fflush(NULL);
        if(token.type==ENDFILE || token.type==ERROR) break;
    }
}

////////////////////////////////////////////////////////////////////////////////////

int main()
{
    printf("Start main()\n"); fflush(NULL);

    CompilerInfo compiler_info("input.txt", "output.txt", "debug.txt");

    //StartScanner(&compiler_info);
    //TreeNode* syntax_tree=Parse(&compiler_info);
    //PrintTree(syntax_tree); fflush(NULL);
    //DestroyTree(syntax_tree);
    StartCompiler(&compiler_info);
    printf("End main()\n"); fflush(NULL);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
