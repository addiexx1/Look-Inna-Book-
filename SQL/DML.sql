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


