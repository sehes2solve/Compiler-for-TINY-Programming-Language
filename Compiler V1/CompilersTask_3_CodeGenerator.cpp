#include <cstdio>
#include <cstdlib>
#include <cstring>
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
/**first program output at x = 5 should be respectively        **/
/**taking in consideration operators precedence & associativity**/
/**1,1,2,4,1, 0,0,0,4,4, -2,2,7,7,4, 5,59,76,-14,1,0 which actually same as code generator output**/
/**
    NOTEEEE IN First Program For Operators Testing purposes:-
    I used some illegal expressions that mix boolean
    with arithmetic that results static semantic errors
    (Like data type syntax errors)at start of output
    (Analyezer Output) however it proceeds execution
    normally because actually on code generator boolean
    expressions are casted into integer & saved in int vars
**/
/*
first program
-------------------------------------
read x;

write 4 & x;
write x & 4;
write x & 4 & 3;
write 3 & 4 & x;
write x & 4 < 3;

write 3 < 4 & x;
write x & 4 = 3;
write 3 = 4 & x;
write x & 4 + 3;
write 3 + 4 & x;

write x & 4 - 3;
write 3 - 4 & x;
write x & 4 * 3;
write 3 * 4 & x;
write x & 4 / 3;

write 3 / 4 & x;
write x & 4 ^ 3;
write 3 ^ 4 & x;
write x & 4 - 3 * x;
write x & 4 - 3 * x < 10 ^ 2;
write x & 3 * 10 = 7 - 2 ^ 2 / 2 + x
-------------------------------------
second program
-------------------------------------
read x; {input an integer}
if 0<x then {compute only if x>=1}
  fact:=1;
  repeat
    fact := fact * x;
    x:=x & 1
  until x=0
else
  fact := 0
end;
write fact {output factorial}
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
                IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
                ASSIGN, EQUAL, LESS_THAN,
                PLUS, MINUS, ABS_DIFF, TIMES, DIVIDE, POWER,/**add absoulte differenece token type**/
                SEMI_COLON,
                LEFT_PAREN, RIGHT_PAREN,
                LEFT_BRACE, RIGHT_BRACE,
                ID, NUM,
                ENDFILE, ERROR
              };

// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
            {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "AbsDiff", "Times", "Divide", "Power",/**add absoulte differenece token type String**/
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
    Token(WRITE, "write")
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
    Token(ABS_DIFF, "&"),/**add absoulte differenece token symbol**/
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
        while(IsDigit(s[j])) j++;

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

// program -> stmtseq
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
/**
            ADD Absolute Difference Rule & Update EBNF
// mathexpr -> newmathexpr { (+|-) newmathexpr }    left associative
// newmathexpr -> term { & term }                   left associative
**/
// term -> factor { (*|/) factor }                  left associative
// factor -> newexpr { ^ newexpr }                  right associative
// newexpr -> ( mathexpr ) | number | identifier

enum NodeKind{
                IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE,
                OPER_NODE, NUM_NODE, ID_NODE
             };

// Used for debugging only /////////////////////////////////////////////////////////
const char* NodeKindStr[]=
            {
                "If", "Repeat", "Assign", "Read", "Write",
                "Oper", "Num", "ID"
            };

enum ExprDataType {VOID, INTEGER, BOOLEAN};

// Used for debugging only /////////////////////////////////////////////////////////
const char* ExprDataTypeStr[]=
            {
                "Void", "Integer", "Boolean"
            };

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling; // used for sibling statements only

    NodeKind node_kind;

    union{TokenType oper; int num; char* id;}; // defined for expression/int/identifier only
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
        tree->num=0; while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');
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

/**
// newmathexpr -> term { & term }
// set the added rule function definition using recursive descent of the lecture
// also it builds the sutree from bottom to up because operator is left associative
// starting(left) exprissions contain this operator should be evaluated first so are
// parsed first at bottom of tree
**/
TreeNode* NewMathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewMathExpr");

/** derive from term rule using term function current token  **/
    TreeNode* tree=Term(pci, ppi);
/** repeat as far there's more successive (&) operators      **/
    while(ppi->next_token.type==ABS_DIFF)
    {
/**
    make new node represents the (&) operator node set it's
    left child the resulted subtree from previous (&)
    operator sub expression & it's right child the next token
    term rule derivation
**/
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;               /**set node type => operator**/
        new_tree->oper=ppi->next_token.type;         /**set type of operator => &**/
        new_tree->line_num=pci->in_file.cur_line_num;/**set operator line location**/

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Term(pci, ppi);

/**     set tree to the new parent operator node to return resulted subtree or connect
///     it with new parent operator node incase of a following abs diff token      **/
        tree=new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

/**
// mathexpr -> newmathexpr { (+|-) newmathexpr }
**/
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");
/** replace used term function by new function NewMathExpr to apply updated EBNF**/
    TreeNode* tree=NewMathExpr(pci, ppi);

    while(ppi->next_token.type==PLUS || ppi->next_token.type==MINUS)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
/**  same here                                      **/
        new_tree->child[1]=NewMathExpr(pci, ppi);

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

// program -> stmtseq
TreeNode* Parse(CompilerInfo* pci)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    TreeNode* syntax_tree=StmtSeq(pci, &parse_info);

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    return syntax_tree;
}

void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==NUM_NODE) printf("[%d]", node->num);
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);

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

    void Insert(const char* name, int line_num)
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
                printf("[Var=%s][Mem=%d]", curv->name, curv->memloc);
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

void Analyze(TreeNode* node, SymbolTable* symbol_table)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
        symbol_table->Insert(node->id, node->line_num);

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table);

    if(node->node_kind==OPER_NODE)
    {
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN;
        else node->expr_data_type=INTEGER;
    }
    else if(node->node_kind==ID_NODE || node->node_kind==NUM_NODE) node->expr_data_type=INTEGER;

    if(node->node_kind==OPER_NODE)
    {
        if(node->child[0]->expr_data_type!=INTEGER || node->child[1]->expr_data_type!=INTEGER)
            printf("ERROR Operator applied to non-integers\n");
    }
    if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN) printf("ERROR If test must be BOOLEAN\n");
    if(node->node_kind==REPEAT_NODE && node->child[1]->expr_data_type!=BOOLEAN) printf("ERROR Repeat test must be BOOLEAN\n");
    if(node->node_kind==WRITE_NODE && node->child[0]->expr_data_type!=INTEGER) printf("ERROR Write works only for INTEGER\n");
    if(node->node_kind==ASSIGN_NODE && node->child[0]->expr_data_type!=INTEGER) printf("ERROR Assign works only for INTEGER\n");

    if(node->sibling) Analyze(node->sibling, symbol_table);
}

////////////////////////////////////////////////////////////////////////////////////
// Code Generator //////////////////////////////////////////////////////////////////

int Power(int a, int b)
{
    if(a==0) return 0;
    if(b==0) return 1;
    if(b>=1) return a*Power(a, b-1);
    return 0;
}

int Evaluate(TreeNode* node, SymbolTable* symbol_table, int* variables)
{
    if(node->node_kind==NUM_NODE) return node->num;
    if(node->node_kind==ID_NODE) return variables[symbol_table->Find(node->id)->memloc];

    int a=Evaluate(node->child[0], symbol_table, variables);
    int b=Evaluate(node->child[1], symbol_table, variables);

    if(node->oper==EQUAL) return a==b;
    if(node->oper==LESS_THAN) return a<b;
    if(node->oper==PLUS) return a+b;
    if(node->oper==MINUS) return a-b;
/** add evaluation method for absolute difference operator**/
    if(node->oper==ABS_DIFF) { int diff = a-b; return (diff>0)?diff:-1*diff;}
    if(node->oper==TIMES) return a*b;
    if(node->oper==DIVIDE) return a/b;
    if(node->oper==POWER) return Power(a,b);

    printf("opertion not found");
    throw 0;
    return 0;
}

void RunProgram(TreeNode* node, SymbolTable* symbol_table, int* variables)
{
    if(node->node_kind==IF_NODE)
    {
        int cond=Evaluate(node->child[0], symbol_table, variables);
        if(cond) RunProgram(node->child[1], symbol_table, variables);
        else if(node->child[2]) RunProgram(node->child[2], symbol_table, variables);
    }
    if(node->node_kind==ASSIGN_NODE)
    {
        int v=Evaluate(node->child[0], symbol_table, variables);
        variables[symbol_table->Find(node->id)->memloc]=v;
    }
    if(node->node_kind==READ_NODE)
    {
        printf("Enter %s: ", node->id);
        scanf("%d", &variables[symbol_table->Find(node->id)->memloc]);
    }
    if(node->node_kind==WRITE_NODE)
    {
        int v=Evaluate(node->child[0], symbol_table, variables);
        printf("Val: %d\n", v);
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
    int* variables=new int[symbol_table->num_vars];
    for(i=0;i<symbol_table->num_vars;i++) variables[i]=0;
    RunProgram(syntax_tree, symbol_table, variables);
    delete[] variables;
}

////////////////////////////////////////////////////////////////////////////////////
// Scanner and Compiler ////////////////////////////////////////////////////////////

void StartCompiler(CompilerInfo* pci)
{
    TreeNode* syntax_tree=Parse(pci);

    SymbolTable symbol_table;
    Analyze(syntax_tree, &symbol_table);

    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n"); fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n"); fflush(NULL);

    printf("Run Program:\n");
    RunProgram(syntax_tree, &symbol_table);
    printf("---------------------------------\n"); fflush(NULL);

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
    StartCompiler(&compiler_info);

    printf("End main()\n"); fflush(NULL);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
