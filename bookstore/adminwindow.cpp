#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::database();

    if (!db.open()){
        QMessageBox::information(this, "Database Connection", "Connection to SQLite Database Failed!");
    }else{
        ui->statusbar->showMessage("Connected to "+db.databaseName());
        qmodel = new QSqlQueryModel();
        gmodel = new QSqlQueryModel();
        amodel = new QSqlQueryModel();
        init();

    }
}

AdminWindow::~AdminWindow()
{
    delete qmodel;
    delete ui;
}

void AdminWindow::init(){
    // populating existing publishers
    QSqlQuery q;
    q.exec("SELECT pub_name FROM publisher; ");
    while (q.next()){
        ui->comboBox_publishers->addItem(q.value(0).toString());
    }
    ui->lineEdit_pub_name->setDisabled(true);
    ui->lineEdit_pub_email->setDisabled(true);
    ui->lineEdit_pub_address->setDisabled(true);
    ui->lineEdit_pub_phone1->setDisabled(true);
    ui->lineEdit_pub_phone2->setDisabled(true);
    ui->lineEdit_pub_bank->setDisabled(true);
    udpateReport();
}
void AdminWindow::on_pushButton_remove_clicked()
{
    QString isbn = ui->lineEdit_remove->text().trimmed();
    if (isbn.isEmpty()){
        ui->statusbar->showMessage("ISBN is required to remove a book!");
        return;
    }
    else{
        QSqlQuery query;
        query.prepare("SELECT * FROM book WHERE ISBN = ?;");
        query.addBindValue(isbn);
        query.exec();
        if(query.next()){
            query.prepare("DELETE FROM book WHERE ISBN = ?;");
            query.addBindValue(isbn);
            query.exec();
            QMessageBox::information(this, "Success", isbn +" is removed!");
        }else{
            QMessageBox::information(this, "Invalid ISBN", isbn +" doesn't exist!");
        }
    }
}

void AdminWindow::on_pushButton_add_clicked()
{
    QString isbn = ui->lineEdit_isbn->text().trimmed();
    QString name = ui->lineEdit_name->text().trimmed();
    QString pages = ui->lineEdit_pages->text().trimmed();
    QString stock = ui->lineEdit_stock->text().trimmed();
    QString cost = ui->lineEdit_cost->text().trimmed();
    QString price = ui->lineEdit_price->text().trimmed();
    QString threshold = ui->lineEdit_threshold->text().trimmed();
    QString royalty = ui->lineEdit_royalty->text().trimmed();
    if (isbn.isEmpty() || name.isEmpty()||pages.isEmpty()||stock.isEmpty()||cost.isEmpty()||price.isEmpty()||threshold.isEmpty()||royalty.isEmpty()){
        ui->statusbar->showMessage("All book fields are required to add a book!");
        return;
    }

    QString pub_name = ui->lineEdit_pub_name->text().trimmed();
    QString pub_email = ui->lineEdit_pub_email->text().trimmed();
    QString pub_address = ui->lineEdit_pub_address->text().trimmed();
    QString pub_phone1 = ui->lineEdit_pub_phone1->text().trimmed();
    QString pub_phone2 = ui->lineEdit_pub_phone2->text().trimmed();
    QString pub_bank = ui->lineEdit_pub_bank->text().trimmed();
    QString author1 = ui->lineEdit_author1->text().trimmed();
    QString author2 = ui->lineEdit_author2->text().trimmed();
    QString genre1 = ui->lineEdit_genre1->text().trimmed();
    QString genre2 = ui->lineEdit_genre2->text().trimmed();
    if (author1.isEmpty() && author2.isEmpty()){
        ui->statusbar->showMessage("Add at least one author!");
        return;
    }
    if (genre1.isEmpty() && genre2.isEmpty()){
        ui->statusbar->showMessage("Add at least one genre!");
        return;
    }
    db.transaction();
    QSqlQuery q;
    q.prepare("SELECT * FROM book WHERE ISBN =?;");
    q.addBindValue(isbn);
    q.exec();
    if(q.next()){
        QMessageBox::information(this, "Invalid ISBN", isbn +" already exists!");
        return;
    }
    if (ui->radioButton_new->isChecked()){
        if(pub_name.isEmpty()||pub_email.isEmpty()||pub_address.isEmpty()||pub_bank.isEmpty()){
            ui->statusbar->showMessage("Publisher fields are required!");
            return;
        }
        if(pub_phone1.isEmpty() && pub_phone2.isEmpty()){
            ui->statusbar->showMessage("Need at least one publisher's phone!");
            return;
        }

        q.prepare("INSERT INTO publisher VALUES (?,?,?,?);");
        q.addBindValue(pub_name);
        q.addBindValue(pub_address);
        q.addBindValue(pub_email);
        q.addBindValue(pub_bank);
        if (q.exec()){
           q.prepare("INSERT INTO pub_phone VALUES (?,?);");
           q.addBindValue(pub_name);
           if(!pub_phone1.isEmpty()){
               q.addBindValue(pub_phone1);
               q.exec();
           }
           if(!pub_phone2.isEmpty()){
               q.prepare("INSERT INTO pub_phone VALUES (?,?);");
               q.addBindValue(pub_name);
               q.addBindValue(pub_phone2);
               q.exec();
           }
        }
    }
    else{
        pub_name = ui->comboBox_publishers->currentText();
    }
    q.prepare("INSERT INTO book (ISBN, name, numPages, cost, price, stock, royalty, threshold, pub_name)\
                VALUES (?,?,?,?,?,?,?,?,?)");
    q.addBindValue(isbn);
    q.addBindValue(name);
    q.addBindValue(pages);
    q.addBindValue(cost);
    q.addBindValue(price);
    q.addBindValue(stock);
    q.addBindValue(royalty);
    q.addBindValue(threshold);
    q.addBindValue(pub_name);
    q.exec();

    if (db.commit()){
        q.prepare("INSERT INTO author VALUES (?,?);");
        if (!author1.isEmpty()){
            q.addBindValue(author1);
            q.addBindValue(isbn);
            q.exec();
        }
        if (!author2.isEmpty()){
            q.addBindValue(author2);
            q.addBindValue(isbn);
            q.exec();
        }
        q.prepare("INSERT INTO genre VALUES (?,?);");
        if (!genre1.isEmpty()){
            q.addBindValue(genre1);
            q.addBindValue(isbn);
            q.exec();
        }
        if (!genre2.isEmpty()){
            q.addBindValue(genre2);
            q.addBindValue(isbn);
            q.exec();
        }
        ui->statusbar->showMessage("New book is added!");
        QMessageBox::information(this, "Success", isbn +" new book is added!");
    }
}
void AdminWindow::on_radioButton_new_toggled(bool checked)
{
    if (checked){
        ui->comboBox_publishers->setDisabled(true);
        ui->lineEdit_pub_name->setDisabled(false);
        ui->lineEdit_pub_email->setDisabled(false);
        ui->lineEdit_pub_address->setDisabled(false);
        ui->lineEdit_pub_phone1->setDisabled(false);
        ui->lineEdit_pub_phone2->setDisabled(false);
        ui->lineEdit_pub_bank->setDisabled(false);
    }else{
        ui->comboBox_publishers->setDisabled(false);
        ui->lineEdit_pub_name->setDisabled(true);
        ui->lineEdit_pub_email->setDisabled(true);
        ui->lineEdit_pub_address->setDisabled(true);
        ui->lineEdit_pub_phone1->setDisabled(true);
        ui->lineEdit_pub_phone2->setDisabled(true);
        ui->lineEdit_pub_bank->setDisabled(true);
    }
}


void AdminWindow::on_pushButton_clicked()
{
    udpateReport();
}
void AdminWindow::udpateReport()
{
    QSqlQuery q;
    q.exec("SELECT SUM(book.price * sale_amount) as total_sales,    \
           SUM(book.cost * book.total_stocked)+SUM(book.price * sale_amount * book.royalty) as total_expenses,  \
           SUM(book.price * sale_amount*book.royalty) as total_royalties_paid, \
           SUM(book.cost * book.total_stocked) as total_purchases_made, \
           SUM(book.price * book.stock) as total_assets, \
           SUM(book.stock) as total_book_in_stock, \
           SUM(sale_amount) as total_book_sold  \
           FROM book LEFT JOIN report_view ON book.ISBN = report_view.ISBN;");
    qmodel->setQuery(q);
    ui->tableView_report->setModel(qmodel);

    q.exec("SELECT genre as Genres, SUM(sale_amount*price) as Sales \
           FROM genre LEFT JOIN report_view \
           ON genre.ISBN = report_view.ISBN \
           GROUP BY genre   \
           ORDER BY Sales DESC;");  \
    gmodel->setQuery(q);
    ui->tableView_genre_report->setModel(gmodel);

    q.exec("SELECT author as Authors, SUM(sale_amount*price) as Sales   \
           FROM author LEFT JOIN report_view    \
           ON author.ISBN = report_view.ISBN    \
           GROUP BY author  \
           ORDER BY Sales DESC;");
    amodel->setQuery(q);
    ui->tableView_author_report->setModel(amodel);

}
