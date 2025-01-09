#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <iostream>

const int ROZMIAR_KRATKI = 20;
const int SZEROKOSC      = 30;
const int WYSOKOSC       = 30;

enum class Kierunek
{
    GORA,
    DOL,
    LEWO,
    PRAWO
};

struct Punkt
{
    int  x, y;
    bool operator==(const Punkt& inny) const { return x == inny.x && y == inny.y; }
};

class GraWWeza
{
public:
    GraWWeza()
        : okno(nullptr),
          renderer(nullptr),
          waz({{SZEROKOSC / 2, WYSOKOSC / 2}}),
          kierunek(Kierunek::LEWO),
          koniecGry(false)
    {
        srand(static_cast< unsigned >(time(nullptr)));
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw std::runtime_error("Nie uda³o siê zainicjalizowaæ SDL");
        }
        okno = SDL_CreateWindow("Gra w Weza",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                SZEROKOSC * ROZMIAR_KRATKI,
                                WYSOKOSC * ROZMIAR_KRATKI,
                                SDL_WINDOW_SHOWN);
        if (!okno)
        {
            throw std::runtime_error("Nie uda³o siê utworzyæ okna SDL");
        }
        renderer = SDL_CreateRenderer(okno, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            throw std::runtime_error("Nie uda³o siê utworzyæ renderer SDL");
        }
        losujJedzenie();
    }

    ~GraWWeza()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(okno);
        SDL_Quit();
    }

    void uruchom()
    {
        while (!koniecGry)
        {
            obsluzZdarzenia();
            aktualizuj();
            rysuj();
            SDL_Delay(100);
        }
    }

private:
    SDL_Window*         okno;
    SDL_Renderer*       renderer;
    std::deque< Punkt > waz;
    Kierunek            kierunek;
    Punkt               jedzenie;
    bool                koniecGry;

    void obsluzZdarzenia()
    {
        SDL_Event zdarzenie;
        while (SDL_PollEvent(&zdarzenie))
        {
            if (zdarzenie.type == SDL_QUIT)
            {
                koniecGry = true;
            }
            else if (zdarzenie.type == SDL_KEYDOWN)
            {
                switch (zdarzenie.key.keysym.sym)
                {
                case SDLK_w:
                    if (kierunek != Kierunek::DOL)
                        kierunek = Kierunek::GORA;
                    break;
                case SDLK_s:
                    if (kierunek != Kierunek::GORA)
                        kierunek = Kierunek::DOL;
                    break;
                case SDLK_a:
                    if (kierunek != Kierunek::PRAWO)
                        kierunek = Kierunek::LEWO;
                    break;
                case SDLK_d:
                    if (kierunek != Kierunek::LEWO)
                        kierunek = Kierunek::PRAWO;
                    break;
                }
            }
        }
    }

    void aktualizuj()
    {
        Punkt nowaGlowa = waz.front();

        switch (kierunek)
        {
        case Kierunek::GORA:
            nowaGlowa.y--;
            break;
        case Kierunek::DOL:
            nowaGlowa.y++;
            break;
        case Kierunek::LEWO:
            nowaGlowa.x--;
            break;
        case Kierunek::PRAWO:
            nowaGlowa.x++;
            break;
        }

        if (nowaGlowa.x < 0 || nowaGlowa.y < 0 || nowaGlowa.x >= SZEROKOSC || nowaGlowa.y >= WYSOKOSC ||
            std::find(waz.begin(), waz.end(), nowaGlowa) != waz.end())
        {
            koniecGry = true;
            return;
        }

        waz.push_front(nowaGlowa);
        if (nowaGlowa == jedzenie)
        {
            losujJedzenie();
        }
        else
        {
            waz.pop_back();
        }
    }

    void rysuj()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const auto& segment : waz)
        {
            SDL_Rect prostokat = {
                segment.x * ROZMIAR_KRATKI, segment.y * ROZMIAR_KRATKI, ROZMIAR_KRATKI, ROZMIAR_KRATKI};
            SDL_RenderFillRect(renderer, &prostokat);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect prostokat = {jedzenie.x * ROZMIAR_KRATKI, jedzenie.y * ROZMIAR_KRATKI, ROZMIAR_KRATKI, ROZMIAR_KRATKI};
        SDL_RenderFillRect(renderer, &prostokat);

        SDL_RenderPresent(renderer);
    }

    void losujJedzenie()
    {
        do
        {
            jedzenie.x = rand() % SZEROKOSC;
            jedzenie.y = rand() % WYSOKOSC;
        } while (std::find(waz.begin(), waz.end(), jedzenie) != waz.end());
    }
};

int main()
{
    try
    {
        GraWWeza gra;
        gra.uruchom();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Wyst¹pi³ b³¹d: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
