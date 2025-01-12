#include "catalog.h"

void createCatalog(catalog k[]) { 
    for (int i = 0; i < PRODUCTS; i++) {
        k[i].code = i + 1; // kodikos proiontos
        sprintf(k[i].name, "Product %d", i + 1); // onoma proiontos
        k[i].price = rand() % 100 + 1; // tuxaia timh
        k[i].stock = 5; // arxiko apothema
    }
}

void handleCustomer(catalog k[], int fd1[], int fd2[], int* profit, int* successful, int* failed, int fail[][CUSTOMERS], int range[]) {
    close(fd1[1]); //kleinoume thn ejodo tou swlhna (egrafh)
    close(fd2[0]); //kleinoume thn eisodo tou swlhna (anagnwsh)

    for (int i = 0; i < ORDERS; i++) {
        int product; 
        char storage[130]; 
        read(fd1[0], &product, sizeof(int));
        int found = 0;

        for (int j = 0; j < PRODUCTS; j++) { // anazhthsh proiontos ston katalogo
            if (k[j].code == product) { 
                range[j]++; // afjish arithmou parageliwn
                if (k[j].stock > 0) { //an uparxei apothema
                    sprintf(storage, "Successful order %d for %s\n", j + 1, k[j].name);
                    write(fd2[1], storage, strlen(storage) + 1);
                    (*profit) += k[j].price;  // Auxhsh kerdous
                    (*successful)++; // Auxhsh epityximenwn paraggelion
                    k[j].stock--;  // Meiwnoume to stock tou proiontos
                } else {
                    fail[j][CUSTOMERS - 1]++; // Katagrafh apotixias gia ton sygkekrimeno pelath
                    (*failed)++; // Auxhsh apotiximenwn paraggeliwn
                    sprintf(storage, "Failed order %d for %s\n", j + 1, k[j].name);   // Apotiximenh paraggelia
                    write(fd2[1], storage, strlen(storage) + 1);  // Stelnoume apanthsh
                }
                found = 1; // Dhlwnoume oti vrethike to proion
                break;
            }
        }

        if (!found) {  // An den vrethike to proion
            sprintf(storage, "Invalid product code: %d\n", product); // Lathos kodikos proiontos
            write(fd2[1], storage, strlen(storage) + 1);
        }
    }

    close(fd1[0]);
    close(fd2[1]);
}

int main() {
    srand(time(NULL));
    catalog k[PRODUCTS]; // Pinakas katalogou proionton
    int fd1[2]; // Pipe fd1 gia epikoinwnia metaksy diergasiwn gia na stelnei paraggelia o pelaths
    int fd2[2]; // Pipe fd2 gia epikoinwnia metaksy diergasiwn gia na stelnei apanthsh
    int successful = 0, failed = 0, profit = 0;  // Metablhtes gia ta apotelesmata
    int fail[PRODUCTS][CUSTOMERS] = {0};  // Pinakas gia na apothikeuoume apotiximenes paraggelies
    int range[PRODUCTS] = {0};  // Pinakas gia thn syxnothta paraggeliwn ana proion

    for (int j = 0; j < CUSTOMERS; j++) {
        pipe(fd1); // stelnei o pelatis thn paragelia
        pipe(fd2); // erxetai ston pelath h apantish
        int id = fork();
        if (id == 0) { // thigatrikh diergasia
            close(fd1[0]);// kleinei to anoigma tou swlhna gia anagnwsh apo fd1 kai to anoigma tou swlhna gia egrafh se fd2
            close(fd2[1]);
            for (int i = 0; i < ORDERS; i++) {
                int product = rand() % PRODUCTS + 1;
                write(fd1[1], &product, sizeof(int)); // apostolh paragelias mesw tou fd1[1] 
                char response[130];  // o  pelaths diabazei thn apantish
                read(fd2[0], response, sizeof(response));
                printf("Customer %d received: %s", j + 1, response); // emfanish apantishs 
            }
            // Kleinei ta anoigmata twn swlhnwn meta thn oloklhrwsh ton parageliwn
            close(fd1[1]);
            close(fd2[0]);
            exit(0);// termatismos thigatrikhs diergasias
        } else { // Patrikh  diergasia
            if (j == 0) { // An einai o protos pelaths
                createCatalog(k); // dimiourgia katalogou proiontwn
            }
             // epejergasia paragelias 
            handleCustomer(k, fd1, fd2, &profit, &successful, &failed, fail, range);
            wait(NULL); // Perimenoume na teleiwsei to paidi
        }
    }

    printf("\n"); // Emfanisi apotelesmaton
    for (int i = 0; i < PRODUCTS; i++) {
        printf("%s was requested: %d times\n", k[i].name, range[i]);  // Plhrofories gia kathe proion
        printf("and was sold: %d times\n", 5 - k[i].stock);
        for (int j = 0; j < CUSTOMERS; j++) {
            if (fail[i][j] > 0) {
                printf("Customer %d failed %d times for %s\n", j + 1, fail[i][j], k[i].name); // Apotuxies pelatwn
            }
        }
        printf("----------------------------------------------\n");
    }
    printf("Total successful orders: %d\n", successful); // Synolikes epityxies
    printf("Total failed orders: %d\n", failed); // Synolikes apotyxies
    printf("Total profit: %d\n", profit); // Synolika kerdh
    printf("----------------------------------------------\n");

    return 0;
}