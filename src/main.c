#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_PATH "data/data.csv"

// define CLEAR_CMD at compile time depending on platform
#if defined(_WIN32) || defined(__MINGW32__)
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

typedef struct {
  int reg;
  int inter;
  int hds;
  int rc;
} Count;

typedef struct {
  char id[12];
  char name[55];
  char nick[21];
  int course;
  char email[61];
  char phone[11];
} Student;

typedef struct {
  int id;
  int name;
  int email;
} CheckDuplicateResponse;


/**
 * FUNTCION: countDataLine - count each line of data file
 *
 * - int *len: pointer to integer to store line count
 *
 *  explanation: 
 *  getc() function get next character from the stream. so the while loop essentialy go through each character one by one, counting '\n'
 */
void countDataLine(int *len) {
  *len = 0;
  char c;
  FILE *f = fopen(DATA_PATH, "r");

  /** if pointer to file is null, return -1 which signifies that the data file can't be open */
  if (f == NULL) {
    printf("[ERR] Could not open file %s\n", DATA_PATH);
    *len = -1;
    return;
  }

  /** loop through each characters in data file to count newline character */
  while ((c = getc(f)) != EOF) {
    if (c == '\n')
      (*len)++;
  }
  fclose(f);
}

/**
 * FUNTCION: getCourseName - convert course value in data file from int to its name
 *
 * - char name[]: pointer to character array to store course name value
 * - int course: course value
 *
 */
void getCourseName(char name[], int course) {
  if (course == 0)
    strcpy(name, "REG");
  else if (course == 1)
    strcpy(name, "INTER");
  else if (course == 2)
    strcpy(name, "HDS");
  else if (course == 3)
    strcpy(name, "RC");
}

/**
 * FUNTCION: printResHeader - print header for search result
 *
 * EXPLAINATION:
 * explanation for format string: %-[num]
 *  - : left-align
 *  num: amount of characters for that slot
 */
void printResHeader() {
  printf("%-15s %-32s %-15s %-10s %-34s %-15s\n", "ID", "FULLNAME", "NICK",
         "COURSE", "EMAIL", "PHONE");
}

/**
 * FUNTCION: printSearchResultLine - print a line of each result
 *
 * - Student cur: 1 student structure
 *
 * EXPLAINATION:
 *  explanation for format string: %-[num]
 *  - : left-align
 *  num: amount of characters for that slot
 */
void printSearchResultLine(Student cur) {
  char courseName[6];
  getCourseName(courseName, cur.course);
  printf("%-15s %-32s %-15s %-10s %-34s %-15s\n", cur.id, cur.name, cur.nick,
         courseName, cur.email, cur.phone);
}

/**
 * FUNCTION: removeTrailingNewLine - remove '\n' from data line
 * 
 *  - char line[]: pointer to data line string
 */
void removeTrailingNewline(char line[]) {
  if (line[strlen(line) - 1] == '\n')
    line[strlen(line) - 1] = '\0';
}

/**
 * FUNTCION: removeElementFromArray - as the name suggested, remove specific element from array
 * 
 * - Student *arr: array of Students
 * - int *len: pointer to data line count value
 * - int idx: index of element to be removed
 * 
 * EXPLAINATION:
 * this function remove element from a Student array at the index provided. it work by
 * shifting the next element from idx+1 to len-1 by -1
 */
void removeElementFromArray(Student *arr, int *len, int idx) {
  if (idx >= 0 && idx < *len) {
    for (int i = idx; i < (*len) - 1; i++)
      arr[i] = arr[i + 1];
    (*len)--;
  }
}

/**
 * FUNTCION: insertionSort - also as the name suggested, insertion sort algorithm
 * 
 * - Student *arr: array of Students
 * - int n: array length
 * 
 * EXPLAINATION:
 * insertion sort works by repeatedly taking an unsorted element 
 * and inserting it into its correct position among the previously sorted elements, 
 * shifting other elements as needed. 
 */
void insertionSort(Student *arr, int n) {
  for (int j = 1; j < n; j++) {
    Student temp = arr[j];
    int i = j - 1;
    while (i >= 0 && strcmp(arr[i].id, temp.id) > 0) {
      arr[i + 1] = arr[i];
      i--;
    }
    arr[i + 1] = temp;
  }
}

/**
 * FUNTCION: sortDataFile - open data file and sort it
 * 
 * - int len: data line count value
 * 
 * EXPLAINATION:
 * reading data file into variable, sort it using insertionSort() order by id, 
 * then write it back to data file
 */
int sortDataFile(int len) {
  /**
   * declare line to store each line from data file, also declare i to 
   * use as an iterator
   */
  char line[256];
  int i = 0;

  /**
   * allocate memory for an array of Students and declare file pointer.
   * if the file doesn't exist, return out of this function
   */
  Student *d = calloc(len, sizeof(Student));
  FILE *f = fopen(DATA_PATH, "r");
  if (f == NULL) {
    printf("[ERR] Cannot read %s while sorting.\n", DATA_PATH);
    return 1;
  }

  /**
   * read data file into student array *d
   */
  while (fgets(line, sizeof(line), f)) {
    removeTrailingNewline(line);
    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", d[i].id, d[i].name,
               d[i].nick, &d[i].course, d[i].email, d[i].phone) == 6) {
      i++;
    }
  }
  fclose(f);

  /** 
   * sort the array with insertion sort then write the sorted data
   * back to the data file 
   */
  insertionSort(d, i);
  char *buffer = malloc(170 * i);
  f = fopen(DATA_PATH, "w");
  for (int j = 0; j < len; j++) {
    fprintf(f, "%s,%s,%s,%d,%s,%s\n", d[j].id, d[j].name, d[j].nick,
            d[j].course, d[j].email, d[j].phone);
  }
  fclose(f);
  free(d);
  return 0;
}

/**
 * FUNTCION: searchById() 
 * COMMAND: search student by id
 * 
 * EXPLAINATION:
 * prompt user for id to query, then do a linear search 
 * through the data file, then print search result 
 */
void searchById() {
  /**
   * declare inp as input buffer variable and m as a
   * match counter, then prompt user for id (or partial id) 
   * to query 
   */
  char inp[20];
  int m = 0;
  system(CLEAR_CMD);
  printf("==============Search by ID==============\n");
  printf("ID: ");
  scanf("%s", inp);

  /**
   * determine if user's input is a full id or partial id
   * 
   * full id: 11 digits
   * partial id: 4 digits
   * 
   * if user's input doesn't match either of the above, 
   * print "Invalid ID" and return to main menu
   */
  int inplen = strlen(inp);
  if (inplen != 11 && inplen != 4) {
    printf("Invalid ID!\n");
  } 
  else {
    /**
     * open data file and declare a buffer variable for
     * 1 line of data, declare a student object to store
     * student's data from the current line being read
     */
    FILE *f = fopen(DATA_PATH, "r");
    char line[256];
    Student cur = {0};
    printf("Results: \n");

    /**
     * read through the file line by line, parse the data
     * into `cur` object. if cur.id match user input, print
     * the result
     */
    while (fgets(line, sizeof(line), f)) {
      removeTrailingNewline(line);
      if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", cur.id, cur.name,
                 cur.nick, &cur.course, cur.email, cur.phone) == 6) {
        /**
         * for partial id: substring cur.id to last 4 digits in subId
         */
        if (inplen == 4) {
          char subId[5] = {cur.id[7], cur.id[8], cur.id[9], cur.id[10]};
          if (strcmp(subId, inp) == 0) {
            if (m == 0)
              printResHeader();
            printSearchResultLine(cur);
            m++;
          }
        } 
        else {
          if (strcmp(cur.id, inp) == 0) {
            if (m == 0)
              printResHeader();
            printSearchResultLine(cur);
            m++;
          }
        }
      }
    }
    fclose(f);
    printf("Total match: %d\n", m);
    printf("========================================\n");
  }
}

/**
 * FUNTCION: searchByFirstName() 
 * COMMAND: search student by firstname
 * 
 * EXPLAINATION:
 * prompt user for firstname (or partial firstname) to query, 
 * then do a linear search through the data file, then print search result 
 */
void searchByFirstName() {
  /**
   * declare inp as input buffer variable and m as a
   * match counter, then prompt user for first (or partial firstname) 
   * to query 
   */
  char inp[20];
  int m = 0, inplen = 0;
  system(CLEAR_CMD);
  printf("===========Search by Firstname==========\n");
  printf("Name: ");
  scanf("%s", inp);

  /**
   * get user's query length and shift any lowercase
   * character to uppercase
   */
  inplen = strlen(inp);
  for (int j = 0; j < inplen; j++) {
    if (inp[j] >= 'a' && inp[j] <= 'z')
      inp[j] -= 32;
  }

  /**
    * open data file and declare a buffer variable for
    * 1 line of data, declare a student object to store
    * student's data from the current line being read
    */
  FILE *f = fopen(DATA_PATH, "r");
  char line[256], fnm[16];
  Student cur = {0};
  printf("Results: \n");

  /**
    * read through the file line by line, parse the data
    * into `cur` object. then substring cur.name into the
    * same length as user's query to allow partial name search
    * (subStr). if user's query match subStr, print that student
    * to result. since we allow partial name search, multiple 
    * matches are possible.
    */
  while (fgets(line, sizeof(line), f)) {
    removeTrailingNewline(line);
    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", cur.id, cur.name,
               cur.nick, &cur.course, cur.email, cur.phone) == 6) {
      if (sscanf(cur.name, "%s %*s", fnm) == 1) {
        char *subStr = calloc(inplen, sizeof(char)); // use calloc to allocate a string with inplen size
        for (int j = 0; j < inplen; j++) {
          subStr[j] = fnm[j];
        }
        subStr[inplen] = '\0';
        if (strcmp(subStr, inp) == 0) {
          if (m == 0)
            printResHeader();
          printSearchResultLine(cur);
          m++;
        }
      }
    }
  }
  fclose(f);
  printf("Total match: %d\n", m);
  printf("========================================\n");
}

/**
 * FUNCTION: searchByNickName
 * COMMAND: search for student(s) by nickname
 * 
 * EXPLAINATION:
 * prompt user for nickname (or partial nickname) to query, 
 * then do a linear search through the data file, then print search result 
 */
void searchByNickName() {
  /**
   * declare inp as input buffer variable, line as a buffer for 
   * data file reading, and m as a match counter, then prompt user 
   * for nickname (or partial nickname) to query 
   */
  char inp[20], line[256];
  int m = 0, inplen;
  system(CLEAR_CMD);
  printf("=============Search by Nick=============\n");
  printf("Nickname: ");
  scanf("%s", inp);

  /**
   * get user's query length and shift any lowercase
   * character to uppercase
   */
  inplen = strlen(inp);
  for (int j = 0; j < inplen; j++) {
    if (inp[j] >= 'a' && inp[j] <= 'z')
      inp[j] -= 32;
  }

  /**
    * open data file and declare a buffer variable for
    * 1 line of data, declare a student object to store
    * student's data from the current line being read
    */
  FILE *f = fopen(DATA_PATH, "r");
  Student cur = {0};
  printf("Results: \n");

  /**
    * read through the file line by line, parse the data
    * into `cur` object. then substring cur.nickname into the
    * same length as user's query to allow partial name search
    * (subStr). if user's query match subStr, print that student
    * to result. since we allow partial name search, multiple 
    * matches are possible.
    */
  while (fgets(line, sizeof(line), f)) {
    removeTrailingNewline(line);
    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", cur.id, cur.name,
               cur.nick, &cur.course, cur.email, cur.phone) == 6) {
      char *subStr = calloc(inplen, sizeof(char)); // use calloc to allocate a string with inplen size
      for (int j = 0; j < inplen; j++) {
        subStr[j] = cur.nick[j];
      }
      subStr[inplen] = '\0';
      if (strcmp(subStr, inp) == 0) {
        if (m == 0)
          printResHeader();
        printSearchResultLine(cur);
        m++;
      }
    }
  }
  fclose(f);
  printf("Total match: %d\n", m);
  printf("========================================\n");
}

/**
 * FUNCTION: allStdCount
 * COMMAND: print student count
 * 
 * EXPLAINATION:
 * count all students in data file then print it.
 * show all sum value and student count in each course
 */
void allStdCount() {
  /**
   * open data file and declare a Count object to store
   * data. declare cur as an iterator and line as a buffer
   * for reading each line of data file
   */
  FILE *f = fopen(DATA_PATH, "r");
  Count c = {0};
  int cur = 0;
  char line[256];

  /**
   * read through each line of data file and count each courses'
   * student using `COURSE` column in the data file
   * 
   * 0: Regular program
   * 1: International program
   * 2: Health Data Science program
   * 3: Residential College program
   * 
   * then print the result down below
   */
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%*[^,],%*[^,],%*[^,],%d", &cur) == 1) {
      if (cur == 0)
        c.reg++;
      else if (cur == 1)
        c.inter++;
      else if (cur == 2)
        c.hds++;
      else if (cur == 3)
        c.rc++;
    }
  }
  fclose(f);
  system(CLEAR_CMD);
  printf("=================Count==================\n");
  printf("All: %d\n", c.reg + c.inter + c.hds + c.rc);
  printf("Reg: %d \t Inter: %d\n", c.reg, c.inter);
  printf("HDS: %d \t RC: %d\n", c.hds, c.rc);
  printf("========================================\n");
}

/**
 * FUNCTION: checkduplicate - for addStd()
 * 
 * - Student x: student data to check
 * - int len: data file length
 * 
 * EXPLAINATION:
 * check if student (or some of their data) already existed
 * in the data file.
 * 
 * checkDuplicate() is used in the addStd() function to check,
 * if the student user currently adding already existed in
 * the data file. checkDuplicate() focuses on 3 values: id, email, name.
 * it counts for the occurence of these 3 values in the data 
 * file and return it back to addStd() where it determines if 
 * the duplication is acceptable.
 */
CheckDuplicateResponse checkDuplicate(Student x, int len) {
  /**
   * declaure CheckDuplicateResponse object to store results,
   * line to store each line from data file, and cur to store
   * data of student from current line being read. then we
   * search through the file for duplicate and if found,
   * increment value in result object by 1
   */
  CheckDuplicateResponse r = {0};
  char line[256];
  FILE *f = fopen(DATA_PATH, "r");
  Student cur = {0};
  while (fgets(line, sizeof(line), f)) {
    removeTrailingNewline(line);
    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", cur.id, cur.name,
               cur.nick, &cur.course, cur.email, cur.phone) == 6) {
      if (strcmp(cur.id, x.id) == 0) {
        r.id++;
      }

      if (strcmp(cur.name, x.name) == 0) {
        r.name++;
      }

      if (strcmp(cur.email, x.email) == 0) {
        r.email++;
      }
    }
  }
  fclose(f);
  return r;
}

/**
 * FUNCTION: addStd
 * COMMAND: add student to data file
 * 
 * - int *len: pointer to data file line count
 * 
 * EXPLAINATION:
 * add one student to the data file. will prompt user for 
 * each data required and check for data validity
 */
int addStd(int *len) {
  Student *x = malloc(sizeof(Student));
  char buffer[255];
  int fd = 0, d = 0;
  system(CLEAR_CMD);
  printf("===============Add Student==============\n");

  // getting input for student id
  printf("Student's id (11 digits): ");
  scanf("%s", buffer);
  if (buffer[11] == '\n')
    buffer[11] == '\0';
  if (strlen(buffer) != 11) {
    printf("Invalid id! returning to main menu.\n");
    free(x);
    return 1;
  }
  strcpy(x->id, buffer);
  // endsection

  // getting input for student firstname
  printf("Student's firstname: ");
  scanf("%s", buffer);
  if (strlen(buffer) > 20) {
    buffer[20] = '\0';
  }
  char *fnm = calloc(21, sizeof(char));
  strcpy(fnm, buffer);
  // endsection

  // getting input for student lastname
  printf("Student's lastname: ");
  scanf("%s", buffer);
  if (strlen(buffer) > 30) {
    buffer[30] = '\0';
  }
  char *lnm = calloc(31, sizeof(char));
  strcpy(lnm, buffer);
  // endsection

  // shift lowercase to uppercase
  for (int j = 0; j < strlen(fnm); j++) {
    if (fnm[j] >= 'a' && fnm[j] <= 'z')
      fnm[j] -= 32;
  }
  for (int j = 0; j < strlen(lnm); j++) {
    if (lnm[j] >= 'a' && lnm[j] <= 'z')
      lnm[j] -= 32;
  }
  // endsection

  // concat firstname and lastname and copy to object, then free fnm and lnm
  sprintf(x->name, "%s %s", fnm, lnm);
  free(fnm);
  free(lnm);
  // endsection

  // getting input for student nickname
  printf("Student's nickname: ");
  scanf("%s", buffer);
  if (strlen(buffer) > 10) {
    buffer[10] = '\0';
  }
  for (int j = 0; j < strlen(buffer); j++) {
    if (buffer[j] >= 'a' && buffer[j] <= 'z')
      buffer[j] -= 32;
  }
  strcpy(x->nick, buffer);
  // endsection

  // getting input for student course
  printf("Student's course (0 for REG, 1 for INTER, 2 for HDS, 3 for RC): ");
  scanf("%d", &x->course);
  if (x->course < 0 || x->course > 3) {
    printf("Invalid course! returning to main menu.\n");
    free(x);
    return 1;
  }
  // endsection

  // getting input for student course
  printf("Student's email: ");
  scanf("%s", buffer);
  int at = 0;
  for (int i = 0; i < strlen(buffer); i++) {
    if (buffer[i] == '@')
      at++;
  }
  if (buffer[strlen(buffer) - 1] == '\n')
    buffer[strlen(buffer) - 1] = '\0';
  if (at != 1 || strlen(buffer) > 60) {
    printf("Invalid email! returning to main menu.\n");
    free(x);
    return 1;
  }
  strcpy(x->email, buffer);
  // endsection

  // getting input for student phone number
  printf("Student's Thai phone number: ");
  scanf("%s", buffer);
  if (buffer[strlen(buffer) - 1] == '\n')
    buffer[strlen(buffer) - 1] = '\0';
  if (strlen(buffer) != 10) {
    printf("Invalid phone number! returning to main menu.\n");
    free(x);
    return 1;
  }
  strcpy(x->phone, buffer);
  // endsection

  // data duplication checking
  // fd means fatal duplication: will not allow user to proceed
  // d means duplication: will prompt user for confirmation before proceeding
  CheckDuplicateResponse dr = checkDuplicate(*x, *len);
  if (dr.id == 1) {
    fd = 1;
    printf("[ERR] %s already exist in the database. Cancelling...\n", x->id);
  }

  if (dr.name == 1) {
    d = 1;
    printf("[WARN] %s already exist in the database\n", x->name);
  }

  if (dr.email == 1) {
    fd = 1;
    printf("[ERR] %s already exist in the database. Cancelling...\n", x->email);
  }

  if (fd > 0) {
    free(x);
    return 1;
  }

  if (d > 0) {
    printf("Do you really want to proceed? (y/N): ");
    scanf("%s", buffer);
  }

  if ((d > 0) && (buffer[0] != 'y' && buffer[0] != 'Y')) {
    printf("Action cancelled. sending you back to main menu...\n");
    free(x);
    return 2;
  }
  // endsection

  // data preview and writing
  printf("Review the student data below:\n");
  printResHeader();
  printSearchResultLine(*x);
  printf("Do you want to proceed? (y/N): ");
  scanf("%s", buffer);
  if (buffer[0] == 'y' || buffer[0] == 'Y') {
    FILE *f = fopen(DATA_PATH, "a");
    if (f == NULL) {
      printf(
          "[ERR] Cannot open %s . Cancelling and returning to main menu...\n",
          DATA_PATH);
      free(x);
      return 1;
    }
    fprintf(f, "%s,%s,%s,%d,%s,%s\n", x->id, x->name, x->nick, x->course,
            x->email, x->phone);
    fclose(f);
    printf("%s has been added to the data file.\n", x->name);
    printf("%s written succesfully!\n", DATA_PATH);
    (*len)++;
    printf("Sorting data. Please wait!\n");
    int ss = sortDataFile(*len);
    if (ss == 1) {
      printf("Data sorting failed!\n");
      free(x);
      return 2;
    } else {
      printf("Data sorted successfully, returning to main menu...\n");
    }
  } else {
    printf("Action cancelled. sending you back to main menu...\n");
    free(x);
    return 2;
  }
  // endsection

  free(x);
  printf("========================================\n");
  return 0;
}

/**
 * FUNCTION: remStd
 * COMMAND: remove student from data file
 * 
 * - int *len: pointer to data file line count
 * 
 * EXPLAINATION:
 * remove one student from data file. will prompt user for
 * student id (or shorthand id) of the student to be removed
 */
int remStd(int *len) {
  /**
   * declare input buffer and prompt user for
   * id (or partial id) to remove
   */
  char inp[20];
  system(CLEAR_CMD);
  printf("==============Remove Student============\n");
  printf("ID (x to cancel): ");
  scanf("%s", inp);
  int inplen = strlen(inp), idx = -1;

  // check for exit command in user input;
  if ((inp[0] == 'x' || inp[0] == 'X') && inplen == 1) {
    printf("Action cancelled. sending you back to main menu...\n");
    printf("========================================\n");
    return 2;
  }

  /**
   * if user's input doesn't match id or partial id
   * length, print "Invalid ID" and return to
   * main meny
   */
  if (inplen != 11 && inplen != 4) {
    printf("Invalid ID!\n");
    printf("========================================\n");
    return 1;
  }

  /**
   * open data file and read the entire file into
   * Student object array.
   */
  FILE *f = fopen(DATA_PATH, "r");
  char line[256];
  int i = 0, fnd = 0;
  Student *d = calloc(*len, sizeof(Student));
  while (fgets(line, sizeof(line), f)) {
    removeTrailingNewline(line);
    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", d[i].id, d[i].name,
               d[i].nick, &d[i].course, d[i].email, d[i].phone) == 6) {
      i++;
    }
  }
  fclose(f);

  /**
   * search for specified student (if student is found, set `fnd` = 1)
   */
  for (int j = 0; j < i; j++) {
    /**
     * for partial id: substring id to last 4 digits before comparing
     */
    if (inplen == 4) {
      char subId[5] = {d[j].id[7], d[j].id[8], d[j].id[9], d[j].id[10], '\0'};
      if (strcmp(subId, inp) == 0) {
        idx = j;
        fnd = 1;
        break;
      }
    } 
    else {
      if (strcmp(d[j].id, inp) == 0) {
        idx = j;
        fnd = 1;
        break;
      }
    }
  }

  /**
   * if no student is found, return to main menu
   */
  if (fnd < 1) {
    printf("ID %s not found! returning to main menu...\n", inp);
    printf("========================================\n");
    free(d);
    return 1;
  }

  /**
   * copy student's name to new variable to use in print 
   * function after student's data got removed
   */
  char *name = calloc(strlen(d[idx].name), sizeof(char));
  strcpy(name, d[idx].name);

  /**
   * prompt user for confirmation, if user cancelled the
   * action, return to main menu
   */
  printf("Do you want to proceed with the deletion of %s? (y/N): ", name);
  scanf("%s", inp);
  if (inp[0] == 'y' || inp[0] == 'Y') {

    /**
     * open data file in write mode. if data file cant be opened,
     * print error and return to main menu.
     */
    f = fopen(DATA_PATH, "w");
    if (f == NULL) {
      printf(
          "[ERR] Cannot open %s . Cancelling and returning to main menu...\n",
          DATA_PATH);
      printf("========================================\n");
      free(d);
      free(name);
      return 1;
    }
    /**
     * remove specified student from data array then
     * write it back to data file, overwriting old data
     */
    removeElementFromArray(d, len, idx);
    for (int k = 0; k < *len; k++) {
      fprintf(f, "%s,%s,%s,%d,%s,%s\n", d[k].id, d[k].name, d[k].nick,
              d[k].course, d[k].email, d[k].phone);
    }
    fclose(f);
    free(d);
    printf("%s has been successfully removed.\n", name);
    printf("========================================\n");
    free(name);
    return 0;
  } 
  else {
    printf("Action cancelled. sending you back to main menu...\n");
    printf("========================================\n");
    free(d);
    free(name);
    return 2;
  }
}

/**
 * FUNCTION: printTheEntireFlippingThing
 * COMMAND: print every row of data file
 * 
 * EXPLAINATION:
 * this should actually be undocumented as 
 * it is just a debug function used in development
 */
void printTheEntireFuckingThing() {
  FILE *f = fopen(DATA_PATH, "r");
  char line[256];
  Student cur = {0};
  int m = 0;
  printf("===========Print Everyone==========\n");
  printf("Results: \n");
  while (fgets(line, sizeof(line), f)) {
    removeTrailingNewline(line);
    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%[^,],%[^,]", cur.id, cur.name,
               cur.nick, &cur.course, cur.email, cur.phone) == 6) {
      if (m == 0)
        printResHeader();
      printSearchResultLine(cur);
      m++;
    }
  }
  fclose(f);
  printf("Total match: %d\n", m);
  printf("========================================\n");
}

/**
 * FUNCTION: helpCmd
 * COMMAND: show command list
 * 
 * EXPLAINATION:
 * self explaining. show list of command.
 */
void helpCmd() {
  printf("==========CPE38 Students List==========\n");
  printf("[ C ] for students count\n");
  printf("[ I ] to search by id\n");
  printf("[ N ] to search by nickname\n");
  printf("[ F ] to search by firstname\n");
  printf("[ A ] to add student\n");
  printf("[ R ] to remove student\n");
  printf("[ H ] to display this help message\n");
  printf("[ X ] to exit the program\n");
}

/**
 * FUNCTION: main - where the magic began
 */
int main() {
  /**
   * initiate len to store data file line count.
   * this will be a main source of truth for
   * any function that need data line count
   */
  int len;

  /**
   * initiate buffer to store user input in 
   * main menu, and initiate c to store 
   * character to be use as commands
   */
  char buf[20];
  char c;

  /**
   * count data lines and store it in len,
   * if the file does not exist, exit the process
   */
  countDataLine(&len);
  if (len < 0) {
    printf("[ERR] Data file not found! Exiting...");
    return 1;
  }

  /**
   * clear terminal and display list of commands
   */
  system(CLEAR_CMD);
  helpCmd();

  /**
   * main process loop
   */
  while (1) {
    /**
     * prompt user for commands and store it 
     * in buffer variable, then get the first letter
     * for the command
     */
    printf("Command: ");
    scanf(" %s", buf);
    c = buf[0];

    /**
     * change all lowercase alphabet to uppercase
     */
    if (c >= 'a' && c <= 'z')
      c -= 32;
    if (c == 'X') // exit command
      break;
    else if (c == 'H') // show list of commands
      helpCmd();
    else if (c == 'N') // search by nickname
      searchByNickName();
    else if (c == 'F') // search by firstname
      searchByFirstName();
    else if (c == 'C') // show student count
      allStdCount();
    else if (c == 'A') // add student to data file
      addStd(&len);
    else if (c == 'R') // remove student file
      remStd(&len);
    else if (c == 'E') // TODO: remove this
      printTheEntireFuckingThing();
    else if (c == 'I')
      searchById();
    else {  // if c doesn't match any of out commands, display invalid command message
      printf(
          "Invalid command! try again. (or try 'h' for a list of commands)\n");
      continue;
    }
  }
  printf("Exiting...\n");
  return 0;
}
