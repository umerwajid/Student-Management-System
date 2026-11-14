# Student-Management-System
# 🎓 Student Management System (C + HTML)
### Miss Kindly read this once so you can understand how it works 
### The admin password is Hard Coded as admin123 .... 
### To run the project on VS code you'll have to run `./sms.exe ` in the integrated terminal

Welcome to my Student Management System project. This application is built entirely using **Pure C Language** and standard **File Handling** as the database. It uses **HTML and CSS** for the graphical user interface, which is displayed in the web browser. 

There are no external web servers (like XAMPP), Python, or SQL databases required. Everything runs directly from the C terminal to the browser.

---

## 🏗️ How It Works (Architecture)
1. **The Terminal:** Acts as the backend. The C program asks for inputs (like passwords, student details, and marks) directly in the terminal.
2. **The Browser:** Acts as the frontend. The C program dynamically generates static `.html` files based on the data entered in the terminal and automatically opens them in your default web browser.
3. **The Database:** The C program uses standard File Handling (`fopen`, `fprintf`, `fgets`) to save all data into simple `.txt` files inside the `output/` folder. 

---

## 🚀 How to Run the Program

### Prerequisites
- Any C Compiler (Dev-C++, VS Code with MinGW/GCC, Code::Blocks).
- A web browser (Chrome, Edge, Firefox).

### Compilation & Execution
1. Open the `sms.c` file in your preferred C IDE.
2. Compile and Run the code. 
   - If using **Dev-C++**: Press `F11` (Compile & Run).
   - If using **VS Code Terminal**: Run `gcc sms.c -o sms.exe` then `.\sms.exe`.
3. The terminal will display a welcome menu.
4. *Note: The program will automatically open HTML files in your browser when you select the "View" options.*

### 🔑 Login Credentials
- **Admin Password:** `admin123`
- **Student Password:** You will create this yourself when you admit a student using the Admin panel.

---

## 📖 Step-by-Step Tutorial (How to Test Everything)

### Part 1: Admin Actions
1. Run the program. Type `1` to **Login as Admin**.
2. Enter the password: `admin123`.
3. **Admit a Student:** Choose `1`. Fill in all the details (Name, Email, Password, Phone, Course, Section, Subjects). *Note the auto-generated Student ID.*
4. **Add Marks & Report:** Choose `3`. Enter the Student ID. Enter a Subject (e.g., Math), Marks (e.g., 85), and a Remark (e.g., "Excellent" or "Warning: Needs focus"). The system automatically calculates the grade (A, B, C, etc.).
5. **Collect Fee:** Choose `2`. Enter the Student ID and an Amount (e.g., 20000).
6. **View Admin Panel:** Choose `5`. Your browser will open a page showing all students, their remaining fees (in red if pending), and student queries.

### Part 2: Student Actions
1. From the main terminal menu, choose `0` to Logout, then type `2` to **Login as Student**.
2. Enter the exact Email and Password you created in Part 1.
3. **View Dashboard:** Choose `1`. The browser will open a beautiful profile showing the student's personal details, total/paid/remaining fees, auto-calculated grades, warnings (in red), and queries.
4. **Ask a Query:** Choose `2`. Type a question (e.g., "Is my fee fully paid?"). 
   *(If you open the Admin Panel now, you will see this question marked as **PENDING** in red).*

### Part 3: Resolving Queries
1. Logout and Login back as **Admin** (`1` -> `admin123`).
2. Choose `4` (Reply to Query). Enter the Query ID (e.g., 1) and type your reply (e.g., "No, you still owe 30000").
3. Choose `5` to view the Admin Panel. The query is now answered.
4. Logout, Login as **Student** again, and View Dashboard. The reply is now visible on the student's side!

---

## ✨ Key Features Handled via C File Handling
- **Role-Based Authentication:** Verifies Admin vs. Student passwords before unlocking menus.
- **Auto-Grade Calculation:** Uses an `if-else` ladder in C to calculate A+, A, B, C, D, or Fail based on marks.
- **Red Alerts:** Uses CSS logic to dynamically turn "Remaining Fees" and "Warnings" red if they are greater than zero.
- **Query Tracker:** Saves the word "PENDING" to `queries.txt` if a student asks a question. When the Admin replies, C overwrites the file and displays the answer.
- **Buffer Clearing:** Uses `while(getchar() != '\n');` to prevent terminal crashes between `scanf` and `fgets` inputs.


- **Where is the database?** 
  *The `.txt` files inside the `output` folder act as tables in a database. C file handling is used to read and write to them.*
- **How do I split the data?** 
  *I use `strtok()` in C to split the text strings by the pipe `|` character to assign specific values to variables.*
- **Why HTML?** 
  *The C program generates HTML files using `fprintf()` and uses `system("start ...")` to open them. This gives a graphical UI while keeping the backend logic in C.*
