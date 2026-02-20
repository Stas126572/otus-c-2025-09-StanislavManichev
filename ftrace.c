/*
 * ДЕМОНСТРАЦИЯ ТРЕССИРОВКИ СИСТЕМНЫХ ВЫЗОВОВ С ПОМОЩЬЮ FTRACE
 * Полностью рабочий пример для Ubuntu 22.04
 * 
 * КАК ИСПОЛЬЗОВАТЬ:
 * 1. Скомпилируйте: gcc -o ftrace_demo ftrace_demo.c
 * 2. Следуйте пошаговым инструкциям ниже
 * 3. Запустите в ДВУХ терминалах одновременно
 */

/*
=== ПОШАГОВАЯ ИНСТРУКЦИЯ ===

ТЕРМИНАЛ 1 (пользовательский терминал):
1. ./ftrace_demo
   # Программа покажет PID и будет ждать нажатия Enter
   # СКОПИРУЙТЕ ЭТОТ НОМЕР PID — он понадобится во втором терминале!

ТЕРМИНАЛ 2 (терминал с правами root — выполните эти команды):
2. sudo -i
3. cd /sys/kernel/debug/tracing
4. echo 0 > tracing_on                    # Остановить любую существующую трассировку
5. echo > trace                           # Очистить буфер трассировки
6. echo function > current_tracer         # Использовать функциональный трассировщик (наиболее надёжный)
7. echo '__x64_sys_*' > set_ftrace_filter # Фильтровать только системные вызовы
8. echo ВАШ_PID_ЗДЕСЬ > set_ftrace_pid    # Замените на PID из терминала 1
9. echo 1 > tracing_on                    # Запустить трассировку

ТЕРМИНАЛ 1 (вернитесь в пользовательский терминал):
10. Нажмите Enter в программе
    # Программа выполнит операции и покажет системные вызовы

ТЕРМИНАЛ 2 (вернитесь в терминал root):
11. echo 0 > tracing_on                   # Остановить трассировку
12. cat trace > /tmp/ftrace_output.txt    # Сохранить результаты
13. echo > set_ftrace_pid                 # Очистить фильтр по PID
14. less /tmp/ftrace_output.txt           # Просмотреть результаты

УСТРАНЕНИЕ НЕПОЛАДОК:
- Если трассировка пуста: проверьте 'cat available_filter_functions | grep sys_'
- Для ARM64: используйте '__arm64_sys_*' вместо '__x64_sys_*'
- Команды ftrace обязательно нужно запускать от root!
- Время критично — установите фильтр по PID ДО нажатия Enter!
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    printf("=== FTRACE SYSCALL DEMO ===\n");
    printf("PID: %d\n", getpid());
    printf("\nCOPY THE PID NUMBER ABOVE!\n");
    printf("Set up ftrace in another terminal, then press Enter...\n");
    printf("Waiting for Enter: ");
    fflush(stdout);
    
    getchar();  // Wait for user to set up ftrace
    
    printf("\n=== SYSCALL OPERATIONS START ===\n");
    
    // SYSCALL 1: openat() - open/create file
    printf("1. Opening file (openat syscall)...\n");
    int fd = open("/tmp/test_ftrace.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); //_x64_sys_openat, analog of old open function, can used not absolute 
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    printf("   File opened, fd = %d\n", fd);
    
    // SYSCALL 2: write() - write data to file
    printf("2. Writing to file (write syscall)...\n");
    const char *message = "Hello from ftrace syscall demo!\n";
    ssize_t bytes_written = write(fd, message, strlen(message));
    printf("   Written %ld bytes\n", bytes_written);
    
    // SYSCALL 3: newfstat() - get file information
    printf("3. Getting file info (newfstat syscall)...\n");
    struct stat file_stat;
    if (fstat(fd, &file_stat) == 0) {
        printf("   File size: %ld bytes\n", file_stat.st_size);
        printf("   File permissions: %o\n", file_stat.st_mode & 0777);
    }
    
    // SYSCALL 4: close() - close file descriptor
    printf("4. Closing file (close syscall)...\n");
    close(fd);
    printf("   File closed\n");
    
    // SYSCALL 5: unlink() - delete file
    printf("5. Deleting file (unlink syscall)...\n");
    if (unlink("/tmp/test_ftrace.txt") == 0) {
        printf("   File deleted\n");
    } else {
        perror("unlink failed");
    }
    
    printf("\n=== SYSCALL OPERATIONS END ===\n");
    printf("Check /tmp/ftrace_output.txt for syscall traces!\n");
    
    return 0;
}

/*
EXPECTED OUTPUT in /tmp/ftrace_output.txt:
- Lines starting with your PID
- Function calls like __x64_sys_openat, __x64_sys_write, etc.
- Entry and exit points for each syscall
- Timing information for each operation

WHAT YOU'LL LEARN:
- How userspace programs trigger kernel syscalls
- The actual kernel function names for syscalls
- Timing and execution flow of system calls
- How ftrace can monitor kernel execution in real-time
*/
