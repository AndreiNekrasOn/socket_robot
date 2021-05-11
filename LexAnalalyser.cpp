#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *lex_keywords[] = {"print", "check", "jmpt"};
const char *lex_builtin_func[] = {"?prod", "?sell"};

enum LexemeType
{
    Number,
    Identificator,
    String,
    Operator,
    Keyword,
    AssigmentOperator,
    Err
};

enum SymbolType
{
    Alpha,
    Digit,
    Separator,
    Quote,
    EqualitySign,
    Colon,
    Mark,
    Operation,
    SymbolError
};

enum ErrorType
{
    UnexpectedSymbol,
    UnknownKeyword,
    UnknownFunc,
    AlphaAfterNumber,
    UnclosedQuote
};

struct Error
{
    int line;
    int pos;
    ErrorType type;
    char *value;
    Error(int l, int p, ErrorType t) : line(l), pos(p), type(t) {}
    const char *getStringErrorType() const;
    void printData() const;
};

void Error::printData() const
{
    printf("Error at line %d: %s\n\t%s\n", line, getStringErrorType(),
            value);
}

const char *Error::getStringErrorType() const
{
    switch (type)
    {
    case UnexpectedSymbol:
        return "UnexpecctedSymbol";
    case UnknownKeyword:
        return "UnknownKeyword";
    case UnknownFunc:
        return "UnknownFunc";
    case AlphaAfterNumber:
        return "AlphaAfterNumber";
    case UnclosedQuote:
        return "UnclosedQuote";
    default:
        return 0;
    }
}

struct Lexeme
{
    int lineNumber;
    LexemeType type;
    char* value;
    Lexeme* next;
    Lexeme(int line, LexemeType t, const char* s)
        : lineNumber(line)
        , type(t)
    {
        if (s != 0)
        {
            value = new char[strlen(s) + 1];
            strcpy(value, s);
        }
        else
            value = 0;
    }
    const char* getTypeString() const;
    Lexeme(const Lexeme& l);
};

Lexeme::Lexeme(const Lexeme& l)
{
    lineNumber = l.lineNumber;
    type = l.type;
    value = new char[strlen(l.value) + 1];
    strcpy(value, l.value);
}

const char* Lexeme::getTypeString() const
{
    switch (type)
    { 
    case Number:
        return "Number";
    case Identificator:
        return "Ident";
    case String:
        return "String";
    case Operator:
        return "Operator";
    case Keyword:
        return "Keyword";
    case AssigmentOperator:
        return "Assigment";
    default:
        return 0;
    }
}

class LexemeList
{
    Lexeme* head;
    Lexeme* tail;
public:
    LexemeList()
        : head(0)
        , tail(0)
    {
    }
    void add(Lexeme l);
    void display();
};

void LexemeList::add(Lexeme l)
{
    Lexeme* tmp = new Lexeme(0, Number, 0);
    *tmp = l;
    tmp->next = 0;
    if (head == 0)
    {
        head = tmp;
        tail = tmp;
    }
    else
    {
        tail->next = tmp;
        tail = tmp;
    }
}

void LexemeList::display()
{
    for (Lexeme* tmp = head; tmp != 0; tmp = tmp->next)
    {
        printf("Lexeme Type: \"%s\"\tLine Number: %d\tValue: %s\n",
               tmp->getTypeString(), tmp->lineNumber, tmp->value);
    }
}

class LexAnalyser
{
    enum State
    {
        N,
        I,
        K,
        A,
        S,
        H,
        E
    };
    enum
    {
        bufSize = 1024
    };
    LexemeList lexList;
    char* buffer;
    int size;
    int currentSymbolType;
    int currentLine;
    Error *err;
    State state;
    void switchState();
    void number();
    void ident();
    void keyword();
    void assigment();
    void string();
    void start();

    void error();

    SymbolType getSymbolType(int c);

public:
    LexemeList getLexemeList()
    {
        return lexList;
    }
    LexAnalyser();
    int step(int c);
    Error *isErrored();
};

LexAnalyser::LexAnalyser()
{
    buffer = new char[bufSize];
    buffer[0] = 0;
    size = 0;
    state = H;
    currentLine = 1;
}

SymbolType LexAnalyser::getSymbolType(int c)
{
    if (isspace(c) || c == ';' || c == ',')
        return Separator;
    else if (isalpha(c))
        return Alpha;
    else if (isdigit(c))
        return Digit;
    else if (c == '$' || c == '@' || c == '?')
        return Mark;
    else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%'
             || c == '<' || c == '>' || c == ')' || c == '(' || c == ']'
             || c == '[')
        return Operation;
    else if (c == '=')
        return EqualitySign;
    else if (c == ':')
        return Colon;
    else if (c == '"')
        return Quote;
    return SymbolError;
}

void LexAnalyser::switchState()
{
    currentSymbolType = getSymbolType(buffer[size - 1]);
    switch (state)
    {
    case N:
        number();
        break;
    case I:
        ident();
        break;
    case K:
        keyword();
        break;
    case A:
        assigment();
        break;
    case S:
        string();
        break;
    case H:
        start();
        break;
    default:
        break;
    }
}

void LexAnalyser::number()
{
    if (currentSymbolType == Operation
        || currentSymbolType == EqualitySign)
    {
        buffer[size] = buffer[size - 1];
        buffer[size - 1] = 0;
        buffer[size + 1] = 0;
        lexList.add(Lexeme(currentLine, Number, buffer));
        buffer[0] = buffer[size];
        size = 1;
        state = H;
    }
    else if (currentSymbolType == Separator)
    {
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, Number, buffer));
        size = 0;
        state = H;
    }
    else if (currentSymbolType == Digit)
    {
        state = N;
    }
    else
    {
        error();
    }
}

void LexAnalyser::error()
{
    switch (state)
    {
    case E:
        return;
    case N:
        err = new Error(currentLine, 0, AlphaAfterNumber);
        break;
    case K:
        err = new Error(currentLine, 0, UnknownKeyword);
    default:
        err = new Error(currentLine, 0, UnexpectedSymbol);
    }
    state = E;
    buffer[size] = 0;
    err->value = new char[strlen(buffer) + 1];
    strcpy(err->value, buffer);
    state = E;
}

void LexAnalyser::ident()
{
    if (currentSymbolType == Operation || currentSymbolType == EqualitySign
        || currentSymbolType == Colon)
    {
        buffer[size] = buffer[size - 1];
        buffer[size - 1] = 0;
        buffer[size + 1] = 0;
        lexList.add(Lexeme(currentLine, Identificator, buffer));
        buffer[0] = buffer[size];
        size = 1;
        state = H;
    }
    else if (currentSymbolType == Separator)
    {
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, Identificator, buffer));
        size = 0;
        state = H;
    }
    else if (currentSymbolType == Alpha || currentSymbolType == Digit)
        state = I;
    else
        error();
}

void LexAnalyser::keyword()
{
    if (currentSymbolType == Operation
        || currentSymbolType == EqualitySign)
    {
        buffer[size] = buffer[size - 1];
        buffer[size - 1] = 0;
        buffer[size + 1] = 0;
        lexList.add(Lexeme(currentLine, Keyword, buffer));
        buffer[0] = buffer[size];
        size = 1;
        state = H;
    }
    else if (currentSymbolType == Separator)
    {
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, Keyword, buffer));
        size = 0;
        state = H;
    }
    else if (currentSymbolType == Alpha)
        state = K;
    
    bool flag = false;
    buffer[size] = 0;
    for (unsigned i = 0; i < 3; i++)
         flag = flag ||
             strncmp(buffer, lex_keywords[i], strlen(buffer)) == 0; 
    if (!flag)
        error();
}

void LexAnalyser::assigment()
{
    if (currentSymbolType == EqualitySign)
    {
        buffer[size] = 0;
        lexList.add(Lexeme(currentLine, AssigmentOperator, buffer));
        size = 0;
        state = H;
    }
    else
        error();
}

void LexAnalyser::string()
{
    if (currentSymbolType == Quote)
    {
        buffer[size] = 0;
        lexList.add(Lexeme(currentLine, String, buffer));
        size = 0;
        state = H;
    }
    else
        state = S;
}

void LexAnalyser::start()
{
    if (size == 2 && getSymbolType(buffer[0]) == Colon)
    {
        state = A;
        assigment();
        return;
    }
    else if (size == 2)
    {
        buffer[size] = buffer[size - 1];
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, String, buffer));
        buffer[0] = buffer[size];
        size = 1;
    }
    if ((currentSymbolType == Operation
         || currentSymbolType == EqualitySign)
        && size == 1)
    {
        buffer[size] = 0;
        lexList.add(Lexeme(currentLine, Operator, buffer));
        state = H;
        size = 0;
    }
    else if (currentSymbolType == Quote)
        state = S;
    else if (currentSymbolType == Alpha)
        state = K;
    else if (currentSymbolType == Digit)
        state = N;
    else if (currentSymbolType == Mark)
        state = I;
    else if (currentSymbolType == Colon)
        state = A;
    else if (currentSymbolType == Separator)
    {
        size = 0;
        state = H;
    }
    else
        error();
}

int LexAnalyser::step(int c)
{
    buffer[size++] = c;
    if (c == '\n')
        currentLine++;
    if (c == EOF) 
    {
        buffer[size - 1] = ' ';
    }
    switchState();
    return c != EOF;
}

Error *LexAnalyser::isErrored()
{
    if (state == E)
        return err;
    return 0;
}

int main(int argc, char* argv[])
{
    LexAnalyser la;
    FILE* file = fopen("test.anlang", "r");

    while (la.step(fgetc(file)));//(c = fgetc(file)) != EOF)
    la.getLexemeList().display();
    Error *e;
    if ((e = la.isErrored()))
        e->printData();
    return 0;
}
