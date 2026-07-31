@echo off
chcp 65001 > nul
cls
echo ====================================================================
echo             ОЧИСТКА РЕПОЗИТОРИЯ И СБРОС ИСТОРИИ КОММИТОВ
echo ====================================================================
echo Этот скрипт очищает историю и приводит файлы НА СЕРВЕРЕ GIT 
echo в соответствие c текущим составом файлов на данном ПК.
echo Перед запуском скрипта скопируйте все архивные/неактивные
echo проекты во внешнее хранилище. УДАЛИТЕ папки неактивных проектов
echo из этой рабочей директории.
echo ====================================================================
echo.
echo Вы готовы продолжить? 
echo [1] Да, начать очистку.
echo [2] Нет, я хочу выйти и проверить файлы.
echo.

set /p user_choice="Введите цифру 1 или 2 и нажмите Enter: "

if "%user_choice%"=="1" goto start_process
if "%user_choice%"=="2" goto cancel_process

:incorrect_choice
echo.
echo [!] Неверный ввод. Пожалуйста, введите 1 или 2.
timeout /t 3 > nul
goto :EOF

:cancel_process
echo.
echo === Отмена операции. Скрипт закрывается, ничего не изменено. ===
pause
exit

:start_process

echo === Запуск очистки репозитория и сброса истории ===
echo.

:: 1. Создание чистой ветки без истории
echo [1/5] Создание временной ветки...
git checkout --orphan temporary_branch
if %errorlevel% neq 0 goto error

:: 2. Индексация всех актуальных файлов и коммит
echo [2/5] Фиксация текущих актуальных файлов...
git add -A
git commit -m "Initial commit: актуальные проекты"
if %errorlevel% neq 0 goto error

:: 3. Удаление старой ветки main и переименование текущей
echo [3/5] Перестроение структуры веток...
git branch -D main
git branch -m main
if %errorlevel% neq 0 goto error

:: 4. Принудительное обновление GitHub
echo [4/5] Отправка чистой истории на GitHub (Force Push)...
git push -f origin main
if %errorlevel% neq 0 goto error

:: 5. Очистка локального мусора на диске
echo [5/5] Запуск сборщика мусора Git для освобождения места...
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git branch -u origin/main

echo.
echo === Успешно! Репозиторий очищен локально и на сервере. ===
pause
exit

:error
echo.
echo [ОШИБКА] Что-то пошло не так. Проверьте сообщения выше.
pause
