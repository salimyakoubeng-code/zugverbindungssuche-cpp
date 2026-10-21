# Zugverbindungssuche mit C++

Eine Konsolenanwendung in C++17 zur Suche nach Zugverbindungen anhand eines lokalen CSV-Fahrplans.

Das Projekt demonstriert objektorientierte Programmierung, Dateiverarbeitung, Eingabevalidierung und automatisierte Tests.

## Funktionen

- Suche nach Startbahnhof, Zielbahnhof und frühester Abfahrtszeit
- Direkte Verbindungen und Verbindungen mit einem Umstieg
- Mindestens fünf Minuten Umsteigezeit
- Sortierung nach frühester Ankunft, anschließend nach Reisedauer und Anzahl der Umstiege
- Anzeige der einzelnen Fahrtabschnitte und der gesamten Reisedauer
- Unterstützung von Ankünften nach Mitternacht
- Prüfung fehlerhafter Eingaben und Fahrplandaten

Die Daten sind fiktiv. Es besteht keine Verbindung zu einem Echtzeitdienst oder zu Systemen der Deutschen Bahn.

## Voraussetzungen

- Ein C++17-kompatibler Compiler, beispielsweise GCC, Clang oder MSVC
- CMake ab Version 3.16

## Projektstruktur

- `include/rail.hpp`: Datentypen und Schnittstellen
- `src/rail.cpp`: Fahrplanverwaltung und Verbindungssuche
- `src/main.cpp`: Benutzereingabe und Ergebnisausgabe
- `data/timetable.csv`: Beispieldaten
- `tests/test_rail.cpp`: Automatisierte Tests
- `CMakeLists.txt`: Buildkonfiguration
- `.github/workflows/main.yml`: GitHub Actions

## Kompilieren

Die folgenden Befehle im Hauptverzeichnis des Projekts ausführen:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Starten

Unter Linux oder mit einem passenden Single-Config-Generator:

```sh
./build/railconnect
```

Bei Verwendung des Visual-Studio-Generators unter Windows:

```powershell
.\build\Release\railconnect.exe
```

Das Programm fragt anschließend nach Startbahnhof, Zielbahnhof und frühester Abfahrtszeit.

Beispieleingaben:

```text
From: Leverkusen Mitte
To: Duisburg Hbf
Earliest departure (HH:MM): 08:00
```

Bahnhofsnamen müssen exakt den Einträgen im CSV-Fahrplan entsprechen.

### Suche mit Kommandozeilenargumenten

```sh
./build/railconnect data/timetable.csv "Leverkusen Mitte" "Duisburg Hbf" 08:00
```

### Verfügbare Bahnhöfe anzeigen

```sh
./build/railconnect data/timetable.csv --stations
```

## Beispielausgabe

```text
RailConnect | Fictional demonstration timetable

1. 08:30 -> 09:20 | 50 min | changes: 0
   RE103 | Leverkusen Mitte -> Duisburg Hbf | 08:30 - 09:20

2. 08:15 -> 09:20 | 65 min | changes: 1
   RE102 | Leverkusen Mitte -> Duesseldorf Hbf | 08:15 - 08:50
   RE202 | Duesseldorf Hbf -> Duisburg Hbf | 09:00 - 09:20
```

## CSV-Format

```csv
train,from,to,departure,arrival
RE101,Leverkusen Mitte,Koeln Hbf,08:05,08:25
```

Jede Zeile beschreibt einen einzeln buchbaren Fahrtabschnitt zwischen zwei Bahnhöfen. Buchungen selbst werden nicht unterstützt.

- Uhrzeiten müssen das Format `HH:MM` haben.
- Felder dürfen keine Kommas oder Anführungszeichen enthalten.
- Führende und nachgestellte Leerzeichen werden nicht entfernt.
- Liegt die Ankunftszeit vor der Abfahrtszeit, wird eine Ankunft am Folgetag angenommen.
- Identische Abfahrts- und Ankunftszeiten werden abgelehnt.

## Tests ausführen

```sh
ctest --test-dir build -C Release --output-on-failure
```

Die Tests prüfen unter anderem:

- Gültige und ungültige Uhrzeiten
- Direkte Verbindungen und Umsteigeverbindungen
- Mindestumsteigezeit und Ergebnissortierung
- Unbekannte Bahnhöfe und erfolglose Suchanfragen
- Ankünfte nach Mitternacht
- Fehlende Dateien und fehlerhafte CSV-Daten

GitHub Actions führt den Build und die Tests bei Änderungen automatisch aus.

## Technischer Aufbau

`Leg` speichert einen Fahrtabschnitt. `Journey` fasst die Abschnitte einer Verbindung zusammen. `Timetable` verwaltet die Daten und stellt die Suchfunktion bereit.

Zeitpunkte werden als Minuten seit Tagesbeginn gespeichert. Dadurch lassen sich Zeiten vergleichen und Reisedauern einschließlich Wartezeiten berechnen.

Die Suche prüft direkte Verbindungen und anschließend mögliche Kombinationen aus zwei Abschnitten. Der Suchaufwand beträgt O(n²), zuzüglich der Sortierung der Ergebnisse.

## Grenzen und mögliche Erweiterungen

Die aktuelle Version unterstützt höchstens einen Umstieg. Alle Abfahrten gehören zu einem Fahrplantag; Abfahrten und Umstiege am Folgetag werden nicht modelliert. Rückfahrten müssen separat im Fahrplan enthalten sein.

Mögliche Erweiterungen:

- Mehrere Umstiege
- Datumsabhängige Fahrpläne
- Effizientere Suchverfahren für größere Datenmengen
- Zusätzliche Filter und eine grafische Oberfläche

## Entwicklungshinweis

Diese Portfolio-Version wurde mit Unterstützung künstlicher Intelligenz erstellt. Sie dient als Lernprojekt und als Grundlage für die eigene Weiterentwicklung.
