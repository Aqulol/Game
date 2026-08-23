# Settlement Game — UE 5.8 vertical slice

C++-owy, grywalny szkielet realistycznej średniowiecznej gry kolonizacyjno-produkcyjnej FPP. Prototyp udowadnia przejście od ręcznego zbierania do pierwszej automatyzacji: magazyn, chata, mieszkaniec, tartak i produkcja desek.

## Co już jest w kodzie

- asset-free pawn FPP, interakcja raycast i HUD diagnostyczny,
- drewno, kamień, deski oraz wspólny komponent inventory/storage,
- placement z preview, siatką, kolizją i kosztami pobieranymi z gracza/magazynów,
- natywne klasy magazynu, chaty, tartaku i platformy zużywającej wyprodukowane deski,
- interwałowa receptura 2 drewna → 2 deski,
- przyspieszony zegar i fazy praca / odpoczynek / sen,
- jeden mieszkaniec z energią, głodem, domem, pracą drwala lub trakowego,
- AIController + NavMesh oraz przepływ drewno → magazyn → tartak → deski → magazyn,
- Data Assets dla zasobów, budynków, receptur i prac,
- automatycznie generowana mała polana testowa z placeholderów silnika,
- testy automatyczne podstawowych reguł inventory.

## Start

Projekt wymaga Unreal Engine **5.8**. Wygeneruj pliki projektu z `SettlementGame.uproject`, skompiluj target `SettlementGameEditor`, utwórz pustą mapę i naciśnij Play. Dokładna instrukcja, sterowanie, test akceptacyjny i komendy builda:

- **[Instrukcja wdrożenia](Docs/WDROZENIE.md)**
- **[Architektura](Docs/ARCHITEKTURA.md)**

## Najważniejsze sterowanie

`WASD` + mysz — FPP, `E` — interakcja, `1/2/3/4` — magazyn/chata/tartak/platforma z desek, LPM — budowa, PPM — anulowanie.

## Zakres

To fundament vertical slice, nie kompletna gra. Celowo nie zawiera ekonomii, save/load, Mass Entity, multiplayera, pogody, pór roku ani proceduralnego świata. Lista świadomych ograniczeń i bezpieczne kierunki rozwoju znajdują się w dokumentacji architektury.
