# Как зайти на сервер

1. Сгенерируйте ключ
```bash
ssh-keygen -t rsa -b 4096 -C "your_name@g.nsu.ru"
```
2.
```bash
chmod 600 ~/.ssh/id_rsa
```
3. 
```bash
eval "$(ssh-agent -s)"
```
4. 
```bash
ssh-add ~/.ssh/id_rsa
```
5. 
```bash
mv Users/.../lin_priv ~/.ssh/
```
6.
```bash
chmod 600 ~/.ssh/lin_priv
```
7. 
```bash
eval "$(ssh-agent -s)"
```
8. 
```bash
ssh-add ~/.ssh/lin_priv
```
9. 
```bash
ssh -i ~/.ssh/lin_priv -p 2222 oppN@84.237.52.21
```

# Как зайти на кластер

Первый вариант:
```bash
ssh -i ~/.ssh/lin_priv hpcuserN@clu.nusc.ru 
```

Второй вариант:
```bash
ssh hpcuserN@10.4.60.10
```

# Компиляция и запуск программ

1. Заходим на кластер
2. Команда
```bash
mpi-selector --list3.
```
Если вылезает что то типо
```bash
perl: warning: Setting locale failed.
perl: warning: Please check that your locale settings:
 LANGUAGE = (unset),
 LC_ALL = (unset),
 LC_CTYPE = "UTF-8",
 LANG = (unset)
    are supported and installed on your system.
perl: warning: Falling back to the standard locale ("C").
```
то 
```bash
vim ~/.bashr
```
нажимаем i и в конец добавляем строки
```bash
export LANG="ru_RU.UTF-8"
export LC_ALL="ru_RU.UTF-8"
```
после нажимаем escape и пишем :wq
Команда
```bash
source ~/.bashrc
```
Опять запускаем
```bash
mpi-selector --list
```
Теперь ошибок быть не должно.
3. Команда
```bash
mpi-selector --set intel_mpi-5.0.1.035
```
Проверяем, что установилось:
```bash
mpi-selector --query
```
4. Команда
```bash
echo 'source /opt/intel/bin/compilervars.sh intel64' >> ~/.bashrc
```
5. Команда
```bash
source ~/.bashrc
```
6. Создаем папку lab1, файлы mpi_slae_1.c и mpi_slae_2.c
7. Компилируем
```bash
mpiicc -o mpi_slae mpi_slae_1.c -lm
```
8. На этом этапе у меня опять вылезали ошибки, поэтому мне пришлось вставить в самое начало кода
```bash
#define _XOPEN_SOURCE 700
```
(это вроде как для библиотеки math.h)
и компилировала я так:
```bash
mpiicc -std=c99 mpi_slae_1.c -o mpi_slae_1 -lm
```
9. Создаем файл run.sh:
```bash
vim run.sh
```
Нажимаем i, вставляем:
```bash
#!/bin/bash
#PBS -l walltime=00:30:00
#PBS -l select=1:ncpus=1:mpiprocs=1
#PBS -m n

cd $PBS_O_WORKDIR

MPI_NP=$(wc -l $PBS_NODEFILE | awk '{ print $1 }')

echo "Number of MPI processes: $MPI_NP"
echo "File $PBS_NODEFILE:"
cat $PBS_NODEFILE
echo

mpirun -machinefile $PBS_NODEFILE -np $MPI_NP ./mpi_slae_1
```
В этом скрипте:
```bash#PBS -l select=1:ncpus=1:mpiprocs=1``` - через двоеточия указаны количества узлов:ядер:процессов
Дальше нужно будет их менять для каждого теста. Количество узлов это вроде количество компов, на которых будет запускаться программа
У меня тесты такие: 
1:1:1
1:2:2
1:4:4
1:8:8
2:8:8
А еще нужно не забыть в последней строчке скрипта изменить mpi_slae_1 на mpi_slae_2 при тестировании второй программы!
10. То есть каждый раз перед запуском программы
```bash vim run.sh```нажимаем i, изменяем количество узлов/ядер/процессов, escape, :wq
Затем запускаем скрипт
```bash
qsub run.sh
```
Он выдаст что то типо 
```bash
7094693.vm-pbs
```
Потом можно командой ls проверить, что в папке появятся 2 файла - run.sh.o... и run.sh.e... (вместо ... вот эти цифры)
```bash
cat run.sh.o7094693
```
и смотрим вывод программы

# Профилирование

1. Скачиваем файл по ссылке
www.ccfit.nsu.ru/~kireev/mpe2_installed_nusc.tgz
2. Распаковываем архив
3. Копируем путь до папки mpe2-nusc-built
4. Локально на своем компьютере пишем
```bash
scp -r /Users/a1234/Downloads/mpe2-nusc-built hpcuserN@clu.nusc.ru:~/
```
5. Подключаемся к кластеру
6. Проверяем что папка скопирована:
```bash
ls ~/mpe2-nusc-built
```
7. Команда
```bash
vim ~/.bashrc
```
8. Нажимаем i, в конец файла вставляем
```bash
export MPE_HOME=$HOME/mpe2-nusc-built
export PATH=$PATH:$MPE_HOME/bin
jumpshot
module load jre/1.8.0escape, :wq.
```
9. Закрываем терминал и заходим заново на кластер.
10. Переходим в директорию, в которой хранятся файлы mpi_slae_1.c и mpi_slae_2.c
11. Команды для компиляции:
```bash
mpecc -mpilog -o mpi_slae_1 mpi_slae_1.c -lm
mpecc -mpilog -o mpi_slae_2 mpi_slae_2.c -lm
```
12.  Cкачиваем https://www.xquartz.org/
13. Устанавливаем и перезапускаем Mac! (но вообще, он вроде бы сам должен перезапуститься)
14. Локально в терминале команда:
```bash
ssh -X hpcuserN@clu.nusc.ru
```
15. После этого автоматически должна запуститься программа XQuartz
16. Команды для запуска:
```bash
mpirun -np 16 ./mpi_slae_1
mpirun -np 16 ./mpi_slae_2
```
17. После этого командой ls проверяем, что в нашей директории появились файлы mpi_slae_1.clog2 и mpi_slae_2.clog2
18. В окне программы XQuartz сверху нажимаем File -> Select -> lab1 -> mpi_slae_N.c -> Open, далее нас перебрасывает в новое окно, снизу нажимаем Convert, после конвертирования OK и у нас открывается окно с нашими графиками профилирования.
