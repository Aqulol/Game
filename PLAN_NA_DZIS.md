# Plan pracy na dziś — prototyp gry w Unreal Engine 5

Data: 2026-08-23  
Projekt roboczy: realistyczna średniowieczna gra kolonizacyjno-produkcyjna z perspektywą FPP

---

## 1. Krótki opis gry

Tworzymy grę o zakładaniu nowej osady po upadku królestwa.

Królestwo zostało wyniszczone przez zarazę, nieudolne decyzje władz i bunt mieszkańców. Gracz prowadzi grupę ocalałych, którzy próbują zbudować nową społeczność od zera.

Gra łączy:

- bezpośrednią perspektywę pierwszoosobową,
- eksplorację i zbieranie zasobów,
- budowanie osady,
- logistykę i produkcję,
- automatyzację pracy przez mieszkańców,
- potrzeby społeczne i konsekwencje decyzji.

Nie robimy od razu ogromnej gry. Najpierw budujemy mały, grywalny vertical slice.

---

## 2. Ustalony kierunek projektowy

### Klimat i świat

- Realistyczne średniowiecze.
- Subtelna fantastyka może istnieć, ale nie dominuje świata.
- Fantastyczne elementy na później:
  - zioła o nietypowych właściwościach,
  - mikstury,
  - pozostałości lub skutki katastrofy,
  - tajemnicze zjawiska związane z zarazą.

### Katastrofa startowa

Łańcuch wydarzeń:

1. Wybuch zarazy w królestwie.
2. Władza nie radzi sobie z kryzysem.
3. Narasta niezadowolenie społeczne.
4. Dochodzi do buntu mieszkańców.
5. Królestwo upada.
6. Ocalałe grupy zakładają nowe osady.

Docelowo gracz może na początku wybierać kolonistów, ale **nie robimy tego w pierwszym vertical slice**.

### Nastrój gry

Główny nastrój:

- trudna, lecz pełna nadziei odbudowa.

Gra nie ma być wyłącznie ponurym survivalem. Ma dawać poczucie, że mimo kryzysu osada rośnie, organizuje się i odzyskuje sprawczość.

### Najważniejszy filar rozgrywki

Najważniejsze ma być:

> Budowanie żyjącej osady z produkcją, logistyką i mieszkańcami wykonującymi pracę automatycznie.

FPP jest ważne, ale z czasem gracz przechodzi bardziej do zarządzania i planowania.

---

## 3. Docelowy gameplay loop

Docelowa pętla rozgrywki:

```text
Eksploracja
↓
Zbieranie zasobów
↓
Budowa podstawowej infrastruktury
↓
Przydzielanie mieszkańców do pracy
↓
Automatyczne zbieranie / transport / produkcja
↓
Magazynowanie zasobów
↓
Usuwanie wąskich gardeł
↓
Rozwój osady
↓
Nowe technologie i budynki
↓
Większa produkcja i nowe problemy społeczne
```

W najprostszej wersji, którą chcemy najpierw udowodnić:

```text
Gracz zbiera drewno i kamień
↓
Buduje magazyn
↓
Buduje chatę
↓
Buduje stanowisko pracy / tartak
↓
Pojawia się mieszkaniec
↓
Mieszkaniec dostarcza drewno do magazynu
↓
Tartak przerabia drewno na deski
↓
Gracz używa desek do dalszej budowy
```

---

## 4. Najważniejsze decyzje techniczne

### Unreal Engine

- Używamy Unreal Engine 5.8.
- Startujemy od prostego projektu, najlepiej z szablonu First Person.
- Projekt docelowo powinien być C++ + Blueprint.

### C++ + Blueprint — podział odpowiedzialności

#### C++

C++ powinien obsługiwać fundamenty systemów:

- definicje zasobów,
- magazyny,
- inventory,
- produkcję,
- czas gry,
- zadania mieszkańców,
- wydajną symulację,
- zapis/odczyt w późniejszym etapie.

#### Blueprinty

Blueprinty powinny służyć do:

- szybkiego prototypowania,
- konfiguracji budynków,
- interakcji w świecie,
- UI,
- podłączania assetów,
- testowania pomysłów.

#### Data Assets / Data Tables

Dane gry powinny być konfigurowalne, np.:

- drewno,
- kamień,
- deski,
- budynki,
- receptury,
- prace mieszkańców,
- technologie.

Dzięki temu przyszłe epoki lub DLC będą mogły dodawać nowe treści bez przepisywania podstawowych systemów.

---

## 5. Systemy potrzebne wcześnie

Na początku projektujemy tylko minimum, ale tak, żeby nie trzeba było go później wyrzucać.

| System | Po co jest potrzebny | Forma na start |
|---|---|---|
| Game Data / Definitions | Jedno źródło prawdy dla zasobów, budynków i receptur | Data Assets lub Data Tables |
| Interaction System | Gracz może patrzeć na obiekt i wejść z nim w interakcję | Interface + Blueprint/C++ |
| Resource System | Drewno, kamień, deski jako typy zasobów | Prosta struktura danych |
| Inventory / Storage | Przechowywanie zasobów gracza i magazynu | Komponent |
| Building System | Stawianie magazynu, chaty i tartaku | Prosty preview + koszt budowy |
| Production System | Przerabianie drewna na deski | Komponent produkcji |
| Time System | Dzień/noc i symulacja w interwałach | Subsystem lub Actor zarządzający czasem |
| Worker System | Jeden mieszkaniec wykonujący pracę | Character + AIController + NavMesh |
| UI | Pokazywanie zasobów i stanu obiektów | UMG Blueprint |

---

## 6. Systemy odkładane na później

Tych rzeczy **nie robimy teraz**, nawet jeśli docelowo są ważne:

- duża mapa,
- proceduralne biomy,
- wiele osad,
- dynamiczna ekonomia pieniędzy,
- ceny, pensje i podatki,
- polityka i frakcje,
- pełne relacje mieszkańców,
- wybór kolonistów na ekranie startowym,
- choroby i zaawansowane zdrowie,
- transport wozami,
- rozbudowane drogi,
- Mass Entity,
- multiplayer,
- DLC i obsługa wielu epok,
- pełny system save/load,
- pogoda i pory roku.

Powód: te systemy są duże i łatwo zablokowałyby pierwszy działający prototyp.

---

## 7. Największe ryzyka projektu

### 1. Zbyt duży zakres

Projekt łączy elementy wielu dużych gier. Największym ryzykiem jest próba zrobienia wszystkiego naraz.

**Decyzja:** pracujemy małymi etapami i kończymy każdy etap działającym rezultatem.

### 2. AI mieszkańców

Mieszkańcy muszą chodzić, pracować, transportować zasoby i reagować na braki. To może szybko stać się bardzo złożone.

**Decyzja:** zaczynamy od jednego mieszkańca z jednym prostym zadaniem.

### 3. Logistyka i magazyny

Problemy typu: gdzie jest zasób, kto go niesie, czy magazyn ma miejsce, co jeśli budynek nie ma wejścia — pojawią się bardzo szybko.

**Decyzja:** pierwszy magazyn będzie prosty, globalnie czytelny i łatwy do debugowania.

### 4. Wydajność

Docelowo setki mieszkańców i tysiące zasobów nie mogą działać jako tysiące Actorów z Tickiem co klatkę.

**Decyzja:** od początku unikamy niepotrzebnego Tick. Symulację produkcji i potrzeb liczymy w interwałach.

### 5. Budowanie na terenie

Budowanie na nierównym terenie, kolizje i dopasowanie do krajobrazu są trudniejsze, niż wyglądają.

**Decyzja:** na start używamy płaskiej polany i budynków o prostych rozmiarach.

### 6. Multiplayer w przyszłości

Kooperacja w przyszłości wpływa na architekturę, ale projektowanie wszystkiego pod multiplayer od pierwszego dnia spowolni prototyp.

**Decyzja:** single-player teraz. Nie robimy aktywnie multiplayera, ale unikamy skrajnie chaotycznych zależności.

---

## 8. Proponowana struktura folderów projektu Unreal

```text
Source/
  SettlementGame/
    Core/
    Data/
    Time/
    Resources/
    Inventory/
    Interaction/
    Buildings/
    Production/
    Workers/
    UI/
    Save/

Content/
  Game/
    Blueprints/
      Characters/
      Player/
      Buildings/
      Resources/
      AI/
      Components/
    Data/
      Resources/
      Buildings/
      Recipes/
      Jobs/
    Maps/
      Prototype/
    UI/
      Widgets/
      Icons/
    Art/
      Environment/
      Props/
      Materials/
    Audio/
    Developer/
```

Na dziś nie musimy tworzyć wszystkich folderów. Ważne, żeby od początku trzymać porządek.

---

## 9. Pierwszy vertical slice

### Cel

Gracz ma poczuć przejście od ręcznej pracy do pierwszej automatyzacji.

### Zakres mapy

- Mała mapa testowa.
- Polana.
- Kilka drzew.
- Kilka kamieni.
- Miejsce na 3 budynki.
- Bez dużego świata i proceduralności.

### Gracz

- Perspektywa pierwszoosobowa.
- Poruszanie.
- Celownik / raycast interakcji.
- Zbieranie drewna i kamienia.

### Zasoby

Na start tylko:

- drewno,
- kamień,
- deski.

### Budynki

Pierwsze trzy budynki:

1. **Magazyn**
   - przechowuje zasoby,
   - pokazuje ilość drewna, kamienia i desek.

2. **Chata**
   - miejsce zamieszkania jednego mieszkańca,
   - później połączona z odpoczynkiem i snem.

3. **Tartak / stanowisko obróbki drewna**
   - pobiera drewno,
   - produkuje deski.

### Mieszkaniec

Na start tylko jeden NPC.

Powinien mieć:

- imię,
- przypisany dom,
- przypisaną pracę,
- głód,
- energię,
- aktualne zadanie.

Pierwsze zachowanie:

```text
Mieszkaniec znajduje drzewo
↓
Idzie do drzewa
↓
Zbiera drewno
↓
Idzie do magazynu
↓
Odkłada drewno
↓
Wraca do pracy albo odpoczywa
```

Drugi krok:

```text
Mieszkaniec pobiera drewno z magazynu
↓
Idzie do tartaku
↓
Przerabia drewno na deski
↓
Odkłada deski do magazynu
```

### Czas

- Przyspieszony cykl dnia i nocy.
- Na start wystarczą trzy fazy:
  - praca,
  - odpoczynek,
  - sen.

### UI

Minimalny UI:

- liczba zasobów w magazynie,
- informacja o wybranym budynku,
- informacja o mieszkańcu:
  - praca,
  - energia,
  - głód,
  - aktualne zadanie.

---

## 10. Kryterium sukcesu pierwszego prototypu

Pierwszy prototyp uznajemy za udany, jeśli gracz może:

1. Uruchomić mapę testową.
2. Poruszać się w FPP.
3. Zebrać drewno i kamień.
4. Postawić magazyn.
5. Postawić chatę.
6. Postawić tartak.
7. Zobaczyć jednego mieszkańca.
8. Przypisać mieszkańca do pracy.
9. Zobaczyć, jak mieszkaniec dostarcza drewno do magazynu.
10. Zobaczyć produkcję desek.
11. Użyć desek do budowy kolejnego prostego obiektu.

To jest nasz pierwszy prawdziwy cel grywalny.

---

## 11. Co robimy dzisiaj

### Dzisiejszy cel główny

Nie budujemy jeszcze całej gry. Dzisiejszy cel to:

> Przygotować projekt Unreal i pierwszy techniczny fundament pod prototyp FPP z interakcją oraz zasobami.

### Plan na dziś

#### Krok 1 — utworzenie projektu Unreal

Utworzyć projekt w Unreal Engine 5.8:

- typ: Games,
- szablon: First Person,
- język: C++,
- platforma: Desktop,
- starter content: opcjonalnie tak,
- ray tracing: niepotrzebny na start,
- nazwa robocza: `SettlementGame` albo inna ustalona nazwa.

#### Krok 2 — uporządkowanie folderów

W `Content/Game/` przygotować podstawowe katalogi:

```text
Blueprints/
Data/
Maps/
UI/
Art/
Developer/
```

Nie tworzymy jeszcze całej docelowej struktury, tylko minimum.

#### Krok 3 — mapa testowa

Utworzyć mapę:

```text
Content/Game/Maps/Prototype/M_Prototype_Polana
```

Na mapie powinny być:

- płaski teren,
- kilka placeholderów drzew,
- kilka placeholderów kamieni,
- miejsce testowe pod budynki.

#### Krok 4 — pierwszy system interakcji

Cel:

- gracz patrzy na obiekt,
- widzi informację, że może wejść z nim w interakcję,
- naciska klawisz interakcji,
- obiekt reaguje.

Na tym etapie obiekt może tylko wypisywać komunikat lub znikać.

#### Krok 5 — pierwsze zasoby

Dodać minimalne zasoby:

- drewno,
- kamień.

Na start mogą być przechowywane w prostym inventory gracza.

#### Krok 6 — test dnia

Na koniec dnia chcemy mieć działający test:

```text
Gracz podchodzi do drzewa
↓
Naciska interakcję
↓
Dostaje drewno
↓
UI lub log pokazuje ilość drewna
↓
Gracz podchodzi do kamienia
↓
Naciska interakcję
↓
Dostaje kamień
```

To jest małe, ale bardzo ważne. Na tym oprzemy budowanie, magazyny i pracę mieszkańców.

---

## 12. Czego dzisiaj nie robimy

Dzisiaj nie robimy:

- mieszkańców,
- AI,
- tartaku,
- produkcji desek,
- pełnego budowania,
- UI finalnego,
- save/load,
- ekonomii,
- polityki,
- chorób,
- wyboru kolonistów.

Jeśli zaczniemy od tych rzeczy, ryzykujemy chaos i brak działającego fundamentu.

---

## 13. Najbliższe etapy po dzisiejszym dniu

### Etap 1 — interakcja i zasoby

- FPP,
- zbieranie drewna,
- zbieranie kamienia,
- proste inventory,
- debug UI.

### Etap 2 — budowanie podstawowe

- ghost/preview budynku,
- koszt budowy,
- postawienie magazynu,
- przekazanie zasobów z gracza do magazynu.

### Etap 3 — magazyn i produkcja

- magazyn jako źródło prawdy dla zasobów osady,
- tartak,
- receptura: drewno → deski.

### Etap 4 — pierwszy mieszkaniec

- NPC,
- NavMesh,
- proste zadanie pracy,
- transport drewna do magazynu.

### Etap 5 — potrzeby i harmonogram

- energia,
- głód,
- praca / odpoczynek / sen,
- chata jako dom mieszkańca.

### Etap 6 — pierwszy zamknięty loop

- gracz buduje osadę,
- mieszkaniec zbiera lub transportuje,
- tartak produkuje deski,
- deski umożliwiają dalszą budowę.

---

## 14. Decyzje, które jeszcze trzeba podjąć

Przed rozpoczęciem pracy praktycznej warto ustalić:

1. Jaka ma być robocza nazwa projektu?
2. Czy startujemy od projektu C++ First Person w Unreal 5.8?
3. Czy repozytorium ma zawierać cały projekt Unreal, czy tylko dokumentację i kod pomocniczy?
4. Czy używamy na start darmowych placeholderów / Starter Content, czy wszystko robimy z prostych brył?
5. Czy pierwszy prototyp robimy po polsku, po angielsku, czy technicznie po angielsku, a opisowo po polsku?

Moja rekomendacja:

- nazwa robocza: `SettlementGame`, później można zmienić,
- projekt: C++ First Person,
- prototyp: proste bryły + placeholdery,
- kod i nazwy klas: angielski,
- dokumentacja i komentarze dla Ciebie: polski.

---

## 15. Od czego konkretnie zaczynamy

Najbardziej praktyczny pierwszy ruch:

1. Otwórz Unreal Engine 5.8.
2. Utwórz projekt `SettlementGame` z szablonu First Person i C++.
3. Po utworzeniu projektu sprawdź, czy kompiluje się bez błędów.
4. Utwórz mapę testową `M_Prototype_Polana`.
5. Daj znać, gdy projekt jest utworzony i działa.

Wtedy następnym krokiem będzie przygotowanie pierwszego systemu interakcji i zbierania zasobów.
