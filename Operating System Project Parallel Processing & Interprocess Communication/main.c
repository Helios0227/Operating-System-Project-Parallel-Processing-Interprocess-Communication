// main.c
// Operating System ÖDEV2
// Created by Kaan on 22.12.2023.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define FILENAME "data.bin"

int main() {
    int pipefds[2]; // Boru hattı dosya tanımlayıcıları
    pid_t pid, parentPid; // Process ID
    int fd, num, squared; // Dosya tanımlayıcısı, okunan sayı ve karesi
    ssize_t bytesRead; // Okunan byte sayısı
    parentPid = getpid();

    // Pipe oluştur, hata kontrolü yap
    if (pipe(pipefds) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    // Dosyayı aç, hata kontrolü yap
    fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        close(pipefds[0]);
        close(pipefds[1]);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 3; ++i) { // (2^^3)-1 = 7 child process oluştur
        pid = fork();
        if (pid == 0) { // Child process
            bytesRead = read(fd, &num, sizeof(int));
            squared = num * num;
            write(pipefds[1], &squared, sizeof(int));
        }
    }

    // Parent process: child process'leri bekle
    if (getpid() == parentPid) {
        for (int i = 0; i < 3; ++i) {
            int status;
            wait(&status);
        }

        close(fd);
        close(pipefds[1]);

        int total = 0, value;
        while (read(pipefds[0], &value, sizeof(int)) > 0) {
            total += value;
        }

        printf("Total sum: %d\n", total);
        close(pipefds[0]);
    }

    return EXIT_SUCCESS;
}
