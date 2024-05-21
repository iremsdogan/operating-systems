#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/sysinfo.h>
#include <limits.h> 

#define MAX_SATIR 100 // Maksimum satır sayısı
#define MAX_ALAN 7 // Maksimum alan sayısı

typedef struct Islem {
    char * pid; // Process ID (string)
    int varisSuresi; // Varış Süresi (sn)
    int oncelik; // Öncelik
    int burstZamani; // Burst Süresi (sn)
    int ramKullanimi; // RAM Kullanımı (Mb)
    int cpuKullanimi; // CPU Kullanımı (% oran)
    int baslangicZamani; // Başlangıç Zamanı (sn)
    int bitisZamani; // Bitiş Zamanı (sn)
}
Islem;

int checkCPUResources(int usage) {

    printf("\n");
    FILE * fp;
    char path[1035];

    // Open the command for reading
    fp = popen("top -bn 1 | awk '/^%Cpu/ {print $2+$6}'", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return 1;
    }

    // Read the output
    fgets(path, sizeof(path) - 1, fp);
    printf("Total CPU Usage:%%%s", path);
    printf("Process CPU Usage:%%%d\n", usage);
    int cpuUsage = atoi(path); // Convert string to integer

    if ((100 - cpuUsage) < usage) {
        printf("Fail,Your cpu resources are not sufficient for this process.Process will not be eveluated!!!");
        return 0;
    } else {
        printf("Success,Your cpu resources are sufficient for this process.Process will be evalated!!!");
        return 1;
    }

    // Close the pipe
    pclose(fp);

}

int checkRAMResources(int usage) {

    printf("\n");

    struct sysinfo si;
    unsigned long long available_ram, total_ram;
    double available_ram_mb, total_ram_mb;

    sysinfo( & si);

    available_ram = si.freeram;
    available_ram_mb = available_ram / (1024 * 1024);

    total_ram = si.totalram;
    total_ram_mb = total_ram / (1024 * 1024);

    printf("Total RAM: %.2f MB\n", total_ram_mb);
    printf("Available RAM: %.2f MB\n", available_ram_mb);
    printf("Process RAM Usage: %d MB\n", usage);

    if (available_ram_mb - usage > 0) {
        printf("Success!!!,There is enough RAM memory in your system.Process will be put on RAM.\n");
        return 1;
    } else {
        printf("Fail!!!,There is not enough RAM memory in your system.Process will not be put on RAM.\n");
        return 0;
    }
}

int memAllacationCPU1() {
    size_t size512 = 512 * 1024 * 1024; // 512mB in bytes

    void * ptr1 = malloc(size512);

    if (ptr1 == NULL) {
        printf("512mb  memory allocation failed!\n");
        return 0;

    } else {
        printf("Memory Allacation is done successfully for CPU1 as 512 mb !!!\n");
        return 1;
    }

    free(ptr1);
}

int memAllacationCPU2() {

    size_t size512 = 512 * 1024 * 1024; // 512mB in bytes
    size_t size1536 = size512 * 3; // 1536mB in bytes

    void * ptr2 = malloc(size1536);
    if (ptr2 == NULL) {
        printf("1536mb memory allocation failed!\n\n");
        return 0;
    } else {

        printf("Memory Allacation is done successfully for CPU2 as 1536mb !!!\n\n");
        return 1;
    }

    free(ptr2);
}

void sjf_sort(Islem *islemler, int n) {
    Islem temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (islemler[i].oncelik == 1 && islemler[j].oncelik == 1 && islemler[j].burstZamani > islemler[j + 1].burstZamani) {
                temp = islemler[j];
                islemler[j] = islemler[j + 1];
                islemler[j + 1] = temp;
            }
        }
    }
}


int main(int argc, char * argv[]) {
    FILE * dosya;
    char satir[MAX_SATIR];
    char * parca;
    void * ptr;
    
    freopen("output.txt", "w", stdout);


    // Dizileri dinamik olarak oluşturma
    Islem * islemler = malloc(MAX_SATIR * sizeof(Islem));
    for (int i = 0; i < MAX_SATIR; i++) {
        islemler[i].pid = malloc(MAX_ALAN * sizeof(char));
    }

    int satirSayisi = 0; // Okunan satır sayısı
    char * dosyaAdi = argv[1];

    dosya = fopen(dosyaAdi, "r");
    if (dosya == NULL) {
        printf("Dosya açılamadı!\n");
        return 1;
    }

    while (fgets(satir, sizeof(satir), dosya)) {
        // Her satırı virgüllerle ayırma
        parca = strtok(satir, ",");

        // Her alanı ilgili yapı üyesine ekleme
        strcpy(islemler[satirSayisi].pid, parca);
        parca = strtok(NULL, ",");
        islemler[satirSayisi].varisSuresi = atoi(parca);
        parca = strtok(NULL, ",");
        islemler[satirSayisi].oncelik = atoi(parca);
        parca = strtok(NULL, ",");
        islemler[satirSayisi].burstZamani = atoi(parca);
        parca = strtok(NULL, ",");
        islemler[satirSayisi].ramKullanimi = atoi(parca);
        parca = strtok(NULL, ",");
        islemler[satirSayisi].cpuKullanimi = atoi(parca);

        satirSayisi++; // Okunan satır sayısını artırma
    }

    fclose(dosya);

    memAllacationCPU1();
    memAllacationCPU2();

    void processyazdir(int i) {
        // Belirli bir aralıkta olmayan indeksler kontrol ediliyor
        if (i < 0 || i >= MAX_SATIR) {
            return;
        }

        // İşlem için öncelik kontrolü
        if (islemler[i].oncelik == 0) {

            checkCPUResources(islemler[i].cpuKullanimi);
            checkRAMResources(islemler[i].ramKullanimi);
            printf("Process %s is queued to be assigned to CPU-1.\nProcess %s is assigned to CPU-1.\nProcess %s is completed and terminated.\n\n\n", islemler[i].pid, islemler[i].pid, islemler[i].pid);
        
        } else if (islemler[i].oncelik == 1) {
            checkCPUResources(islemler[i].cpuKullanimi);
            checkRAMResources(islemler[i].ramKullanimi);
            printf("Process %s is queued to be assigned to CPU-2.\nProcess %s is assigned to CPU-2.\nProcess %s is completed and terminated.\n\n\n", islemler[i].pid, islemler[i].pid, islemler[i].pid);
        
        } else if (islemler[i].oncelik == 2 || islemler[i].oncelik == 3) {
            checkCPUResources(islemler[i].cpuKullanimi);
            checkRAMResources(islemler[i].ramKullanimi);
            printf("Process %s is queued to be assigned to CPU-2.\nProcess %s is assigned to CPU-2.\n", islemler[i].pid, islemler[i].pid);
            
            // İşlemin burst süresine göre tamamlanıp tamamlanmadığı kontrol ediliyor
            if (islemler[i].burstZamani < 8) {
                printf("Process %s is completed and terminated.\n\n", islemler[i].pid);
            } else {
                printf("Process %s run until the defined quantum time and is queued again because the process is not completed.\n", islemler[i].pid);
                islemler[i].burstZamani -= (islemler[i].oncelik == 2) ? 8 : 16; // Önceliğe göre kalan burst süresi azaltılıyor
                processyazdir(i); // İşlem yeniden kuyruğa alınıyor
            }
        }
    }

    printf("\n\n");

    int * que1 = malloc(sizeof(int) * 15);
    int * que2 = malloc(sizeof(int) * 15);
    int * que3 = malloc(sizeof(int) * 15);
    int * que4 = malloc(sizeof(int) * 15); 
   
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    for (int i = 0; i < 25; i++) {

        if (islemler[i].oncelik == 0) {
            que1[a] = i + 1;
            a++;
        } else if (islemler[i].oncelik == 1) { 
            que2[b] = i + 1;                   
            b++;
        } else if (islemler[i].oncelik == 2) {
            que3[c] = i + 1;
            c++;
        } else if (islemler[i].oncelik == 3) {
            que4[d] = i + 1;
            d++;
        }
    }
    
   

    for (int i = 0; i < 15; i++) {
        processyazdir(que1[i] - 1);
    }
    for (int i = 0; i < 15; i++) {
        processyazdir(que2[i] -1);
    }
    

    for (int i = 0; i < 15; i++) {
        processyazdir(que3[i] - 1);
    }
    for (int i = 0; i < 15; i++) {
        processyazdir(que4[i] - 1);
    }


    // Close the file redirection
    fclose(stdout);

    // Revert stdout to the terminal
    freopen("/dev/tty", "w", stdout);

    FILE *output_file = fopen("output.txt", "a"); // Open file in append mode
    if (output_file == NULL) {
        printf("Error opening output file\n");
        return 1;
    }

    printf("\nCPU-1 que1(priority-0) (FCFS)=> ");
    fprintf(output_file, "\nCPU-1 que1(priority-0) (FCFS)=> ");
    for (int i = 0; i < satirSayisi; i++) {
        if (islemler[i].oncelik == 0) {
            printf("%s,", islemler[i].pid);          // Print to terminal
            fprintf(output_file, "%s,", islemler[i].pid); // Write to file
        }
    }
    
    // Sjf Algoritmasi uygulaniyor.
    sjf_sort(islemler, satirSayisi);
    
    // Repeat the same process for the other queues
    printf("\nCPU-2 que2(priority-1) (SJF)=> ");
    fprintf(output_file, "\nCPU-2 que2(priority-1) (SJF)=> ");
    for (int i = 0; i < satirSayisi; i++) {
        if (islemler[i].oncelik == 1) {
            printf("%s,", islemler[i].pid);          // Print to terminal            
            fprintf(output_file, "%s,", islemler[i].pid); // Write to file
        }
    }

    printf("\nCPU-2 que3(priority-2) (RR-q8)=> ");
    fprintf(output_file, "\nCPU-2 que3(priority-2) (RR-q8)=> ");
    for (int i = 0; i < satirSayisi; i++) {
        if (islemler[i].oncelik == 2) {
            printf("%s,", islemler[i].pid);          // Print to terminal
            fprintf(output_file, "%s,", islemler[i].pid); // Write to file
        }
    }

    printf("\nCPU-2 que4(priority-3) (RR-q16)=> ");
    fprintf(output_file, "\nCPU-2 que4(priority-3) (RR-q16)=> ");
    for (int i = 0; i < satirSayisi; i++) {
        if (islemler[i].oncelik == 3) {
            printf("%s,", islemler[i].pid);          // Print to terminal
            fprintf(output_file, "%s,", islemler[i].pid); // Write to file
        }
    }

    fclose(output_file); // Close the file
    printf("\n");



    // Dinamik olarak ayrılan belleği serbest bırakma
    for (int i = 0; i < MAX_SATIR; i++) {
        free(islemler[i].pid);
    }

    free(islemler);

    return 0;
}