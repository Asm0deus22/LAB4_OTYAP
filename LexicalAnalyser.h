/////////////////////////////
// FILE: LexicalAnalyser.h //
/////////////////////////////
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
#include <cstring>

namespace Lexemes {
    enum LexemeType { // типы лексем. Это пригодится для будущего - синтаксического анализатора
        kw,
        id,
        co,
        ao,
        vl,
        wl,
        eq,
        sc
    };
    struct Lexeme { // структура готовой лексемы. Почему два типа лексем? Чтобы состояния не пересекались, да и модульность...
        LexemeType type;
        char *lexComponent;
        ~Lexeme() {
            delete[] lexComponent; // деструктор. Очень полезно. Если без прикола - автоматическое удаление куда выгодней, чем самому писать тысячи раз(при необходимости)
        }
    };
    void LexTypeEnumToCString(LexemeType type, char *res) { // ничего более адекватного я не придумал. Записывает enum в строковое представление. Без терминального нуля
        switch (type) {
            case kw: {
                *res++ = 'k';
                *res = 'w';
                break;
            }
            case id: {
                *res++ = 'i';
                *res = 'd';
                break;
            }
            case co: {
                *res++ = 'c';
                *res = 'o';
                break;
            }
            case ao: {
                *res++ = 'a';
                *res = 'o';
                break;
            }
            case vl: {
                *res++ = 'v';
                *res = 'l';
                break;
            }
            case wl: {
                *res++ = 'w';
                *res = 'l';
                break;
            }
            case eq: {
                *res++ = 'e';
                *res = 'q';
                break;
            }
            case sc: {
                *res++ = 's';
                *res = 'c';
                break;
            }
        }
    }
    char *getLexemeCString(Lexeme &lex) { // функция получения лексемы в спец виде
        char *p = lex.lexComponent;
        int count = 0;
        while (*p++) count++;
        char *str = new char[count + 5];
        p = lex.lexComponent;
        char *g = str;
        while (*p) {
            *g = *p;
            g++; p++;
        }
        *g++ = '[';
        LexTypeEnumToCString(lex.type, g);
        g += 2;
        *g++ = ']';
        *g = 0;
        return str;
    }
} 
namespace LexicalAnalyser { // пространство имен, где все будет использоваться
    using namespace Lexemes;
    using namespace std;
    using namespace CustomList;
    
    enum State { // список состояний расширенного конечного автомата
        S,
        ID,
        VL,
        WL,
        CO,
        COO,
        AO,
        EQ,
        SC,
        KW,
        F
    };
    
    State shiftTable[9][9] = {  // таблица сдвигов в новые состояния. Используются: 0-9 a-Z > < = +*/- ' ' ; ?
        {VL, ID, CO, COO, EQ, AO, S, SC, WL}, // S
        {ID, ID, F, F, F, F, F, F, WL}, // ID
        {VL, WL, F, F, F, F, F, F, WL}, // VL
        {WL, WL, F, F, F, F, F, F, WL}, // WL
        {F, F, F, F, CO, F,  F, F,  F}, // CO
        {F, F, CO, F, CO, F, F, F, F}, // COO
        {F, F, F, F, F, F, F, F, F}, // AO
        {F, F, F, F, F, F, F, F, F}, // EQ
        {F, F, F, F, F, F, F, F, F}  // SC
    };
    
    struct RawLexeme { // структура необработанной лексемы с деструктором
        State type;
        char *str;
        ~RawLexeme() {
            delete[] str;
        }
    };
    
    const int keywordsSize = 7;
    const char* keywords[keywordsSize] = {"if", "elseif", "then", "end", "and", "or", "not"}; // перечисление всех ключевых хренотеней
    
    int getCode(char c) { // функцuя oпрeдeления типa сuмвoлa
        if (c >= '0' && c <= '9') return 0;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
        if (c == '>') return 2;
        if (c == '<') return 3;
        if (c == '=') return 4;
        if (c == '+' || c == '-' || c == '*' || c == '/') return 5;
        if (c == ' ' || c == '\n' || c == '\t' || c == '\0') return 6;
        if (c == ';') return 7;
        return 8; // что-то там такое особое попалось
    }
    
    State validateLexeme(RawLexeme *lex) { // функция валидации и подправки сырых лексем
        if (lex->type == ID) {
            for (int i = 0; i < keywordsSize; i++) {
                if (!strcmp(lex->str, keywords[i])) return KW;
            }
            if (strlen(lex->str) <= 5) return ID;
            return WL; // невалидный, к сожалению
        }
        if (lex->type == VL) {
            ATOSUtils::ATOSResult result = ATOSUtils::ATOS(lex->str);
            if (!result.overflowFlag && !result.invalidFlag) return VL; // найдено переполнение/невалидность
            return WL;
        }
        if (lex->type == COO) return CO;
        return lex->type;
    }
    
    Lexeme *convertRawToFinalLexeme(RawLexeme *lexeme) { // функция конвертации в финальную лексему
        Lexeme *result = new Lexeme();
        result->lexComponent = new char[strlen(lexeme->str) + 1]; // остается для терминального нуля. Также места хавать будет в разы меньше, чем не хотелось бы
        char *pR = result->lexComponent, *pL = lexeme->str;
        while (*pL) {
            *pR = *pL;
            pR++; pL++;
        }
        *pR = 0;
        switch (lexeme->type) {
            case CO:
            case COO: {
                result->type = co;
                break;
            }
            case KW: {
                result->type = kw;
                break;
            }
            case ID: {
                result->type = id;
                break;
            }
            case AO: {
                result->type = ao;
                break;
            }
            case VL: {
                result->type = vl;
                break;
            }
            case WL: {
                result->type = wl;
                break;
            }
            case EQ: {
                result->type = eq;
                break;
            }
            case SC: {
                result->type = sc;
                break;
            }
        }
        return result;
    }
    
    void analyze(List<Lexeme*> &list, char *source, int size) {
        State currentState = S; // начальное состояние
        char *p; // указатель на подстроку(для сырых лексем)
        RawLexeme *lex = new RawLexeme();
        lex->str = new char[size + 1];
        *(lex->str + size) = 0; // обнуляем последний символ
        p = lex->str;
        int offset;
        for (offset = 0; offset < size; offset++) {
            currentState = shiftTable[currentState][getCode(*source)]; // переходим в новое состояние
            
            if (currentState == F) {
                *p = 0;
                lex->type = validateLexeme(lex); // немного правим состояние лексемы до финальной
                list.pushBack(convertRawToFinalLexeme(lex)); // финализируем и добавляем готовую лексему
                delete lex;
                
                lex = new RawLexeme();
                lex->str = new char[size - offset + 1];
                p = lex->str;
                currentState = S;
                currentState = shiftTable[currentState][getCode(*source)]; // переход в новое состояние
            }
            if (currentState != S) { // если не начальное состояние
                *p++ = *source; // записываем символ в строку
                lex->type = currentState;
            }
            source++;
        }
        delete lex;
    }
}
/////////////////////////////////
// END FILE: LexicalAnalyser.h //
/////////////////////////////////