////////////////////
// FILE: main.cpp //
////////////////////

// ИСХОДНИКИ - ЗДЕСЬ -> https://github.com/Asm0deus22/LAB4_OTYAP

#include <fstream>
#include <iostream>
#include <List.h>
#include <Utils.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>

using namespace std;
using namespace CustomList;
using namespace Lexemes;

int main() { // я не хочу это в main, это же ведь тест
    ifstream reading("input.txt"); // готовим чтение
    
    reading.seekg(0, ios::end); // узнаем размер файла
    int size = (int) reading.tellg();
    reading.seekg(0, ios::beg);
    
    char *str = new char[size + 1]; // готовим строку для сохранения
    reading.read(str, size); // читаем
    str[size] = 0; // зануляем конец
    reading.close(); // закрываем чтение
    // инициализируем списки лексем
    List<Lexeme*> *lexemes = new List<Lexeme*>();
    
    LexicalAnalyser::analyze(*lexemes, str, size + 1); // запускаем лексический анализатор
    
    List<SyntaxAnalyser::MarkedLexeme*> *markedLexemes = new List<SyntaxAnalyser::MarkedLexeme*>(); // формируем список маркированных лексем
    
    SyntaxAnalyser::SyntaxAnalyseResult *result = SyntaxAnalyser::analyze(*lexemes, *markedLexemes); // запускаем синтаксический анализатор
    sortLexical(*(result->awaited));
    
    /////////////////// ВЫВОДИМ ИНФУ /////////////////////
    ofstream writing("output.txt"); // готовим запись
    char *out;
    bool isFirst = true;
    SyntaxAnalyser::MarkedLexeme *current;
    int i;
    for (i = 0; i < markedLexemes->getSize(); i++) { // Выводим все лексемы
        current = markedLexemes->operator[](i);
        out = SyntaxAnalyser::getLexemeCString(*current);
        
        if (isFirst) isFirst = !isFirst;
        else {
            cout << ' ';
            writing << ' ';
        }
        cout << out;
        writing << out;
        
        delete[] out; // очищаем временную строку
    }
    cout << endl;
    writing << endl;
    if (result->isOK) {
        cout << "OK";
        writing << "OK";
    } else {
        cout << result->posERROR << ' ';
        isFirst = true;
        for (i = 0; i < result->awaited->getSize(); i++) {
            if (isFirst) isFirst = !isFirst;
            else {
                cout << ' ';
                writing << ' ';
            }
            cout << result->awaited->get(i);
            writing << result->awaited->get(i);
        }
    }
    /////////////////// КОНЕЦ ИНФЫ /////////////////////
    
    // все нафиг сносим и закрываем адекватно
    writing.close();
    delete result;
    while (markedLexemes->getSize()) delete markedLexemes->popBack();
    delete markedLexemes;
    while (lexemes->getSize()) delete lexemes->popBack(); // достаточно удалять только из этого списка, т.к. в ids, constants хранятся именно что копии указателей
    delete lexemes;
    delete[] str;

    return 0;
}
///////////////////////////
// END OF FILE: main.cpp //
///////////////////////////