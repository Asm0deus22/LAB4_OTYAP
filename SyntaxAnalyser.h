////////////////////////////
// FILE: SyntaxAnalyser.h //
////////////////////////////
/*
if <логическое выражение> then
<операторы> 
[ elseif <логическое выражение> then <операторы> ]
end

kw - if, elseif, then, end, and, not, or
co - <, >, <=, >=, <>
ao - +, -, *, /
id - символы алфавита
vl - цифры
wl - инвалидное/иное
eq - =
sc - ;
 
<логическое выражение> → <выражение сравнения> |
<унарная логическая операция><выражение сравнения> |
<логическое выражение><бинарная логическая операция><выражение сравнения>
<выражение сравнения> → <операнд> | <операнд><операция сравнения><операнд> 
<операция сравнения> → < | > | = | <> 
<унарная логическая операция> → not 
<бинарная логическая операция> → and | or 
<операнд> → <идентификатор> | <константа> 
<операторы> → <операторы>; <оператор> | <оператор> 
<оператор> → <идентификатор> = <арифметическое выражение>
<арифметическое выражение> → <операнд> |
<арифметическое выражение><арифметическая операция><операнд>
<арифметическая операция> → + | - | / | * 
*/
namespace SyntaxAnalyser { // пространство имен, где все будет использоваться
    using namespace std;
    using namespace CustomList;
    
    enum State { // список состояний расширенного конечного автомата
        S,
        IF,
        NT_1,
        IC_1,
        CO_1,
        IC_2,
        AO_1,
        TH,
        ID,
        EQ,
        IC_3,
        SC,
        AP,
        END,
        EI,
        NT_2,
        IC_4,
        CO_2,
        IC_5,
        AO_2,
        ERR
    };
    enum Type { // список типов лексем
        K_if,
        K_th,
        K_ei,
        D_co,
        D_eq, // 5
        D_id,
        D_vl,
        D_ao,
        K_lo,
        K_no, // 10
        D_sc,
        D_wl,
        K_en
    };
    
    State shiftTable[21][13] = {  // таблица сдвигов в новые состояния. Используются все в таком порядке из enum Type(столбцы). Подробнее - https://asm0deus22.github.io/LAB4_OTYAP/CreateAUTO.png
        {IF, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // S,
        {ERR, ERR, ERR, ERR, ERR, IC_1, IC_1, ERR, ERR, NT_1, ERR, ERR, ERR}, // IF
        {ERR, ERR, ERR, ERR, ERR, IC_1, IC_1, ERR, ERR, ERR, ERR, ERR, ERR}, // NT_1
        {ERR, ERR, ERR, CO_1, CO_1, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // IC_1
        {ERR, ERR, ERR, ERR, ERR, IC_2, IC_2, ERR, ERR, ERR, ERR, ERR, ERR}, // CO_1
        {ERR, TH, ERR, ERR, ERR, ERR, ERR, ERR, AO_1, ERR, ERR, ERR, ERR}, // IC_2
        {ERR, ERR, ERR, ERR, ERR, IC_1, IC_1, ERR, ERR, NT_1, ERR, ERR, ERR}, // AO_1
        {ERR, ERR, ERR, ERR, ERR, ID, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // TH
        {ERR, ERR, ERR, ERR, EQ, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // ID
        {ERR, ERR, ERR, ERR, ERR, IC_3, IC_3, ERR, ERR, ERR, ERR, ERR, ERR}, // EQ
        {ERR, ERR, EI, ERR, ERR, ERR, ERR, AP, ERR, ERR, SC, ERR, END}, // IC_3
        {ERR, ERR, ERR, ERR, ERR, ID, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // SC
        {ERR, ERR, ERR, ERR, ERR, IC_3, IC_3, ERR, ERR, ERR, ERR, ERR, ERR}, // AP
        {ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // END
        {ERR, ERR, ERR, ERR, ERR, IC_4, IC_4, ERR, ERR, NT_2, ERR, ERR, ERR}, // EI
        {ERR, ERR, ERR, ERR, ERR, IC_4, IC_4, ERR, ERR, ERR, ERR, ERR, ERR}, // NT_2
        {ERR, ERR, ERR, CO_2, CO_2, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // IC_4
        {ERR, ERR, ERR, ERR, ERR, IC_5, IC_5, ERR, ERR, ERR, ERR, ERR, ERR}, // CO_2
        {ERR, TH, ERR, ERR, ERR, ERR, ERR, ERR, AO_2, ERR, ERR, ERR, ERR}, // IC_5
        {ERR, ERR, ERR, ERR, ERR, IC_4, IC_4, ERR, ERR, NT_1, ERR, ERR, ERR}, // AO_2
        {ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}, // ERR
    };
    
    struct MarkedLexeme {
        Type type;
        Lexemes::Lexeme* source;
    };
    
    MarkedLexeme* markLexeme(Lexemes::Lexeme *lex) { // функция маркировки лексемы
        MarkedLexeme *mlex = new MarkedLexeme();
        mlex->source = lex;
        switch (lex->type) {
            case Lexemes::kw: {
                if (!strcmp(lex->lexComponent, "if")) mlex->type = K_if;
                else if (!strcmp(lex->lexComponent, "then")) mlex->type = K_th;
                else if (!strcmp(lex->lexComponent, "elseif")) mlex->type = K_ei;
                else if (!strcmp(lex->lexComponent, "and")) mlex->type = K_lo;
                else if (!strcmp(lex->lexComponent, "or")) mlex->type = K_lo;
                else if (!strcmp(lex->lexComponent, "not")) mlex->type = K_no;
                else if (!strcmp(lex->lexComponent, "end")) mlex->type = K_en;
                else mlex->type = D_wl;
                break;
            }
            case Lexemes::id: {
                mlex->type = D_id;
                break;
            }
            case Lexemes::co: {
                mlex->type = D_co;
                break;
            }
            case Lexemes::ao: {
                mlex->type = D_ao;
                break;
            }
            case Lexemes::vl: {
                mlex->type = D_vl;
                break;
            }
            case Lexemes::wl: {
                mlex->type = D_wl;
                break;
            }
            case Lexemes::eq: {
                mlex->type = D_eq;
                break;
            }
            case Lexemes::sc: {
                mlex->type = D_sc;
                break;
            }
        }
        return mlex;
    }
    
    char *lexTypeEnumToCString(Type type) { // функция перевода типа в строку
        char *src;
        switch (type) {
            case K_if: { src = "if"; break; }
            case K_th: { src = "th"; break; }
            case K_ei: { src = "ei"; break; }
            case D_co: { src = "co"; break; }
            case D_eq: { src = "eq"; break; }
            case D_id: { src = "id"; break; }
            case D_vl: { src = "vl"; break; }
            case D_ao: { src = "ao"; break; }
            case K_lo: { src = "lo"; break; }
            case K_no: { src = "no"; break; }
            case D_sc: { src = "sc"; break; }
            case D_wl: { src = "wl"; break; }
            case K_en: { src = "en"; break; }
        }
        char *t = new char[strlen(src) + 1];
        strcpy(t, src);  // копируем строку и дело в шляпе
        return t;
    }
    char *getLexemeCString(MarkedLexeme &lex) { // функция получения лексемы в спец виде
        char *p = lex.source->lexComponent;
        int count = 0;
        while (*p++) count++;
        char *str = new char[count + 5];
        p = lex.source->lexComponent;
        char *g = str;
        while (*p) {
            *g = *p;
            g++; p++;
        }
        *g++ = '[';
        char* t = lexTypeEnumToCString(lex.type);
        strcpy(g, t);
        delete[] t;
        g += 2;
        *g++ = ']';
        *g = 0;
        return str;
    }
    int lexTypeToInt(Type t) { // функция получения индекса перечисления по типу
        switch (t) {
            case K_if: { return 0; }
            case K_th: { return 1; }
            case K_ei: { return 2; }
            case D_co: { return 3; }
            case D_eq: { return 4; }
            case D_id: { return 5; }
            case D_vl: { return 6; }
            case D_ao: { return 7; }
            case K_lo: { return 8; }
            case K_no: { return 9; }
            case D_sc: { return 10; }
            case D_wl: { return 11; }
            case K_en: { return 12; }
        }
    }
    Type getTypeByIndex(int index) { // получить тип по индексу через статичный каст
        if (index >= 0 && index < 13) {
            return static_cast<Type>(index);
        }
        throw std::out_of_range("Invalid index");
    }
    
    struct SyntaxAnalyseResult { // структура-отчет о данной хренотени(точней о результате синтаксического анализа данного текстового файла(а что?))
        bool isOK; // OK, не OK - решать ему
        int posERROR; // указывает на место ошибки
        List<MarkedLexeme*> *mLexemes; // список маркированных лексем
        List<char*> *awaited; // список пожеланий
        SyntaxAnalyseResult(List<MarkedLexeme*> *m) {
            isOK = false;
            posERROR = 0;
            awaited = new List<char*>;
            mLexemes = m;
        }
        ~SyntaxAnalyseResult() {
            while (awaited->getSize()) delete[] awaited->popBack();
            delete awaited;
        }
    };
    
    SyntaxAnalyseResult* analyze(List<Lexemes::Lexeme*> &list, List<MarkedLexeme*> &markedLexemes) {
        State currentState = S, nextState; // начальное состояние
        int size = list.getSize(); // получаем список лексем
        int i; // временная переменная
        SyntaxAnalyseResult *result = new SyntaxAnalyseResult(&markedLexemes); // формируем результат
        bool handleError{false}; // переменная-флаг ошибки
        MarkedLexeme *marked;
        for (i = 0; i < size; i++) { // маркируем все лексемы, заодно и проходимся синтаксисом
            marked = markLexeme(list.get(i));
            markedLexemes.pushBack(marked);
            if (!handleError) {
                nextState = shiftTable[currentState][marked->type];
                
                if (nextState == ERR) { // упс, поймали ошибку. Кажется, что-то не то
                    handleError = true;
                    result->posERROR = i + 1;
                    
                    for (int j = 0; j < 13; j++) { // перебираем все возможные типажи, которые синтаксический анализатор БЫ СХАВАЛ
                        if (shiftTable[currentState][j] != ERR) {
                            result->awaited->pushBack(lexTypeEnumToCString(getTypeByIndex(j))); // добавляем в список ожидания
                        }
                    }
                }
                currentState = nextState; // смещаем
            }
        }
        if (currentState == END) { // если последнее состояние явно успешное - то просто добавляем в отчет о том, что все хорошо
            result->isOK = true;
        } else if (!handleError) { // если нету завершающей конструкции - то явно что-то не так с исходником
            result->posERROR = size;
            for (int j = 0; j < 13; j++) { // перебираем все возможные типажи, которые синтаксический анализатор БЫ СХАВАЛ
                if (shiftTable[currentState][j] != ERR) {
                    result->awaited->pushBack(lexTypeEnumToCString(getTypeByIndex(j))); // добавляем в список ожидания
                }
            }
        }
        
        return result;
    }
}
////////////////////////////////
// END FILE: SyntaxAnalyser.h //
////////////////////////////////