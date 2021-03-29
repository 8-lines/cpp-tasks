// Задача №1 - копирование файлов.

// Сам файл XML должен иметь четкую структуру (<config>,  <file, />, </config>).

// Допускается: любое количество пробелов в начале строки, 
// любой порядок описания полей каждого файла, 
// любое количество файлов для копирования.

// Не допускается: пустые строки, 
// отклонение от названий полей (file_name, destination_path и т.д.),
// копирование в несуществующую директорию.

// При недопустимых действиях будет выведена соответствующая ошибка.
// При досрочном завершении программы уже обработанные файлы сохраняются.

#include <iostream>
#include <fstream>
#include <string>
#include <map>

// Обработка ошибок
// Ошибки в структуре конфигурационного файла (1-3) приводят к завершению программы.
// Ошибки в описании файлов (4-6) вызывают пропуск текущего файла.
void throw_error(int code)
{
    switch (code)
    {
    case 1:
        std::cout << "Error. Config file not found\n";
        exit(1);
        break;
    case 2:
        std::cout << "Error. Config file header is corrupted\n";
        exit(2);
        break;
    case 3:
        std::cout << "Error. Config file description is corrupted\n";
        exit(3);
        break;
    case 4:
        std::cout << "Error. Described file not found\n";
        break;
    case 5:
        std::cout << "Error. Destination path does not exist\n";
        break;
    case 6:
        std::cout << "Error. Wrong file description\n";
        break;
    }

    return;
}

// Удаление лишних символов из строки
void get_clean(std::string &str)
{
    // Удаляем лишние пробелы в начале строки
    while (str[0] == ' ')
        str.erase(0, 1);

    // Удаляем служебный символ в конце
    int len = str.length();
    if (str[len-1] == '\r')
        str.erase(len-1, 1);

    return;
}

// Считывание и запись информации о файле
void get_fileInfo(std::ifstream &cfg, std::map <std::string, std::string> &pathMap)
{
    std::string str;

    for (int i = 0; i < 3; i++)
    {
        // Считываем новую строку
        getline(cfg, str);

        // Удаляем лишние пробелы в начале строки
        get_clean(str);

        // Выделяем ключ и значение
        int pos = str.find("=");
        std::string key = str.substr(0, pos);
        std::string value = str.substr(pos + 2, str.length() - pos - 3);

        // При обращении по относительному пути добавляем символ текущей директории - '.'
        if (value[0] == '/' || value[0] == '\\')
            value = "." + value;

        // Записываем в соответствующее поле словаря если оно существует
        if (pathMap.count(key) == 1)
            pathMap[key] = value;
    }
    
    return;
}


int main()
{
    std::string str, src_path, dst_path, file_name;
    int counter = 0;

    // Словарь для хранения информации о текущем файле
    std::map <std::string, std::string> pathMap = { { "source_path",      "" },
                                                    { "destination_path", "" },
                                                    { "file_name",        "" } };

    // Открываем информационный файл
    std::ifstream  cfg("config.xml", std::ios::binary);
    if (!cfg)
    {
        // Если config файл не открылся - конфигурационный файл не найден
        throw_error(1);
    }

    // Считываем строку и проверяем что файл начинается с <config>
    getline(cfg, str);
    get_clean(str);
    if (str != "<config>")
        throw_error(2);

    // Считываем до конца конфигурационного файла
    while (!cfg.eof())
    {
        getline(cfg, str);
        get_clean(str);

        // Если структура конфигурационного файла соблюдается
        if (str == "<file")
        {
            // Считываем информацию о следующем файле
            get_fileInfo(cfg, pathMap);
            counter++;
            std::cout << "File #" << counter << ": ";

            if (pathMap["source_path"] != "" && pathMap["destination_path"] != "" && pathMap["file_name"] != "")
            {
                // Если все поля не пусты, дописываем имя файла в пути и открываем
                pathMap["source_path"] += "/" + pathMap["file_name"];
                pathMap["destination_path"] += "/" + pathMap["file_name"];
                std::ifstream  src(pathMap["source_path"], std::ios::binary);
                std::ofstream  dst(pathMap["destination_path"], std::ios::binary);

                if (!src)
                {
                    //Если файл не открылся - ошибка, файл не найден
                    throw_error(4);
                }
                else
                {
                    // Создаем копию файла
                    dst << src.rdbuf();

                    // Проверяем создался ли файл
                    if (dst)
                    {
                        // Успех
                        std::cout << "Success. Copy created" << std::endl;
                    }
                    else
                    {
                        // Если нет - путь назначения не существует
                        throw_error(5);
                    }
                }
            }
            else
            {
                // Если хотя бы одно из полей пустое - ошибка описания файла
                throw_error(6);
            }

            // Считываем следующую строку и проверяем что структура файла конфигурации соблюдается
            getline(cfg, str);
            get_clean(str);

            if (str != "/>")
            {
                // Иначе - ошибка структуры
                throw_error(3);
            }

        }
        else if (str == "</config>")
        {
            // Дошли до конца файла - завершаем программу
            break;
        }
        else
        {
            // Если встретилась посторонняя строка - ошибка структуры
            throw_error(3);
        }
    }

    return 0;
}