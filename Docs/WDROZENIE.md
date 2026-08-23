# Wdrożenie i uruchomienie — Unreal Engine 5.8

## Wymagania

- Unreal Engine **5.8** z modułami C++.
- Windows: Visual Studio 2022 z workloadem **Game development with C++**, Windows SDK i narzędziami Unreal.
- macOS: aktualny Xcode. Linux: toolchain wspierany przez daną dystrybucję UE 5.8.
- Około 5–10 GB wolnego miejsca na pliki wygenerowane lokalnie (nie trafiają do Git).

## Pierwsze uruchomienie

1. Sklonuj repozytorium i przejdź na dostarczoną gałąź.
2. Kliknij prawym przyciskiem `SettlementGame.uproject` i wybierz **Generate Visual Studio project files**. Alternatywnie uruchom `UnrealVersionSelector` dla tego pliku.
3. Otwórz `SettlementGame.sln` i zbuduj konfigurację `Development Editor` / `Win64`, albo otwórz bezpośrednio `.uproject` i zaakceptuj przebudowę modułu.
4. W edytorze wybierz **File → New Level → Empty Level**. Zapis mapy jako `Content/Game/Maps/Prototype/L_Prototype`.
5. Otwórz **World Settings** i upewnij się, że GameMode to `PrototypeGameMode` (jest też ustawiony globalnie w `DefaultEngine.ini`).
6. Dodaj `Player Start` na `(0, 0, 150)`; prototyp poradzi sobie również bez niego, ale jawny start jest stabilniejszy.
7. Naciśnij **Play**. GameMode utworzy ziemię, światło, drzewa, kamienie i runtime NavMesh.

> Kod nie wymaga Starter Content. Placeholdery korzystają tylko z `/Engine/BasicShapes/Cube`.

## Sterowanie i scenariusz akceptacyjny

| Klawisz | Akcja |
|---|---|
| `WASD`, mysz, `Spacja` | ruch, rozglądanie, skok |
| `E` | zebranie zasobu / interakcja / cykl pracy mieszkańca |
| `1`, `2`, `3`, `4` | wybór magazynu, chaty, tartaku, platformy z desek |
| LPM | postawienie wybranego budynku |
| PPM | wyjście z trybu budowy |

Koszty prototypowe:

- magazyn: 10 drewna + 5 kamienia,
- chata: 8 drewna + 3 kamienia,
- tartak: 12 drewna + 6 kamienia,
- platforma: 4 wyprodukowane deski.

Przejdź następujący test:

1. Zbierz minimum 30 drewna i 14 kamienia.
2. `1` → postaw magazyn. Spójrz na magazyn i użyj `E`, aby odłożyć nadmiar.
3. `2` → postaw chatę. Pojawi się Aldric.
4. Użyj `E` na mieszkańcu: otrzyma pracę drwala i zacznie nosić drewno do magazynu.
5. `3` → postaw tartak.
6. Ponownie użyj `E` na mieszkańcu: przejdzie na pracę trakowego (jeżeli tartak jest w świecie).
7. Obserwuj HUD: drewno przechodzi magazyn → tartak, po 4 s powstają deski, które wracają do magazynu.
8. Poczekaj na 4 deski, naciśnij `4` i postaw platformę. Koszt zostanie pobrany z magazynu — domyka to ostatnie kryterium vertical slice.

## NavMesh i diagnostyka AI

- Naciśnij `P` w edytorze: zielony obszar powinien pokrywać polanę.
- `RuntimeGeneration=Dynamic` jest ustawione w `Config/DefaultEngine.ini`.
- Jeśli NPC stoi, usuń automatyczny NavMesh i dodaj ręcznie `Nav Mesh Bounds Volume` o rozmiarze około 6000 × 6000 × 1000, potem przesuń dowolny wierzchołek/aktor, by wymusić przebudowę.
- Budynki mogą chwilowo przebudowywać nawigację; do finalnej gry należy dodać wejścia i linki nawigacyjne.

## Blueprint i Data Assets

Po uruchomieniu kodu można stworzyć:

- **Miscellaneous → Data Asset → ResourceDefinition** dla Wood/Stone/Planks,
- `ProductionRecipeDefinition` dla `Recipe.WoodToPlanks`,
- `BuildingDefinition` dla kosztów i klas Blueprint,
- `WorkerJobDefinition` dla pracy drwala/trakowego,
- Blueprinty po `StorageBuilding`, `HouseBuilding`, `SawmillBuilding` w `Content/Game/Blueprints/Buildings`.

Native defaults działają bez tych assetów. Data Assets są przewidzianą ścieżką konfiguracji, nie warunkiem uruchomienia.

## Testy automatyczne

W edytorze: **Tools → Test Automation**, filtr `Settlement.Inventory`.

Z wiersza poleceń (dostosuj ścieżkę do UnrealEditor):

```text
UnrealEditor-Cmd SettlementGame.uproject -unattended -nop4 \
  -ExecCmds="Automation RunTests Settlement.Inventory;Quit" -TestExit="Automation Test Queue Empty"
```

## Build Development

Windows z katalogu silnika:

```powershell
Engine\Build\BatchFiles\RunUAT.bat BuildCookRun `
  -project="C:\path\Game\SettlementGame.uproject" -noP4 `
  -platform=Win64 -clientconfig=Development -build -cook -stage -pak
```

Przed cookiem ustaw mapę `L_Prototype` w **Project Settings → Packaging → List of maps to include**. Repozytorium celowo nie zawiera binarnego `.umap`, więc krok zapisania mapy w edytorze jest wymagany.

## Zasady Git

Commitujemy `Source`, `Config`, dokumentację i autorskie `.uasset`. Nie commitujemy `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, plików IDE ani paczek builda — reguły są w `.gitignore`.
