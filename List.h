//////////////////
// FILE: List.h //
//////////////////

#include <initializer_list>
#include <stdexcept>
// личная библиотека-лист. Удобно очень
namespace CustomList { // вынес в отдельное пространство имен, чтобы если что, не пересекалось лишний раз ничего
    template <typename T> // данная штукенция делает мой список шаблонным
    class List {
        private:
            struct Element { // структура элемента
                T value; // значение
                Element* next; // след. элемент
                Element(T val) : value(val), next(nullptr) {} // конструктор
            };
    
            Element* head; // указатель на первый элемент
            int size; // размер
        public:
            List() : head(nullptr), size(0) {} // конструктор списка
            
            List(std::initializer_list<T> init) : head(nullptr), size(0) { // специфичный конструктор списка
                for (const T& val : init) pushBack(val);
            }
    
            List(const List& other) : head(nullptr), size(0) { // конструктор копирования
                Element* current = other.head;
                while (current) {
                    pushBack(current->value);
                    current = current->next;
                }
            }
    
            ~List() { // деструктор
                while (head) {
                    Element* temp = head;
                    head = head->next;
                    delete temp;
                }
            }
    
            void pushBack(T value) { // метод добавления значения в конец списка
                Element* el = new Element(value);
                if (!head) head = el;
                else {
                    Element* current = head;
                    while (current->next) current = current->next;
                    current->next = el;
                }
                size++;
            }
            void pushFront(T value) { // метод добавления значения в начало списка
                Element* el = new Element(value);
                el->next = head;
                head = el;
                size++;
            }
            T popBack() { // метод изъятия последнего элемента
                if (!head) throw std::runtime_error("Element is not available");
                Element* current = head;
                Element* prev = nullptr;
                while (current->next) {
                    prev = current;
                    current = current->next;
                }
                T val = current->value;
                if (!prev) head = nullptr;
                else prev->next = nullptr;
                delete current;
                size--;
                return val;
            }
    
            T get(int index) const { // метод получения элемента по индексу
                if (index < 0 || index >= size) throw std::runtime_error("Element is not available");
                Element* current = head;
                for (int i = 0; i < index; ++i) current = current->next;
                return current->value;
            }
    
            void put(int index, T value) { // метод установки значения по индексу
                if (index < 0 || index >= size) return;
                Element* current = head;
                for (int i = 0; i < index; ++i) current = current->next;
                current->value = value;
            }
    
            int getSize() const { return size; } // метод получения размера списка
            
            List& operator=(const List& other) { // перегрузка оператора присваивания(для избежания проблем с памятью)
                if (this != &other) {
                    clear(); // Удаляем текущие элементы
                    // Копируем элементы из other
                    Element* current = other.head;
                    while (current) {
                        pushBack(current->value);
                        current = current->next;
                    }
                }
                return *this;
            }
            void clear() { // метод очистки списка ото всего
                while (head) { // пока есть голова
                    Element* temp = head;
                    head = head->next; // получаем голову, сохраняем голову в temp, устанавливаем новую голову, удаляем temp
                    delete temp;
                }
                size = 0; // не забываем поставить размер в нуль
            }
            // Перегрузка оператора [] для неконстантного доступа (чтение/запись)
            T& operator[](int index) {
                if (index < 0 || index >= size) {
                    throw std::out_of_range("List index out of range");
                }
                Element* current = head;
                for (int i = 0; i < index; ++i) {
                    current = current->next;
                }
                return current->value;
            }

            // Перегрузка оператора [] для константного доступа (только чтение)
            const T& operator[](int index) const {
                if (index < 0 || index >= size) {
                    throw std::out_of_range("List index out of range");
                }
                Element* current = head;
                for (int i = 0; i < index; ++i) {
                    current = current->next;
                }
                return current->value;
            }
    };
}

/////////////////////////
// END OF FILE: List.h //
/////////////////////////