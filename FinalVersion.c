/*#define DEBUG*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DATABASE "AMS_Accounts"
#define MAX_ACCOUNTS 5
#define MAX_CHAR_LEN 10
#define MAX_SECQ_LEN 56
/*******************************************************************************
* Structures
*******************************************************************************/
struct account{
    char username[MAX_CHAR_LEN +1];
    char password[MAX_CHAR_LEN +1];
    char securityAnswer[MAX_SECQ_LEN +1];
    int key[1];

};
typedef struct account account_t;
/*******************************************************************************
 * Function prototypes
*******************************************************************************/
int initialiseProg(account_t amsAccounts[], int saved);
void testFunction(int amsSaved, account_t amsAccount[]);
void print_menu(void);
/*******************************************************************************
For Creation of new create_account
*******************************************************************************/
int create_account(account_t amsAccount[], int saved);
int obtainCred( account_t  const accounts[], char *tempCred, char *question);
void obtainKey(int tempKey[], char *tempUsername);
int obtainSecurityQ(char *tempAnswer);
void confirmDetails(char *tempUsername, char *tempPassword, char *tempAnswer);
void amsStore(char username[], char password[], char securityAnswer[],\
     account_t amsAccount[], int amsSaved);
int checkDup(char initial[],  account_t  const savedAccounts[]);
int verifyCredentials(char tempCredi[], char tempCredc[]);
void saveProg(account_t accounts[], int saved);
int backFunc(char array[]);
/*******************************************************************************
For logging in to existing account
******************************************************************************/
int loginMenu(account_t amsAccount[], int saved);

int enterUser(account_t amsAccount[], int saved);
void encryption(account_t amsAccount[], int saved);
void decryption(account_t amsAccount[], int saved);
int enterPass(account_t amsAccount[], int saved, int arrayLoc);

void forgetPassword(account_t amsAccount[], int saved);

/*******************************************************************************
Logged in functions
******************************************************************************/
void deleteAccount(account_t amsAccounts[], int saved, int loc );
void shuffleArray(account_t amsAccounts[], int saved, int loc);
void changePassword(account_t amsAccount[], int saved, int loc);
int successLogin(account_t amsAccounts[], int saved, int arrayLoc );
/*******************************************************************************
 * Main
*******************************************************************************/
int main(void)
{
    account_t amsAccount[MAX_ACCOUNTS];

    /* user input is saved in this variable */
    int input;
    /*Stores the amount of accounts already in memory */
    int amsSaved = 0;
    /*loads the database file into memory,and counts how many users are stored*/
    amsSaved = initialiseProg(amsAccount, amsSaved);

    #ifdef DEBUG
    printf("amsSaved after initialiseProg is %d\n", amsSaved);
    #endif

    while(1)
    {
        print_menu();
        scanf("%d", &input);

        switch(input)
        {
            case 1:
                /* when user inputs 1, check to see account limit isnt reached
                then create new account */
                if(amsSaved == MAX_ACCOUNTS){
                    printf("Limit Reached\n");
                    break;
                }

                #ifdef DEBUG
                printf("amsSaved before create account is %d\n", amsSaved);
                #endif

                /* starts the account creation shell */
                /* stores new amount of accounts into amsSaved */
                amsSaved = create_account(amsAccount, amsSaved);

                #ifdef DEBUG
                printf("amsSaved after create account is %d\n", amsSaved);
                #endif
            break;

            case 2:
            /* stores new amount of accounts into amsSaved */
                amsSaved = loginMenu(amsAccount, amsSaved);
            break;

            case 3:
                /* quits the program */
                printf("Exiting Program\n");
                return 0;
            break;
            #ifdef DEBUG
            case 4:
                /*Debug function that prints all usernames, passwords, answers*/
                    testFunction(amsSaved, amsAccount);
            break;
            #endif

            default:
            /* if none of the inputs expected are inputted, print this */
            printf("Invalid choice please choose again\n");
        }
    }
    return(0);
}

/*******************************************************************************
 * This function creates the database file, and loads any data already stored
 * inputs:
 * - none
 * outputs:
 * - the number of accounts already stored in file
 * AUTHOR: Howin Tam
 * EDITED: Johnson, Howin
*******************************************************************************/
int initialiseProg(account_t amsAccount[], int saved)
{
    FILE* fp = NULL;
    int i;
    int end = 1;
    int loaded = 0;
    /* Reads the database file. If it is not found, create one */
    fp = fopen(DATABASE, "r");
    if(fp == NULL){
        /* Create file if file is not there */
        fp = fopen(DATABASE, "w");

        #ifdef DEBUG
        printf("file made\n");
        #endif
    }
    else
    {
        while(end == 1)
        {
            for(i=0; i< MAX_ACCOUNTS; i++)
            {
                #ifdef DEBUG
                printf(" file read\n");
                #endif

                fscanf(fp, "%s", amsAccount[i].username);
            /*if End of File before ending another 4 values, stop and clear*/
                if(feof(fp))
                {
                    end = 0;
                    break;
                }

                fscanf(fp, "%s", amsAccount[i].password);
                fscanf(fp, "%s", amsAccount[i].securityAnswer);
                fscanf(fp, "%d", amsAccount[i].key);
                loaded++;
                if(feof(fp))
                {
                    end = 0;
                    break;
                }
            }
        }
    }
    fclose(fp);
    return(loaded);
}
/*******************************************************************************
 * This function prints the initial menu with all instructions on how to use
 * this program.
 * inputs:
 * - none
 * outputs:
 * - none
 * AUTHOR: Divya
*******************************************************************************/
void print_menu(void)
{
    printf("--------------------------------------------------------------\n");
    printf("Main Menu\n");
    printf("--------------------------------------------------------------\n");
    printf("1. Create new account.\n");
    printf("2. Login to existing account.\n");
    printf("3. Exit the program.\n");
    printf("Enter a number from 1-3\n>");
    #ifdef DEBUG
        printf("Testing\n");
    #endif
}

/*******************************************************************************
 * This function holds all the functions connected to this branch
 * inputs:
 *  account_t amsAccount
 *  int saved
 * outputs:
 * - Amount of accounts stored in program, amsSaved
 * Author: Divya
 * edited: Johnson, Howin
*******************************************************************************/
int create_account(account_t amsAccount[], int saved)
{
    /* Temporary arrays to store/compare user inputs */
    char tempUsername [MAX_CHAR_LEN + 1];
    char tempPassword [MAX_CHAR_LEN + 1];
    char tempAnswer [MAX_CHAR_LEN + 1];
    /* stores the question */
    char user[9] = {"username"};
    char pass[9] = {"password"};
    /* user input */
    int input;
    int check = 0;
    /* Repeats the function if account is not successfully created */
    int loop = 0;

    while(loop == 0)
    {
        printf("--------------------------------------------------------------\
        \n");
        /* Ask user for new username/password */
        check = obtainCred(amsAccount,tempUsername, user);
        if(check == 10){
            return(saved);
        }
        check = obtainCred(amsAccount,tempPassword, pass);
        if(check == 10){
            return(saved);
        }
        /* Program generates the key */
        obtainKey(amsAccount[saved].key,tempUsername);
        check  = obtainSecurityQ(tempAnswer);
        if(check == 10){
            return(saved);
        }
        confirmDetails(tempUsername, tempPassword, tempAnswer);
        scanf("%d", &input);
        if(input == 1){
            /*saves values from temp arrays into official array*/
            amsStore(tempUsername, tempPassword, tempAnswer, amsAccount, saved);
            /* encrypts the newest account created */
            encryption(amsAccount, saved);
            saved++;
            saveProg(amsAccount, saved);

            printf("Your account has been created!\n");
            #ifdef DEBUG
            printf("saved in create account is %d\n", saved);
            #endif
            return(saved);
        }
        else if(input == 2){
            loop = 0;
        }
        else {
            printf("Not an option\n");
        }
    }
    return(0);
}

/*******************************************************************************
 * This function promts user for the username of the account
 * inputs:
 * - account_t const amsAccount
 * - char *tempCred
 * - char *question
 * outputs:
 * - none
 * Author: Howin Tam
 * edited: Johnson, Howin Tam
*******************************************************************************/
int obtainCred( account_t const accounts[], char *tempCred, char *question)
{
    char initial[MAX_CHAR_LEN + 1];
    char compare[MAX_CHAR_LEN + 1];
    char duplicates[9] = {"username"};
    int end  = 0;
    int clear = 0;
    while(end == 0)
    {
        if(end == 0)
        {
            for(clear = 0; clear <= MAX_CHAR_LEN + 1; clear++)
            {
                initial[clear] = '\0';
                compare[clear] = '\0';
            }
        }
        printf ("Enter %s with a maximum of ten characters \n", question);
        printf("Type 'back' to exit account creation\n>");
        scanf ("%11s", initial );
        if( backFunc(initial) == 1){
            return(10);
        }
        while(getchar()!='\n');
        printf ("Enter %s again to confirm.\n", question);
        printf("Type 'back' to exit account creation\n>");
        scanf("%11s", compare);
        if( backFunc(compare) == 1){
            return(10);
        }
        while(getchar()!='\n');

        end = verifyCredentials(initial, compare);
        if(strcmp(duplicates, question) == 0 && end == 1)
        {
            end = checkDup(initial, accounts);
        }
    }
    strcpy(tempCred, initial);
    return(0);
}

/*******************************************************************************
 * This function generates a encryption key based on the number of characters
 * that the username contains, as well as a random number generator
 *
 * inputs
 * - int tempKey[]
 * - char* tempUsername
* outputs
* - none
* Author: Johnson
* Edited: Howin Tam. Johnson
*******************************************************************************/

void obtainKey(int tempKey[], char *tempUsername)
{
    int len;
    int num;
    len = strlen(tempUsername);
    num = rand() % 20;
    tempKey[0] = len + num;
    #ifdef DEBUG
    printf("rand = %d\n ", num);
    #endif

}
/*******************************************************************************
 * This function prompts user for an answer to the security question
 * inputs:
 * - none
 * outputs:
 * - none
 * AUTHOR: Johnson
 * Edited: Howin Tam
*******************************************************************************/
int obtainSecurityQ(char *tempAnswer)
{
    char initial[MAX_SECQ_LEN + 1];
    char compare[MAX_SECQ_LEN + 1];
    int end = 0;
    /* If input from user is incorrect, restart the function */
    while(end == 0){
        printf("Security Question - What country were you born in? ");
        printf("(Limited to a maximum of 56 characters)\n");
        printf("Type 'back' to exit account creation\n>");
        /* takes 57 characters as max input from user */
        scanf("%57[^\n]s", initial);
        if( backFunc(initial) == 1){
            return(10);
        }
        /* clears the stdin buffer so that it doesnt overflow to next scanf*/
        while(getchar()!='\n');
        printf("Enter your answer again to confirm.\n");
        printf("Type 'back' to exit account creation\n>");
        scanf("%57[^\n]s", compare);
        if( backFunc(compare) == 1){
            return(10);
        }
        while(getchar()!='\n');
        /* ensures input is valid, within 57 characters */
        if(strlen(initial) > 56 || strlen(initial) < 1)
        {
            printf("Input too long/short \n");
            end = 0;
        }
        /* if input is same, then save into correct account array */
        if(strcmp(initial, compare) == 0)
        {
            strcpy(tempAnswer, initial);
            end = 1;
        }
        else
        {
            end = 0;
            printf("Input is not the same.\n");
        }

    }
    return(0);
}
/*******************************************************************************
 * This function prompts user to confirm the account details
 * inputs:
 * - char *tempUsername
 * - char *tempPassword
 * - char *tempAnswer
 * outputs:
 * - none
 * Author: Johnson
 * edited: Howin, Aamir
*******************************************************************************/
void confirmDetails(char *tempUsername, char *tempPassword, char *tempAnswer)
{
    printf("--------------------------------------------------------------\n");
    printf("Please confirm the information below\n");
    printf("Username: %s\n", tempUsername);
    printf("Password: %s\n", tempPassword);
    printf("Security Answer: %s\n", tempAnswer);
    printf("Yes (1) or No (2)\n>");

}
/*******************************************************************************
 * This function checks whether the new username hasnt already been taken
 * inputs:
 * - char initial[]
 * - account_t const savedAccounts[]
 * outputs:
 * - int 1 or 0
 * Author: Howin Tam
 * edited: Johnson
*******************************************************************************/
int checkDup(char initial[],  account_t const savedAccounts[]){
    int i = 0;
    /* searches first - last account for same username */
    for(i=0; i< MAX_ACCOUNTS; i++)
    {
        /* if username is the same, tell user and ask for new input */
        if(strcmp(initial, savedAccounts[i].username) == 0)
        {
            printf("Username is already taken\n");
            return(0);
        }
    }
    return(1);
}
/******************************************************************************
* This function stores the values from the temporary arrays into the real array
* inputs:
* - char username[]
* - char password[]
* - char securityAnswer[]
* outputs:
* - none
* Author: Howin Tam
*******************************************************************************/
void amsStore(char username[], char password[], char securityAnswer[],\
    account_t amsAccount[], int amsSaved)
{
    /* copies info from temporary arrays into account array */
    strcpy(amsAccount[amsSaved].username, username);
    strcpy(amsAccount[amsSaved].password, password);
    strcpy(amsAccount[amsSaved].securityAnswer, securityAnswer);

}
/******************************************************************************
* This function ensures username/password that have been inputted are valid
* an follow the rules, checking if username is taken.
* inputs:
* - char tempUsername/userPassword
* - account_t amsAccount
* outputs:
* - none
* Author: Howin
* edited: Johnson
*******************************************************************************/
int verifyCredentials(char tempCredi[], char tempCredc[])
{
    /* if input is bigger/smaller than expected ask for new input */
    if(strlen(tempCredi) > 10 || strlen(tempCredi) < 1)
    {
        printf("Input is too long/short \n");
        return (0);
    }
    /* checks if answer + comparison is correct */
    if(strcmp(tempCredi, tempCredc) == 0)
    {
        return(1);
    } else {
        /* runs if answer is not the same */
        printf("Input is not the same\n");
        return (0);
    }
    return (0);
}
/******************************************************************************
* This function saves stored accounts into external binary file
* inputs:
* - account_t accounts[]
* - int saved
* outputs:
* - none
* Author: Howin Tam
* Edited: Johnson, Howin
*******************************************************************************/
void saveProg(account_t accounts[], int saved)
{
    FILE* fp = NULL;
    /* counter to track which account is being saved */
    int i;
    /* counter to track which character of cred is being saved */
    int j;
    /* saves password length */
    int passLen;
    fp = fopen(DATABASE, "w");
    /* runs the same amount as the number of accounts saved */
    for(i = 0; i < saved; i++)
    {
        for(j = 0; j <= 10; j++)
        {
            /* if character slot is null value, stop saving */
            if(accounts[i].username[j] == '\0')
            {

                #ifdef DEBUG
                printf("break\n");
                #endif
                break;
            }
            else
            {
                fprintf(fp,"%c", accounts[i].username[j]);
            }

        }
        fprintf(fp, " ");
        /* gets password length for [i] account */
        passLen = strlen(accounts[i].password);

        for(j =0; j <= passLen; j++)
        {
            /* if password slot is null value, stop saving */
            if(accounts[i].password[j] == '\0')
            {

                break;
            }
            else
            {
                fprintf(fp,"%c", accounts[i].password[j]);
                #ifdef DEBUG
                printf(" saved to file \n");
                #endif
            }

        }
        fprintf(fp, " ");
        for(j =0; j <= 10; j++)
        {
            /* if character slot is null value, stop saving */
            if(accounts[i].securityAnswer[j] == '\0')
            {
                break;
            }
            else
            {
            fprintf(fp,"%c", accounts[i].securityAnswer[j]);
            }
            #ifdef DEBUG
            printf(" filekey = %d \n", accounts[i].key[j] );
            #endif
        }
        fprintf(fp, " ");
        /* saves encryption key for the account */
        fprintf(fp,"%d", accounts[i].key[0]);
        fprintf(fp, " ");
    }
    #ifdef DEBUG
    printf("worked\n");
    #endif
    fclose(fp);
}
/******************************************************************************
* This function takes the encryption key from stored account and applies it
* to the password, encrypting it
* inputs:
* - account_t amsAccount[]
* - int saved
* outputs:
* - none
* AUTHOR: Johnson
* Edited: Howin
*******************************************************************************/
void encryption(account_t amsAccount[], int saved)
{
    /* counter to track position in password */
    int i;
    /* gets password length */
    int passLen = strlen(amsAccount[saved].password);

    for(i = 0; i < passLen; i++){
        /* Take the ASCII value of character i, then add encryption key
        value to it */
        amsAccount[saved].password[i] = amsAccount[saved].password[i] +
        amsAccount[saved].key[0];
        #ifdef DEBUG
        printf("amsAccount[saved] = %c\n", amsAccount[saved].password[i]);
        printf(" password encryption = %s\n", amsAccount[saved].password);
        #endif
        /* stops the function once all characters are encrypted */
        if(amsAccount[saved].password[i] == '\0')
        {
            break;
        }
    }

}
/*******************************************************************************
* This function takes the encryption key from stored account and removes it
* to the password
* inputs:
* - account_t amsAccount[]
* - int saved
* outputs:
* - none
* AUTHOR: Johnson
* Edited: Howin
*******************************************************************************/
void decryption(account_t amsAccount[], int saved)
{
    /* counter to track position in password */
    int i;
    /* gets password length */
    int passLen = strlen(amsAccount[saved].password);
    #ifdef DEBUG
    printf("passLen in decryption is %d\n", passLen);
    printf("saved in decryption is %d\n", saved);
    printf("%-10s\n", amsAccount[saved].password);
    #endif
    for(i=0; i<passLen; i++)
    {
        /* Take the ASCII value of character i, then removes encryption key
        value from it */
        amsAccount[saved].password[i] = amsAccount[saved].password[i] -
        amsAccount[saved].key[0];

    }
}

/******************************************************************************
* This function presents the menu for successfully logging into the account
* inputs:
* - account_t amsAccount[]
* - int saved
* outputs:
* - value of int saved
* AUTHOR: Aamir
* Edited: Johnson, Howin
*******************************************************************************/
int loginMenu(account_t amsAccount[], int saved)
{
    int input;
    int check = 0;

    while(1)
    {
        /* prints the menu for user */
        printf("--------------------------------------------------------------\
        \n");
        printf("Login Menu\n");
        printf("1. Login\n");
        printf("2. Forget Password\n");
        printf("3. Return to Main menu\n");
        #ifdef DEBUG
        printf("amsSaved in loginMenu is %d\n>", saved);
        #endif
        /* waits for user input */
        scanf("%d", &input);

        switch(input)
        {
            /* If user input is 1, ask for username and password */
           case 1:
               check = enterUser(amsAccount, saved);
               if(check == -1){
                   return(saved);
               } else {
                   check = saved;
                   return (saved);
               }
               #ifdef DEBUG
               printf("password selected %s\n", amsAccount[saved].password);
               printf("saved in login menu = %d\n", saved);
               #endif

           break;
           /* If user input is 1, ask securityanswer tied to specific account */
           case 2:
               forgetPassword(amsAccount, saved);
           break;
           /* goes to main menu */
           case 3:
               return (saved);
           break;

           default:
           printf("Invalid choice please choose again\n");
        }
    }
}
/******************************************************************************
* This function prompts user to enter a username checking to see if its valid
* inputs:
* - account_t amsAccount[], int saved
* outputs:
* - int saved
* AUTHOR: Johnson
* Edited: Howin
*******************************************************************************/
int enterUser(account_t amsAccount[], int saved)
{
    char compareUser[MAX_CHAR_LEN+1]; /* temp array to store user input */
    /* tracks which account is being checked */
    int i = 0;
    /*  variables to ensure repeating is present */
    int end = 0;
    int check = 0;
    while(end == 0)
    {
        #ifdef DEBUG
        printf("saved in enterUser is %d\n", saved);
        #endif
        printf("--------------------------------------------------------------\
        \n");
        printf("Enter your Username\n");
        printf("Type 'back' to exit account creation\n>");
        scanf("%s", compareUser);
        if( backFunc(compareUser) == 1){
            return(-1);
        }
        for(i=0; i<saved; i++)
        {
            /* if user input and stored username is same, ask for stored pass*/
            if(strcmp(compareUser, amsAccount[i].username) == 0)
            {
                /* stop repetition */
                end = 1;
                check = 1;
                decryption(amsAccount, i);
                #ifdef DEBUG
                printf("password selected %s\n", amsAccount[i].password);
                #endif
                check = enterPass(amsAccount, saved, i);
                if(check == -1){
                    return(saved);
                } else {
                    check = saved;
                    return (saved);
                }
                break;
            }
        }
        /* if no stored username is found, restart */
        if(check==0){
            printf("No matching username\n");
        }

    }

    return(saved);
}

/******************************************************************************
* This function prompts user to enter the password
* inputs:
* - account_t amsAccount[]
* - int savedAccounts
* - int arrayLoc
* outputs:
* - int saved
* AUTHOR: Johnson
* EDITOR: Howin
*******************************************************************************/
int enterPass(account_t amsAccount[], int saved, int arrayLoc)
{
    char comparePass[MAX_CHAR_LEN+1]; /* temp array to store user input */
    /*  variables to ensure repeating is present */
    int i = 0;
    int end = 0;
    int check = 0;

    while(end == 0)
    {
        #ifdef DEBUG
        printf("amsSaved in login is %d\n", saved);
        #endif
        printf("Enter your Password\n");
        printf("Type 'back' to exit account creation\n>");
        scanf("%s", comparePass);
        if( backFunc(comparePass) == 1){
            return(-1);
        }
        for(i=0; i<saved; i++)
        {
            /* if user input and stored password is same,run login menu */
            if(strcmp(comparePass, amsAccount[i].password) == 0)
            {
                saved = successLogin(amsAccount, saved, i);
                /* ends function */
                end = 1;
                check = 1;
                break;
            }
        }
        if(check==0){
            printf("No matching password\n");
        }
    }
    return(saved);
}
/******************************************************************************
* This function presents the menu after login is successful
* inputs:
* - account_t amsAccounts
* - int saved
* - int arrayLoc
* outputs:
* - none
* Author: Johnson
* Edited: Howin
*******************************************************************************/
int successLogin(account_t amsAccounts[], int saved, int arrayLoc)
{
    printf("--------------------------------------------------------------\n");
    printf("Successful login\n");
    #ifdef DEBUG
    printf("saved = %d\n", saved);
    #endif
    int input;
    /* re encrypts password for safety */
    encryption(amsAccounts, arrayLoc);
    while(1)
    {
        printf("1. Change Password\n2. Delete Account\n3. Logout \n>");
        scanf("%d", &input);

        switch(input)
        {
            case 1:
                /* changes password for selected acount */
                changePassword(amsAccounts, saved, arrayLoc);
            break;

            case 2:
                /* removes account from the database and memory */
                deleteAccount(amsAccounts, saved, arrayLoc);
                printf("Account Deleted\n");
                /* organises the array in memory so that there are no empty
                spaces */
                shuffleArray(amsAccounts, saved, arrayLoc);
                /* changes number of saved accounts to correct one */
                saved--;
                /* resave into file */
                saveProg(amsAccounts, saved);
                #ifdef DEBUG
                printf("amount Saved out = %d", saved);
                #endif
                return(saved);
            break;

            case 3:
                /* logout of account */
                return(saved);
            break;

            default:
                printf("Invalid choice please choose again\n");
        }
    }
    return(saved);
}
/******************************************************************************
* This function deletes the selected account from memory
* inputs:
* - account_t amsAccounts[]
* - int savedAccounts
* - int loc
* outputs:
* - none
* Author: Howin
*******************************************************************************/
void deleteAccount(account_t amsAccounts[], int saved, int loc)
{
    /* tracks what character the program is working on */
    int i;
    for(i=0;i< MAX_CHAR_LEN;i++)
    /* clears the array and fills it with null values */
    {
        amsAccounts[loc].username[i] = '\0';
        amsAccounts[loc].password[i] = '\0';
    }
    for(i=0;i< MAX_SECQ_LEN;i++)
    {
        amsAccounts[loc].securityAnswer[i] = '\0';
    }
    amsAccounts[loc].key[0] = '\0';
}
/******************************************************************************
* This function reorganises the array to ensure no empty space is present
* inputs:
* - account_t amsAccounts[]
* - int savedAccounts
* - int loc
* outputs:
* - none
* Author: Howin
*******************************************************************************/
void shuffleArray(account_t amsAccounts[], int saved, int loc)
{
    int i;
    /* only shuffles array if the one being deleted is not the last one */
    if (saved != loc)
    {
        /* shuffles everything after the one being deleted */
        for(i = loc ; i < saved - 1 ; i++)
        {
            /* copies array element +1 into the element before it */
            strcpy(amsAccounts[i].username, amsAccounts[i + 1].username);
            strcpy(amsAccounts[i].password, amsAccounts[i + 1].password);
            strcpy(amsAccounts[i].securityAnswer, amsAccounts[i + 1].securityAnswer);
            amsAccounts[i].key[0] = amsAccounts[i + 1].key[0];
            #ifdef DEBUG
            printf(" iuser = %s", amsAccounts[i].username);
            printf(" ipass = %s", amsAccounts[i].password);
            printf(" ise = %s", amsAccounts[i].securityAnswer);
            printf(" ikey = %d", amsAccounts[i].key[0]);
            #endif
        }
        /* Clears the last array so that it is ready for new account */
        for(i=0;i< MAX_CHAR_LEN;i++)
        {
            amsAccounts[saved].username[i] = '\0';
            amsAccounts[saved].password[i] = '\0';
        }
        for(i=0;i< MAX_SECQ_LEN;i++)
        {
            amsAccounts[saved].securityAnswer[i] = '\0';
        }
        amsAccounts[saved].key[0] = '\0';
    }
    else
    {
        return;
    }
}
/******************************************************************************
* This function allows user to change their password
* inputs:
* - account_t amsAccounts[]
* - int savedAccounts
* - int loc
* outputs:
* - none
*******************************************************************************/
void changePassword(account_t amsAccount[],int saved, int loc)
{
    int loop = 0;
    int check =0;
    int input;
    char tempPassword[MAX_CHAR_LEN + 1];
    char pass[9] = {"password"};
    printf("--------------------------------------------------------------\n");
    printf("Change Password\n");
    while(loop == 0)
    {
        #ifdef DEBUG
        printf("loc in changePassword is %d\n", loc);
        #endif
        /* requests new password from user */
        check  = obtainCred(amsAccount,tempPassword, pass);
        if(check == 10){
            return;
        }
        /* confirmation of new password */
        printf("Please confirm the new password\n");
        printf("Password: %s\n", tempPassword);
        printf("Yes (1) or No (2)\n>");
        scanf("%d", &input);
        if(input == 1)
        {
            /* saves password into offical array */
            strcpy(amsAccount[loc].password, tempPassword);
            /* encrypts new password */
            encryption(amsAccount, loc);
            /* updates database file */
            saveProg(amsAccount, saved);
            loop = 1;
        }
        else if(input == 2)
        {
            loop = 0;
        }
        else {
            printf("Not an option\n");
        }
    }
    return;
}
/******************************************************************************
* This function allows use to change password if they forget it
* inputs:
* - account_t amsAccount[]
* - int saved
* outputs:
* - none
Author: Johnson
Editor: Howin
*******************************************************************************/
void forgetPassword(account_t amsAccount[], int saved){
    int loop = 0;
    int i;
    char compareUser[MAX_CHAR_LEN+1];
    char tempAnswer[MAX_SECQ_LEN + 1];
    printf("--------------------------------------------------------------\n");
    printf("Forget Password\n");
    while(loop == 0)
    {
        printf("Enter your Username:\n");
        printf("Type 'back' to exit account creation\n>");
        scanf("%s", compareUser);
        if( backFunc(compareUser) == 1){
            return;
        }

        for(i=0; i<saved; i++)
        {
            /* searches for user input with stored usernames */
            /* if it is true, then it asks for security answer */
            if(strcmp(compareUser, amsAccount[i].username) == 0)
            {
                printf("Enter your security answer to this question\n");
                printf("What country were you born in?\n");
                printf("Type 'back' to exit account creation\n>");
                scanf("%57[^\n]s", tempAnswer);
                if( backFunc(tempAnswer) == 1){
                    return;
                }
                /* if true, change password */
                if(strcmp(tempAnswer, amsAccount[i].securityAnswer) == 0)
                {
                    changePassword(amsAccount, saved, i);
                    loop = 1;
                }
            } else {
                printf("That username does not exist\n");
            }
        }
    }

    return;
}
/******************************************************************************
* This function debugs to check all accounts are saved in ram properly
* inputs:
* - int amsSaved
* - account_t amsAccount[]
* outputs:
* - none
* AUTHOR: Johnson, Howin
*******************************************************************************/
#ifdef DEBUG
void testFunction(int amsSaved, account_t amsAccount[])
{
    int i;
    printf("saved in testFunction is %d\n", amsSaved);
    printf("Username  Password  Answer\n");
    for(i=0; i<amsSaved; i++){
        printf("%-9s %-9s %s\n", amsAccount[i].username, amsAccount[i].password,
         amsAccount[i].securityAnswer);
        /*printf("%s", amsAccount[i].password);*/
    }
}
#endif
/******************************************************************************
* This function allows the user to go back to menus
* inputs:
* - char array[]
* outputs:
* - int 0/1
* AUTHOR: Howin
*******************************************************************************/
int backFunc(char array[]){
    char back[4] ={"back"};
    if(strcmp(back, array) == 0){
        return(1);
    } else{
        return(0);
    }
}
