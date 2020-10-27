Programowanie współbieżne 
Projekt  „Reversi”    Szymon Dębski

1. Zadanie Zadanie polegało na stworzeniu gry Reversi, inaczej gry Othello. Gra rozpoczyna się ustawieniem 2 pionków białych i dwóch pionków czarnych na środku planszy. Następnie losuje się rozpoczynającego gracza, który to zaczyna grę pionkami koloru czarnego. Gracz ustawiają pionki na planszy w ten sposób, aby przejąć pionki przeciwnika,  a dzieję się to,  gdy pionki przeciwnika będą otoczone pionkami gracza. Gra kończy się, gdy gracze oboje spasują, bądź gdy wszystkie pola zostaną zajęte. Następuje wtedy przeliczenie pionków. O zwycięstwie decyduje ich ilość. 
 
 
2. Wybrana metoda współbieżności.  Do tej gry turowej wybrałem metodę pamięci współdzielonej za pomocą której to komunikują się procesy. Jest to najszybszy sposób do komunikowania się między procesami. Pierwszy proces tworzy segment pamięci współdzielonej, dowiązuje go powodując jego odwzorowanie w bieżący obszar danych procesu i zapisuje w stworzonym segmencie dane. Następnie kolejny proces, dzięki nadanym mu prawom do odczytu i zapisu  może korzystać z tego segmentu pamięci. W owym segmencie pamięci umieściłem strukturę stan gry, który posiadał potrzebne pola jak, numer rozpoczynającego  gracza i informacja o turze. 
 
 
3. Funkcjonowanie programu Interfejs, zgodnie z wymaganiami, powstał przy wykorzystaniu XLib. Przy kompilacji jest potrzebny argument -lX11. Zatem komenda wygląda następująco: gcc -o reversi reversi.c -lX11  a potem, aby uruchomić program: ./reversi Otwierając grę jako pierwsi, jesteśmy informowani, że oczekujemy na drugiego gracza, oraz już dostajemy informację, czy grami pionkami białymi czy czarnymi. Wybór gracza następuje za pomocą pseudolosowej funkcji rand(). Gry pojawi się drugi gracz, to w zależności, kto został wylosowany, rozpoczyna gracz z pionkami czarnymi. 
 
 
4. Zabezpieczenia Zostało zaimplementowanych parę elementów, które stoją na straży prawidłowego przebiegu gry. Dzięki turom, gracz może oddziaływać na grę jedynie podczas swojej kolejki i to w ściśle określony sposób, a mianowicie wykonując poprawne ruchy. Jest on informowany, gdy robi coś nie tak. Program w pamięci współdzielonej również posiada pole odpowiadające aktualnej liczbie graczy. Ma to zapobiegać dołączeniu się do gry osoby trzeciej. Gracz może wykonać tylko poprawny ruch i tylko podczas swojej kolejki. W razie wyłączenia gry przez jednego z użytkowników, drugiemu wyświetlany jest stosowny komunikat. 
 