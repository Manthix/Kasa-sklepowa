/*
Michal Matejek - kasa sklepowa v0.99


-Dane wejściowe - stan początkowy kasy (15 różnych banknotów) w pliku o nazwie "StanKasy.txt"
-Pętla obsługi klienta:
	kwota do zapłaty
	pieniądze do kasy
	reszta (gdy nie ma banknotów do wydania reszty komunikat plus odwołanie realizacji tranzakcji)
	aktualizacja kasy

1 zł czy grosze                                 INT
2 algorytm wydawania reszty (zachłanny?)        Hybrydowe (glownie dynamicznie)
3 truktura danych (przetrzymywanie danych)      ilosci banknotow przetrzymywane w pliku i wykorzystywane to zaktualizowania obiektów
4 styl programistyczny                          modularny
5 praca nad projektem
6 dokumentacja
*/

#include <iostream>
#include <fstream>
using namespace std;

int const IloscB = 15;
//Obiekt banknot
struct Banknot {
    string const nazwa;
    int const wartosc;
    int ilosc;
};
//Deklaracja funkcji
int Suma( Banknot kasa[] );
void WypiszNom( Banknot banknoty[] );
bool WczytajKase( Banknot kasa[] );
bool ZapiszKase( Banknot kasa[] );
void PodajBanknoty(Banknot klient[], int cena);
void ObslugaKlienta(Banknot kasa[]);
void ZaktualizujKase(Banknot klient[], Banknot kasa[]);
bool Reszta(Banknot kasa[], int reszta);
int zmieniarkadynamiczna( Banknot kasa[], int reszta, int C[], int s[] );
bool Akceptacja();

int main()
{
    //utworzenie obiektu kasy zawierajacego banknoty
    Banknot kasa[ IloscB ] = {
        { "1 gr", 1, 0 },
        { "2 gr", 2, 0 },
        { "5 gr", 5, 0 },
        { "10 gr", 10, 0 },
        { "20 gr", 20, 0 },
        { "50 gr", 50, 0 },
        { "1 zl", 100, 0 },
        { "2 zl", 200, 0 },
        { "5 zl", 500, 0 },
        { "10 zl", 1000, 0 },
        { "20 zl", 2000, 0 },
        { "50 zl ", 5000, 0 },
        { "100 zl", 10000, 0 },
        { "200 zl", 20000, 0 },
        { "500 zl", 50000, 0 },
    };
    //wczytanie kasy
    if(WczytajKase(kasa)==0)
        return 0;
    //po udanym wczytaniu rozpoczecie petli obslugi klienta
    ObslugaKlienta(kasa);
    return 0;
}

void ObslugaKlienta(Banknot kasa[])
{
    system("cls");
    WczytajKase( kasa );
    WypiszNom( kasa );

    cout<<"Podaj cene zakupow: \n";
    double doubleCena;
    cin>> doubleCena;               // podanie ceny zakupow
    int cena = doubleCena*100;
    Banknot klient[ IloscB ] = {
        { "1 gr", 1, 0 },
        { "2 gr", 2, 0 },
        { "5 gr", 5, 0 },
        { "10 gr", 10, 0 },
        { "20 gr", 20, 0 },
        { "50 gr", 50, 0 },
        { "1 zl", 100, 0 },
        { "2 zl", 200, 0 },
        { "5 zl", 500, 0 },
        { "10 zl", 1000, 0 },
        { "20 zl", 2000, 0 },
        { "50 zl ", 5000, 0 },
        { "100 zl", 10000, 0 },
        { "200 zl", 20000, 0 },
        { "500 zl", 50000, 0 },
    };
    PodajBanknoty(klient, cena);                    //podanie banknotow posiadanych przez klienta
    if(cena<Suma(klient)){                          //sprawdzenie czy wymagana jest reszta
        int reszta;
        reszta = Suma(klient)-cena;                 //obliczenie reszty
        cout<<"Reszta: "<<reszta/100<<"zl\n\n";

        cout << "Stan kasy przed tranzakcja - ";
        WypiszNom( kasa );
        ZaktualizujKase(klient, kasa);              //dodanie banknotow do kasy

        int * C = new int[ reszta + 1 ];            // optymalne liczby monet potrzebne do wydania kwoty "i"
        int * s = new int[ reszta + 1 ];            // indeksy nominałów, od których należy rozpocząć wydawanie kwoty "i"
        zmieniarkadynamiczna( kasa, reszta, C, s );

        cout << "Uzyte monety: ";
        int k = reszta, i=0;
        while( k && i!=1) {

            if(kasa[ s[ k ] ].ilosc > 0)                //sprawdzenie czy dynamiczne wydanie reszty jest poprawne
            {
                cout << kasa[ s[ k ] ].nazwa << " ";
                kasa[ s[ k ] ].ilosc -= 1;              //usniecie banknotu z kasy
                k -= kasa[ s[ k ] ].wartosc;            //odjecie banknotu od reszty
            }
            else{                                       //jezeli dynamiczne wydanie reszty nie jest poprawne
                i=1;                                    //zacznij wyliczanie zachłanne
                if(!Reszta(kasa, k)){                   //jezeli nie da sie wydac reszty
                    system("cls");
                    cout<< "Brak mozliwosci wydania resszty\n\n\n";
                    int akcept;
                    cout<<"Kliknij aby zaakceptowac"<<endl;
                    cin>>akcept;
                    switch(akcept){
                    default:
                    delete[] C;
                    delete[] s;
                    ObslugaKlienta(kasa);
                    }
                }
            }
        }

        cout << "\nStan kasy po tranzakcji - ";
        WypiszNom( kasa );
        delete[] C;
        delete[] s;
    }
    else{
        cout << "Stan kasy przed tranzakcja - ";
        WypiszNom( kasa );
        ZaktualizujKase(klient, kasa);
        cout << "Stan kasy po tranzakcji - ";
        WypiszNom( kasa );
    }

    if(Akceptacja())
    {
        ZapiszKase( kasa );
        ObslugaKlienta(kasa);
    }
    else ObslugaKlienta(kasa);
}

//Suma wartosci wszystkich banknotow
int Suma( Banknot kasa[] )
{
    int suma = 0;
    for( int i = 0; i < IloscB; i++ )
    {
        suma += kasa[ i ].ilosc * kasa[ i ].wartosc;
    }
    return suma;
}
//Wyswietlenie banknotow w kasie i ich sumy
void WypiszNom( Banknot banknoty[] )
{
    cout << "Suma: " << Suma( banknoty ) / 100 << "zl \n";
    for( int i = 0; i < IloscB; i++ ){
        if(banknoty[ i ].ilosc!=0) //Niepokazuj banknotow ktorych nie ma
            if(banknoty[ i ].ilosc==1)
                cout << banknoty[ i ].ilosc << " sztuka " << banknoty[ i ].nazwa << endl;
            else if(banknoty[ i ].ilosc>0 && banknoty[ i ].ilosc<5)
                cout << banknoty[ i ].ilosc << " sztuki " << banknoty[ i ].nazwa << endl;
            else
                cout << banknoty[ i ].ilosc << " sztuk " << banknoty[ i ].nazwa << endl;
    }
    cout<<endl;
}
//Wczytanie do pliku kasy
bool WczytajKase( Banknot kasa[] )
{
    fstream plik;
    plik.open("StanKasy.txt");
    if( plik.good() == true )
    {
        for( int i = 0; i < IloscB; i++ )
        {
            plik >> kasa[ i ].ilosc;
        }
        plik.close();
        return 1;
    }
    else{
        cout << "\nNieudany odczyt stanu kasy!\n" << endl;
        plik.close();
        return 0;
    }
}
//Zapisanie do pliku kasy
bool  ZapiszKase( Banknot kasa[] )
{
    fstream plik;
    plik.open("StanKasy.txt");
    if( plik.good() == true )
    {
        for( int i=0; i<IloscB; i++ )
        {
            plik << kasa[ i ].ilosc <<"\n";
        }
        plik.close();
        return 1;
    }
    else{
        cout << "\nNieudany zapis stanu kasy!\n" << endl;
        plik.close();
        return 0;
    }
}
//Dodanie pieniedzy klienta do kasy
void ZaktualizujKase(Banknot klient[], Banknot kasa[])
{
    for(int i=0; i<IloscB; i++)
    {
        kasa[ i ].ilosc += klient[ i ].ilosc;
    }

}
//Podanie banknotow posiadanych przez klienta
void PodajBanknoty(Banknot klient[], int cena)
{
    system("cls");
    int ilosc; double banknot;
    do{
        cout<<"Cena: "<< cena/100<<" zl\n";
        cout<<"Aktualny stan portfela klienta - ";
        WypiszNom( klient );
        cout<<"\nPodaj wartosc banknotu: ";
        cin>>banknot;

        for( int i = 0; i < IloscB; i++ ){
            if(banknot*100==klient[i].wartosc){
                cout<<"\nPodaj ilosc banknotow o tym nominale: ";
                cin>>ilosc;
                klient[i].ilosc+=ilosc;
                break;
            }
        }
        system("cls");
    }while(Suma(klient)<cena);
    cout<<"Cena: "<< cena/100<<" zl\n";
    cout<<"Aktualny stan portfela klienta - ";
    WypiszNom( klient );
}
//wybor tak/nie
bool Akceptacja()
{
    int akcept;
    cout<<"Kliknij 1 aby zaakceptowac \nKliknij 2 aby anulowac"<<endl;
    cin>>akcept;
    switch(akcept){
    case 1:
        return 1;
        break;
    case 2:
        return 0;
        break;
    default:
        Akceptacja();
    }
}
//wydanie reszty zachlannie
bool Reszta(Banknot kasa[], int reszta)
{
    int i=IloscB-1;
    while(reszta!=0 && i >= 0 ){
        if(kasa[i].ilosc>0 && kasa[i].wartosc<=reszta)
        {
            cout<<kasa[i].nazwa<< " ";
            reszta-=kasa[i].wartosc;
            kasa[i].ilosc-=1;
        }
        else i--;
    }
    cout<<endl;
    if(reszta>0) return 0;
    else return 1;
}
//wydanie reszty dynamiczne
int zmieniarkadynamiczna( Banknot kasa[], int reszta, int C[], int s[] )
{
    C[0] = 0;
    for(int j=1; j<=reszta; j++)
    {
        C[j]=INT_MAX;
        for( int i=0; i<IloscB; i++)
        {
            if( j>=kasa[i].wartosc && 1+C[j-kasa[i].wartosc]<C[j] && ( kasa[i].ilosc>=1))
            {
                C[j]=1+C[j-kasa[i].wartosc];
                s[j] = i;
            }
        }
    }
    return C[reszta];
}
