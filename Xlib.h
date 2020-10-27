#define Klucz_1 1234
#define Klucz_2 1235

#define Puste 0
#define Czarne 1
#define Biale 2

#define Poza_Plansza 0
#define Plansza 1

#define rozmiar_planszy 8
#define rozmiar_komorki 45
#define plansza_pion rozmiar_planszy * rozmiar_komorki
#define margin_poziom 40
#define margin_pion 40

Display *display;
Window window;
int screen;
Visual *visual;
int depth;
GC gc;
XEvent event;
Colormap colormap;
XColor color, exact_color;
int x11_file_descriptor;

typedef enum { false, true } bool;

typedef struct kordynaty_st {
  int plansza;
  int x;
  int y;
} kordynaty_t;

typedef struct stan_gry_st {
  int gracz_rozpoczynajacy_gre;
  int czy_wystapilo_spasowanie;
  int tura_gracza;
  int gracz_licznik;
} stan_gry_t;


char* czarne_tytul = "Czarne:  ";
char* biale_tytul = "Biale:  ";
char * biale_liczba_str = NULL;
char * czarne_liczba_str = NULL;
char* info_dla_gracza;
int gracz;
int przeciwnik;
int twoj_kolor;
bool czy_wykonales_poprawny_ruch = false;
int losowa_1_albo_2;
int liczba_bialych;
int liczba_czarnych;


struct timeval tv;
int** plansza_do_gry;

stan_gry_t* stan_gry;
int shm_plansza_id;
int shm_stan_gry_id;


void X_stworz_plansze()
{
  XAllocNamedColor(display, colormap, "grey", &color, &exact_color);
  XSetForeground(display, gc, color.pixel);

  int x1, y1, x2, y2;
  int offset_x = plansza_pion + margin_poziom;
  for (int i = 0; i < rozmiar_planszy + 1; i++)
  {
    x1 = margin_poziom;
    y1 = margin_pion + i * rozmiar_komorki;
    x2 = margin_poziom + plansza_pion;
    y2 = margin_pion + i * rozmiar_komorki;
    XDrawLine(display, window, gc, x1, y1, x2, y2);

    x1 = margin_poziom + i * rozmiar_komorki;
    y1 = margin_pion;
    x2 = margin_poziom + i * rozmiar_komorki;
    y2 = margin_pion + plansza_pion;
    XDrawLine(display, window, gc, x1, y1, x2, y2);
  }
}

kordynaty_t X_okresl_gdzie_kliknal_gracz(int x, int y)
{
  int plansza = Poza_Plansza;
  int pole_x = 0;
  int pole_y = 0;

  bool player_x = x >= margin_poziom && x <= plansza_pion + margin_poziom;
  bool player_y = y >= margin_pion && y <= margin_pion + plansza_pion;
  if (player_x == true && player_y == true)
  {
    plansza = Plansza;
  }

  pole_x = (x - margin_poziom) / rozmiar_komorki;
  pole_y = (y - margin_pion) / rozmiar_komorki;
  kordynaty_t koordy = { .plansza = plansza, .x = pole_x, .y = pole_y };
  return koordy;
}