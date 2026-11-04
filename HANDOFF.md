# island-simulator — aktuální předání

Revize dokumentu: 2026-11-04. Tento soubor shrnuje stav údržby; chronologie je v Gitu.

## Co platí teď

- Projekt je C++23 aplikace na CNA. Pracovní větev je `develop`.
- Poslední změny produkčního kódu: `52d8c56e` (kontakt chobotnic se dnem),
  `05077820` (odstup od útesových balvanů), `5e74c5d0` (společné osvětlení chobotnic a rejnoků).
- Poslední webový build vznikl ze zdrojového stavu `30106358` dne 2026-11-03.
  Tento commit také vrátil omylem přidané `AGENTS.md` a `CLAUDE.md`; žádná nová AI policy tím neplatí.
- Vizuální kampaň má doložené nativní softwarové ověření. To není úplná kvalifikace všech platforem.
  Níže jsou výslovně uvedené neúspěšné webové kontroly a otevřené problémy.
- Staré pokyny k nekonečnému pokračování a hodinové cíle v pracovních denících nejsou aktuální zadání.

## Kde hledat kód

| Oblast | Hlavní vstupy |
| --- | --- |
| Herní smyčka, kamery, propojení systémů | [IslandGame.cpp](game/src/IslandGame.cpp) |
| Výška terénu a skutečně vykreslený povrch | [IslandTerrain.cpp](game/src/IslandTerrain.cpp), [RenderedTerrainSurface.cpp](game/src/RenderedTerrainSurface.cpp) |
| Vlhkost, expozice a pobřežní prostředí | [IslandEnvironmentField.hpp](game/include/IslandSimulator/IslandEnvironmentField.hpp) |
| Rozmístění vegetace a přírodních objektů | [JungleLayout.cpp](game/src/JungleLayout.cpp), [GrassLayout.cpp](game/src/GrassLayout.cpp), [GroundCoverField.cpp](game/src/GroundCoverField.cpp), [NaturalPropLayout.cpp](game/src/NaturalPropLayout.cpp) |
| Vegetace, oceán, stíny | [JungleRenderer.cpp](game/src/JungleRenderer.cpp), [OceanRenderer.cpp](game/src/OceanRenderer.cpp), [SunShadowMap.cpp](game/src/SunShadowMap.cpp) |
| Počasí a denní cyklus | [WeatherSystem.cpp](game/src/WeatherSystem.cpp), [TimeOfDayRenderer.cpp](game/src/TimeOfDayRenderer.cpp), [SkyRenderer.cpp](game/src/SkyRenderer.cpp) |
| Chůze a vozidlo | [PlayerMotion.cpp](game/src/PlayerMotion.cpp), [QuadBike.cpp](game/src/QuadBike.cpp) |
| Zvířata a obyvatel | Skupiny/stáda a jejich `*Renderer.cpp`; např. [OctopusGroup.cpp](game/src/OctopusGroup.cpp), [ResidentMotion.cpp](game/src/ResidentMotion.cpp) |

Rozhraní jsou v `game/include/IslandSimulator/`, testy v `tests/`, sestavení a validátory v `tools/`.
Podrobnější popis je v [architektuře](docs/architecture.md); seznam všech funkcí není nutné číst před úpravou jednoho systému.

## Bezpečnost a závislosti

- Kompilovat nejvýše na **čtyřech CPU jádrech**. Používat `taskset --cpu-list 0,1,2,3` a `-j2`;
  stejný limit musí zdědit i konfigurační sestavení závislostí a optimalizace WebAssembly.
- **AMDGPU GPUVM incident zůstává otevřený.** Automatické grafické kontroly spouštět přes existující
  izolované softwarové launchery. Ověřit skutečný renderer; samotné nastavení proměnné nestačí.
  Postup a omezení: [incident](docs/incidents/2026-10-08-amdgpu-gpuvm-reset.md).
- Externí CNA a Sharp Runtime jsou pouze ke čtení. Neměnit jejich worktree ani ovladače systému.
  Připnuté revize určuje [dependencies.lock](dependencies.lock), nikoli aktuální upstream `next`.
- Zachovat cizí změny v pracovním stromu; nepoužívat reset/clean ani přepis vzdálené historie.

## Sestavení a spuštění na tomto počítači

Existující `build/` je **Release/OPENGL33**, `build-probe/` je **Release/WEBGL2**.
Pro novou instalaci použít [Linux](docs/build-linux.md) nebo [Web](docs/build-web.md);
níže uvedené příkazy využívají už nakonfigurované adresáře.

Nativní sestavení a testy bez grafického zařízení:

```sh
export CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv
taskset --cpu-list 0,1,2,3 cmake --build build -j2
taskset --cpu-list 0,1,2,3 ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2
python3 tests/SoftwareCaptureTests.py
```

Dva vynechané validátory potřebují samostatně vytvořené vstupní artefakty; jejich vynechání není grafický test.
Bezpečný nativní snímek: `python3 tools/run_software_capture.py build jungle`.
Launcher vyžaduje přístup ke kernel journalu a ověřuje izolaci i softwarový renderer.

Webové sestavení, balení a místní HTTP server:

```sh
source /home/robertvokac/emsdk/emsdk_env.sh
export CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv
export EMCC_CORES=2 BINARYEN_CORES=2 CMAKE_BUILD_PARALLEL_LEVEL=2
taskset --cpu-list 0,1,2,3 cmake --build --preset web-webgl2
tools/package_web_release.sh build-probe
python3 -m http.server 8765 --bind 127.0.0.1 --directory build-probe/web-release
```

Otevřít `http://127.0.0.1:8765/island-simulator.html`; soubory nespouštět přes `file://`.
Poslední ZIP: `build-probe/island-simulator-web-30106358.zip` (14 473 341 bajtů).
Balení obsahuje HTML, JS, WASM, data, gzip varianty a manifest s hashi.

## Poslední doložené ověření — 2026-11-03

| Kontrola | Výsledek a hranice |
| --- | --- |
| Linux Release/OPENGL33 | 109/109 bezpečných CTestů a 79 testů launcheru prošlo. Display testy a dva samostatné validátory nejsou součástí tohoto počtu. |
| Nativní obraz | 36 aktuálních softwarových scén / 80 PNG; zkontrolováno 36 hlavních snímků a 28 dalších fází, 16 fází přesně odpovídá dříve zkontrolovaným snímkům. llvmpipe/Pixman, nikoli fyzické GPU. |
| Webové sestavení | Release/WEBGL2, validace assetů a integrita ZIP/gzip prošly. |
| Chrome / SwiftShader | Přehled ostrova se vykreslil; 60 intervalů snímků, GL chyba 0, kontext zachován. Celý smoke test **selhal** na zastaralém očekávání geometrie stromu. |
| Firefox / llvmpipe | Produkční stránka se spustila; ovládání, změna velikosti a návrat do aktivního panelu byly ověřené před navazující kontrolou populace. Celý smoke test **selhal** na zastaralých počtech vegetace. Tři snímky produkční stránky byly vizuálně zkontrolované. |
| Ostatní platformy | Žádné nové fyzické GPU, Android, Debug ani jiné backendy tím nejsou kvalifikované. Starší výsledky mají vlastní data v platformních dokumentech. |

Úprava tohoto předání nepřidává nové výsledky sestavení ani vykreslování.

## Otevřené problémy a navazující práce

1. **Webové smoke testy mají neaktuální konstanty.** Chrome očekává strom 4998/4864
   vrcholů/trojúhelníků blízké sítě a 1071/1120 vzdálené; aktuální nativní i webový build mají
   5010/4888 a 1076/1130. Firefox očekává celkové populace 261/1747/569/3087,
   aktuální jsou 255/1637/543/2952. Při opravě porovnat s generátory a znovu spustit příslušný test;
   nepřeznačovat uložené neúspěšné běhy za úspěšné. Skripty jsou v `tools/run_web_smoke.mjs`
   a `tools/run_firefox_web_smoke.mjs`.
2. **CNA-ISLAND-002:** potvrzená chyba stock skinning shaderu na testovaném GLES limitu.
   Herní workaround pro opice zůstává aktivní. Audit upstreamu z 2026-11-03 není nový Android test.
3. **CNA-ISLAND-001:** podezření na Android surface/Home-resume race; oprava není ověřená.
   Oba nálezy a podmínky uzavření jsou v [BUGS.md](BUGS.md).
4. **AMDGPU/letová nestabilita:** příčina a hardwarová kvalifikace zůstávají otevřené.
   Softwarové snímky tyto incidenty neuzavírají.
5. Zůstávají omezení procedurálních pohybů, průniků měkké vegetace a zpětné trakce čtyřkolky
   na přibližně 18° rampě. Přehled: [známá omezení](docs/known-limitations.md).
6. Historické časy celého procesu kolísají s hostitelem/cache/JIT. Kontrolovaný softwarový
   experiment kontaktu a stínu chobotnice naměřil průměr +1,10 % procesorového času a asi
   +0,156 ms při odeslání stínového průchodu; není to obecný limit ani prokázané zrychlení celé hry.

## Důkazy a historie — pouze podle potřeby

- Nativní ověření a jeho dodání: `build/campaign-contact-final-verification.json`,
  `build/campaign-contact-final-delivery.json`.
- Poslední webový build a oba neúspěšné smoke běhy: `build/web-build-20261103-result.json`.
- Ochrana dříve citovaných snímků před automatickým úklidem:
  `build/handoff-evidence-retention.json`. Jde o strojový index, nikoli další dokument k prostudování.
- Soubory v `build*/` jsou lokální ignorované artefakty; čistý Git checkout je neobsahuje.
- Plné předchozí deníky jsou v historii, bez další kopie v živé dokumentaci:
  `git show 30106358:handoff.md` a `git show 30106358:HANDOFF.md`.
- [README.md](README.md) je vstup do projektu, [BUGS.md](BUGS.md) evidence frameworkových chyb,
  [ASSET_LICENSES.md](ASSET_LICENSES.md) původ assetů. [PLAN.md](PLAN.md) obsahuje také rozsáhlé
  historické pracovní zápisy; staré termíny, stavy a pokyny nepřebírat jako dnešní zadání.
