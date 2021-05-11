#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

enum LexemeType
{
    Number,
    Identificator,
    String,
    Operator,
    Keyword,
    AssigmentOperator,
    Error
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

struct Lexeme 
{
    int lineNumber;
    LexemeType type;
    char *value;
    Lexeme *next;
    Lexeme(int line, LexemeType t, const char *s) : lineNumber(line), type(t) 
    {
        if (s != 0)
        {
            value = new char[strlen(s) + 1];
            strcpy(value, s);
        }
        else
            value = 0;
    }
    const char *getTypeString();
    Lexeme(const Lexeme &l);
};

Lexeme::Lexeme(const Lexeme &l)
{
    lineNumber = l.lineNumber;
    type = l.type;
    value = new char[strlen(l.value) + 1];
    strcpy(value, l.value);
}

const char *Lexeme::getTypeString()
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
    Lexeme *head;
    Lexeme *tail;
public:
    LexemeList() : head(0), tail(0) {}
    void add(Lexeme l);
    void display();
};

void LexemeList::add(Lexeme l) 
{     
    Lexeme *tmp =new Lexeme(0, Number, 0); 
    *tmp = l;
    tmp->next = 0;
    if(head == 0)
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
    for (Lexeme *tmp = head; tmp != 0; tmp = tmp->next)
    {
        printf("Lexeme Type: \"%s\"\tLine Number: %d\tValue: %s\n", 
                tmp->getTypeString(), tmp->lineNumber, tmp->value);
    }
} 

class LexAnalyser
{
    friend int main(int argc, char *argv[]); // for debug!
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
    char *buffer;
    int size;
    int currentSymbolType;
    int currentLine;
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
    LexemeList getLexemeList() { return lexList; }
    LexAnalyser();
    void step(int c);
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
    if (isspace(c) || c == ';' || c == ',' || c == '{' || c == '}')
        return Separator;
    else if (isalpha(c)) 
        return Alpha;
    else if (isdigit(c))
        return Digit;
    else if (c == '$' || c == '@' || c == '?')
        return Mark;
    else if (c == '+' || c == '-' || c == '*' || c == '/' || 
            c == '%' || c == '<' || c == '>'|| c == ')' || 
            c == '(' || c == ']' || c == '[')
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
    if (currentSymbolType == Operation || currentSymbolType == EqualitySign)
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
    state = E;
}

void LexAnalyser::ident()
{
    if(currentSymbolType == Operation || currentSymbolType == EqualitySign || currentSymbolType == Colon){
        buffer[size] = buffer[size - 1];
        buffer[size - 1] = 0;
        buffer[size + 1] = 0;
        lexList.add(Lexeme(currentLine, Identificator, buffer));
        buffer[0] = buffer[size];
        size = 1;
        state = H;
    }
    else if(currentSymbolType == Separator){
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, Identificator, buffer));
        size = 0;
        state = H;
    }
    else if(currentSymbolType == Alpha || currentSymbolType == Digit)
        state = I;
    else
        error();
}

void LexAnalyser::keyword()
{
    if(currentSymbolType == Operation || currentSymbolType == EqualitySign){
        buffer[size] = buffer[size - 1];
        buffer[size - 1] = 0;
        buffer[size + 1] = 0;
        lexList.add(Lexeme(currentLine, Keyword, buffer));
        buffer[0] = buffer[size];
        size = 1;
        state = H;
    }
    else if(currentSymbolType == Separator){
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, Keyword, buffer));
        size = 0;
        state = H;
    }
    else if(currentSymbolType == Alpha)
        state = K;
    else
        error();
    
}

void LexAnalyser::assigment()
{
    if(currentSymbolType == EqualitySign){
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
    if(currentSymbolType == Quote){
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
    else if (size == 2) // dont understand yet
    {
        buffer[size] = buffer[size - 1]; 
        buffer[size - 1] = 0;
        lexList.add(Lexeme(currentLine, String, buffer));
        // lexems = addToList(buf, line, H);
        buffer[0] = buffer[size];
        size = 1;
    }
    if((currentSymbolType == Operation || currentSymbolType == EqualitySign) && size == 1){
        buffer[size] = 0;
        lexList.add(Lexeme(currentLine, Operator, buffer));
        state = H;
        size = 0;
    }
    else if(currentSymbolType == Quote)
        state = S;
    else if(currentSymbolType == Alpha)
        state = K;
    else if(currentSymbolType == Digit)
        state = N;
    else if(currentSymbolType == Mark)
        state = I;
    else if(currentSymbolType == Colon)
        state = A;
    else if(currentSymbolType == Separator){
        size = 0;
        state = H;
    }
    else
        error();

}



void LexAnalyser::step(int c)
{
    buffer[size++] = c;
    if (c == '\n')
        currentLine++;
    switchState();
}

int main(int argc, char *argv[])
{
    int c;
    LexAnalyser la;
    FILE *file = fopen("test.anlang", "r");

    while ((c = fgetc(file)) != EOF)
        la.step(c);

    la.step(' ');
    la.getLexemeList().display();
    return 0;
}
