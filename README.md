    Acest program realizeaza un client web prin care putem comunica cu un server api si
trimite comenzi pentru a obtine informatii despre o biblioteca. Pentru realizarea acestuia,
am folosit ca schelet laboratorul 9 (fisierele buffer sunt neschimbate, dar apar mai multe functii
in fisierele de tip requests si helper) si biblioteca de parsare JSON parson, din care am inclus
doua fisiere deoarece acestea contineau functiile ajutatoare necesare pentru a parsa un string intr-un
obiect json(serialization_register_login/serialization_add) si un string formatat json
in alta formatare(show_one_book/show_multiple_books).
    Citirea se realizeaza de la stdin, iar functiile disponibile sunt register, login, enter_library,
get_books, get_book, add_book, delete_book, logout si exit. Daca niciuna dintre acestea nu este apelata, 
atunci se va afisa mesajul "Wrong command!". 
    Daca apelam register, citim de la tastatura username-ul si password-ul, si folosim functia register
pentru a crea un nou cont. Realizam acest lucru prin a ne conecta la server si a trimite un mesaj tip POST 
la api-ul /api/v1/tema/auth/register, ce contine cei doi parametri anteriori,care sunt formatati json cu functia 
serialization_register_login. Trimitem mesajul cu functia send_to_server si primim un raspuns cu functia
receive_from_server; apoi analizam ce tip de raspuns am primit: daca este "Bad Request", inseamna ca username-ul
este deja folosit, altfel se adauga noul utilizator in baza de date a serverului.
    Daca apelam login, citim de la tastatura username-ul si password-ul, si folosim functia login
pentru a ne autentifica, adica pentru a primi un cookie. Daca eram conectati inainte la alt cont, 
trebuie sa stergem cookie-ul si jwt-ul precedent. Ne conectam la server si trimitem un mesaj tip POST 
la api-ul /api/v1/tema/auth/login, ce contine cei doi parametri anteriori, formatati json cu functia 
serialization_register_login. Trimitem mesajul cu functia send_to_server si primim un raspuns cu functia
receive_from_server; apoi analizam ce tip de raspuns am primit: daca este "Bad Request", inseamna ca credentialele
nu sunt potrivite(nu se gasesc in baza de date), altfel extragem cookie-ul din header-ul Set-Cookie trimis de server si il returnam pe acesta.
    Daca apelam enter_library, apelam functia enter care va returna un token jwt sau eroare. Ne conectam la server si trimitem un mesaj tip GET 
la api-ul /api/v1/tema/library/access. Trimitem mesajul cu functia send_to_server si primim un raspuns cu functia
receive_from_server; apoi analizam ce tip de raspuns am primit: daca este "Unauthorized", inseamna ca nu suntem logati,
altfel extragem jwt-ul din mesajul json trimis de server si ii adaugam  string-ul Bearer, folositor pentru crearea mesajelor viitoare, 
si il returnam pe acesta.
    Daca apelam get_books, apelam functia get_books care va afisa toate cartile/titlurile disponibile in biblioteca noastra.
Ne conectam la server si construim un mesaj tip GET la api-ul /api/v1/tema/library/books. Trimitem mesajul cu functia send_to server 
si primim un raspuns cu functia receive_from server; apoi analizam ce tip de raspuns am primit: daca este "Forbidden", 
inseamna ca nu avem acces permis la biblioteca(nu avem un token de acces), altfel verificam daca nu avem nicio carte, in cazul in care
afisam un mesaj "The list is empty. Fill it with books!", altfel folosim functia show_multiple_books, care formateaza json-ul
in stilul field=valoare si afiseaza la stdout.
    Daca apelam get_book, citim de la stdin un id, apelam functia get_book care va afisa informatiile despre cartea ceruta sau eroare.
Ne conectam la server si construim un mesaj tip GET la api-ul /api/v1/tema/library/books/id. Trimitem mesajul cu functia send_to_server 
si primim un raspuns cu functia receive_from_server; apoi analizam ce tip de raspuns am primit: daca este "Forbidden", 
inseamna ca nu avem acces permis la biblioteca(nu avem un token de acces), altfel verificam daca primim eroarea in cazul in care cartea cu id-ul respectiv nu exista,
altfel folosim functia show_one_book, care formateaza json-ul in stilul field=valoare si afiseaza la stdout.
    Daca apelam add_book, citim de la stdin datele necesare despre carte si verificam daca field-urile au 
fost completate conform, daca au ramas necompletate sau daca nu am oferit un numar pentru page_count. 
Daca se va intampla vreun caz anterior, afisam "Invalid input". Ne conectam la server si construim un mesaj 
tip POST la api-ul /api/v1/tema/library/books. Trimitem mesajul cu functia send_to_server 
si primim un raspuns cu functia receive_from_server; apoi analizam ce tip de raspuns am primit: daca totul este ok, anuntam utilizatorul
ca aceasta carte a fost adaugata.
    Daca apelam delete_book, citim de la tastatura id-ul cartii de sters si apelam functia delete_book, care sterge cartea.
Realizam acest lucru prin a ne conecta la server si a trimite un mesaj tip DELETE cu functia compute_delete_request,
la api-ul /api/v1/tema/library/books/id. Trimitem mesajul cu functia send_to_server si primim un raspuns cu functia
receive_from_server; apoi analizam ce tip de raspuns am primit: daca este "Forbidden", 
inseamna ca nu avem acces permis la biblioteca(nu avem un token de acces), altfel verificam daca primim eroarea 
in cazul in care cartea cu id-ul respectiv e invalid, altfel am sters cu succes cartea.
    Daca apelam logout, apelam functia logout. Ne conectam la server si trimitem un mesaj tip GET,
la api-ul /api/v1/tema/auth/logout. Trimitem mesajul cu functia send_to_server si primim un raspuns cu functia
receive_from_server; apoi analizam ce tip de raspuns am primit: daca este "Bad Request", 
inseamna ca nu suntem logati, altfel ne-am delogat cu succes din cont.
    Daca apelam exit, atunci iesim din while si incheiem programul.
