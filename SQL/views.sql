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
