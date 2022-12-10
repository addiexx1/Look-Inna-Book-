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
