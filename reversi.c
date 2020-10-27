#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<sys/shm.h>
#include<signal.h>
#include<unistd.h>
#include "Xlib.h"

int inicjuj_gre()
{
  if ((shm_plansza_id = shmget(Klucz_1, 1024, 0666 | IPC_CREAT | IPC_EXCL)) != -1)
  {
    srand(time(NULL));
    losowa_1_albo_2 = (rand()% 2) + 1;
    printf("\nRozpoczyna gracz = %d\n", losowa_1_albo_2);
    printf("\nStart - gracz 1\n");
    gracz = 1;
    przeciwnik = 2;
    shm_stan_gry_id = shmget(Klucz_2, sizeof(stan_gry_t), 0666 | IPC_CREAT);
  }
  else
  {
    printf("\nStart - gracz 2\n");
    gracz = 2;
    przeciwnik = 1;
    shm_plansza_id = shmget(Klucz_1, 1024, 0666 | IPC_CREAT);
    shm_stan_gry_id = shmget(Klucz_2, sizeof(stan_gry_t), 0666 | IPC_CREAT);
  }
  int* shm_plansza = shmat(shm_plansza_id, 0, 0);
  stan_gry = shmat(shm_stan_gry_id, 0, 0);
  plansza_do_gry = malloc(rozmiar_planszy * sizeof(plansza_do_gry[0]));

  if (stan_gry->gracz_licznik >= 2)
  {
    printf("\nWystepuje juz para graczy.\n");
    exit(-1);
  }

  for (int i = 0; i < rozmiar_planszy; i++)
  {
    plansza_do_gry[i] = shm_plansza + i * rozmiar_planszy;
  }

  if (gracz == 1)
  {
    for (int i = 0; i < rozmiar_planszy; i++)
    {
      for (int j = 0; j < rozmiar_planszy; j++)
      {
        plansza_do_gry[i][j] = Puste;
      }
    }
    plansza_do_gry[3][3] = Biale;
    plansza_do_gry[4][4] = Biale;
    plansza_do_gry[3][4] = Czarne;
    plansza_do_gry[4][3] = Czarne;

    stan_gry->gracz_licznik = 0;
    stan_gry->tura_gracza = gracz;
    stan_gry->gracz_rozpoczynajacy_gre = losowa_1_albo_2;
    stan_gry->czy_wystapilo_spasowanie = 0;
    twoj_kolor = (losowa_1_albo_2 == 1) ? Czarne : Biale;
    printf("Prowadzisz pokoj z gra.\n");
  }
  else twoj_kolor = (stan_gry->gracz_rozpoczynajacy_gre == 2) ? Czarne : Biale;

  stan_gry->gracz_licznik += 1;
  printf("\nstan graczy: %d", stan_gry->gracz_licznik);
}

void usun_segementy(int signal)
{
  stan_gry->tura_gracza = -przeciwnik;
  stan_gry->gracz_licznik -= 1;
  shmctl(shm_plansza_id, IPC_RMID, 0);
  shmctl(shm_stan_gry_id, IPC_RMID, 0);
  exit(0);
}

void usun_okno()
{
  printf("\nusun_okno");
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}

void inicjuj_Xlib()
{
  display = XOpenDisplay("");
  if(display == NULL)
  {
    printf("Nie mozna wyswieltic\n");
    usun_segementy(0);
  }

  screen = DefaultScreen(display);
  visual = DefaultVisual(display, screen);
  depth = DefaultDepth(display, screen);


  if(display == NULL)
  {
    printf("Nie mozna wyswieltic\n");
    usun_segementy(0);
  }

  gc = DefaultGC(display, screen);
  window = XCreateSimpleWindow(display,RootWindow(display, screen),0,0,plansza_pion + 2 * margin_poziom, plansza_pion + 3 * margin_pion,1,BlackPixel(display, screen),WhitePixel(display, screen));
  Atom delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(display, window, &delete_window, 1);
  XGrabPointer(display, window, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
  XMapWindow(display, window);
  colormap = DefaultColormap(display, screen);
  x11_file_descriptor = ConnectionNumber(display);
}


void stworz_plansze_reversi()
{
  for( int i = 0; i< rozmiar_planszy; i++)
  {
    for (int j = 0; j < rozmiar_planszy; j++)
    {
      switch (plansza_do_gry[i][j])
      {
        case Puste:
            XAllocNamedColor(display, colormap, "green", &color, &exact_color);
            break;
        case Biale:
            XAllocNamedColor(display, colormap, "white", &color, &exact_color);
            break;
        case Czarne:
            XAllocNamedColor(display, colormap, "black", &color, &exact_color);
            break;
      }
      XSetForeground(display, gc, color.pixel);
      int x1 = margin_poziom + i * rozmiar_komorki + 1;
      int y1 = margin_pion + j * rozmiar_komorki + 1;
      int width = rozmiar_komorki - 1;
      int height = rozmiar_komorki - 1;
      XFillRectangle(display, window, gc, x1, y1, width, height);
    }
  }
}

void stworz_teksty()
{
  char* naglowek;

  if (biale_liczba_str != NULL){
    free(biale_liczba_str);
    biale_liczba_str = (char *)malloc(10);
  } else biale_liczba_str = (char *)malloc(10);

  if (czarne_liczba_str != NULL){
    free(czarne_liczba_str);
    czarne_liczba_str = (char *)malloc(10);
  } else czarne_liczba_str = (char *)malloc(10);

  sprintf(czarne_liczba_str, "%d", liczba_czarnych);
  sprintf(biale_liczba_str, "%d", liczba_bialych);

  if( gracz == 1){
    if( stan_gry->gracz_rozpoczynajacy_gre == 1)
    naglowek = "Gracz 1 - Czarne ( rozpoczyna )";
    else naglowek = "Gracz 1 - Biale ( nie rozpoczyna )";
  }else {
    if( stan_gry->gracz_rozpoczynajacy_gre == 2)
    naglowek = "Gracz 2 - Czarne ( rozpoczyna )";
    else naglowek = "Gracz 2 - Biale ( nie rozpoczyna )";
  }
  XAllocNamedColor(display, colormap, "black", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  int x = 0;
  int y = plansza_pion + margin_pion * 2;
  int width = (plansza_pion + margin_poziom) + margin_poziom;
  int height = margin_pion;
  XFillRectangle(display, window, gc, x, y, width, height);

  XAllocNamedColor(display, colormap, "red", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  x = margin_poziom;
  y = plansza_pion + margin_pion * 3 - 5;
  XDrawString(display, window, gc, x, y, info_dla_gracza, strlen(info_dla_gracza));
  XDrawString(display, window, gc, x, margin_pion - 5, naglowek, strlen(naglowek)); // nazwa gracza

  XAllocNamedColor(display, colormap, "yellow", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  XFillRectangle(display, window, gc, x + 75, 0, 20, 20);

  XAllocNamedColor(display, colormap, "red", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);
  XDrawString(display, window, gc, x, margin_pion - 20, czarne_tytul, strlen(czarne_tytul)); //czarne licznik punktow
  XDrawString(display, window, gc, x + 80, margin_pion - 20, czarne_liczba_str, strlen(czarne_liczba_str)); //biale licznik punktow

  XDrawString(display, window, gc, x, margin_pion - 30, biale_tytul, strlen(biale_tytul)); //biale licznik punktow
  XDrawString(display, window, gc, x + 80, margin_pion - 30, biale_liczba_str, strlen(biale_liczba_str)); //biale licznik punktow

}

void przejmij_pionki_gracza(int x, int y, int wektor_x, int wektor_y, int przeciwny_kolor){
  plansza_do_gry[x][y] = twoj_kolor;
  int poprzednia_komorka = -1;
  int sprawdzana_komorka;
  for(int i = 1; i<8; i++){
    if ( ( (x+wektor_x*i) >= 0 && (x+wektor_x*i) <= 7) && ( (y+wektor_y*i) >= 0 && (y+wektor_y*i) <= 7) ) {
      sprawdzana_komorka = plansza_do_gry[x + wektor_x * i][y + wektor_y * i];
      if (sprawdzana_komorka == przeciwny_kolor) {
        poprzednia_komorka = sprawdzana_komorka;
        plansza_do_gry[x + wektor_x * i][y + wektor_y * i] = twoj_kolor;
      }
      if(sprawdzana_komorka == twoj_kolor && poprzednia_komorka == przeciwny_kolor) break;
    }
  } 
}


int sprawdz_czy_mozna_i_przejmij(int x, int y, int wektor_y, int wektor_x, int flaga){
  int poprzednia_komorka = -1;
  int sprawdzana_komorka; // sprawdz czy kolejna komorka w danym kierunku istnieje, a potem popatrz, czy nastepuje tam ciag 
  int przeciwny_kolor = (twoj_kolor == Czarne) ? Biale : Czarne;
  for(int i = 1; i < 8; i++){
    if ( ( (x+wektor_x*i) >= 0 && (x+wektor_x*i) <= 7) && ( (y+wektor_y*i) >= 0 && (y+wektor_y*i) <= 7) ) {
        sprawdzana_komorka = plansza_do_gry[x+wektor_x*i][y+wektor_y*i];
        if ( sprawdzana_komorka == przeciwny_kolor ){
          poprzednia_komorka = sprawdzana_komorka;
        } else if ( (sprawdzana_komorka == twoj_kolor) && (poprzednia_komorka == przeciwny_kolor) ){
          if( flaga == 1)
            przejmij_pionki_gracza(x, y, wektor_x, wektor_y, przeciwny_kolor);
          return 1;
        } 
        else return 0;
      }
    else return 0;
  }
}

void policz_pionki(){
  int stan;
  int pomoc_czarne = 0;
  int pomoc_biale = 0;
  for(int i = 0; i < rozmiar_planszy; i++ ) {
    for(int j = 0; j< rozmiar_planszy; j++) {
        stan = plansza_do_gry[i][j];
        if (stan == Biale) ++pomoc_biale;
        else if( stan == Czarne) ++pomoc_czarne;
    }
  }
  liczba_bialych = pomoc_biale;
  liczba_czarnych = pomoc_czarne;
}

bool czy_plansza_pelna () {
  policz_pionki();
  if(liczba_bialych + liczba_czarnych == 64)
    return true;
  else return false;
}

void stworz_gre()
{
  policz_pionki();
  X_stworz_plansze();
  stworz_plansze_reversi();
  stworz_teksty();
  XFlush(display);
}


bool sprawdz_czy_mozna(int i, int j, int flag){
  int poszukiwane = ( twoj_kolor == Biale) ? Czarne : Biale;
  int tablica[] = {0, 0, 0, 0, 0, 0, 0, 0};
  // 0 - N
  // 1 - NE
  // 2 - E
  // 3 - SE
  // 4 - S
  // 5 - SW 
  // 6 - W
  // 7 - NW
  if(plansza_do_gry[i][j] == Puste){
    tablica[0] = sprawdz_czy_mozna_i_przejmij(i, j, -1, 0, flag);
    tablica[1] = sprawdz_czy_mozna_i_przejmij(i, j, -1, 1, flag);
    tablica[2] = sprawdz_czy_mozna_i_przejmij(i, j, 0, 1, flag);
    tablica[3] = sprawdz_czy_mozna_i_przejmij(i, j, 1, 1, flag);
    tablica[4] = sprawdz_czy_mozna_i_przejmij(i, j, 1, 0, flag);
    tablica[5] = sprawdz_czy_mozna_i_przejmij(i, j, -1, -1, flag);
    tablica[6] = sprawdz_czy_mozna_i_przejmij(i, j, 0, -1, flag);
    tablica[7] = sprawdz_czy_mozna_i_przejmij(i, j, -1, -1, flag);
  }
  for(int i = 0; i < 8; i++){
    if (tablica[i] == 1) return true;
  }
  return false;
}

int wybierz_pole(kordynaty_t wybrane_pole)
{
  czy_wykonales_poprawny_ruch = false;

  if (wybrane_pole.plansza == Poza_Plansza)
  {
    printf("Nie w plansze\n");
    info_dla_gracza = "Kliknij w plansze";
    return Poza_Plansza;
  }

  int pole = plansza_do_gry[wybrane_pole.x][wybrane_pole.y];
  if (wybrane_pole.plansza == Plansza)
  {
    if (pole == Biale)
    {
      printf("\nTrafiles w pole Biale\n");
      info_dla_gracza = "Biale. Wybierz pole przy polu przeciwnika";
    }
    if (pole == Czarne)
    {
      printf("\nTrafiles w pole Czarne\n");
      info_dla_gracza = "Czarne. Wybierz pole przy polu przeciwnika.";
    }

    int x = wybrane_pole.x;
    int y = wybrane_pole.y;
    if (pole == Puste)
    {
      if(sprawdz_czy_mozna(x, y, 1)) {
        printf("\nDobrze! Wykonuje ruch\n");
        info_dla_gracza = "Dobrze, wykonujesz ruch";
        czy_wykonales_poprawny_ruch = true;
      } else {
        printf("\nNiestety nie mozna tu wykonac ruchu.\n");
        info_dla_gracza = "Blad, nie mozesz tu wykonac ruchu";
      }
    }
  }
}

  
int ustaw_stan_poczatkowy()
{
  if (stan_gry->gracz_licznik == 1)
    info_dla_gracza = "Oczekiwanie na drugiego gracza";
  else {
    info_dla_gracza= "Kliknij, aby rozpoczac";
    while (true) {
      stworz_gre();
      XNextEvent(display, &event);
      if(event.type == ButtonPress)
      {
        break;
      }
      if(event.type == ClientMessage)
      {
        printf("\nZamkniecie okna\n");
        stan_gry->tura_gracza = -przeciwnik;
        return -1;
      }
    }
  }

  while (stan_gry->gracz_licznik < 2)
  {
    stworz_gre();
    XNextEvent(display, &event);
    if(event.type == ButtonPress)
    {
      info_dla_gracza = "Poczekaj na drugiego gracza";
    }
    if(event.type == ClientMessage)
    {
      printf("\nZamkniecie okna\n");
      stan_gry->tura_gracza = -przeciwnik;
      return -1;
    }
  }
}

bool czy_mozna_wykonac_ruch(){
  for (int i = 0; i < rozmiar_planszy; i++){
    for (int j = 0; j < rozmiar_planszy; j++){
      if( plansza_do_gry[i][j] == Puste){
        if (sprawdz_czy_mozna(i, j, 0) ) return true;
      }
    }
  }
  return false;
}

void rozstrzygnij_gre(){
  int wygrane = (liczba_czarnych >= liczba_bialych) ? Czarne : Biale;
  if(liczba_bialych == liczba_czarnych)
    wygrane == -1;
  if( wygrane == -1){
    stan_gry->tura_gracza = -3;
  } else {
    if( twoj_kolor == wygrane ){
      stan_gry->tura_gracza = -gracz;
    } else stan_gry->tura_gracza = -przeciwnik;
  }
}


void gra()
{
  fd_set in_fds;
  info_dla_gracza = "Gra Start!";
  while(stan_gry->tura_gracza > 0)
  {
    FD_ZERO(&in_fds);
    FD_SET(x11_file_descriptor, &in_fds);
    tv.tv_usec = 0;
    tv.tv_sec = 1;

    int num_ready_fds = select(x11_file_descriptor + 1, &in_fds, NULL, NULL, &tv);
    if (num_ready_fds == 0)
    {
        if (stan_gry->tura_gracza == gracz)
        {
          info_dla_gracza = "Twoj ruch";
          stworz_gre();
        }
        else {
          info_dla_gracza = "Ruch przeciwnika";
          stworz_gre();
        }
    }

    while(XPending(display))
    {
      XNextEvent(display, &event);
      if(event.type == ButtonPress)
      {
        if (stan_gry->tura_gracza == gracz)
        {
          if ( czy_mozna_wykonac_ruch() ) {
            stan_gry->czy_wystapilo_spasowanie = 0;
            wybierz_pole(X_okresl_gdzie_kliknal_gracz(event.xbutton.x, event.xbutton.y));
            if(czy_wykonales_poprawny_ruch){
              if (czy_plansza_pelna()) {
                rozstrzygnij_gre();
              }
              stan_gry->tura_gracza = przeciwnik;
            }
          } else {
            stan_gry->czy_wystapilo_spasowanie++;
            if (stan_gry->czy_wystapilo_spasowanie == 2)
              rozstrzygnij_gre();
            stworz_gre();
            info_dla_gracza = "\nBrak mozliwosci wykonania ruchu\n";
          }
          stworz_gre();
        }
      }
      if(event.type == ClientMessage)
      {
        printf("\nOkno zamkniete\n");
        stan_gry->tura_gracza = -przeciwnik;
        return;
      }
      XFlush(display);
    }
  }

  if (stan_gry->tura_gracza == -gracz)
  {
    if(stan_gry->czy_wystapilo_spasowanie == 2) 
    {
      info_dla_gracza = "Brak mozliwosci ruchow. Koniec gry. Wygrales.";
      printf("\nBrak mozliwosci ruchow. Koniec gry - wygrales.\n");
    } else {
      info_dla_gracza = "Koniec gry. Wygrales.";
      printf("\nKoniec gry - wygrales.\n");
    }
  }
  if (stan_gry->tura_gracza == -przeciwnik)
  {
    if(stan_gry->czy_wystapilo_spasowanie == 2)
    {
      info_dla_gracza = "Brak mozliwosci ruchow. Koniec gry, przegrales.";
      printf("Brak mozliwosci ruchow. przegrales.\n");
    } else {
      info_dla_gracza = "Koniec gry, przegrales.";
      printf("przegrales.\n");
    }   
  }
  if (stan_gry->tura_gracza == -3)
  {
    if(stan_gry->czy_wystapilo_spasowanie == 2)
    {
      info_dla_gracza = "Brak mozliwych ruchow. REMIS!";
      printf("Brak mozliwych ruchow, remis.");
    } else {
      info_dla_gracza = "REMIS!";
      printf("remis!");
    }
  }
  stworz_gre();
  XNextEvent(display, &event);
}


int main()
{

  signal(SIGINT, usun_segementy);
  inicjuj_gre();
  inicjuj_Xlib();
  int state = ustaw_stan_poczatkowy();
  if (state == -1){
    usun_okno();
    return -1;
  }
  stan_gry->tura_gracza = stan_gry->gracz_rozpoczynajacy_gre;
  if(stan_gry->tura_gracza == gracz) info_dla_gracza = "Twoj Ruch";
  else info_dla_gracza = "Ruch przeciwnika";
  stworz_gre();
  while (stan_gry->tura_gracza != gracz) { 
    usleep(60 * 1000); 
  }
  gra();
  usun_okno();
  usun_segementy(0);
  return 0;
}
