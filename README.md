# Игрушечный интерпретатор.

Запускает испольняемые файлы по полному пути с указанными аргументами.
В случае ошибки системного вызова выводит в cerr название мистемного вызова и краткое сообщение об ошибке. В случае успешного исполнения выводит код его завершения. Если исполнение было прервано сигналом, то выводит номер сигнала

Поддерживает переменные окружения через set VARIABLE [VALUE] и unset VARIABLE

