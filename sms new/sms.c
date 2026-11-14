#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
  #include <direct.h>
  #define MAKE_DIR(d) _mkdir(d)
  #define OPEN_CMD "start \"\" output\\%s"
#else
  #include <unistd.h>
  #define MAKE_DIR(d) mkdir(d, 0755)
  #define OPEN_CMD "open output/%s 2>/dev/null || xdg-open output/%s 2>/dev/null"
#endif

// Data Files
#define STUDENTS_F "output/students.txt"
#define FEES_F     "output/fees.txt"
#define QUERIES_F  "output/queries.txt"
#define REPORTS_F  "output/reports.txt"

/* =========================================================
   HELPER FUNCTIONS
   ========================================================= */
void openBrowser(const char *page) {
    char cmd[256];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), OPEN_CMD, page);
#else
    snprintf(cmd, sizeof(cmd), OPEN_CMD, page, page);
#endif
    system(cmd);
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void cleanStr(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}

char* getGrade(int marks) {
    if (marks >= 90) return "A+";
    if (marks >= 80) return "A";
    if (marks >= 70) return "B";
    if (marks >= 60) return "C";
    if (marks >= 50) return "D";
    return "F (Fail)";
}

/* =========================================================
   HTML GENERATORS
   ========================================================= */
void generateCSS() {
    FILE *fp = fopen("output/style.css", "w");
    fprintf(fp,
    "body{font-family:Arial;background:#f4f6f9;padding:20px;}\n"
    ".container{max-width:900px;margin:auto;background:#fff;padding:30px;border-radius:10px;box-shadow:0 4px 10px rgba(0,0,0,0.1);}\n"
    "h1,h3{color:#1e3a8a;}\n"
    "table{width:100%%;border-collapse:collapse;margin-top:15px;}\n"
    "th,td{padding:10px;border:1px solid #ddd;text-align:left;}\n"
    "th{background:#1e3a8a;color:#fff;}\n"
    ".danger{color:red;font-weight:bold;}\n"
    ".success{color:green;font-weight:bold;}\n"
    ".grid{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:20px;}\n"
    ".box{background:#f8fafc;padding:10px;border-left:4px solid #1e3a8a;}\n");
    fclose(fp);
}

void generateHome() {
    FILE *fp = fopen("output/index.html", "w");
    fprintf(fp, "<html><head><link rel='stylesheet' href='style.css'></head><body><div class='container'>"
                "<h1 style='text-align:center;'>Student Management System</h1>"
                "<div style='background:#fef3c7;padding:20px;border-radius:8px;text-align:center;margin-top:20px;'>"
                "<h3>How to Access</h3>"
                "<p>To access the Admin Panel or the Student Dashboard, please login via the <b>Terminal Menu</b>.</p>"
                "<p>Once logged in, select your specific option to view your personalized page.</p>"
                "</div>"
                "</div></body></html>");
    fclose(fp);
}

void generateAdminPanel() {
    FILE *fp = fopen("output/admin_panel.html", "w");
    fprintf(fp, "<html><head><link rel='stylesheet' href='style.css'></head><body><div class='container'><h1>Admin Dashboard</h1>");

    // 1. Students & Fees
    fprintf(fp, "<h3>All Students & Fees</h3><table><tr><th>ID</th><th>Name</th><th>Email</th><th>Total</th><th>Paid</th><th>Remaining</th></tr>");
    FILE *fs = fopen(STUDENTS_F, "r"); FILE *ff = fopen(FEES_F, "r");
    if (fs && ff) {
        char sline[512], fline[256];
        while (fgets(sline, 512, fs)) {
            int id; char name[50], email[100];
            char *token = strtok(sline, "|");
            if(!token) continue; id = atoi(token);
            token = strtok(NULL, "|"); if(token) strcpy(name, token);
            token = strtok(NULL, "|"); if(token) strcpy(email, token);
            
            float total = 0, paid = 0;
            if (fgets(fline, 256, ff)) {
                char *f_tok = strtok(fline, "|");
                f_tok = strtok(NULL, "|"); if(f_tok) total = atof(f_tok);
                f_tok = strtok(NULL, "|"); if(f_tok) paid = atof(f_tok);
            }
            float rem = total - paid;
            fprintf(fp, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%.2f</td><td>%.2f</td><td class='%s'>%.2f</td></tr>", 
                    id, name, email, total, paid, (rem > 0) ? "danger" : "success", rem);
        }
        fclose(fs); fclose(ff);
    }

    // 2. Queries (Memory fixed)
    fprintf(fp, "</table><h3>Student Queries</h3><table><tr><th>Query ID</th><th>Student ID</th><th>Query</th><th>Answer</th></tr>");
    FILE *fq = fopen(QUERIES_F, "r");
    if (fq) {
        char line[512];
        while (fgets(line, 512, fq)) {
            int qid = 0, sid = 0; char msg[200] = "", ans[200] = "";
            
            char *tok = strtok(line, "|"); 
            if(!tok) continue; 
            qid = atoi(tok); 
            if (qid == 0) continue; // Skip broken/empty lines
            
            tok = strtok(NULL, "|"); if(tok) sid = atoi(tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(msg, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(ans, tok); 
            
            if (strlen(ans) < 3 || strcmp(ans, "PENDING") == 0) {
                fprintf(fp, "<tr><td>%d</td><td>%d</td><td>%s</td><td class='danger'>PENDING</td></tr>", qid, sid, msg);
            } else {
                fprintf(fp, "<tr><td>%d</td><td>%d</td><td>%s</td><td>%s</td></tr>", qid, sid, msg, ans);
            }
        }
        fclose(fq);
    }
    fprintf(fp, "</table></div></body></html>");
    fclose(fp);
}

void generateStudentDashboard(int sid) {
    FILE *fp = fopen("output/dashboard.html", "w");
    if (sid == -1) {
        fprintf(fp, "<html><head><link rel='stylesheet' href='style.css'></head><body><div class='container'><h1>Not Logged In</h1><p>Please login via Terminal.</p></div></body></html>");
        fclose(fp); return;
    }

    fprintf(fp, "<html><head><link rel='stylesheet' href='style.css'></head><body><div class='container'>");

    // 1. Student Profile
    FILE *fs = fopen(STUDENTS_F, "r"); char line[512]; 
    char name[50]="", email[100]="", phone[15]="", course[50]="", section[10]="", subj[50]="";
    if (fs) {
        while (fgets(line, 512, fs)) {
            int id; char pass[50];
            char *tok = strtok(line, "|"); if(!tok) continue; id = atoi(tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(name, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(email, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(pass, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(phone, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(course, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(section, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(subj, tok);
            if (id == sid) break;
        }
        fclose(fs);
    }

    fprintf(fp, "<h1>Welcome, %s</h1><h3>Personal Profile</h3><div class='grid'>"
                "<div class='box'><b>ID:</b> %d</div><div class='box'><b>Name:</b> %s</div>"
                "<div class='box'><b>Email:</b> %s</div><div class='box'><b>Phone:</b> %s</div>"
                "<div class='box'><b>Course:</b> %s</div><div class='box'><b>Section:</b> %s</div>"
                "<div class='box' style='grid-column: span 2;'><b>Subjects:</b> %s</div></div>", 
                name, sid, name, email, phone, course, section, subj);

    // 2. Fees
    FILE *ff = fopen(FEES_F, "r"); float total = 0, paid = 0;
    if (ff) {
        while (fgets(line, 256, ff)) {
            int id; char *tok = strtok(line, "|"); if(!tok) continue; id = atoi(tok);
            tok = strtok(NULL, "|"); if(tok) total = atof(tok);
            tok = strtok(NULL, "|"); if(tok) paid = atof(tok);
            if (id == sid) break;
        }
        fclose(ff);
    }
    float rem = total - paid;
    fprintf(fp, "<h3>Fees</h3><table><tr><th>Total</th><th>Paid</th><th>Remaining</th></tr>"
                "<tr><td>%.2f</td><td>%.2f</td><td class='%s'>%.2f</td></tr></table>", 
                total, paid, (rem > 0) ? "danger" : "success", rem);

    // 3. Marks & Reports
    fprintf(fp, "<h3>Test Marks & Reports</h3><table><tr><th>Subject</th><th>Marks</th><th>Grade</th><th>Remarks/Warning</th></tr>");
    FILE *fr = fopen(REPORTS_F, "r"); int found = 0;
    if (fr) {
        while (fgets(line, 256, fr)) {
            int id, marks; char subject[50], remark[100];
            char *tok = strtok(line, "|"); if(!tok) continue; id = atoi(tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(subject, tok);
            tok = strtok(NULL, "|"); if(tok) marks = atoi(tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(remark, tok);
            
            if (id == sid) {
                fprintf(fp, "<tr><td>%s</td><td>%d</td><td class='success'>%s</td><td class='%s'>%s</td></tr>", 
                        subject, marks, getGrade(marks), strstr(remark, "Warning")?"danger":"", remark);
                found = 1;
            }
        }
        fclose(fr);
    }
    if(!found) fprintf(fp, "<tr><td colspan='4'>No marks added yet.</td></tr>");
    fprintf(fp, "</table>");

    // 4. Queries (Memory fixed)
    fprintf(fp, "<h3>My Queries</h3><table><tr><th>Question</th><th>Admin Reply</th></tr>");
    FILE *fq = fopen(QUERIES_F, "r"); found = 0;
    if (fq) {
        while (fgets(line, 512, fq)) {
            int qid = 0, q_sid = 0; char msg[200] = "", ans[200] = "";
            
            char *tok = strtok(line, "|"); if(!tok) continue; qid = atoi(tok);
            if (qid == 0) continue; // Skip broken/empty lines
            
            tok = strtok(NULL, "|"); if(tok) q_sid = atoi(tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(msg, tok);
            tok = strtok(NULL, "|"); if(tok) strcpy(ans, tok); 
            
            if (q_sid == sid) {
                if (strlen(ans) < 3 || strcmp(ans, "PENDING") == 0) {
                    fprintf(fp, "<tr><td>%s</td><td class='danger'>PENDING</td></tr>", msg);
                } else {
                    fprintf(fp, "<tr><td>%s</td><td>%s</td></tr>", msg, ans);
                }
                found = 1;
            }
        }
        fclose(fq);
    }
    if(!found) fprintf(fp, "<tr><td colspan='2'>No queries yet.</td></tr>");
    
    fprintf(fp, "</table></div></body></html>");
    fclose(fp);
}

/* =========================================================
   MAIN PROGRAM
   ========================================================= */
int main() {
    MAKE_DIR("output");
    generateCSS(); generateHome(); generateAdminPanel(); generateStudentDashboard(-1); 

    int roleChoice;
    do {
        clearScreen();
        printf("\n==================================================\n");
        printf("       STUDENT MANAGEMENT SYSTEM (SMS)\n");
        printf("==================================================\n");
        printf(" 1. Login as Admin\n");
        printf(" 2. Login as Student\n");
        printf(" 0. Exit\n");
        printf("==================================================\n");
        printf(" Who are you? (Enter choice): ");
        if (scanf("%d", &roleChoice) != 1) { while(getchar() != '\n'); continue; }
        while(getchar() != '\n'); 

        if (roleChoice == 1) {
            // --- ADMIN ---
            char pass[50];
            printf("\n--- ADMIN AUTHENTICATION ---\n");
            printf(" Enter Admin Password: "); fgets(pass, 50, stdin); cleanStr(pass);
            if (strcmp(pass, "admin123") != 0) { printf("\n[!] Wrong Password!\n"); pauseScreen(); continue; }

            int ac;
            do {
                clearScreen();
                printf("\n==================================================\n");
                printf("                ADMIN DASHBOARD\n");
                printf("==================================================\n");
                printf(" 1. Admit New Student\n");
                printf(" 2. Collect Fee\n");
                printf(" 3. Add Marks / Report\n");
                printf(" 4. Reply to Query\n");
                printf(" 5. View Admin Panel (Browser)\n");
                printf(" 0. Logout\n");
                printf("==================================================\n");
                printf(" Enter choice: ");
                if (scanf("%d", &ac) != 1) { while(getchar() != '\n'); continue; }
                while(getchar() != '\n'); 

                if (ac == 1) {
                    char name[50], email[100], p[50], phone[15], course[50], section[10], subject[50];
                    printf("\n--- ADMIT NEW STUDENT ---\n");
                    printf(" Name: "); fgets(name, 50, stdin); cleanStr(name);
                    printf(" Email: "); fgets(email, 100, stdin); cleanStr(email);
                    printf(" Password: "); fgets(p, 50, stdin); cleanStr(p);
                    printf(" Phone: "); fgets(phone, 15, stdin); cleanStr(phone);
                    printf(" Course: "); fgets(course, 50, stdin); cleanStr(course);
                    printf(" Section: "); fgets(section, 10, stdin); cleanStr(section);
                    printf(" Subjects: "); fgets(subject, 50, stdin); cleanStr(subject);
                    
                    int nextId = 1; FILE *fs = fopen(STUDENTS_F, "r");
                    if(fs) { char l[512]; int m=0; while(fgets(l,512,fs)) { int c=atoi(l); if(c>m) m=c; } fclose(fs); nextId=m+1; }
                    fs = fopen(STUDENTS_F, "a"); fprintf(fs, "%d|%s|%s|%s|%s|%s|%s|%s\n", nextId, name, email, p, phone, course, section, subject); fclose(fs);
                    FILE *ff = fopen(FEES_F, "a"); fprintf(ff, "%d|50000.00|0.00\n", nextId); fclose(ff);
                    printf("\n[OK] Student Admitted! Assigned ID: %d\n", nextId);
                    generateAdminPanel(); pauseScreen();
                }
                else if (ac == 2) {
                    int sid; float amt;
                    printf("\n--- COLLECT FEE ---\n");
                    printf(" Student ID: "); scanf("%d", &sid); 
                    printf(" Amount Paid: "); scanf("%f", &amt); 
                    while(getchar()!='\n'); 
                    
                    FILE *ff = fopen(FEES_F, "r"); FILE *tmp = fopen("output/tmp.txt", "w");
                    char l[256]; int found = 0;
                    while(fgets(l,256,ff)) { 
                        char *tok = strtok(l, "|"); int id = atoi(tok); 
                        tok = strtok(NULL, "|"); float t = atof(tok);
                        tok = strtok(NULL, "|"); float p = atof(tok);
                        if(id==sid) { p+=amt; found=1; } 
                        fprintf(tmp, "%d|%.2f|%.2f\n", id, t, p); 
                    }
                    fclose(ff); fclose(tmp); remove(FEES_F); rename("output/tmp.txt", FEES_F);
                    if(found) { printf("\n[OK] Fee Recorded.\n"); generateAdminPanel(); } else printf("\n[!] ID not found.\n");
                    pauseScreen();
                }
                else if (ac == 3) {
                    int sid, marks; char subject[50], remark[100];
                    printf("\n--- ADD MARKS & REPORT ---\n");
                    printf(" Student ID: "); scanf("%d", &sid); 
                    while(getchar() != '\n'); 
                    printf(" Subject: "); fgets(subject, 50, stdin); cleanStr(subject);
                    printf(" Marks (0-100): "); scanf("%d", &marks); 
                    while(getchar() != '\n'); 
                    printf(" Remarks/Warning: "); fgets(remark, 100, stdin); cleanStr(remark);
                    
                    FILE *fr = fopen(REPORTS_F, "a"); fprintf(fr, "%d|%s|%d|%s\n", sid, subject, marks, remark); fclose(fr);
                    printf("\n[OK] Marks & Report Added.\n");
                    generateAdminPanel(); pauseScreen();
                }
                else if (ac == 4) {
                    int qid; char rep[200];
                    printf("\n--- REPLY TO QUERY ---\n");
                    printf(" Query ID: "); scanf("%d", &qid); 
                    while(getchar() != '\n'); 
                    printf(" Reply: "); fgets(rep, 200, stdin); cleanStr(rep);
                    
                    FILE *fq = fopen(QUERIES_F, "r"); FILE *tmp = fopen("output/tmp.txt", "w");
                    char l[512]; int found = 0;
                    while(fgets(l,512,fq)) { 
                        char *tok = strtok(l, "|"); int id = atoi(tok);
                        tok = strtok(NULL, "|"); int s_id = atoi(tok);
                        tok = strtok(NULL, "|"); char m[200]; strcpy(m, tok);
                        tok = strtok(NULL, "|"); char a[200]; strcpy(a, (tok) ? tok : "PENDING");
                        
                        if(id == qid) { strcpy(a, rep); found = 1; }
                        fprintf(tmp, "%d|%d|%s|%s\n", id, s_id, m, a);
                    }
                    fclose(fq); fclose(tmp); remove(QUERIES_F); rename("output/tmp.txt", QUERIES_F);
                    if(found) printf("\n[OK] Query Answered.\n"); else printf("\n[!] Query ID not found.\n");
                    generateAdminPanel(); pauseScreen();
                }
                else if (ac == 5) { 
                    printf("\n Opening Admin Panel...\n");
                    openBrowser("admin_panel.html"); pauseScreen(); 
                }
            } while (ac != 0);
        }
        else if (roleChoice == 2) {
            // --- STUDENT ---
            char email[100], pass[50];
            printf("\n--- STUDENT LOGIN ---\n");
            printf(" Email: "); fgets(email, 100, stdin); cleanStr(email);
            printf(" Password: "); fgets(pass, 50, stdin); cleanStr(pass);

            FILE *fs = fopen(STUDENTS_F, "r"); int sid = -1; char l[512];
            if(fs) { 
                while(fgets(l,512,fs)) { 
                    char *tok = strtok(l, "|"); int id = atoi(tok);
                    tok = strtok(NULL, "|");
                    tok = strtok(NULL, "|"); char e[100]; strcpy(e, tok);
                    tok = strtok(NULL, "|"); char p[50]; strcpy(p, tok);
                    if(strcmp(e,email)==0 && strcmp(p,pass)==0) { sid=id; break; } 
                } 
                fclose(fs); 
            }

            if (sid == -1) { printf("\n[!] Invalid Credentials!\n"); pauseScreen(); continue; }

            int sc;
            do {
                clearScreen();
                printf("\n==================================================\n");
                printf("                STUDENT DASHBOARD\n");
                printf("==================================================\n");
                printf(" 1. View My Dashboard (Browser)\n");
                printf(" 2. Ask a Query\n");
                printf(" 0. Logout\n");
                printf("==================================================\n");
                printf(" Enter choice: ");
                if (scanf("%d", &sc) != 1) { while(getchar() != '\n'); continue; }
                while(getchar() != '\n');

                if (sc == 1) { 
                    printf("\n Generating Dashboard...\n");
                    generateStudentDashboard(sid); 
                    openBrowser("dashboard.html"); 
                    pauseScreen(); 
                }
                else if (sc == 2) {
                    char msg[200];
                    printf("\n--- ASK A QUERY ---\n");
                    printf(" Your Question: "); fgets(msg, 200, stdin); cleanStr(msg);
                    
                    int nextQid = 1; FILE *fq = fopen(QUERIES_F, "r");
                    if(fq) { char ql[512]; int m=0; while(fgets(ql,512,fq)) { int c=atoi(ql); if(c>m) m=c; } fclose(fq); nextQid=m+1; }
                    
                    fq = fopen(QUERIES_F, "a"); fprintf(fq, "%d|%d|%s|PENDING\n", nextQid, sid, msg); fclose(fq);
                    printf("\n[OK] Query Sent to Admin! Your Query ID: %d\n", nextQid);
                    generateAdminPanel(); pauseScreen();
                }
            } while (sc != 0);
        }
    } while (roleChoice != 0);

    return 0;
}