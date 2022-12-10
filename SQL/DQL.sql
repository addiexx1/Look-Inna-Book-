---------------------------------------- DQL ----------------------------------------------------

-- User Login --
SELECT password FROM user WHERE username = username;

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
