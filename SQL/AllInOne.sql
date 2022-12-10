-------------------------------------- View ---------------------------------------------------
-- List all the books with authors and genres (up to 20) --
CREATE VIEW IF NOT EXISTS table_view 
AS 
SELECT ISBN, name, price, pub_name AS publisher, group_concat(DISTINCT(author)) AS authors,
group_concat(DISTINCT(genre)) AS genres, numPages AS pages 
FROM book 
NATURAL JOIN author 
NATURAL JOIN genre 
GROUP BY ISBN 
LIMIT 20;

-- used to report total sales, expenses, etc. --
CREATE VIEW IF NOT EXISTS report_view 
AS 
SELECT book.ISBN, price, cost, royalty, stock, total_stocked, SUM(order_amount)AS sale_amount 
FROM book, [order], order_book
WHERE book.ISBN = order_book.ISBN AND [order].order_id = order_book.order_id
GROUP by order_book.ISBN;

---------------------------------------- Trigger ------------------------------------------------
  -- initialize total_stocked when insert a new book --
CREATE TRIGGER IF NOT EXISTS init_total_stocked
AFTER INSERT ON book
BEGIN
	UPDATE book
	SET total_stocked = NEW.stock
	WHERE ISBN = NEW.ISBN;
END;

  -- update total_stocked when bookstore increases the stock on books(made purchase) --
CREATE TRIGGER IF NOT EXISTS update_total_stocked
AFTER UPDATE OF stock ON book
WHEN NEW.stock > OLD.stock
BEGIN
	UPDATE book
	SET total_stocked = OLD.total_stocked + NEW.stock - OLD.stock
	WHERE ISBN = NEW.ISBN;
END;

  -- restock 30 when stock < threshold --
CREATE TRIGGER IF NOT EXISTS restock_book
AFTER UPDATE OF stock, threshold ON book
WHEN NEW.stock < NEW.threshold
BEGIN
	UPDATE book
	SET stock = 30 + NEW.stock
	WHERE ISBN = NEW.ISBN;
END;

---------------------------------------- DQL ----------------------------------------------------
-- User Login --
SELECT password FROM user WHERE username = username;

-- User Signup --
INSERT INTO user VALUES ('input name','input pw','input fname','input lname','input address');

-- Browse all (TOP 20) books --
SELECT * FROM table_view;

-- Search books--
SELECT * FROM table_view WHERE authors LIKE '%input%' ;
SELECT * FROM table_view WHERE genres LIKE '%input%';
SELECT * FROM table_view WHERE name LIKE '%input%' ;
SELECT * FROM table_view WHERE ISBN LIKE '%input%' ;
SELECT * FROM table_view WHERE price <= input ;

-- Sort by --
   -- price low to high
SELECT * FROM table_view ORDER BY price ASC;
   -- price high to low
SELECT * FROM table_view ORDER BY price DESC;

   -- best sellers all time
SELECT ISBN, name, price, publisher, authors,genres, pages 
FROM table_view NATURAL JOIN order_book 
GROUP BY ISBN 
ORDER BY SUM(order_amount) DESC;

  -- best sellers last month
SELECT ISBN, name, price, publisher, authors,genres, pages 
FROM table_view 
NATURAL JOIN (
	SELECT ISBN, order_amount 
	FROM order_book NATURAL JOIN [order] 
	WHERE order_date BETWEEN datetime('now', '-31 days') AND datetime ('now', 'localtime')
	) 
GROUP BY ISBN ORDER BY SUM(order_amount) DESC;

-- Tracking Order--
SELECT order_date as 'order placed on', order_id AS 'order id', shipping_address AS 'shipped to', tracking_info AS tracking
FROM [order] 
WHERE order_id = input_id AND username = 'username';

-- For Place Order --
  -- get last inserted auto-increment order id
SELECT seq FROM sqlite_sequence WHERE name = 'order';
SELECT stock FROM book where ISBN = ? ;

-- Admin Report --
SELECT SUM(book.price * sale_amount) as total_sales, 
SUM(book.cost * book.total_stocked)+SUM(book.price * sale_amount * book.royalty) as total_expenses,
SUM(book.price * sale_amount*book.royalty) as total_royalties_paid, 
SUM(book.cost * book.total_stocked) as total_purchases_made,
SUM(book.price * book.stock) as total_assets, 
SUM(book.stock) as total_book_in_stock, 
SUM(sale_amount) as total_book_sold
FROM book LEFT JOIN report_view ON book.ISBN = report_view.ISBN;

-- sales by genre
SELECT genre as Genres, SUM(sale_amount*price) as Sales
FROM genre LEFT JOIN report_view
ON genre.ISBN = report_view.ISBN
GROUP BY genre
ORDER BY Sales DESC;

-- sales by author
SELECT author as Authors, SUM(sale_amount*price) as Sales
FROM author LEFT JOIN report_view
ON author.ISBN = report_view.ISBN
GROUP BY author
ORDER BY Sales DESC;

---------------------------------------- DDL ----------------------------------------------------
CREATE TABLE IF NOT EXISTS publisher (
	pub_name	TEXT NOT NULL,
	address	TEXT NOT NULL,
	email	TEXT NOT NULL,
	bank_account	TEXT NOT NULL UNIQUE,
	PRIMARY KEY(pub_name)
);
CREATE TABLE IF NOT EXISTS user (
	username	TEXT NOT NULL,
	password	TEXT NOT NULL,
	fname	TEXT NOT NULL,
	lname	TEXT NOT NULL,
	address	TEXT NOT NULL,
	PRIMARY KEY(username)
);
CREATE TABLE IF NOT EXISTS book (
	ISBN	TEXT NOT NULL,
	name	TEXT NOT NULL,
	numPages	INTEGER NOT NULL CHECK(numPages>0),
	cost	NUMERIC NOT NULL CHECK(cost>=0),
	price	NUMERIC NOT NULL CHECK(price>=0),
	stock	INTEGER NOT NULL CHECK(stock>=0),
	royalty	NUMERIC NOT NULL CHECK(royalty>=0),
	threshold	INTEGER NOT NULL CHECK(threshold>=0),
	pub_name	TEXT NOT NULL,
	total_stocked	INTEGER NOT NULL DEFAULT 0,
	PRIMARY KEY(ISBN),
	FOREIGN KEY(pub_name) REFERENCES publisher(pub_name) ON DELETE RESTRICT ON UPDATE CASCADE
);
CREATE TABLE IF NOT EXISTS pub_phone (
	pub_name	TEXT NOT NULL,
	phone	TEXT NOT NULL,
	PRIMARY KEY(pub_name,phone)
);
CREATE TABLE IF NOT EXISTS genre (
	genre	TEXT NOT NULL,
	ISBN	TEXT NOT NULL,
	PRIMARY KEY(genre,ISBN),
	FOREIGN KEY(ISBN) REFERENCES book(ISBN) ON DELETE CASCADE ON UPDATE CASCADE
);
CREATE TABLE IF NOT EXISTS author (
	author	TEXT NOT NULL,
	ISBN	TEXT NOT NULL,
	FOREIGN KEY(ISBN) REFERENCES book(ISBN) ON DELETE CASCADE ON UPDATE CASCADE,
	PRIMARY KEY(author,ISBN)
);
CREATE TABLE IF NOT EXISTS [order] (
	order_id	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	shipping_address	TEXT NOT NULL,
	billing_address	TEXT NOT NULL,
	tracking_info	TEXT NOT NULL DEFAULT 'Order placed',
	order_date	TEXT NOT NULL DEFAULT CURRENT_DATE,
	username	TEXT NOT NULL,
	FOREIGN KEY(username) REFERENCES user(username) ON DELETE CASCADE ON UPDATE CASCADE
);
CREATE TABLE IF NOT EXISTS order_book (
	ISBN	TEXT NOT NULL,
	order_id	INTEGER NOT NULL,
	order_amount	INTEGER NOT NULL CHECK(order_amount>0),
	FOREIGN KEY(ISBN) REFERENCES book(ISBN) ON DELETE CASCADE ON UPDATE CASCADE,
	PRIMARY KEY(ISBN,order_id),
	FOREIGN KEY(order_id) REFERENCES [order](order_id) ON DELETE CASCADE ON UPDATE CASCADE
);

---------------------------------------- DML ----------------------------------------------------
-- clean table data --
DELETE FROM order_book;
DELETE FROM [order];
DELETE FROM author;
DELETE FROM genre;
DELETE FROM pub_phone;
DELETE FROM book;
DELETE FROM publisher;
DELETE FROM [user];

-- insert dummy data --
INSERT INTO publisher VALUES ('Continent Inc.','53 Grant Ave Saint Paul, MN 55104','contact@continent.com','0789-3332231');
INSERT INTO publisher VALUES ('Simon & Schuster','236 Clinton St. Westlake, ON 44145','contact@ss.com','0013-78900221');
INSERT INTO publisher VALUES ('Pearson Education','12 Vermont St. Ovi, FL 32765','contactus@pearson.com','0900-90129388');
INSERT INTO publisher VALUES ('Marvel Comics','22 Garfield Ave.Vland, NJ 08360','sales@marvelcomics.com','0003-99099099');

INSERT INTO user VALUES ('student','student','John','Doe','682 NE. Annadale Drive, NJ 07083');

INSERT INTO book VALUES ('0-4468-4857-3','Star Wars: Bounty Hunters',230,19.95,29.95,50,0.1,10,'Marvel Comics', 0);
INSERT INTO book VALUES ('0-1704-1348-9','Intro to Calculus II',450,29.95,46.95,40,0.15,10,'Pearson Education', 0);
INSERT INTO book VALUES ('0-6257-1942-5','Intro to Calculus I',550,29.95,49.95,30,0.15,10,'Pearson Education', 0);
INSERT INTO book VALUES ('0-8521-1367-6','Star Wars: Doctor Aphra',178,15.5,24.5,50,0.1,10,'Marvel Comics', 0);
INSERT INTO book VALUES ('0-1104-5785-4','Shang-Chi and the Ten Rings',155,20,29.95,50,0.1,10,'Marvel Comics', 0);
INSERT INTO book VALUES ('0-5287-4530-1','The Moonlight',390,15.95,25.95,30,0.1,10,'Continent Inc.', 0);
INSERT INTO book VALUES ('0-1144-0720-7','Apollo 11: Landing on the Moon',360,20.05,30.05,30,0.1,10,'Simon & Schuster', 0);
INSERT INTO book VALUES ('0-8075-3873-6','Ten Things We Did',290,13,23,35,0.1,10,'Continent Inc.', 0);
INSERT INTO book VALUES ('0-5464-5226-4','Pre-Calculus 12 Student Book',655,78,159,50,0.2,10,'Pearson Education', 0);
INSERT INTO book VALUES ('0-9759-5419-9','Me Before You',298,15.8,28.5,70,0.15,10,'Simon & Schuster', 0);
INSERT INTO book VALUES ('0-8472-7748-8','The Hating Game',386,20,30,30,0.15,10,'Simon & Schuster', 0);
INSERT INTO book VALUES ('0-3774-7473-8','Ender''s Game (comics)',252,19,28,45,0.15,10,'Marvel Comics', 0);

INSERT INTO pub_phone VALUES ('Continent Inc.','3456782899');
INSERT INTO pub_phone VALUES ('Continent Inc.','7789901234');
INSERT INTO pub_phone VALUES ('Simon & Schuster','8890091928');
INSERT INTO pub_phone VALUES ('Pearson Education','4035569992');
INSERT INTO pub_phone VALUES ('Marvel Comics','18902234556');
INSERT INTO pub_phone VALUES ('Marvel Comics','3078890090');

INSERT INTO genre VALUES ('fiction','0-4468-4857-3');
INSERT INTO genre VALUES ('textbook','0-1704-1348-9');
INSERT INTO genre VALUES ('textbook','0-6257-1942-5');
INSERT INTO genre VALUES ('fiction','0-8521-1367-6');
INSERT INTO genre VALUES ('fiction','0-1104-5785-4');
INSERT INTO genre VALUES ('novel','0-5287-4530-1');
INSERT INTO genre VALUES ('narrative','0-1144-0720-7');
INSERT INTO genre VALUES ('non-fiction','0-1144-0720-7');
INSERT INTO genre VALUES ('short story','0-8075-3873-6');
INSERT INTO genre VALUES ('non-fiction','0-8075-3873-6');
INSERT INTO genre VALUES ('textbook','0-5464-5226-4');
INSERT INTO genre VALUES ('romance novel','0-9759-5419-9');
INSERT INTO genre VALUES ('romance novel','0-8472-7748-8');
INSERT INTO genre VALUES ('fiction','0-3774-7473-8');
INSERT INTO genre VALUES ('comics','0-3774-7473-8');
INSERT INTO genre VALUES ('non-fiction','0-5287-4530-1');

INSERT INTO author VALUES ('John Wall','0-4468-4857-3');
INSERT INTO author VALUES ('Kevin L.','0-4468-4857-3');
INSERT INTO author VALUES ('Alysha Hart','0-1704-1348-9');
INSERT INTO author VALUES ('Khalil Rios','0-1704-1348-9');
INSERT INTO author VALUES ('Alysha Hart','0-6257-1942-5');
INSERT INTO author VALUES ('Maya Bell','0-8521-1367-6');
INSERT INTO author VALUES ('Sam Lee','0-1104-5785-4');
INSERT INTO author VALUES ('Ned Dawson','0-5287-4530-1');
INSERT INTO author VALUES ('Diana Levy','0-1144-0720-7');
INSERT INTO author VALUES ('Margaret H.','0-1144-0720-7');
INSERT INTO author VALUES ('Amirah Lyons','0-8075-3873-6');
INSERT INTO author VALUES ('Rufus Ali','0-5464-5226-4');
INSERT INTO author VALUES ('Sid Heath','0-9759-5419-9');
INSERT INTO author VALUES ('Sally Thorne','0-8472-7748-8');
INSERT INTO author VALUES ('Orson S. Card','0-3774-7473-8');
INSERT INTO author VALUES ('Khalil Rios','0-6257-1942-5');
INSERT INTO author VALUES ('John Wall','0-8521-1367-6');

INSERT INTO [order] VALUES (1,'8355 Williams St.Vista, CA 92083','682 NE. Annadale Drive, NJ 07083','Order shipped','2022-11-29','student');
INSERT INTO [order] VALUES (2,'682 NE. Annadale Drive, NJ 07083','682 NE. Annadale Drive, NJ 07083','In transit','2022-12-07','student');
INSERT INTO [order] VALUES (3,'8355 Williams St.Vista, CA 92083','8355 Williams St.Vista, CA 92083','Order shipped','2022-10-20','student');

INSERT INTO order_book VALUES ('0-4468-4857-3',1,20);
INSERT INTO order_book VALUES ('0-1704-1348-9',1,20);
INSERT INTO order_book VALUES ('0-6257-1942-5',1,30);
INSERT INTO order_book VALUES ('0-8521-1367-6',1,20);
INSERT INTO order_book VALUES ('0-1104-5785-4',1,30);
INSERT INTO order_book VALUES ('0-5287-4530-1',1,10);
INSERT INTO order_book VALUES ('0-1144-0720-7',1,15);
INSERT INTO order_book VALUES ('0-8075-3873-6',1,30);
INSERT INTO order_book VALUES ('0-5464-5226-4',1,25);
INSERT INTO order_book VALUES ('0-9759-5419-9',1,50);
INSERT INTO order_book VALUES ('0-8472-7748-8',1,5);
INSERT INTO order_book VALUES ('0-3774-7473-8',2,5);
INSERT INTO order_book VALUES ('0-3774-7473-8',3,50);

-- dml to update tables -- 
UPDATE book SET stock = ? WHERE ISBN = ? ;
-- dml to delete rows
DELETE FROM book WHERE ISBN = ?;



