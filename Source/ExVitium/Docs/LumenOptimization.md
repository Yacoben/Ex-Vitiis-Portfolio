# Lumen — notatki robocze (UE 5.5+ / 5.7 / 5.8)

Notatki z optymalizacji Lumena. Skrót: **co jest drogie, jaki CVar to rusza, w jakiej kolejności ciąć.**

> Uwaga na CVary z internetu: nazwy zmieniają się między wersjami silnika. Zanim wpiszesz coś do `DefaultEngine.ini`,
> sprawdź w konsoli (`Help <nazwa>`) albo w źródłach swojej wersji. Przykład z życia: `r.Lumen.SceneUpdateRayTracingBudget`
> **nie istnieje** — realne kontrolki kosztu update'u sceny to `r.LumenScene.SurfaceCache.*` + gęstość probe'ów.

---

## TL;DR — 10 zasad

1. **Najpierw mierz, potem optymalizuj.** `stat gpu` → który pass dominuje → atakuj tylko jego.
2. **Reflections są zwykle droższe niż diffuse GI.** Pierwszy duży lever.
3. **Screen probe gather** to drugi duży lever: liczba rayów (`TracingOctahedronResolution`) + gęstość probe'ów (`DownsampleFactor`).
4. **Wnętrza i open world to dwa różne problemy** — nie ma jednego zestawu ustawień.
5. **Distance Fields to fundament Software Ray Tracing.** Złe SDF = light leaking, a nie problem wydajności.
6. **Mniej świateł, więcej emissive.** Każde światło dokłada do radiosity i surface cache.
7. **Static zostaje Static.** Movable = Lumen zakłada zmianę co klatkę = więcej update'ów sceny.
8. **Wizualizuj scenę Lumena** (`ShowFlag.VisualizeLumenScene 1`) — czego tam nie widać, tego Lumen nie oświetla.
9. **Lumen nie zbiega w jednej klatce** — akumulacja temporalna. Ghosting i wolne "wypełnianie się" GI to cecha, nie bug.
10. **Wysyłaj presety scalability**, nie jedną konfigurację.

---

## Jak Lumen liczy światło (skrót pipeline'u)

Bez tego nie da się sensownie optymalizować — trzeba wiedzieć, za co się płaci.

### Trzy metody trace'owania

| Metoda | Koszt | Jakość | Ograniczenie |
|---|---|---|---|
| **Screen traces** | prawie darmowe | dobra | tylko to, co jest na ekranie |
| **Mesh Distance Fields (SWRT)** | średni | przybliżona | cienkie ściany, wklęsłości, gęste wnętrza wychodzą źle |
| **Hardware Ray Tracing (HWRT)** | wysoki | dokładna | wymaga RT cores (RTX 2000+ / RX 6000+) |

- **Screen traces** — Lumen najpierw strzela w bufory ekranowe (depth, normal, scene color z poprzedniej klatki). Dane już są w pamięci, więc to praktycznie gratis. Kamera odwróci się od czerwonej ściany → bounce z niej znika.
- **Mesh Distance Fields** — każdy static mesh generuje przy imporcie signed distance field (wolumen 3D z odległością do najbliższej powierzchni). Ray przeskakuje puste obszary w pojedynczych krokach zamiast iść trójkąt po trójkącie. To jest koń pociągowy Software Ray Tracing.
- **HWRT** — trace po realnych trójkątach na RT cores. Dokładniej, drożej. Szczegóły niżej.

### Lumen Scene + surface cache

Lumen trzyma **uproszczoną reprezentację levelu** (Lumen Scene), osobną od głównej sceny renderowej. Składa się z **surface cache cards** — płaskich łatek z zapamiętanymi właściwościami materiału (base color, emissive, roughness). Dzięki temu trafienie raya nie musi ewaluować pełnego shadera PBR. To jest właśnie powód, dla którego koszt hita jest do udźwignięcia.

### Akumulacja temporalna

Lumen strzela ograniczony budżet rayów na klatkę i **akumuluje wynik w czasie**. Stąd:
- GI "dochodzi" przez kilka klatek po wejściu kamery w nowy obszar,
- przy szybkim ruchu widać ghosting / wolną zbieżność.

To świadomy trade-off: rozłożenie pracy na klatki trzyma koszt per-frame w ryzach.

---

## SWRT vs HWRT — kiedy co

**Bierz HWRT, gdy:**
- target to high-end PC i masz zapas w budżecie,
- geometria źle reprezentowana przez SDF (cienkie ściany, gęsta siatka, złożone wnętrza),
- dokładność wizualna jest priorytetem,
- **light leaking nie znika mimo podnoszenia Distance Field Resolution**.

**Nie bierz HWRT, gdy:**
- target konsolowy (ograniczony RT throughput) — tam SWRT jest często właściwym domyślnym,
- mid-range GPU przy celu 60 fps,
- scena zdominowana przez outdoor, gdzie SWRT i tak wystarcza,
- sprzęt nie ma RT cores.

**Wzorzec hybrydowy** (często najlepszy stosunek jakości do kosztu):

```ini
r.Lumen.DiffuseIndirect.HardwareRayTracing=0   ; GI po SDF — przybliżenie akceptowalne
r.Lumen.Reflections.HardwareRayTracing=1       ; reflections po trójkątach — tu dokładność widać
```

> Od UE 5.5 HWRT jest domyślną i rekomendowaną ścieżką przez Epic — ale to rekomendacja pod sprzęt z RT cores, nie uniwersalna prawda.

---

## Budżet klatki

60 fps = **16,67 ms na klatkę**. Przykładowy rozkład (orientacyjny, nie wyrocznia):

| Pass | Budżet |
|---|---|
| Lumen GI | 3–4 ms |
| Lumen Reflections | 1–2 ms |
| Lumen Scene Update | 1 ms |
| Nanite Rasterization | 2–3 ms |
| Shadow Rendering | 2 ms |
| Post Processing | 1–2 ms |
| Game Logic + CPU | 3–4 ms |
| **Razem** | **~16 ms** |

**Reguła kciuka:** jeśli Lumen łącznie zjada >5–6 ms, trzeba schodzić z jakości albo upraszczać scenę.

Te liczby są mocno zależne od projektu i targetu — traktuj je jako punkt startowy, a nie cel. Realny budżet mierzy się na własnej scenie.

---

## CVary, które faktycznie coś robią

Lumen ma długą listę zmiennych, większość to wewnętrzne debug toggle. Poniżej te, które ruszają wydajność w produkcji.

### Master switche (do A/B testów kosztu)

| CVar | Opis |
|---|---|
| `r.Lumen.DiffuseIndirect.Allow` | Włącznik diffuse GI. Ustaw `0`, żeby w sekundę zmierzyć, ile kosztuje GI. |
| `r.Lumen.Reflections.Allow` | Włącznik reflections. Zwykle **duża** część kosztu Lumena. Brak wyraźnie błyszczących powierzchni w scenie → wyłącz i zjedź na SSR + reflection captures. To jedna z największych pojedynczych oszczędności. |

### Screen Probe Gather (główny pass GI)

| CVar | Opis | Kierunek |
|---|---|---|
| `r.Lumen.ScreenProbeGather.TracingOctahedronResolution` | Liczba rayów na probe przez mapowanie oktaedryczne — wartość N oznacza N×N kierunków. **Realne wartości z `BaseScalability.ini` (5.8): `4` na Medium, `8` na High i Epic, `16` na Cine.** | ↓ = taniej, ale więcej noise'u, wolniejsza zbieżność, słabszy color bleeding. **Jeden z najmocniejszych pokręteł.** |
| `r.Lumen.ScreenProbeGather.DownsampleFactor` | Gęstość probe'ów (jeden probe na blok N×N pikseli). **W 5.8: `32` na High, `16` na Epic, `8` na Cine.** | ↑ = mniej probe'ów = taniej, ale ryzyko niskoczęstotliwościowych artefaktów w GI. |
| `r.Lumen.ScreenProbeGather.MaxRayIntensity` | Cap intensywności pojedynczego raya (tłumienie firefly). | ↑ = zachowuje mocne źródła (jasne emissive, tarcza słońca w oknie). ↓ = spokojniejszy, mniej zaszumiony obraz, szybsza zbieżność. |
| `r.Lumen.ScreenProbeGather.SpatialFilterNumPasses` | Liczba przebiegów filtra przestrzennego na danych probe'ów. | ↑ = gładsze GI, minimalnie drożej. ↓ = taniej, więcej noise'u. |
| `r.Lumen.ScreenProbeGather.StochasticInterpolation` | Przenosi część pipeline'u probe'ów na niższą precyzję dla szybkości. | Dobry lever pod tier konsolowy — sprawdź wizualny koszt na swoim contencie. |
| `r.Lumen.ScreenProbeGather.TemporalFilterProbes` | Akumulacja temporalna probe'ów. | ↑ = gładsze GI, więcej ghostingu. |

### Reflections

| CVar | Opis |
|---|---|
| `r.Lumen.Reflections.MaxRoughnessToTrace` | Lumen strzela rayami odbiciowymi tylko poniżej tego progu roughness. **Domyślnie `-1`, co znaczy „użyj ustawienia z Post Process Volume”** — normalnym miejscem strojenia jest więc PPV, a nie CVar; wartość `>= 0` nadpisuje PPV globalnie. Obniżenie wypycha średnio-szorstkie powierzchnie na dane z probe'ów GI. **Powyżej ~0.4 roughness i tak nie widać wyraźnego odbicia** — duża oszczędność przy znikomym koszcie wizualnym. |
| `r.Lumen.Reflections.MaxRoughnessToTraceClamp` | Twardy clamp na powyższe, z flagą scalability. Właściwy knob, jeśli chcesz ograniczyć odbicia per tier bez ruszania PPV. |
| `r.Lumen.Reflections.DownsampleFactor` | `2` = pół rozdzielczości odbić. Zwykle **1–2 ms** w plecy. |

### Hardware Ray Tracing

| CVar | Opis |
|---|---|
| `r.Lumen.HardwareRayTracing` | `1` = HWRT, `0` = SWRT / distance fields. |
| `r.Lumen.DiffuseIndirect.HardwareRayTracing` | Per-feature toggle dla diffuse GI. |
| `r.Lumen.Reflections.HardwareRayTracing` | Per-feature toggle dla reflections. |
| `r.Lumen.HardwareRayTracing.LightingMode` | `0` = surface cache (tanio), `1` = hit lighting (lepsza jakość odbić, drożej). |
| `r.Lumen.HardwareRayTracing.HitLighting.Allowed` | To jest CVar, którym steruje scalability w 5.8: `0` na High, `1` dopiero na Epic. Zejście z Epic na High wyłącza hit lighting **i to jest jedna z największych różnic kosztowych między tymi tierami**. |
| `r.Lumen.HardwareRayTracing.MaxTraceDistance` | Maksymalny dystans raya. Krótszy = taniej, szczególnie we wnętrzach. |
| `r.RayTracing.Culling` | Culling obiektów ze struktury akceleracyjnej RT. **Domyślnie już `3`** (najagresywniejszy tryb: odcinaj po dystansie LUB kącie bryłowym, przed i za kamerą) — nie ma tu czego włączać. |
| `r.RayTracing.Culling.Radius` | **To jest realny lever, nie sam `Culling`.** Domyślnie `30000`, czyli 300 m. Dla mapy areny/wnętrza to wielokrotnie za dużo — zejście do rozmiaru faktycznie odwiedzanej przestrzeni tnie BVH. Zbyt agresywne = popping w odbiciach. |
| `r.RayTracing.Culling.Angle` | Próg kąta bryłowego (domyślnie `1` stopień) — odcina drobiazgi na dystansie. |

### Lumen Scene / surface cache

| CVar | Opis |
|---|---|
| `r.LumenScene.SurfaceCache.MeshCardsMinSize` | Minimalny rozmiar mesha (jednostki świata) kwalifikujący do generowania cards. Podniesienie progu wyrzuca drobne propsy z Lumen Scene i tnie koszt update'u. Mocny lever w open worldzie z tysiącami małych meshy. |
| `r.Lumen.TraceMeshSDFs` | Trace po mesh SDF (ścieżka software). |

### Lumen Scene Detail

Ustawienie w **Post Process Volume / project settings**, nie CVar. Kontroluje rozdzielczość i pokrycie surface cache.

- start: **1.0** (default),
- **0.5** gdy budżet na scene update jest napięty (typowo rozległe levele zewnętrzne),
- **>1.0** tylko dla contentu archviz-owego, gdzie dużo drobnych obiektów musi dokładnie uczestniczyć w GI.

Za nisko → nieprzykryte powierzchnie i artefakty GI. Za wysoko → płacisz w VRAM i czasie update'u.

---

## Distance Fields — praktyka

Rozdzielczość SDF liczy się automatycznie z bounding boxa. Override per-mesh: **Static Mesh Editor → Build Settings → Distance Field Resolution Scale**.

- **Za nisko** → SDF kiepsko przybliża geometrię: raye przechodzą przez cienkie ściany, wnętrza przeciekają światłem z zewnątrz, wklęsłości się "zalewają".
- **Za wysoko** → VRAM + koszt update'u Lumen Scene.

| Typ mesha | Zalecenie |
|---|---|
| **Architektura** (ściany, podłogi, sufity) | Trzymaj przyzwoitą rozdzielczość. Widzisz light leaking → podnieś `Distance Field Resolution Scale`. To fundament poprawnego GI we wnętrzach, wart pamięci. |
| **Średnie propsy** | Default zwykle wystarcza. Ewentualnie skala `0.5` na mniej istotnych. |
| **Drobne propsy** (kubki, gruz, kamyki) | Rozważ **całkowite wyłączenie generowania SDF** na meshu albo globalne odcięcie przez `MeshCardsMinSize`. |
| **Foliage** | SDF cienkich liści wygląda jak kleks niezależnie od rozdzielczości. Lumen i tak opiera się tu głównie na screen traces i surface cache — obniżenie rozdzielczości rzadko cokolwiek psuje. |

- Do generowania SDF używaj **prostszych collision meshy**.
- Pamięć monitoruj przez **`stat distancefields`**.
- Editor **flaguje meshe z nieudanym/kiepskim SDF** — to są dokładnie te, których Lumen nie oświetli poprawnie. Naprawiaj na etapie contentu, nie na etapie optymalizacji. Najlepiej zachowują się meshe watertight, o rozsądnej gęstości siatki, bez ekstremalnie cienkich fragmentów.

---

## Scena — co robić z contentem

### Światła

- **Max ~4 dynamiczne światła na jedną powierzchnię.** Każde źródło dokłada koszt trace'owania.
- **Ciasne attenuation radius** na point/spot lightach — ogranicza objętość, którą Lumen musi liczyć.
- **Wyłącz "Affect Global Illumination"** na małych światłach akcentowych, które nie muszą się odbijać.
- **Konsoliduj źródła.** Rząd świetlówek nie potrzebuje dziesięciu light actorów — jeden–dwa rect lighty + materiał emissive czytają się identycznie i kosztują ułamek.
- **Rect lights > point lights** we wnętrzach: miękkie cienie lepiej współpracują z surface cache.

### Materiały

- **Emissive** — trzymaj rozsądną powierzchnię. Wielkie emissive (ekrany, lawa, neony) zmuszają Lumena do trace'owania z wielu kierunków. `Emissive Boost` używaj ostrożnie.
- **Translucent** — Lumen **nie trace'uje przez powierzchnie półprzezroczyste**. Projektuj z tym założeniem.
- **Two-sided foliage** — drogie dla Lumena, preferuj single-sided.
- **Surface cache** — istotne powierzchnie potrzebują sensownego pokrycia UV.

### Scatter / foliage w dużych ilościach

Jeden kamień kosztuje zero. Pięć tysięcy rozrzuconych kamieni, każdy z distance fieldem, to realna presja na update Lumen Scene i traversal SWRT.

- **Ciasne cull distance** na warstwach scatteru — dalekie kamienie nie muszą uczestniczyć w Lumenie; od tego są HLOD-y i impostory.
- **Wyłącz generowanie SDF** na trawie, gruzie i drobnych detalach — nie wnoszą nic do kształtu GI.
- **Pilnuj liczby instancji** uczestniczących w Lumenie — to one skalują koszt traversalu.

---

## Wnętrza vs open world

To dwa różne zestawy ustawień.

### Wnętrza — najlepsza jakość, najtrudniejsza wydajność

Multi-bounce GI we wnętrzu to jest to, co sprawia, że Lumen wygląda świetnie. Jednocześnie: dużo powierzchni zasłoniętych, dużo bliskich źródeł światła, agresywniejsze radiosity.

- Konsoliduj światła (emissive zamiast light actorów).
- **Trzymaj zdrowe SDF na ścianach i sufitach** — przeciekanie światła przez ścianę to najbardziej rażący artefakt we wnętrzach.
- Rect lights zamiast point lightów.
- **Post Process Volume per pomieszczenie** z dostrojonymi ustawieniami Lumena, zamiast jednego globalnego volume.

### Open world — długie trace'y, dominacja nieba

- Zjedź z **Lumen Scene Detail**.
- Ciasne cull distance dla uczestnictwa w distance fieldach.
- **Niższa gęstość probe'ów i mniej rayów** — outdoorowe GI jest zdominowane przez niebo i jedno directional light, zbiega przy mniejszej liczbie rayów.
- **Wyłącz Lumen Reflections**, chyba że masz dużą wodę albo mokre powierzchnie.
- Opieraj dalekie trace'y na **global distance field**.
- Rozważ **Far Field** do obniżenia jakości trace'ów na dystansie.

### Przejścia (drzwi, okna, wyloty jaskiń)

Najtrudniejszy przypadek. Używaj occluder volumes, stawiaj granice Post Process Volume dokładnie w progu (przełączanie presetów), resztę wygładzi akumulacja temporalna.

---

## Profilowanie — workflow

**Nigdy nie optymalizuj na ślepo.**

```
stat gpu            // rozbicie czasu GPU po kategoriach — start tutaj
stat LumenScene     // statystyki Lumena
stat distancefields // pamięć distance fieldów
stat RHI            // pamięć i zasoby
profilegpu          // szczegółowa lista passów z jednej klatki
```

Passy Lumena, których szukasz w `stat gpu` / `profilegpu`:

| Pass | Co to |
|---|---|
| **Lumen Scene Update** | Aktualizacja reprezentacji sceny |
| **Lumen Screen Probe Gather** | Główne liczenie GI |
| **Lumen Reflections** | Koszt trace'owania odbić |
| **Lumen Radiosity** | Odbicia pośrednie (bounce) |

Suma tych czterech = łączny koszt Lumena.

**Unreal Insights** — do analizy klatka po klatce:

1. Odpal z `-trace=default,gpu`
2. Otwórz panel **Timing Insights**
3. Filtruj po "Lumen"
4. Porównaj czasy klatek z różnych pozycji kamery

**Wizualizacja:** `ShowFlag.VisualizeLumenScene 1` — pokazuje, co Lumen faktycznie widzi. Jeśli powierzchni tam nie ma, nie wnosi nic do GI. To zwykle źródło artefaktu, który w ogóle sprowokował optymalizację.

---

## Quick wins — kolejność cięcia

Jesteś ponad budżetem? Po kolei:

1. **Pół rozdzielczości odbić** — `r.Lumen.Reflections.DownsampleFactor=2` → często **1–2 ms**.
2. **Rzadsze probe'y GI** — podnieś `ScreenProbeGather.DownsampleFactor` powyżej `32`. Tańsze i mniej ryzykowne wizualnie niż zbijanie `TracingOctahedronResolution` z `8` na `4`, bo to drugie uderza w noise i zbieżność.
3. **Wyłącz hit lighting** — `r.Lumen.HardwareRayTracing.LightingMode=0` (albo po prostu zejdź z tieru Epic na High, który robi to sam przez `HitLighting.Allowed=0`).
4. **Obniż `MaxRoughnessToTrace`** — odbicia tylko na gładkich powierzchniach.
5. **Ściśnij attenuation radius** świateł — mniejsza objętość do trace'owania.
6. **Upscaling** — TSR / FSR / DLSS, render w niższej rozdzielczości wewnętrznej.

### Audyt odbić (największy pojedynczy lever)

1. Sprawdź, które powierzchnie **naprawdę** potrzebują dokładnych odbić. Beton, drewno, tkanina, ziemia — wszystko powyżej ~0.5 roughness nie daje widocznego speculara. Realnie korzystają: szkło, metal, woda, polerowany kamień, mokre powierzchnie.
2. Obniż `MaxRoughnessToTrace`.
3. Brak wyraźnie błyszczących powierzchni → **wyłącz Lumen Reflections całkowicie**, zjedź na SSR + reflection captures. SSR jest dużo tańsze, kosztem odbijania tylko tego, co na ekranie.
4. Rozstaw **reflection capture actors** w kluczowych miejscach jako fallback dla SSR poza ekranem.
5. **Lustra to worst case.** Płaskie, idealnie gładkie, w pełni metaliczne wymagają wysokiej jakości trace'u. Ogranicz je do konkretnych miejsc, zaakceptuj noise, albo użyj planar reflections / render-to-texture dla lustra "bohaterskiego".

---

## Pułapki i typowe błędy

| Pułapka | Objaw / mitygacja |
|---|---|
| **Open world** | Trace'y drożeją przy długich dystansach widzenia. → Far Field, krótsze max trace distance, hybryda: Lumen wewnątrz, prostsze GI na zewnątrz. |
| **Wielkie emissive** | Ekrany, lawa, neony zmuszają do trace'owania z wielu kierunków. → ogranicz powierzchnię. |
| **Lustra** | Wymagają wysokiej jakości trace'ów odbić. → limituj liczbę luster w jednym kadrze. |
| **Drobne obiekty z dużym detalem** | Drogie distance fieldy. → LOD-y albo wyłączenie SDF na małych propsach. |
| **Za dużo małych świateł** | Każde point/spot dokłada do radiosity i update'u surface cache. → rect light + emissive. |
| **Ignorowanie ostrzeżeń o distance fieldach** | To są meshe, których Lumen nie trace'uje poprawnie. → napraw content, nie ustawienia. |
| **Wszystko ustawione na Movable** | Lumen traktuje movable jako zmienne co klatkę → więcej update'ów Lumen Scene. → geometria statyczna zostaje **Static**, Movable tylko dla tego, co naprawdę się rusza. |
| **Pomijanie trybów wizualizacji** | Nie wiesz, co Lumen widzi. → `ShowFlag.VisualizeLumenScene 1`. |
| **Optymalizacja bez pomiaru** | Najdroższy błąd ze wszystkich. → `stat gpu`, `profilegpu`, Insights. |

---

## Scalability — presety do wysyłki

Nie wybieraj jednej konfiguracji Lumena. **Zanim napiszesz własny `DefaultScalability.ini`, sprawdź stockowe presety** w `Engine/Config/BaseScalability.ini` — w 5.8 są dobrze dobrane i bardzo często któryś z nich to dokładnie to, czego chcesz. Twój plik projektu tylko nadpisuje/dokłada wpisy do sekcji z Base.

Grupy istotne dla Lumena:

| Grupa scalability | Co obejmuje |
|---|---|
| `GlobalIlluminationQuality@0..3` | całe diffuse GI: `FinalGatherMethod`, `ScreenProbeGather.*`, `LumenScene.SurfaceCache.*`, `Radiosity.*`, `HardwareRayTracing.HitLighting.Allowed` |
| `ReflectionQuality@0..3` | `Lumen.Reflections.Allow`, `Reflections.DownsampleFactor`, `SSR.Quality` |
| `ShadowQuality@0..3` | Virtual Shadow Maps **oraz cały `r.MegaLights.*`** — MegaLights nie siedzi w grupie GI, tylko tutaj |

Zmiana jakości GI w runtime **działa bez restartu** — Lumen rekonwerguje w kilka klatek.

### Co realnie robią stockowe poziomy (UE 5.8)

| Poziom | GI | Reflections | MegaLights |
|---|---|---|---|
| **0 — Low** | `Lumen.DiffuseIndirect.Allow=0` (Lumen off, zostaje DFAO) | `Lumen.Reflections.Allow=0`, `SSR.Quality=0` | brak wpisów |
| **1 — Medium** | `FinalGatherMethod=0` (Irradiance Volume zamiast probe gather), `SurfaceCache.AtlasSize=2048`, `TraceMeshSDFs.Allow=0` | `Reflections.Allow=0`, `SSR.Quality=2` half-res | `NumSamplesPerPixel=2` |
| **2 — High** | `FinalGatherMethod=1`, `DownsampleFactor=32`, `TracingOctahedronResolution=8`, `AtlasSize=3584`, `Radiosity.ProbeSpacing=8`, **`HitLighting.Allowed=0`** | `Reflections.Allow=1`, **`DownsampleFactor=2`** (pół rozdzielczości), `SSR.Quality=2` | `NumSamplesPerPixel=4`, `Volume.GridPixelSize=16` |
| **3 — Epic** | `DownsampleFactor=16` (2× więcej probe'ów), `AtlasSize=4096`, `Radiosity.ProbeSpacing=4`, `TraceMeshSDFs.Allow=1`, **`HitLighting.Allowed=1`** | `DownsampleFactor=1` (pełna rozdzielczość), `SSR.Quality=3` | `NumSamplesPerPixel=4`, `Volume.GridPixelSize=8`, `GridSizeZ=128` |

**Największy skok kosztu jest między High a Epic**, i to z dwóch powodów naraz: Epic włącza hit lighting na HWRT i podnosi odbicia do pełnej rozdzielczości. Jeśli walczysz o 60 fps, to jest pierwsze miejsce, w które patrzysz — a nie ręczne dłubanie w CVarach.

### Pułapka: domyślnie wszystko leci na Epic

`FQualityLevels::SetDefaults()` ustawia **wszystkie grupy na Epic (3)**, nie na auto-detect. Jeśli projekt nie ma `Config/DefaultGameUserSettings.ini` z sekcją `[ScalabilityGroups]`, to świeży profil gracza (i edytor bez zapisanych ustawień) startuje na najdroższym tierze. Przypnij docelowy poziom jawnie:

```ini
; Config/DefaultGameUserSettings.ini
[ScalabilityGroups]
sg.GlobalIlluminationQuality=2
sg.ReflectionQuality=2
sg.ShadowQuality=2
```

Alternatywa dla gry na wysyłkę: `UGameUserSettings::RunHardwareBenchmark()` + `ApplyHardwareBenchmarkResults()` przy pierwszym uruchomieniu, żeby dobrać tier pod GPU gracza.

---

## MegaLights

Włączane per projekt przez `r.MegaLights.EnableForProject`, strojone w grupie **`ShadowQuality`** (nie GI).

MegaLights zmienia sposób liczenia **direct lighting i cieni od lokalnych świateł** — zamiast płacić za każde światło osobno, próbkuje je stochastycznie ze wspólnego budżetu. Praktyczna konsekwencja dla contentu:

- **Zasada „max ~4 dynamiczne światła na powierzchnię" przestaje obowiązywać.** To jest cały sens MegaLights — wiele cieniujących świateł lokalnych staje się policzalne.
- Koszt przestaje skalować się liniowo z liczbą świateł, a zaczyna zależeć od `r.MegaLights.NumSamplesPerPixel` (`2` na Medium, `4` na High/Epic) i rozdzielczości grida wolumetrycznego.
- Cena: szum przy dużej liczbie świateł o dużym promieniu, zależność od akumulacji temporalnej.
- `r.LumenScene.DirectLighting.MaxLightsPerTile` (`4` na High, `8` na Epic) nadal ogranicza, ile świateł wchodzi do **Lumen Scene** na potrzeby GI — to osobna sprawa od direct lightingu przez MegaLights.

**Uwaga przy planowaniu contentu:** jeśli MegaLights jest włączone, agresywne wycinanie point lightów z pochodni i świec może być pracą na marne. Zmierz wariant z lightami przy włączonym MegaLights, zanim zrobisz osobne assety bez nich.

---

## Lumen + Nanite

Zaprojektowane do współpracy:

- Nanite dostarcza dokładne dane do **surface cache**,
- trace'y HWRT korzystają ze struktury **BVH** Nanite'u,
- automatyczne LOD-y Nanite'u trzymają złożoność geometryczną w ryzach dla trace'ów.

Warunek: meshe Nanite muszą mieć sensowny layout UV pod surface cache.
W naszym przypadku Nanite mozna pominac. Nie jest on potrzebny do optymalizacji Lumena. Ale mozna w przyszlosci o tym pomyslec zeby nie musiec robic LODow i miec wiecej szczegolow w scenie. Nanite jest drogi w VRAM i CPU, ale daje lepsza geometrie do traceowania.

---

## Ustawienia projektu ExVitium — stan i cel

**Cel:** RTX 3060, Lumen GI na HWRT w jakości High, Lumen Reflections włączone w połowie rozdzielczości, stabilne 60 fps (chętnie więcej). Światło w pełni dynamiczne (fizyka + destrukcja), więc baked lighting nie wchodzi w grę.

**Decyzje architektoniczne** (`DefaultEngine.ini` → `[/Script/Engine.RendererSettings]`) — to są przełączniki on/off, nie strojenie jakości:

| CVar | Wartość | Dlaczego |
|---|---|---|
| `r.RayTracing` | `True` | wymagane przez HWRT |
| `r.Lumen.HardwareRayTracing` | `True` | wybrana ścieżka trace'owania |
| `r.MegaLights.EnableForProject` | `True` | dużo lokalnych świateł (pochodnie, świece) |
| `r.Lumen.HardwareRayTracing.LightingMode` | `0` | surface cache zamiast hit lighting |
| `r.AllowStaticLighting` | `False` | światło dynamiczne obowiązkowe |
| `r.GenerateMeshDistanceFields` | `True` | DFAO, distance field shadows, far field |
| `r.Nanite.ProjectEnabled` | `False` | świadomie — patrz sekcja Lumen + Nanite |
| `r.RayTracing.Shadows` | `False` | cienie idą przez VSM + MegaLights |

**Jakość** siedzi wyłącznie w scalability (`DefaultGameUserSettings.ini` → `[ScalabilityGroups]`), przypięta na poziom **2 (High)** dla `GlobalIlluminationQuality`, `ReflectionQuality` i `ShadowQuality`. Reszta grup na Epic.

> **Nie duplikuj wpisów scalability w `[/Script/Engine.RendererSettings]`.** Utrzymuj rozdział: `DefaultEngine.ini` = co jest włączone, scalability = jak dobre. Wpisy z `RendererSettings` lecą raz na starcie, scalability nadpisuje je przy każdej zmianie tieru — mieszanie tych dwóch miejsc kończy się ustawieniami, które „nie działają" bez widocznego powodu.

### Kolejność szukania headroomu ponad 60 fps

Wszystko dopiero **po** `stat gpu` — poniższe to kolejność, nie recepta.

1. `r.RayTracing.Culling.Radius` — domyślne `30000` (300 m) jest absurdalnie duże dla mapy areny. Zejście do rzeczywistego rozmiaru przestrzeni zmniejsza BVH i koszt trace'ów HWRT. Najtańszy zysk w tym projekcie.
2. `r.Lumen.ScreenProbeGather.DownsampleFactor` — z `32` na `48`. Mniej ryzykowne wizualnie niż zbijanie `TracingOctahedronResolution`.
3. `r.MegaLights.NumSamplesPerPixel` — z `4` na `2` (to jest wartość z tieru Medium). Sprawdź szum na scenie z wieloma pochodniami.
4. `r.LumenScene.Radiosity.UpdateFactor` / `ProbeSpacing` — jeśli **Lumen Scene Update** dominuje w `stat gpu`.
5. `MaxRoughnessToTrace` w Post Process Volume — jeśli **Lumen Reflections** dominuje.
6. TSR z `r.ScreenPercentage` poniżej 100 — ostateczność, ale daje najwięcej.

### Znane drobiazgi do posprzątania

- `r.MSAACount=4` przy `r.AntiAliasingMethod=2` i deferred shading — MSAA nie działa w deferred, ustawienie jest martwe (mylące, nie szkodliwe).
- `r.Substrate=True` z `BytesPerPixel=80` i `ClosuresPerPixel=4` — gruby GBuffer, realny koszt obok Lumena. `EnableLayerSupport=False` sugeruje, że warstwy Substrate nie są używane; sprawdź w `stat gpu`, ile kosztuje base pass, zanim zaczniesz stroić sam Lumen.
- `r.ReflectionCaptureResolution=128` — nabiera znaczenia tylko, gdyby Lumen Reflections poszły do wyłączenia i SSR potrzebowałby fallbacku.
