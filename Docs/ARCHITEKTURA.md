# Architektura vertical slice

## Zasady

- Stan runtime przechowuje stabilne `FName` (`Resource.Wood`), a nie wskaźnik do assetu.
- Definicje zawartości są `UPrimaryDataAsset`: zasób, budynek, receptura i praca.
- Stan zasobów zawsze przechodzi przez `UResourceInventoryComponent`.
- Produkcja, czas i pracownik wykonują symulację w interwałach; nie mają `Tick` co klatkę.
- Jedyny warunkowy Tick służy do przesuwania podglądu budynku i jest wyłączony poza trybem budowania.
- C++ dostarcza reguły i stan. Blueprint ma konfigurować klasy, UI, assety i reakcje wizualne.

## Moduły

| Katalog | Odpowiedzialność |
|---|---|
| `Resources` | identyfikatory, ilości i węzły drewna/kamienia |
| `Inventory` | pojemność, atomowe koszty, transfery |
| `Interaction` | interfejs oraz throttlowany raycast FPP |
| `Buildings` | wspólna baza, magazyn, chata, tartak, placement |
| `Production` | receptura i interwałowa produkcja |
| `Time` | światowy zegar, fazy praca/odpoczynek/sen |
| `Workers` | potrzeby, przydział pracy i dwie proste strategie AI |
| `Player` | pawn FPP i mapowanie wejścia |
| `UI` | natychmiastowy HUD Canvas do debugowania slice'a |
| `Core` | GameMode tworzący testową polanę |
| `Data` | klasy bazowe Data Assets |

## Przepływ zasobów

1. Gracz używa `E` na `AHarvestableResourceNode`; zasób trafia do inventory gracza.
2. Placement sumuje środki gracza i wszystkich magazynów, pobiera koszt i stawia aktora.
3. Interakcja z magazynem przenosi do niego niesione zasoby.
4. Drwal pobiera drewno z drzewa do inventory osobistego i dostarcza do magazynu.
5. Trakowy pobiera 2 drewna z magazynu i dostarcza do lokalnego inventory tartaku.
6. `UProductionComponent` rezerwuje wejście, po 4 s produkuje 2 deski.
7. Trakowy odbiera deski i odkłada je w magazynie.

## Rozszerzanie bez przebudowy fundamentów

- Nowy zasób: utworzyć `DA_Resource_*`, nadać unikalny `ResourceId`; nie trzeba zmieniać inventory.
- Nowa receptura: utworzyć `DA_Recipe_*` i przypisać do komponentu produkcji.
- Nowy budynek: Blueprint pochodny po `ABuildingBase` lub nowa klasa C++; koszt może pochodzić z `UBuildingDefinition`.
- Nowa praca: na poziomie slice'a wymaga nowej strategii `EWorkerJobBehavior`; po udowodnieniu pętli warto wydzielić obiekty strategii/job queue.
- Setki NPC: obecnej logiki Character/AI nie należy skalować wprost. Najpierw profilowanie, potem centralny scheduler LOD symulacji, a dopiero w razie potrzeby Mass Entity.

## Świadome ograniczenia prototypu

- Jeden magazyn logicznie nie jest globalny, ale pracownicy wybierają najbliższy dostępny.
- Brak jedzenia: głód jest obserwowalnym stanem, jeszcze bez konsekwencji.
- Brak save/load i multiplayera.
- Budynki są natychmiastowe i używają prostego box-overlap.
- Rezerwacja zadań nie jest potrzebna przy jednym mieszkańcu; przed dodaniem kolejnych trzeba dodać rezerwacje drzewa, partii i miejsca w magazynie.
