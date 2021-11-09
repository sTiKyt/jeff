# cc-jeff

Джефф - настраиваемый и расширяемый чат-бот для десктопов.

## Сборка

Генерация Make-файла выполняется командой `$ cmake -S ./ -B /path/of/makefile/dir`.

## Описание

### Встроенный поиск выражений-триггеров

Джефф предоставляет встроенный поиск триггеров в введённых выражениях пользователя. Для ответа на них используются базы данных \*.j.db (SQLite).

Чтобы создать базу данных, откройте `Файл > Менеджер источников` или напишите сообщение `/src`. В ответном сообщении нажмите `Добавить контейнер |> Создать`, введите любое название источника, выберите файл для сохранения и нажмите `Сохранить`. Для сохранения изменений нажмите `Сохранить и закрыть`.

Содержимое базы данных включает в себя таблицу `sources` с определёнными свойствами, никнеймом источника (в одной базе данных может быть несколько источников) и его UUID, а также сами таблицы источников.

Источник представляет из себя таблицу с тремя столбцами: 

- адрес выражения
- само выражение
- ссылки от данного выражения на другие
- исполняемый бит (будет добавлен в версии 0.5.0).

Таким образом, использование баз данных может заключаться в подобном примере:

```
1 | Привет!                              | 1,2,3 | 0
2 | Здравствуй!                          | 1,2,3 | 0
3 | {"handle_in_script": "greetings.py"} |       | 1
4 | Что такое                            | 5     | 0
5 | {"handle_in_script": "wiki.py"}      |       | 1
```

Выражения могут ссылаться сами на себя. Выбор между вариантами происходит случайным образом.

Ядро поиска триггеров находится в классе `NLPmodule`. Ядро собирает из кэша выражения (если не находит - из остальных источников), освобождает от знаков препинания, делает регистронезависимыми, затем ищет их вхождения в пользовательский ввод. Если выражение активатора было найдено с определённой точностью, выражение реагирования добавляется в список кандидатов. Из списка кандидатов выбираются выражения с максимальным покрытием, наибольшим весом и наименьшим числом использования. Если треть ввода не покрыта ответными выражениями, поиск повторяется по источникам, если использовался только кэш.

### Скрипты и гибкая система настройки поведения (в версии 0.5.0)

Джефф может обрабатывать выражения при помощи внешних скриптов в нескольких случаях:

1. При запуске программы (startup).
2. При любом вводе пользователя (custom_scan).
3. При необходимости ответить на ввод пользователя (answer).
4. При необходимости иначе скомпоновать полученные результаты (custom_compose).
5. При вводе специальных команд, когда ввод предназначен только для одного скрипта (action_provider и go_action).
6. При создании обновляемых сообщений (get_status_update).

Скрипты пишутся на Python и имеют расширение \*.j.py.

#### Запуск программы

Скрипт должен иметь функцию `startup` без аргументов.

#### Обработка ввода пользователя

Выражение имеет вид:

```json
{
  "handle_in_script": "путь до файла", 
  "type": "custom_scan", 
  "get_hist": "число последних сообщений"
}
```

Скрипт должен иметь функцию `custom_scan`. Единственным аргументом передаётся JSON-строка с историей сообщений.

#### Ответ на ввод пользователя

Выражение имеет вид:

```json
{
  "handle_in_script": "путь до файла",
  "type": "answer",
  "get_hist": "число последних сообщений"
}
```

Скрипт должен иметь функцию `answer`. Единственным аргументом передаётся JSON-строка с историей сообщений.

#### Кастомная компоновка результатов сканирования NLPmodule

Выражение имеет вид:

```json
{
  "handle_in_script": "путь до файла",
  "type": "custom_compose",
  "input": "ввод пользователя",
  "candidates": {
    "c1": ["активатор1", "реагент1"],
    "c2": ["активатор2", "реагент2"]
  }
}
```

Скрипт должен иметь функцию `custom_compose`.

Кастомная компоновка не предоставляет какого-либо ответного выражения, но если активатор попадает в выборку, то компоновка автоматически передаётся скрипту с наибольшим весом. В функцию передаются вводная строка и предлагаемые варианты. (TODO)

#### Специальные команды

Скрипт должен иметь функцию `action_provider`, возвращающий уникальные для конфигурации Джеффа команды, например, `/spotify-tui`, и их краткие описания (до 120 символов), в виде пар ключей-значений. Помимо этого, также должна быть функция `go_action`, которая принимает строку ввода, содержащую в том числе название команды.

#### Обновляемые сообщения

Выражение имеет вид:

```json
{
  "handle_in_script": "путь до файла",
  "type": "status",
  "statusID": "идентификатор"
}
```

Скрипт должен иметь функцию `get_status_update` с аргументом - идентификатором сообщения.

#### Дополнительные опции запроса

`"value": "передать данные под данным ключом"` - отправляет дополнительно в функцию значение из памяти

#### Вывод скрипта

Формат вывода - JSON. Пример вывода:

```json
{
  "send": "Привет, как дела?",
  "onKey1": "val123",
  "storeValue1": "Получается, так"
}
```

##### send

1. `"send": "сообщение для вывода на экран"` - отправляет сообщение пользователю
2. `"sendOnX": "сообщение для вывода на экран"` - отправляет сообщение пользователю через X вводов пользователя
3. `"sendForXMins": "сообщение для вывода на экран"` - отправляет сообщение пользователю через X минут

##### sendStatus

4. `"statusID": номер ключа в этом скрипте` - нумерует нижеуказанный статус для передачи в скрипт в функцию `get_status_update(key)`
5. `"sendStatus": "сообщение для вывода на экран"` - отправляет статус на экран с начальным текстом
6. `"sendStatusWithValue": "ключ значения"` - сообщение статуса автоматически обновляется при обновлении значения

##### storeValue

7. `"onKeys": ["ключи значений",..]` - устанавливает ключи для записи нижеуказанных значений в память
8. `"storeValues": ["значения",..]` - устанавливает значения для ключей из списка `onKeys`

## Руководство по заполнению баз данных

