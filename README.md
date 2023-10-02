# cpp-spreadsheet
Дипломный проект: Электронная таблица (Простой аналог MS Office Excel)

# Требования

C++17 и выше

[Java SE Runtime Environment 8.](https://www.oracle.com/java/technologies/javase-jre8-downloads.html)

[ANTLR](https://www.antlr.org/)

# Описание
Программа реализована, используя динамический полиморфизм, интеграцию библиотеки ANTLR (java) для работы с абстрактно синтаксическим деревом, хэш-таблицу в качестве основной структуры данных для реализации интерфейса (Лист/sheet Excel). Данные хранятся в ячейках и защищены от циклических зависимостей. Абстрактное синтаксическое дерево применяется для решения формульных задач по аналогии MS Office Excel. В ячейках также можно размещать и текст.

*Пример:*
````
sheet->SetCell("A1"_pos, "2");
sheet->SetCell("A2"_pos, "=A1+1");
sheet->SetCell("A3"_pos, "=A2+2");
sheet->SetCell("A4"_pos, "=A3+3");
sheet->SetCell("A5"_pos, "=A1+A2+A3+A4");

auto* cell_A5_ptr = sheet->GetCell("A5"_pos);
ASSERT_EQUAL(std::get<double>(cell_A5_ptr->GetValue()), 18);
````
*При обнаружении циклической ссылки будет выброшено исключение*

![Снимок экрана 2023-10-02 233235](https://github.com/Evgenicast/cpp-transport-catalogue/assets/107400788/ffef4bd5-b9fe-40cc-aa23-3b56d8f2b930)

*Все реализованный исключения:*

- **#DIV0!** - деление на ноль
- **#VALUE!** - если операнд содержит текст, а не числовое значение.
- **#REF!** - если обращение идет к ячейке (ссылка) за пределами Листа (sheet)

#Порядок сборки
1. Установить [Java SE Runtime Environment 8.](https://www.oracle.com/java/technologies/javase-jre8-downloads.html)
2. Установить [ANTLR](https://www.antlr.org/) (ANother Tool for Language Recognition), выполнив все пункты в меню Quick Start.
   В случае установки на Windows может быть полезно данное [видео.](https://youtu.be/p2gIBPz69DM)
4. Проверить в файлах FindANTLR.cmake и CMakeLists.txt название файла antlr-X.X.X-complete.jar на корректность версии. Вместо "X.X.X" указать свою версию antlr.
5. Создайть папку с названием "antlr4_runtime" без кавычек и скачайть в неё [файлы.](https://github.com/antlr/antlr4/tree/master/runtime/Cpp)
6. Запустить cmake build с CMakeLists.txt.
