#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bookstore.db");
    if (!db.open()){
        QMessageBox::information(this, "Database Connection", "Connection to SQLite Database Failed!");
    }else{
        // if db is connected successfully, initialize adminwindow(hidden)and connect it with the current db
        adminUI = new AdminWindow(this);
        qmodel = new QSqlQueryModel();
        trackmodel = new QSqlQueryModel();
        if (DBInit()){
            createTrigger();
            if(DBInsertDummyData()){
              ui->statusbar->showMessage(db.databaseName()+": initialized successfully with dummy data");
              createView();
            }
            else{
                ui->statusbar->showMessage(db.databaseName()+": initialized without dummy data");
            }
        }
        else{
            ui->statusbar->showMessage(db.databaseName()+": failed to initialize!");
        }
    }
    ui->tabWidget_2->setTabEnabled(1,false);
    ui->tabWidget_2->setTabEnabled(2,false);
    ui->tabWidget_2->setTabEnabled(3,false);

}

MainWindow::~MainWindow()
{
    db.close();
    delete qmodel;
    delete adminUI;
    delete ui;
}

bool MainWindow::DBInit() {

    db.transaction();

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS publisher (  \
               pub_name	TEXT NOT NULL,  \
               address	TEXT NOT NULL,  \
               email	TEXT NOT NULL,  \
               bank_account	TEXT NOT NULL UNIQUE,   \
               PRIMARY KEY(pub_name)    \
           );");
    query.exec("CREATE TABLE IF NOT EXISTS user (  \
                username	TEXT NOT NULL,  \
                password	TEXT NOT NULL,  \
                fname	TEXT NOT NULL,  \
                lname	TEXT NOT NULL,  \
                address	TEXT NOT NULL,  \
                PRIMARY KEY(username)   \
            );");
    query.exec("CREATE TABLE IF NOT EXISTS book (   \
                ISBN	TEXT NOT NULL,  \
                name	TEXT NOT NULL,  \
                numPages	INTEGER NOT NULL CHECK(numPages>0), \
                cost	NUMERIC NOT NULL CHECK(cost>=0),    \
                price	NUMERIC NOT NULL CHECK(price>=0),   \
                stock	INTEGER NOT NULL CHECK(stock>=0),   \
                royalty	NUMERIC NOT NULL CHECK(royalty>=0), \
                threshold	INTEGER NOT NULL CHECK(threshold>=0),   \
                pub_name	TEXT NOT NULL,  \
                total_stocked	INTEGER NOT NULL DEFAULT 0, \
                PRIMARY KEY(ISBN),  \
                FOREIGN KEY(pub_name) REFERENCES publisher(pub_name) ON DELETE RESTRICT ON UPDATE CASCADE   \
            );");
    query.exec("CREATE TABLE IF NOT EXISTS pub_phone (  \
                pub_name	TEXT NOT NULL,  \
                phone	TEXT NOT NULL,  \
                PRIMARY KEY(pub_name,phone) \
            );");
    query.exec("CREATE TABLE IF NOT EXISTS genre (  \
                genre	TEXT NOT NULL,  \
                ISBN	TEXT NOT NULL,  \
                PRIMARY KEY(genre,ISBN),    \
                FOREIGN KEY(ISBN) REFERENCES book(ISBN) ON DELETE CASCADE ON UPDATE CASCADE \
            );");
    query.exec("CREATE TABLE IF NOT EXISTS author ( \
                author	TEXT NOT NULL,  \
                ISBN	TEXT NOT NULL,  \
                FOREIGN KEY(ISBN) REFERENCES book(ISBN) ON DELETE CASCADE ON UPDATE CASCADE,    \
                PRIMARY KEY(author,ISBN)    \
            );");
    query.exec("CREATE TABLE IF NOT EXISTS [order] (    \
                order_id	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, \
                shipping_address	TEXT NOT NULL,  \
                billing_address	TEXT NOT NULL,  \
                tracking_info	TEXT NOT NULL DEFAULT 'Order placed',   \
                order_date	TEXT NOT NULL DEFAULT CURRENT_DATE, \
                username	TEXT NOT NULL,  \
                FOREIGN KEY(username) REFERENCES user(username) ON DELETE CASCADE ON UPDATE CASCADE \
            );");
    query.exec("CREATE TABLE IF NOT EXISTS order_book ( \
                ISBN	TEXT NOT NULL,  \
                order_id	INTEGER NOT NULL,   \
                order_amount	INTEGER NOT NULL CHECK(order_amount>0), \
                FOREIGN KEY(ISBN) REFERENCES book(ISBN) ON DELETE CASCADE ON UPDATE CASCADE,    \
                PRIMARY KEY(ISBN,order_id), \
                FOREIGN KEY(order_id) REFERENCES [order](order_id) ON DELETE CASCADE ON UPDATE CASCADE  \
            );");
    return db.commit();
}

bool MainWindow::DBInsertDummyData(){
    db.transaction();

    QSqlQuery query;
    query.exec("DELETE FROM order_book;");
    query.exec("DELETE FROM [order];");
    query.exec("DELETE FROM author;");
    query.exec("DELETE FROM genre;");
    query.exec("DELETE FROM pub_phone;");
    query.exec("DELETE FROM book;");
    query.exec("DELETE FROM publisher;");
    query.exec("DELETE FROM [user];");
    query.exec("INSERT INTO publisher VALUES ('Continent Inc.','53 Grant Ave Saint Paul, MN 55104','contact@continent.com','0789-3332231');");
    query.exec("INSERT INTO publisher VALUES ('Simon & Schuster','236 Clinton St. Westlake, ON 44145','contact@ss.com','0013-78900221');");
    query.exec("INSERT INTO publisher VALUES ('Pearson Education','12 Vermont St. Ovi, FL 32765','contactus@pearson.com','0900-90129388');");
    query.exec("INSERT INTO publisher VALUES ('Marvel Comics','22 Garfield Ave.Vland, NJ 08360','sales@marvelcomics.com','0003-99099099');");
    query.exec("INSERT INTO user VALUES ('student','student','John','Doe','682 NE. Annadale Drive, NJ 07083');");
    query.exec("INSERT INTO book VALUES ('0-4468-4857-3','Star Wars: Bounty Hunters',230,19.95,29.95,50,0.1,10,'Marvel Comics', 0);");
    query.exec("INSERT INTO book VALUES ('0-1704-1348-9','Intro to Calculus II',450,29.95,46.95,40,0.15,10,'Pearson Education', 0);");
    query.exec("INSERT INTO book VALUES ('0-6257-1942-5','Intro to Calculus I',550,29.95,49.95,30,0.15,10,'Pearson Education', 0);");
    query.exec("INSERT INTO book VALUES ('0-8521-1367-6','Star Wars: Doctor Aphra',178,15.5,24.5,50,0.1,10,'Marvel Comics', 0);");
    query.exec("INSERT INTO book VALUES ('0-1104-5785-4','Shang-Chi and the Ten Rings',155,20,29.95,50,0.1,10,'Marvel Comics', 0);");
    query.exec("INSERT INTO book VALUES ('0-5287-4530-1','The Moonlight',390,15.95,25.95,30,0.1,10,'Continent Inc.', 0);");
    query.exec("INSERT INTO book VALUES ('0-1144-0720-7','Apollo 11: Landing on the Moon',360,20.05,30.05,30,0.1,10,'Simon & Schuster', 0);");
    query.exec("INSERT INTO book VALUES ('0-8075-3873-6','Ten Things We Did',290,13,23,35,0.1,10,'Continent Inc.', 0);");
    query.exec("INSERT INTO book VALUES ('0-5464-5226-4','Pre-Calculus 12 Student Book',655,78,159,50,0.2,10,'Pearson Education', 0);");
    query.exec("INSERT INTO book VALUES ('0-9759-5419-9','Me Before You',298,15.8,28.5,70,0.15,10,'Simon & Schuster', 0);");
    query.exec("INSERT INTO book VALUES ('0-8472-7748-8','The Hating Game',386,20,30,30,0.15,10,'Simon & Schuster', 0);");
    query.exec("INSERT INTO book VALUES ('0-3774-7473-8','Ender''s Game (comics)',252,19,28,45,0.15,10,'Marvel Comics', 0);");
    query.exec("INSERT INTO pub_phone VALUES ('Continent Inc.','3456782899');");
    query.exec("INSERT INTO pub_phone VALUES ('Continent Inc.','7789901234');");
    query.exec("INSERT INTO pub_phone VALUES ('Simon & Schuster','8890091928');");
    query.exec("INSERT INTO pub_phone VALUES ('Pearson Education','4035569992');");
    query.exec("INSERT INTO pub_phone VALUES ('Marvel Comics','18902234556');");
    query.exec("INSERT INTO pub_phone VALUES ('Marvel Comics','3078890090');");
    query.exec("INSERT INTO genre VALUES ('fiction','0-4468-4857-3');");
    query.exec("INSERT INTO genre VALUES ('textbook','0-1704-1348-9');");
    query.exec("INSERT INTO genre VALUES ('textbook','0-6257-1942-5');");
    query.exec("INSERT INTO genre VALUES ('fiction','0-8521-1367-6');");
    query.exec("INSERT INTO genre VALUES ('fiction','0-1104-5785-4');");
    query.exec("INSERT INTO genre VALUES ('novel','0-5287-4530-1');");
    query.exec("INSERT INTO genre VALUES ('narrative','0-1144-0720-7');");
    query.exec("INSERT INTO genre VALUES ('non-fiction','0-1144-0720-7');");
    query.exec("INSERT INTO genre VALUES ('short story','0-8075-3873-6');");
    query.exec("INSERT INTO genre VALUES ('non-fiction','0-8075-3873-6');");
    query.exec("INSERT INTO genre VALUES ('textbook','0-5464-5226-4');");
    query.exec("INSERT INTO genre VALUES ('romance novel','0-9759-5419-9');");
    query.exec("INSERT INTO genre VALUES ('romance novel','0-8472-7748-8');");
    query.exec("INSERT INTO genre VALUES ('fiction','0-3774-7473-8');");
    query.exec("INSERT INTO genre VALUES ('comics','0-3774-7473-8');");
    query.exec("INSERT INTO genre VALUES ('non-fiction','0-5287-4530-1');");
    query.exec("INSERT INTO author VALUES ('John Wall','0-4468-4857-3');");
    query.exec("INSERT INTO author VALUES ('Kevin L.','0-4468-4857-3');");
    query.exec("INSERT INTO author VALUES ('Alysha Hart','0-1704-1348-9');");
    query.exec("INSERT INTO author VALUES ('Khalil Rios','0-1704-1348-9');");
    query.exec("INSERT INTO author VALUES ('Alysha Hart','0-6257-1942-5');");
    query.exec("INSERT INTO author VALUES ('Maya Bell','0-8521-1367-6');");
    query.exec("INSERT INTO author VALUES ('Sam Lee','0-1104-5785-4');");
    query.exec("INSERT INTO author VALUES ('Ned Dawson','0-5287-4530-1');");
    query.exec("INSERT INTO author VALUES ('Diana Levy','0-1144-0720-7');");
    query.exec("INSERT INTO author VALUES ('Margaret H.','0-1144-0720-7');");
    query.exec("INSERT INTO author VALUES ('Amirah Lyons','0-8075-3873-6');");
    query.exec("INSERT INTO author VALUES ('Rufus Ali','0-5464-5226-4');");
    query.exec("INSERT INTO author VALUES ('Sid Heath','0-9759-5419-9');");
    query.exec("INSERT INTO author VALUES ('Sally Thorne','0-8472-7748-8');");
    query.exec("INSERT INTO author VALUES ('Orson S. Card','0-3774-7473-8');");
    query.exec("INSERT INTO author VALUES ('Khalil Rios','0-6257-1942-5');");
    query.exec("INSERT INTO author VALUES ('John Wall','0-8521-1367-6');");
    query.exec("INSERT INTO [order] VALUES (1,'8355 Williams St.Vista, CA 92083','682 NE. Annadale Drive, NJ 07083','Order shipped','2022-11-29','student');");
    query.exec("INSERT INTO [order] VALUES (2,'682 NE. Annadale Drive, NJ 07083','682 NE. Annadale Drive, NJ 07083','In transit','2022-12-07','student');");
    query.exec("INSERT INTO [order] VALUES (3,'8355 Williams St.Vista, CA 92083','8355 Williams St.Vista, CA 92083','Order shipped','2022-10-20','student');");
    query.exec("INSERT INTO order_book VALUES ('0-4468-4857-3',1,20);");
    query.exec("INSERT INTO order_book VALUES ('0-1704-1348-9',1,20);");
    query.exec("INSERT INTO order_book VALUES ('0-6257-1942-5',1,30);");
    query.exec("INSERT INTO order_book VALUES ('0-8521-1367-6',1,20);");
    query.exec("INSERT INTO order_book VALUES ('0-1104-5785-4',1,30);");
    query.exec("INSERT INTO order_book VALUES ('0-5287-4530-1',1,10);");
    query.exec("INSERT INTO order_book VALUES ('0-1144-0720-7',1,15);");
    query.exec("INSERT INTO order_book VALUES ('0-8075-3873-6',1,30);");
    query.exec("INSERT INTO order_book VALUES ('0-5464-5226-4',1,25);");
    query.exec("INSERT INTO order_book VALUES ('0-9759-5419-9',1,50);");
    query.exec("INSERT INTO order_book VALUES ('0-8472-7748-8',1,5)");
    query.exec("INSERT INTO order_book VALUES ('0-3774-7473-8',2,5);");
    query.exec("INSERT INTO order_book VALUES ('0-3774-7473-8',3,50);");

    return db.commit();
}
bool MainWindow::createView(){
    db.transaction();

    QSqlQuery query;
    query.exec("CREATE VIEW IF NOT EXISTS table_view AS \
               SELECT ISBN, name, price, pub_name AS publisher, group_concat(DISTINCT(author)) AS authors, group_concat(DISTINCT(genre)) AS genres, numPages AS pages \
               FROM book NATURAL JOIN author NATURAL JOIN genre GROUP BY ISBN LIMIT 20;");
    query.exec("CREATE VIEW IF NOT EXISTS report_view AS \
               SELECT book.ISBN, price, cost, royalty, stock, total_stocked, SUM(order_amount)AS sale_amount \
               FROM book, [order], order_book   \
               WHERE book.ISBN = order_book.ISBN AND [order].order_id = order_book.order_id \
               GROUP by order_book.ISBN;");
    return db.commit();
}
bool MainWindow::createTrigger(){
    db.transaction();

    QSqlQuery query;
    query.exec("CREATE TRIGGER IF NOT EXISTS init_total_stocked \
               AFTER INSERT ON book \
               BEGIN    \
                   UPDATE book  \
                   SET total_stocked = NEW.stock    \
                   WHERE ISBN = NEW.ISBN;   \
               END;");
    query.exec("CREATE TRIGGER IF NOT EXISTS update_total_stocked   \
               AFTER UPDATE OF stock ON book    \
               WHEN NEW.stock > OLD.stock   \
               BEGIN    \
                   UPDATE book  \
                   SET total_stocked = OLD.total_stocked + NEW.stock - OLD.stock    \
                   WHERE ISBN = NEW.ISBN;   \
               END;");
    query.exec("CREATE TRIGGER IF NOT EXISTS restock_book   \
               AFTER UPDATE OF stock, threshold ON book \
               WHEN NEW.stock < NEW.threshold   \
               BEGIN    \
                   UPDATE book  \
                   SET stock = 30 + NEW.stock   \
                   WHERE ISBN = NEW.ISBN;   \
               END;");
    return db.commit();
}

// Admin
void MainWindow::on_pushButton_admin_login_clicked()
{
    if (ui->lineEdit_admin_username->text().toLower() == "admin" && ui->lineEdit_admin_password->text() == "admin"){
        if (adminUI->isHidden()){
            adminUI->show();
            ui->statusbar->showMessage("Admin logged in successfully!");
        }
    }
    else{
        ui->statusbar->showMessage("Admin login failed, wrong credentials!");
    }
}
// User Login
void MainWindow::on_pushButton_user_login_clicked()
{
    QString username = ui->lineEdit_username->text();
    QSqlQuery query;
    query.prepare("SELECT password FROM user WHERE username = ? ");
    query.addBindValue(username);
    query.exec();
    if (query.next()){
        if (ui->lineEdit_user_password->text() == query.value(0).toString()){
            ui->statusbar->showMessage(username+" logged in successfully!");
            ui->tabWidget_2->setTabEnabled(1,true);
            ui->tabWidget_2->setTabEnabled(2,true);
            ui->tabWidget_2->setTabEnabled(3,true);
            if (current_user != username){
                // clean shopping cart
                ui->tableWidget_cart->setRowCount(0);
                cart_map.clear();
                cart_amount.clear();
                current_user = username;
                ui->label_currentUser->setText("Shopping cart for : "+current_user);
                listAllBooks();
            }
        }
        else{
            ui->statusbar->showMessage("Incorrect password!");
        }
    }
    else{
        ui->statusbar->showMessage("Username doesn't exist!");
    }

}
// Register
void MainWindow::on_pushButton_register_clicked()
{
    QString username = ui->lineEdit_register_name->text().trimmed();
    QString pw = ui->lineEdit_register_pw->text().trimmed();
    QString fname = ui->lineEdit_register_fname->text().trimmed();
    QString lname = ui->lineEdit_register_lname->text().trimmed();
    QString address = ui->plainTextEdit_register_addr->toPlainText().trimmed();
    if (username.isEmpty()||pw.isEmpty()||fname.isEmpty()||lname.isEmpty()||address.isEmpty()){
        ui->statusbar->showMessage("All fields are required to sign up!");
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT username FROM user WHERE username = ? ");
    query.addBindValue(username);
    query.exec();
    if (query.next()){
        ui->statusbar->showMessage("Username already exists! Enter another one!");
        return;
    }
    query.prepare("INSERT INTO user VALUES (?,?,?,?,?);");
    query.addBindValue(username);
    query.addBindValue(pw);
    query.addBindValue(fname);
    query.addBindValue(lname);
    query.addBindValue(address);
    if (query.exec()){
        ui->statusbar->showMessage("Registered! You can login now!");
    }
}
// List all books from view (limit 20)
void MainWindow::listAllBooks(){
    qmodel->setQuery("SELECT * FROM table_view;");
    ui->tableView->setModel(qmodel);
    //connect(ui->tableView->selectionModel(),&QItemSelectionModel::currentChanged, this, &MainWindow::tableIndexChanged);
}

// Search books
void MainWindow::on_pushButton_clicked()
{
    QString searchValue = ui->lineEdit_search->text().trimmed();
    if(searchValue.isEmpty()){
        listAllBooks();
    }
    else{
        QString searchBy = ui->comboBox->currentText();
        QSqlQuery query;
        if(searchBy == "ISBN"){
            query.prepare("SELECT * FROM table_view WHERE ISBN LIKE ? ;");
            query.addBindValue("%"+searchValue+"%");
        }
        else if (searchBy == "book name"){
            query.prepare("SELECT * FROM table_view WHERE name LIKE ? ;");
            query.addBindValue("%"+searchValue+"%");
        }
        else if (searchBy == "genre"){
            query.prepare("SELECT * FROM table_view WHERE genres LIKE ? ;");
            query.addBindValue("%"+searchValue+"%");
        }
        else if (searchBy == "price under"){
            query.prepare("SELECT * FROM table_view WHERE price <= ? ;");
            query.addBindValue(searchValue);
        }
        else if (searchBy == "author name"){
            query.prepare("SELECT * FROM table_view WHERE authors LIKE ? ;");
            query.addBindValue("%"+searchValue+"%");
        }

        if (query.exec()){
            qmodel->setQuery(query);
            ui->tableView->setModel(qmodel);
            //connect(ui->tableView->selectionModel(),&QItemSelectionModel::currentChanged, this, &MainWindow::tableIndexChanged);
        }else{
            ui->statusbar->showMessage("Loading books failed!");
        }
    }
}
// tracking order
void MainWindow::on_pushButton_track_clicked()
{
    QString order_id = ui->lineEdit_order->text().trimmed();
    if (order_id.isEmpty()){
        return;
    }
    QSqlQuery query;
    query.prepare("SELECT order_date as 'order placed on', order_id AS 'order id', shipping_address AS 'shipped to', tracking_info AS tracking \
                    FROM [order] WHERE order_id = ? AND username = ? ;");
    query.addBindValue(order_id);
    query.addBindValue(current_user);
    query.exec();
    if (query.next()){
        trackmodel->setQuery(query);
        ui->tableView_tracking->setModel(trackmodel);
    }else{
        ui->statusbar->showMessage("You don't have an order with id = "+order_id);
    }
}
// sort books
void MainWindow::on_pushButton_sort_clicked()
{
     QString sortBy = ui->comboBox_sort->currentText();

     QSqlQuery query;
     if(sortBy == "price low to high"){
         query.prepare("SELECT * FROM table_view ORDER BY price ASC;");
     }
     else if (sortBy == "price high to low"){
         query.prepare("SELECT * FROM table_view ORDER BY price DESC;");
     }
     else if (sortBy == "best sellers all time"){
         query.prepare("SELECT ISBN, name, price, publisher, authors,genres, pages \
                        FROM table_view NATURAL JOIN order_book GROUP BY ISBN ORDER BY SUM(order_amount) DESC;");
     }
     else if (sortBy == "best sellers last month"){
         query.prepare("SELECT ISBN, name, price, publisher, authors,genres, pages  \
                       FROM table_view NATURAL JOIN (   \
                           SELECT ISBN, order_amount    \
                           FROM order_book NATURAL JOIN [order] \
                           WHERE order_date BETWEEN datetime('now', '-31 days') AND datetime ('now', 'localtime')) \
                       GROUP BY ISBN ORDER BY SUM(order_amount) DESC;");
     }

     if (query.exec()){
         qmodel->setQuery(query);
         ui->tableView->setModel(qmodel);
         //connect(ui->tableView->selectionModel(),&QItemSelectionModel::currentChanged, this, &MainWindow::tableIndexChanged);
     }else{
         ui->statusbar->showMessage("Loading books failed!");
     }

}

//void MainWindow::tableIndexChanged(const QModelIndex &current, const QModelIndex &previous){
//    QAbstractItemModel *model = ui->tableView->model();
//    qDebug()<<"---curr: "<<model->data(model->index(current.row(), 0)).toString();
//    current_isbn = model->data(model->index(current.row(), 0)).toString();
//    current_book = model->data(model->index(current.row(), 1)).toString();
//    current_price = model->data(model->index(current.row(), 2)).toString();
//}


void MainWindow::on_pushButton_addToCart_clicked()
{
    QAbstractItemModel *model = ui->tableView->model();
    QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();
    if (selection.count() <= 0){
        return;
    }
    // remove duplicate rows
    QList<int> rows;
    for (int i=0; i< selection.count(); i++){
        if(!rows.contains(selection.at(i).row())){
            rows.append(selection.at(i).row());
        }
    }
    for (int i=0; i< rows.count(); i++){
        QString current_isbn = model->data(model->index(rows.at(i), 0)).toString();
        QString current_book = model->data(model->index(rows.at(i), 1)).toString();
        QString current_price = model->data(model->index(rows.at(i), 2)).toString();
        cart_map.insert(current_isbn, qMakePair(current_book, current_price));
        if (cart_amount.contains(current_isbn)){
            cart_amount.insert(current_isbn,cart_amount.value(current_isbn)+1);
        }
        else{
            cart_amount.insert(current_isbn,1);
        }
    }
    updateCart();
}

// Update cart table
void MainWindow::updateCart(){
    //clean table
    ui->tableWidget_cart->setRowCount(0);
    // set row #
    ui->tableWidget_cart->setRowCount(cart_map.keys().count());
    int i = 0;
    for (auto isbn: cart_map.keys()){
        ui->tableWidget_cart->setItem(i, 0, new QTableWidgetItem(isbn));
        ui->tableWidget_cart->setItem(i, 1, new QTableWidgetItem(cart_map.value(isbn).first));
        ui->tableWidget_cart->setItem(i, 2, new QTableWidgetItem(cart_map.value(isbn).second));
        QSpinBox *spin = new QSpinBox();
        spin->setValue(cart_amount.value(isbn));
        ui->tableWidget_cart->setCellWidget(i, 3, spin);
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
        qDebug()<<"--my cart item: "<<isbn<<cart_map.value(isbn).first<<cart_map.value(isbn).second;
        i++;
    }
    qDebug()<<"\n";
    updateTotal();
}
void MainWindow::updateTotal(){
    double total = 0;
    for (int i = 0; i < ui->tableWidget_cart->rowCount(); i++){
        double price = ui->tableWidget_cart->item(i, 2)->text().toDouble();
        int amount = qobject_cast<QSpinBox*>(ui->tableWidget_cart->cellWidget(i,3))->value();
        total += price * amount;
    }
    ui->label_total->setText("Total price: $"+QString::number(total));
}

void MainWindow::spinBox_valueChanged(int value){
    int row = ui->tableWidget_cart->currentRow();
    QString isbn = ui->tableWidget_cart->item(row, 0)->text();
    // remove the item if amount is reduced to 0
    if(value == 0){
        cart_map.remove(isbn);
        cart_amount.remove(isbn);
        updateCart();
    }
    else{
        cart_amount.insert(isbn, value);
        updateTotal();
    }
}


void MainWindow::on_pushButton_order_clicked()
{
    QString shipping_address = ui->lineEdit_order_shipping->text().trimmed();
    QString billing_address = ui->lineEdit_order_billing->text().trimmed();
    if (shipping_address.isEmpty() || billing_address.isEmpty()){
        QMessageBox::information(this, "Order is not placed", "Address is required!");
        return;
    }
    db.transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO [order] (shipping_address, billing_address, username) VALUES (?,?,?);");
    query.addBindValue(shipping_address);
    query.addBindValue(billing_address);
    query.addBindValue(current_user);
    if (!query.exec()){
        QMessageBox::information(this, "Order is not placed", "Error: order is not added to the order table!");
        return;
    }
    // get last inserted order id
    query.exec("SELECT seq FROM sqlite_sequence WHERE name = 'order';");
    int order_id =1;
    if(query.next()){
        order_id = query.value(0).toInt();
    }
    // check if stock is enough
    int stock = 0;
    for (auto isbn: cart_amount.keys()){
        query.prepare("SELECT stock FROM book where ISBN = ? ;");
        query.addBindValue(isbn);
        query.exec();
        if (query.next()){
            stock = query.value(0).toInt();
            if (stock < cart_amount.value(isbn)){
                QMessageBox::information(this, "Order is not placed", isbn+" has only "+QString::number(stock)+" left.");
                db.rollback();
                return;
            }
        }
        query.prepare("UPDATE book SET stock = ? WHERE ISBN = ? ;");
        query.addBindValue(stock - cart_amount.value(isbn));
        query.addBindValue(isbn);
        query.exec();
        // add to order_book
        query.prepare("INSERT INTO order_book VALUES (?,?,?);");
        query.addBindValue(isbn);
        query.addBindValue(order_id);
        query.addBindValue(cart_amount.value(isbn));
        query.exec();
    }

    if (db.commit()){
        QMessageBox::information(this, "Order is placed", "Order has been placed successfully! Order id: "+QString::number(order_id));
    }
}
